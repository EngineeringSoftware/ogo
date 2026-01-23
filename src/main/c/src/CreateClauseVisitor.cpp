#include "CreateClauseVisitor.h"

#include "cypher_expression.h"
#include <algorithm>

using namespace util;
using namespace std;
using namespace graph;

extern vector<ClassInfo *> classNameList;
// extern map<string, string> queryClassName2dynamicClassName;

CreateClauseVisitor::CreateClauseVisitor(
    Agent *agent,
    map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap) {
  this->nodeObjectMap = nodeObjectMap;
  this->agent = agent;
}

CreateClauseVisitor::~CreateClauseVisitor() {}

any CreateClauseVisitor::visitOC_SinglePartQuery(
    CypherParser::OC_SinglePartQueryContext *ctx) {
  for (int j = 0; j < ctx->oC_UpdatingClause().size(); j++) {
    ctx->oC_UpdatingClause()[j]->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_MultiPartQuery(
    CypherParser::OC_MultiPartQueryContext *ctx) {
  for (int j = 0; j < ctx->oC_UpdatingClause().size(); j++) {
    ctx->oC_UpdatingClause()[j]->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_UpdatingClause(
    CypherParser::OC_UpdatingClauseContext *ctx) {
  if (ctx->oC_Create()) {
    ctx->oC_Create()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_PatternElement(
    CypherParser::OC_PatternElementContext *ctx) {
  if (ctx->oC_NodePattern()) {
    ctx->oC_NodePattern()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_NodePattern(
    CypherParser::OC_NodePatternContext *ctx) {
  string nodeLabel;
  string nodeVariable;
  vector<jobject> fieldNames;
  vector<string> fieldNamesStr;
  vector<jobject> fieldTypesStr;
  vector<Agent::MethodReturnType> fieldTypes;
  vector<string> fieldTypesCStr;
  vector<jvalue> fieldValues;
  int valueCount = 0;

  /*Get primitive field names, types and values of the node*/
  if (ctx->oC_Properties()) {
    map<cypher::LiteralExpr *, cypher::Expression *> properties;
    AST_NODE_RETURN(ctx->oC_Properties()->accept(this), properties,
                    map<cypher::LiteralExpr *, cypher::Expression *>)
    for (const auto &property : properties) {
      cypher::LiteralExpr *lhsExpr = property.first;
      cypher::LiteralExpr *rhsExpr =
          (cypher::LiteralExpr *)((cypher::Expression *)(property.second))
              ->evaluate(this->nodeObjectMap);
      jobject fieldName = (lhsExpr->getValue()).l;
      fieldNames.push_back(fieldName);
      fieldNamesStr.push_back(agent->evaluateStringObject((jstring)fieldName));
      fieldTypes.push_back(rhsExpr->type);
      string fieldType = cypher::LiteralExpr::mapToDescriptor.at(rhsExpr->type);
      if (rhsExpr->type == Agent::MethodReturnType::JSTRING ||
          rhsExpr->type == Agent::MethodReturnType::JOBJECT ||
          rhsExpr->type == Agent::MethodReturnType::JNULL) {
        fieldType += ";";
      }
      fieldTypesCStr.push_back(fieldType);
      fieldTypesStr.push_back(agent->createNewString(fieldType));
      fieldValues.push_back(rhsExpr->getValue());
    }
  }

  /*Node Label
   *Guaranteed to return only one label (labels correspond to the type of the
   *instance)*/
  if (ctx->oC_NodeLabels()) {
    cypher::LiteralExpr *nodeLabelExpr = NULL;
    AST_NODE_RETURN(ctx->oC_NodeLabels()->accept(this), nodeLabelExpr,
                    cypher::LiteralExpr *)
    nodeLabel = STRING::cypherLabelToClass(
        agent->evaluateStringObject((jstring)(nodeLabelExpr->getValue()).l));
  }

  /*If the pattern is referenced by any variable then store it*/
  if (ctx->oC_Variable()) {
    cypher::LiteralExpr *nodeVariableExpr = NULL;
    AST_NODE_RETURN(ctx->oC_Variable()->accept(this), nodeVariableExpr,
                    cypher::LiteralExpr *)
    nodeVariable =
        agent->evaluateStringObject((jstring)(nodeVariableExpr->getValue()).l);
  }

  jobject object;
  string nodeLabelJavaClassFormat = STRING::replacePeriodWithSlash(nodeLabel);
  /*jclass nodeClass = agent->getClass(nodeLabelJavaClassFormat);

  if(!(nodeClass == NULL)){
    object = agent->createObjectWithoutConstructor(nodeClass);
    for(int j = 0; j < fieldNamesStr.size(); j++){
      agent->setObjectPrimitiveField(nodeLabelJavaClassFormat,
                                     object,
                                     fieldNamesStr.at(j),
                                     fieldTypesCStr.at(j),
                                     fieldTypes.at(j),
                                     fieldValues.at(j));
    }
  }
  else{*/
  vector<jvalue> args;
  vector<jobject> fieldValuesResolved;
  jvalue returnValue;
  Agent::MethodReturnType returnType;

  jvalue classNameArg;
  jvalue fieldNameArg;
  jvalue fieldTypeArg;
  jvalue fieldValueArg;

  for (int j = 0; j < fieldTypes.size(); j++) {
    switch (fieldTypes.at(j)) {
    case Agent::MethodReturnType::JBOOLEAN:
    case Agent::MethodReturnType::JSTRING:
    case Agent::MethodReturnType::JDOUBLE:
    case Agent::MethodReturnType::JLONG:
      fieldValuesResolved.push_back(
          agent->createObject(fieldValues.at(j), fieldTypes.at(j)));
      break;
    default:
      cout << "Error : CREATE clause can only create String, Long, Double and "
              "Boolean fields"
           << "\n";
      break;
    }
  }

  classNameArg.l = agent->createNewString(nodeLabelJavaClassFormat);
  fieldNameArg.l = agent->createObjectArray(fieldNames);
  fieldTypeArg.l = agent->createObjectArray(fieldTypesStr);
  fieldValueArg.l = agent->createObjectArray(fieldValuesResolved);

  // vector<jvalue> args2;
  // jvalue returnValue2;
  // Agent::MethodReturnType returnType2;

  // args2 = {classNameArg, fieldTypeArg};

  // agent->callStaticMethod("org/ogo/client/OGO",
  //			      "getDynamicClassName",
  //		      "(Ljava/lang/String;[Ljava/lang/String;)Ljava/lang/String;",
  //		      args2,
  //		      returnValue2,
  //		      returnType2);
  //       string modClassName =
  //       agent->evaluateStringObject((jstring)returnValue2.l);

  // cout << "Modified Class Name " << modClassName << "\n\n";

  args = {classNameArg, fieldNameArg, fieldTypeArg, fieldValueArg};

  agent->callStaticMethod("org/ogo/util/ClassHelper", "createClass",
                          "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/"
                          "String;[Ljava/lang/Object;)Ljava/lang/Class;",
                          args, returnValue, returnType);
  jclass createdClass = (jclass)(returnValue.l);
  object = agent->createObjectV(createdClass, "()V");
  //}
  nodeObjectMap->insert(pair<string, vector<pair<InstanceInfo *, jobject>>>{
      nodeVariable, vector<pair<InstanceInfo *, jobject>>()});
  nodeObjectMap->at(nodeVariable)
      .push_back(pair<InstanceInfo *, jobject>{NULL, object});
  return any();
}

any CreateClauseVisitor::visitOC_Variable(
    CypherParser::OC_VariableContext *ctx) {
  if (ctx->oC_SymbolicName()) {
    return ctx->oC_SymbolicName()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_SymbolicName(
    CypherParser::OC_SymbolicNameContext *ctx) {
  if (ctx->UnescapedSymbolicName())
    return new cypher::LiteralExpr(
        ctx->UnescapedSymbolicName()->getSymbol()->getText(),
        Agent::MethodReturnType::JSTRING);
  else if (ctx->EscapedSymbolicName())
    return new cypher::LiteralExpr(
        ctx->EscapedSymbolicName()->getSymbol()->getText(),
        Agent::MethodReturnType::JSTRING);
  else if (ctx->HexLetter())
    return new cypher::LiteralExpr(ctx->HexLetter()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  else if (ctx->COUNT())
    return new cypher::LiteralExpr(ctx->COUNT()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  else if (ctx->FILTER())
    return new cypher::LiteralExpr(ctx->FILTER()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  else if (ctx->EXTRACT())
    return new cypher::LiteralExpr(ctx->EXTRACT()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  else if (ctx->ANY())
    return new cypher::LiteralExpr(ctx->ANY()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  else if (ctx->NONE())
    return new cypher::LiteralExpr(ctx->NONE()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  else if (ctx->SINGLE())
    return new cypher::LiteralExpr(ctx->SINGLE()->getSymbol()->getText(),
                                   Agent::MethodReturnType::JSTRING);
  return any();
}

any CreateClauseVisitor::visitOC_Properties(
    CypherParser::OC_PropertiesContext *ctx) {
  if (ctx->oC_MapLiteral()) {
    return ctx->oC_MapLiteral()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_MapLiteral(
    CypherParser::OC_MapLiteralContext *ctx) {
  if (ctx->oC_PropertyKeyName().size() > 0 &&
      ctx->oC_PropertyKeyName().size() == ctx->oC_Expression().size()) {
    map<cypher::LiteralExpr *, cypher::Expression *> properties;
    for (int j = 0; j < ctx->oC_PropertyKeyName().size(); j++) {
      cypher::LiteralExpr *lhsExpr = NULL;
      cypher::Expression *rhsExpr = NULL;
      AST_NODE_RETURN(ctx->oC_PropertyKeyName()[j]->accept(this), lhsExpr,
                      cypher::LiteralExpr *)
      AST_NODE_RETURN(ctx->oC_Expression()[j]->accept(this), rhsExpr,
                      cypher::Expression *)
      properties.insert(
          pair<cypher::LiteralExpr *, cypher::Expression *>{lhsExpr, rhsExpr});
    }
    return properties;
  }
  return any();
}

any CreateClauseVisitor::visitOC_PropertyKeyName(
    CypherParser::OC_PropertyKeyNameContext *ctx) {
  if (ctx->oC_SchemaName()) {
    return ctx->oC_SchemaName()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_Expression(
    CypherParser::OC_ExpressionContext *ctx) {
  if (ctx->oC_OrExpression()) {
    return ctx->oC_OrExpression()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_OrExpression(
    CypherParser::OC_OrExpressionContext *ctx) {
  if (ctx->oC_XorExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_XorExpression(),
                         cypher::BinaryExpr::BinaryOperators::OR, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_XorExpression().size() == 1) {
    return ctx->oC_XorExpression(0)->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_XorExpression(
    CypherParser::OC_XorExpressionContext *ctx) {
  if (ctx->oC_AndExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_AndExpression(),
                         cypher::BinaryExpr::BinaryOperators::XOR, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_AndExpression().size() == 1) {
    return ctx->oC_AndExpression(0)->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_AndExpression(
    CypherParser::OC_AndExpressionContext *ctx) {
  if (ctx->oC_NotExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_NotExpression(),
                         cypher::BinaryExpr::BinaryOperators::AND, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_NotExpression().size() == 1) {
    return ctx->oC_NotExpression(0)->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_NotExpression(
    CypherParser::OC_NotExpressionContext *ctx) {
  if (ctx->oC_ComparisonExpression()) {
    cypher::Expression *expr = NULL;
    AST_NODE_RETURN(ctx->oC_ComparisonExpression()->accept(this), expr,
                    cypher::Expression *);
    for (int j = 0; j < ctx->NOT().size(); j++) {
      cypher::Expression *temp = expr;
      expr = (cypher::Expression *)new cypher::UnaryExpr(
          temp, cypher::UnaryExpr::UnaryOperators::NOT);
    }
    return expr;
  }
  return any();
}

any CreateClauseVisitor::visitOC_ComparisonExpression(
    CypherParser::OC_ComparisonExpressionContext *ctx) {
  if (ctx->oC_PartialComparisonExpression().size() > 0) {
    cypher::Expression *left = NULL;
    cypher::Expression *binaryExpr = NULL;
    AST_NODE_RETURN(ctx->oC_AddOrSubtractExpression()->accept(this), left,
                    cypher::Expression *)
    for (int j = 0; j < ctx->oC_PartialComparisonExpression().size(); j++) {
      cypher::Expression *right = NULL;
      AST_NODE_RETURN(ctx->oC_PartialComparisonExpression(j++)->accept(this),
                      right, cypher::Expression *)
      if (binaryExpr == NULL) {
        binaryExpr = new cypher::BinaryExpr(
            left, right, cypher::BinaryExpr::BinaryOperators::EQ);
      } else {
        left = (cypher::Expression *)binaryExpr;
        binaryExpr = new cypher::BinaryExpr(
            left, right, cypher::BinaryExpr::BinaryOperators::EQ);
      }
    }
    return binaryExpr;
  } else if (ctx->oC_AddOrSubtractExpression()) {
    return ctx->oC_AddOrSubtractExpression()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_AddOrSubtractExpression(
    CypherParser::OC_AddOrSubtractExpressionContext *ctx) {
  if (ctx->oC_MultiplyDivideModuloExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_MultiplyDivideModuloExpression(),
                         cypher::BinaryExpr::BinaryOperators::ADD, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_MultiplyDivideModuloExpression().size() == 1) {
    return ctx->oC_MultiplyDivideModuloExpression(0)->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_MultiplyDivideModuloExpression(
    CypherParser::OC_MultiplyDivideModuloExpressionContext *ctx) {
  if (ctx->oC_PowerOfExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_PowerOfExpression(),
                         cypher::BinaryExpr::BinaryOperators::MUL, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_PowerOfExpression().size() == 1) {
    return ctx->oC_PowerOfExpression(0)->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_PowerOfExpression(
    CypherParser::OC_PowerOfExpressionContext *ctx) {
  if (ctx->oC_UnaryAddOrSubtractExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_UnaryAddOrSubtractExpression(),
                         cypher::BinaryExpr::BinaryOperators::EXP, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_UnaryAddOrSubtractExpression().size() == 1) {
    return ctx->oC_UnaryAddOrSubtractExpression(0)->accept(this);
  }
}

any CreateClauseVisitor::visitOC_UnaryAddOrSubtractExpression(
    CypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx) {
  if (ctx->oC_StringListNullOperatorExpression()) {
    return ctx->oC_StringListNullOperatorExpression()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_StringListNullOperatorExpression(
    CypherParser::OC_StringListNullOperatorExpressionContext *ctx) {
  if (ctx->oC_PropertyOrLabelsExpression()) {
    return ctx->oC_PropertyOrLabelsExpression()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_PropertyOrLabelsExpression(
    CypherParser::OC_PropertyOrLabelsExpressionContext *ctx) {
  if (ctx->oC_Atom()) {
    return ctx->oC_Atom()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_Atom(CypherParser::OC_AtomContext *ctx) {
  if (ctx->oC_Literal()) {
    return ctx->oC_Literal()->accept(this);
  }
  return any();
}

any CreateClauseVisitor::visitOC_Literal(CypherParser::OC_LiteralContext *ctx) {
  if (ctx->oC_NumberLiteral()) {
    return ctx->oC_NumberLiteral()->accept(this);
  } else if (ctx->oC_BooleanLiteral()) {
    return ctx->oC_BooleanLiteral()->accept(this);
  } else if (ctx->StringLiteral()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->StringLiteral()->getSymbol()->getText(),
        Agent::MethodReturnType::JSTRING);
  } else if (ctx->NULL_()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        "NULL", Agent::MethodReturnType::JNULL);
  }
  return any();
}

any CreateClauseVisitor::visitOC_NumberLiteral(
    CypherParser::OC_NumberLiteralContext *ctx) {
  for (auto *child : ctx->children) {
    if (child) {
      return child->accept(this);
    }
  }
  return any();
}

any CreateClauseVisitor::visitOC_IntegerLiteral(
    CypherParser::OC_IntegerLiteralContext *ctx) {
  if (ctx->DecimalInteger()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->DecimalInteger()->getSymbol()->getText(),
        Agent::MethodReturnType::JLONG);
  }
  return any();
}

any CreateClauseVisitor::visitOC_DoubleLiteral(
    CypherParser::OC_DoubleLiteralContext *ctx) {
  if (ctx->RegularDecimalReal()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->RegularDecimalReal()->getSymbol()->getText(),
        Agent::MethodReturnType::JDOUBLE);
  }
  return any();
}

any CreateClauseVisitor::visitOC_BooleanLiteral(
    CypherParser::OC_BooleanLiteralContext *ctx) {
  if (ctx->TRUE()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->TRUE()->getSymbol()->getText(), Agent::MethodReturnType::JBOOLEAN);
  } else if (ctx->FALSE()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->FALSE()->getSymbol()->getText(),
        Agent::MethodReturnType::JBOOLEAN);
  }
  return any();
}
