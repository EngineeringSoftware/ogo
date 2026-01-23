#include "MatchClauseVisitor.h"
#include "instanceInfo.h"

#include <any>

using namespace std;
using namespace graph;
using namespace util;

extern vector<ClassInfo *> classNameList;

MatchClauseVisitor::MatchClauseVisitor(
    Agent *agent,
    map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap) {
  this->agent = agent;
  this->nodeObjectMap = nodeObjectMap;
}

MatchClauseVisitor::~MatchClauseVisitor() {}

any MatchClauseVisitor::visitOC_SinglePartQuery(
    CypherParser::OC_SinglePartQueryContext *ctx) {
  for (int j = 0; j < ctx->oC_ReadingClause().size(); j++) {
    ctx->oC_ReadingClause()[j]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_MultiPartQuery(
    CypherParser::OC_MultiPartQueryContext *ctx) {
  for (int j = 0; j < ctx->oC_ReadingClause().size(); j++) {
    ctx->oC_ReadingClause()[j]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_ReadingClause(
    CypherParser::OC_ReadingClauseContext *ctx) {
  if (ctx->oC_Match()) {
    ctx->oC_Match()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_PatternElement(
    CypherParser::OC_PatternElementContext *ctx) {
  if (ctx->oC_NodePattern()) {
    ctx->oC_NodePattern()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_NodePattern(
    CypherParser::OC_NodePatternContext *ctx) {
  map<string, pair<string, Agent::jvmtiExtendedFieldType>> properties;
  string nodeLabel = "";
  string nodeVariable;
  vector<InstanceInfo *> matchedInstances;
  any result;

  /*Properties of the node*/
  if (ctx->oC_Properties()) {
    AST_NODE_RETURN(ctx->oC_Properties()->accept(this), properties,
                    map<string, pair<string, Agent::jvmtiExtendedFieldType>>)
  }

  /*Labels to the node
   *Guaranteed to return only one label (labels correspond to the type of the
   *instance)*/
  if (ctx->oC_NodeLabels()) {
    AST_NODE_RETURN(ctx->oC_NodeLabels()->accept(this), nodeLabel, string)
    nodeLabel = STRING::cypherLabelToClass(nodeLabel);
  }

  /*If the pattern is referenced by any variable then store it*/
  if (ctx->oC_Variable()) {
    AST_NODE_RETURN(ctx->oC_Variable()->accept(this), nodeVariable, string)
  }

  for (ClassInfo *cInfo : classNameList) {
    if (nodeLabel.size() == 0 || cInfo->name == nodeLabel) {
      long *tags = NULL;
      int tagCount = 0;
      jobject *objects = NULL;
      jvmtiError err;

      agent->getObjectsWithTags(cInfo->instanceTags.data(),
                                cInfo->instances.size(), &objects, &tags,
                                &tagCount);
      bool isMatch = false;
      int fCount = properties.size();
      for (FieldInfo *fInfo : cInfo->fields) {
        for (const auto &property : properties) {
          if (property.first == fInfo->name) {
            fCount--;
          }
        }
      }
      cout << "FCount is : " << fCount << "\n";
      isMatch = fCount == 0;

      for (int i = 0; i < tagCount; i++) {
        int hashCode = agent->getObjectHashCode(objects[i]);
        InstanceInfo *iInfo = NULL;
        cInfo->getInstanceInfoWithTag(tags[i], &iInfo);
        if (properties.size() == 0) {
          vector<pair<InstanceInfo *, jobject>> temp;
          temp.push_back(pair<InstanceInfo *, jobject>{iInfo, objects[i]});
          nodeObjectMap->insert({nodeVariable, temp});
        }
        for (const auto &property : properties) {
          if (property.first == "hash" &&
              stol(property.second.first) == hashCode) {
            vector<pair<InstanceInfo *, jobject>> temp;
            temp.push_back(pair<InstanceInfo *, jobject>{iInfo, objects[i]});
            nodeObjectMap->insert({nodeVariable, temp});
            return any();
          } else if (isMatch) {
            vector<pair<InstanceInfo *, jobject>> temp;
            temp.push_back(pair<InstanceInfo *, jobject>{iInfo, objects[i]});
            nodeObjectMap->insert({nodeVariable, temp});
            return any();
          }
        }
      }
    }
  }
  return any();
}

any MatchClauseVisitor::visitOC_Variable(
    CypherParser::OC_VariableContext *ctx) {
  if (ctx->oC_SymbolicName()) {
    return ctx->oC_SymbolicName()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_SymbolicName(
    CypherParser::OC_SymbolicNameContext *ctx) {
  if (ctx->UnescapedSymbolicName())
    return ctx->UnescapedSymbolicName()->getSymbol()->getText();
  else if (ctx->EscapedSymbolicName())
    return ctx->EscapedSymbolicName()->getSymbol()->getText();
  else if (ctx->HexLetter())
    return ctx->HexLetter()->getSymbol()->getText();
  else if (ctx->COUNT())
    return ctx->COUNT()->getSymbol()->getText();
  else if (ctx->FILTER())
    return ctx->FILTER()->getSymbol()->getText();
  else if (ctx->EXTRACT())
    return ctx->EXTRACT()->getSymbol()->getText();
  else if (ctx->ANY())
    return ctx->ANY()->getSymbol()->getText();
  else if (ctx->NONE())
    return ctx->NONE()->getSymbol()->getText();
  else if (ctx->SINGLE())
    return ctx->SINGLE()->getSymbol()->getText();
  return any();
}

any MatchClauseVisitor::visitOC_Properties(
    CypherParser::OC_PropertiesContext *ctx) {
  if (ctx->oC_MapLiteral()) {
    return ctx->oC_MapLiteral()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_MapLiteral(
    CypherParser::OC_MapLiteralContext *ctx) {
  if (ctx->oC_PropertyKeyName().size() > 0 &&
      ctx->oC_PropertyKeyName().size() == ctx->oC_Expression().size()) {
    map<string, pair<string, Agent::jvmtiExtendedFieldType>> properties;
    for (int j = 0; j < ctx->oC_PropertyKeyName().size(); j++) {
      any lhsReturn = ctx->oC_PropertyKeyName()[j]->accept(this);
      any rhsReturn = ctx->oC_Expression()[j]->accept(this);
      if (lhsReturn.has_value() && rhsReturn.has_value()) {
        properties.insert(
            {any_cast<string>(lhsReturn),
             any_cast<pair<string, Agent::jvmtiExtendedFieldType>>(rhsReturn)});
      }
    }
    return properties;
  }
  return any();
}

any MatchClauseVisitor::visitOC_PropertyKeyName(
    CypherParser::OC_PropertyKeyNameContext *ctx) {
  if (ctx->oC_SchemaName()) {
    return ctx->oC_SchemaName()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_Expression(
    CypherParser::OC_ExpressionContext *ctx) {
  if (ctx->oC_OrExpression()) {
    return ctx->oC_OrExpression()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_OrExpression(
    CypherParser::OC_OrExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_XorExpression().size() == 1) {
    return ctx->oC_XorExpression()[0]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_XorExpression(
    CypherParser::OC_XorExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_AndExpression().size() == 1) {
    return ctx->oC_AndExpression()[0]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_AndExpression(
    CypherParser::OC_AndExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_NotExpression().size() == 1) {
    return ctx->oC_NotExpression()[0]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_NotExpression(
    CypherParser::OC_NotExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_ComparisonExpression()) {
    return ctx->oC_ComparisonExpression()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_ComparisonExpression(
    CypherParser::OC_ComparisonExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_AddOrSubtractExpression()) {
    return ctx->oC_AddOrSubtractExpression()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_AddOrSubtractExpression(
    CypherParser::OC_AddOrSubtractExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_MultiplyDivideModuloExpression().size() == 1) {
    return ctx->oC_MultiplyDivideModuloExpression()[0]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_MultiplyDivideModuloExpression(
    CypherParser::OC_MultiplyDivideModuloExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_PowerOfExpression().size() == 1) {
    return ctx->oC_PowerOfExpression()[0]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_PowerOfExpression(
    CypherParser::OC_PowerOfExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_UnaryAddOrSubtractExpression().size() == 1) {
    return ctx->oC_UnaryAddOrSubtractExpression()[0]->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_UnaryAddOrSubtractExpression(
    CypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_StringListNullOperatorExpression()) {
    return ctx->oC_StringListNullOperatorExpression()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_StringListNullOperatorExpression(
    CypherParser::OC_StringListNullOperatorExpressionContext *ctx) {
  /* Property based node pattern filtering can only have one property
   * value per property key */
  if (ctx->oC_PropertyOrLabelsExpression()) {
    return ctx->oC_PropertyOrLabelsExpression()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_PropertyOrLabelsExpression(
    CypherParser::OC_PropertyOrLabelsExpressionContext *ctx) {
  if (ctx->oC_Atom()) {
    return ctx->oC_Atom()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_Atom(CypherParser::OC_AtomContext *ctx) {
  if (ctx->oC_Literal()) {
    return ctx->oC_Literal()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_Literal(CypherParser::OC_LiteralContext *ctx) {
  if (ctx->oC_NumberLiteral()) {
    return ctx->oC_NumberLiteral()->accept(this);
  } else if (ctx->StringLiteral()) {
    string stringLiteral = ctx->StringLiteral()->getSymbol()->getText();
    stringLiteral = stringLiteral.substr(1, stringLiteral.size() - 2);
    return pair<string, Agent::jvmtiExtendedFieldType>{
        stringLiteral, Agent::JVMTI_EXTENDED_FIELD_TYPE_STRING};
  } else if (ctx->oC_BooleanLiteral()) {
    return ctx->oC_BooleanLiteral()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_NumberLiteral(
    CypherParser::OC_NumberLiteralContext *ctx) {
  if (ctx->oC_IntegerLiteral()) {
    return ctx->oC_IntegerLiteral()->accept(this);
  } else if (ctx->oC_DoubleLiteral()) {
    return ctx->oC_DoubleLiteral()->accept(this);
  }
  return any();
}

any MatchClauseVisitor::visitOC_DoubleLiteral(
    CypherParser::OC_DoubleLiteralContext *ctx) {
  /*+TODO : convert string to double*/
  if (ctx->ExponentDecimalReal()) {
    return ctx->ExponentDecimalReal()->getSymbol()->getText();
  } else if (ctx->RegularDecimalReal()) {
    return pair<string, Agent::jvmtiExtendedFieldType>{
        ctx->RegularDecimalReal()->getSymbol()->getText(),
        Agent::JVMTI_EXTENDED_FIELD_TYPE_FLOAT};
  }
  return any();
}

any MatchClauseVisitor::visitOC_IntegerLiteral(
    CypherParser::OC_IntegerLiteralContext *ctx) {
  if (ctx->DecimalInteger()) {
    return pair<string, Agent::jvmtiExtendedFieldType>{
        ctx->DecimalInteger()->getSymbol()->getText(),
        Agent::JVMTI_EXTENDED_FIELD_TYPE_INT};
  }
  /*+TODO : convert string to int*/
  else if (ctx->HexInteger()) {
    return ctx->HexInteger()->getSymbol()->getText();
  }
  /*+TODO : convert string to int*/
  else if (ctx->OctalInteger()) {
    return ctx->OctalInteger()->getSymbol()->getText();
  }
  return any();
}

any MatchClauseVisitor::visitOC_BooleanLiteral(
    CypherParser::OC_BooleanLiteralContext *ctx) {
  if (ctx->TRUE()) {
    return pair<string, Agent::jvmtiExtendedFieldType>{
        ctx->TRUE()->getSymbol()->getText(),
        Agent::JVMTI_EXTENDED_FIELD_TYPE_BOOLEAN};
  } else if (ctx->FALSE()) {
    return pair<string, Agent::jvmtiExtendedFieldType>{
        ctx->FALSE()->getSymbol()->getText(),
        Agent::JVMTI_EXTENDED_FIELD_TYPE_BOOLEAN};
  }
  return any();
}
