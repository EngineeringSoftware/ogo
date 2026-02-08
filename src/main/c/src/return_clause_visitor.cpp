#include "return_clause_visitor.hpp"
#include "ParserRuleContext.h"
#include "class_info.hpp"
#include "cypher_expression.hpp"
#include "method_info.hpp"
#include "string_utils.hpp"

#include "tree/TerminalNode.h"
#include <any>
#include <iostream>
#include <jni.h>
#include <jni_util.hpp>
#include <string>
#include <vector>

using namespace util;
using namespace graph;
using namespace std;

ReturnClauseVisitor::ReturnClauseVisitor(
    Agent *agent,
    map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap,
    vector<jobject> *result) {
  this->agent = agent;
  this->nodeObjectMap = nodeObjectMap;
  this->result = result;
}

ReturnClauseVisitor::~ReturnClauseVisitor() {}

any ReturnClauseVisitor::visitOC_SinglePartQuery(
    CypherParser::OC_SinglePartQueryContext *ctx) {
  if (ctx->oC_Return()) {
    ctx->oC_Return()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_Return(CypherParser::OC_ReturnContext *ctx) {
  if (ctx->oC_ProjectionBody()) {
    return ctx->oC_ProjectionBody()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_ProjectionBody(
    CypherParser::OC_ProjectionBodyContext *ctx) {
  if (ctx->oC_ProjectionItems()) {
    return ctx->oC_ProjectionItems()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_ProjectionItems(
    CypherParser::OC_ProjectionItemsContext *ctx) {
  for (int j = 0; j < ctx->oC_ProjectionItem().size(); j++) {
    cypher::Expression *expr = NULL;
    AST_NODE_RETURN(ctx->oC_ProjectionItem(j)->accept(this), expr,
                    cypher::Expression *);
    cypher::LiteralExpr *litExpr =
        (cypher::LiteralExpr *)expr->evaluate(this->nodeObjectMap);
    this->result->push_back(
        agent->createObject(litExpr->getValue(), litExpr->type));
  }
  return any();
}

any ReturnClauseVisitor::visitOC_ProjectionItem(
    CypherParser::OC_ProjectionItemContext *ctx) {
  if (ctx->oC_Expression()) {
    return ctx->oC_Expression()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_Expression(
    CypherParser::OC_ExpressionContext *ctx) {
  if (ctx->oC_OrExpression()) {
    return ctx->oC_OrExpression()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_OrExpression(
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

any ReturnClauseVisitor::visitOC_XorExpression(
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

any ReturnClauseVisitor::visitOC_AndExpression(
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

any ReturnClauseVisitor::visitOC_NotExpression(
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

any ReturnClauseVisitor::visitOC_ComparisonExpression(
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

any ReturnClauseVisitor::visitOC_AddOrSubtractExpression(
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

any ReturnClauseVisitor::visitOC_MultiplyDivideModuloExpression(
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

any ReturnClauseVisitor::visitOC_PowerOfExpression(
    CypherParser::OC_PowerOfExpressionContext *ctx) {
  if (ctx->oC_UnaryAddOrSubtractExpression().size() > 1) {
    cypher::Expression *binaryExpr = NULL;
    AST_MAKE_BINARY_EXPR(ctx->oC_UnaryAddOrSubtractExpression(),
                         cypher::BinaryExpr::BinaryOperators::EXP, binaryExpr)
    return binaryExpr;
  } else if (ctx->oC_UnaryAddOrSubtractExpression().size() == 1) {
    return ctx->oC_UnaryAddOrSubtractExpression(0)->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_UnaryAddOrSubtractExpression(
    CypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx) {
  if (ctx->oC_StringListNullOperatorExpression()) {
    return ctx->oC_StringListNullOperatorExpression()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_StringListNullOperatorExpression(
    CypherParser::OC_StringListNullOperatorExpressionContext *ctx) {
  if (ctx->oC_PropertyOrLabelsExpression()) {
    return ctx->oC_PropertyOrLabelsExpression()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_PropertyOrLabelsExpression(
    CypherParser::OC_PropertyOrLabelsExpressionContext *ctx) {
  if (ctx->oC_PropertyLookup().size() > 0 && ctx->oC_Atom()) {
    cypher::Expression *expr = NULL;
    string propertyName;

    AST_NODE_RETURN(ctx->oC_Atom()->accept(this), expr, cypher::Expression *)
    AST_NODE_RETURN(ctx->oC_PropertyLookup()[0]->accept(this), propertyName,
                    string)
    return (cypher::Expression *)new cypher::PropertyLookupExpr(
        ((cypher::LiteralExpr *)expr)->value, propertyName);
  } else if (ctx->oC_Atom()) {
    return ctx->oC_Atom()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_Atom(CypherParser::OC_AtomContext *ctx) {
  if (ctx->oC_Variable()) {
    return ctx->oC_Variable()->accept(this);
  } else if (ctx->oC_Literal()) {
    return ctx->oC_Literal()->accept(this);
  } else if (ctx->oC_FunctionInvocation()) {
    return ctx->oC_FunctionInvocation()->accept(this);
  } else if (ctx->COUNT()) {
  }
  return any();
}

any ReturnClauseVisitor::visitOC_FunctionInvocation(
    CypherParser::OC_FunctionInvocationContext *ctx) {
  vector<cypher::LiteralExpr *> functionArgs;

  for (int j = 0; j < ctx->oC_Expression().size(); j++) {
    cypher::Expression *expr = NULL;
    AST_NODE_RETURN(ctx->oC_Expression()[j]->accept(this), expr,
                    cypher::Expression *)
    if (expr != NULL) {
      functionArgs.push_back((cypher::LiteralExpr *)expr);
    }
  }

  if (ctx->oC_FunctionName()) {
    cypher::Expression *function = NULL;
    AST_NODE_RETURN(ctx->oC_FunctionName()->accept(this), function,
                    cypher::Expression *)
    for (cypher::LiteralExpr *litExpr : functionArgs) {
      ((cypher::FunctionInvocationExpr *)function)->addArg(litExpr);
    }
    return function;
  }

  return any();
}

any ReturnClauseVisitor::visitOC_FunctionName(
    CypherParser::OC_FunctionNameContext *ctx) {
  string callerNode = "";
  string calledFunction = "";

  if (ctx->oC_SymbolicName()) {
    AST_NODE_RETURN(ctx->oC_SymbolicName()->accept(this), calledFunction,
                    string)
    calledFunction = STRING::cypherLabelToClass(calledFunction);
  }

  if (ctx->oC_Namespace()) {
    AST_NODE_RETURN(ctx->oC_Namespace()->accept(this), callerNode, string)
  }
  return (cypher::Expression *)new cypher::FunctionInvocationExpr(
      callerNode, calledFunction);
}

any ReturnClauseVisitor::visitOC_Namespace(
    CypherParser::OC_NamespaceContext *ctx) {
  /*TODO : Cannot chain instances while invoking methods. Only one instance
   * allowed */
  if (ctx->oC_SymbolicName().size() == 1) {
    return ctx->oC_SymbolicName()[0]->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_SymbolicName(
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

any ReturnClauseVisitor::visitOC_Variable(
    CypherParser::OC_VariableContext *ctx) {
  if (ctx->oC_SymbolicName()) {
    string nodeName;
    AST_NODE_RETURN(ctx->oC_SymbolicName()->accept(this), nodeName, string)
    return (cypher::Expression *)new cypher::LiteralExpr(
        nodeName, Agent::MethodReturnType::JOBJECT);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_SchemaName(
    CypherParser::OC_SchemaNameContext *ctx) {
  if (ctx->oC_SymbolicName()) {
    return ctx->oC_SymbolicName()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_PropertyLookup(
    CypherParser::OC_PropertyLookupContext *ctx) {
  if (ctx->oC_PropertyKeyName()) {
    return ctx->oC_PropertyKeyName()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_PropertyKeyName(
    CypherParser::OC_PropertyKeyNameContext *ctx) {
  if (ctx->oC_SchemaName()) {
    return ctx->oC_SchemaName()->accept(this);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_Literal(CypherParser::OC_LiteralContext *ctx) {
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

any ReturnClauseVisitor::visitOC_NumberLiteral(
    CypherParser::OC_NumberLiteralContext *ctx) {
  for (auto *child : ctx->children) {
    if (child) {
      return child->accept(this);
    }
  }
  return any();
}

any ReturnClauseVisitor::visitOC_IntegerLiteral(
    CypherParser::OC_IntegerLiteralContext *ctx) {
  if (ctx->DecimalInteger()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->DecimalInteger()->getSymbol()->getText(),
        Agent::MethodReturnType::JLONG);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_DoubleLiteral(
    CypherParser::OC_DoubleLiteralContext *ctx) {
  if (ctx->RegularDecimalReal()) {
    return (cypher::Expression *)new cypher::LiteralExpr(
        ctx->RegularDecimalReal()->getSymbol()->getText(),
        Agent::MethodReturnType::JDOUBLE);
  }
  return any();
}

any ReturnClauseVisitor::visitOC_BooleanLiteral(
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
