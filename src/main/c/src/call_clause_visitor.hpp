#pragma once

#include "CypherBaseVisitor.h"
#include "instance_info.hpp"
#include "jni_util.hpp"
#include "ogo_constants.hpp"
#include "string_utils.hpp"

#include <string>
#include <vector>

using namespace std;
using namespace util;
using namespace graph;

class CallClauseVisitor : public CypherBaseVisitor {

#define AST_NODE_RETURN(VISIT, ASSIGN, ...)                                    \
  {                                                                            \
    any result = VISIT;                                                        \
    if (result.has_value()) {                                                  \
      ASSIGN = any_cast<__VA_ARGS__>(result);                                  \
    }                                                                          \
  }

public:
  CallClauseVisitor(
      Agent *agent,
      map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap);
  ~CallClauseVisitor();

  any visitOC_SinglePartQuery(CypherParser::OC_SinglePartQueryContext *ctx);
  any visitOC_MultiPartQuery(CypherParser::OC_MultiPartQueryContext *ctx);
  any visitOC_ReadingClause(CypherParser::OC_ReadingClauseContext *ctx);
  any visitOC_ExplicitProcedureInvocation(
      CypherParser::OC_ExplicitProcedureInvocationContext *ctx);
  any visitOC_InQueryCall(CypherParser::OC_InQueryCallContext *ctx);
  any visitOC_ProcedureName(CypherParser::OC_ProcedureNameContext *ctx);
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
  any visitOC_Namespace(CypherParser::OC_NamespaceContext *ctx);
  any visitOC_SymbolicName(CypherParser::OC_SymbolicNameContext *ctx);
  any visitOC_Variable(CypherParser::OC_VariableContext *ctx);
  map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap;

private:
  Agent *agent;
};
