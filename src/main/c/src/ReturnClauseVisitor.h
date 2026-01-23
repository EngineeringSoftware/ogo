#pragma once

#include "CypherBaseVisitor.h"
#include "CypherParser.h"
#include "ParserRuleContext.h"
#include "classInfo.h"
#include "cypher_expression.h"
#include "instanceInfo.h"
#include "jni_util.h"
#include "ogoConstants.h"
#include "string_utils.h"

#include <jni.h>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace util;
using namespace graph;

class ReturnClauseVisitor : public CypherBaseVisitor {

#define AST_NODE_RETURN(VISIT, ASSIGN, ...)                                    \
  {                                                                            \
    any result = VISIT;                                                        \
    if (result.has_value()) {                                                  \
      ASSIGN = any_cast<__VA_ARGS__>(result);                                  \
    }                                                                          \
  }

#define AST_MAKE_BINARY_EXPR(VISIT, OP, ASSIGN)                                \
  {                                                                            \
    cypher::BinaryExpr *_binaryExpr = NULL;                                    \
    for (int j = VISIT.size() - 1; j >= 0;) {                                  \
      cypher::Expression *left = NULL;                                         \
      cypher::Expression *right = NULL;                                        \
      if (_binaryExpr == NULL) {                                               \
        AST_NODE_RETURN(VISIT[j--]->accept(this), left, cypher::Expression *); \
        AST_NODE_RETURN(VISIT[j--]->accept(this), right,                       \
                        cypher::Expression *);                                 \
        _binaryExpr = new cypher::BinaryExpr(left, right, OP);                 \
      } else {                                                                 \
        AST_NODE_RETURN(VISIT[j--]->accept(this), left, cypher::Expression *); \
        right = (cypher::Expression *)_binaryExpr;                             \
        _binaryExpr = new cypher::BinaryExpr(left, right, OP);                 \
      }                                                                        \
    }                                                                          \
    ASSIGN = _binaryExpr;                                                      \
  }

public:
  ReturnClauseVisitor(
      Agent *agent,
      map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap,
      vector<jobject> *result);
  ~ReturnClauseVisitor();

  any visitOC_SinglePartQuery(CypherParser::OC_SinglePartQueryContext *ctx);
  any visitOC_Return(CypherParser::OC_ReturnContext *ctx);
  any visitOC_ProjectionBody(CypherParser::OC_ProjectionBodyContext *ctx);
  any visitOC_ProjectionItems(CypherParser::OC_ProjectionItemsContext *ctx);
  any visitOC_ProjectionItem(CypherParser::OC_ProjectionItemContext *ctx);
  any visitOC_Expression(CypherParser::OC_ExpressionContext *ctx);
  any visitOC_OrExpression(CypherParser::OC_OrExpressionContext *ctx);
  any visitOC_XorExpression(CypherParser::OC_XorExpressionContext *ctx);
  any visitOC_AndExpression(CypherParser::OC_AndExpressionContext *ctx);
  any visitOC_NotExpression(CypherParser::OC_NotExpressionContext *ctx);
  any visitOC_ComparisonExpression(
      CypherParser::OC_ComparisonExpressionContext *ctx);
  any visitOC_AddOrSubtractExpression(
      CypherParser::OC_AddOrSubtractExpressionContext *ctx);
  any visitOC_MultiplyDivideModuloExpression(
      CypherParser::OC_MultiplyDivideModuloExpressionContext *ctx);
  any visitOC_PowerOfExpression(CypherParser::OC_PowerOfExpressionContext *ctx);
  any visitOC_UnaryAddOrSubtractExpression(
      CypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx);
  any visitOC_StringListNullOperatorExpression(
      CypherParser::OC_StringListNullOperatorExpressionContext *ctx);
  any visitOC_PropertyOrLabelsExpression(
      CypherParser::OC_PropertyOrLabelsExpressionContext *ctx);
  any visitOC_Atom(CypherParser::OC_AtomContext *ctx);
  any visitOC_FunctionInvocation(
      CypherParser::OC_FunctionInvocationContext *ctx);
  any visitOC_FunctionName(CypherParser::OC_FunctionNameContext *ctx);
  any visitOC_Namespace(CypherParser::OC_NamespaceContext *ctx);
  any visitOC_SymbolicName(CypherParser::OC_SymbolicNameContext *ctx);
  any visitOC_Variable(CypherParser::OC_VariableContext *ctx);
  any visitOC_SchemaName(CypherParser::OC_SchemaNameContext *ctx);
  any visitOC_PropertyLookup(CypherParser::OC_PropertyLookupContext *ctx);
  any visitOC_PropertyKeyName(CypherParser::OC_PropertyKeyNameContext *ctx);
  any visitOC_Literal(CypherParser::OC_LiteralContext *ctx);
  any visitOC_NumberLiteral(CypherParser::OC_NumberLiteralContext *ctx);
  any visitOC_IntegerLiteral(CypherParser::OC_IntegerLiteralContext *ctx);
  any visitOC_DoubleLiteral(CypherParser::OC_DoubleLiteralContext *ctx);
  any visitOC_BooleanLiteral(CypherParser::OC_BooleanLiteralContext *ctx);

  map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap;
  vector<jobject> *result;

private:
  Agent *agent;
};
