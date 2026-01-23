#pragma once

#include "CypherBaseVisitor.h"
#include "ogoConstants.h"
#include "jni_util.h"
#include "string_utils.h"
#include "classInfo.h"
#include "instanceInfo.h"

#include <vector>
#include <utility>

using namespace std;
using namespace graph;
using namespace util;

class MatchClauseVisitor : public CypherBaseVisitor {

#define AST_NODE_RETURN(VISIT, ASSIGN, ...)		   \
  {							   \
    any result = VISIT;				   \
    if(result.has_value()) {				   \
      ASSIGN = any_cast<__VA_ARGS__>(result);	   \
    }							   \
  }

public :
    MatchClauseVisitor(Agent* agent,
                       map<string, vector<pair<InstanceInfo*, jobject>>> *nodeObjectMap);
    ~MatchClauseVisitor();

    any visitOC_SinglePartQuery(CypherParser::OC_SinglePartQueryContext *ctx);
    any visitOC_MultiPartQuery(CypherParser::OC_MultiPartQueryContext *ctx);
    any visitOC_ReadingClause(CypherParser::OC_ReadingClauseContext *ctx);
    any visitOC_PatternElement(CypherParser::OC_PatternElementContext *ctx);
    any visitOC_NodePattern(CypherParser::OC_NodePatternContext *ctx);
    any visitOC_Variable(CypherParser::OC_VariableContext *ctx);
    any visitOC_SymbolicName(CypherParser::OC_SymbolicNameContext *ctx);
    any visitOC_Properties(CypherParser::OC_PropertiesContext *ctx);
    any visitOC_MapLiteral(CypherParser::OC_MapLiteralContext *ctx);
    any visitOC_PropertyKeyName(CypherParser::OC_PropertyKeyNameContext *ctx);
    any visitOC_Expression(CypherParser::OC_ExpressionContext *ctx);
    any visitOC_OrExpression(CypherParser::OC_OrExpressionContext *ctx);
    any visitOC_XorExpression(CypherParser::OC_XorExpressionContext *ctx);
    any visitOC_AndExpression(CypherParser::OC_AndExpressionContext *ctx);
    any visitOC_NotExpression(CypherParser::OC_NotExpressionContext *ctx);
    any visitOC_ComparisonExpression(CypherParser::OC_ComparisonExpressionContext *ctx);
    any visitOC_AddOrSubtractExpression(CypherParser::OC_AddOrSubtractExpressionContext *ctx);
    any visitOC_MultiplyDivideModuloExpression(CypherParser::OC_MultiplyDivideModuloExpressionContext *ctx);
    any visitOC_PowerOfExpression(CypherParser::OC_PowerOfExpressionContext *ctx);
    any visitOC_UnaryAddOrSubtractExpression(CypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx);
    any visitOC_StringListNullOperatorExpression(CypherParser::OC_StringListNullOperatorExpressionContext *ctx);
    any visitOC_PropertyOrLabelsExpression(CypherParser::OC_PropertyOrLabelsExpressionContext *ctx);
    any visitOC_Atom(CypherParser::OC_AtomContext *ctx);
    any visitOC_Literal(CypherParser::OC_LiteralContext *ctx);
    any visitOC_NumberLiteral(CypherParser::OC_NumberLiteralContext *ctx);
    any visitOC_DoubleLiteral(CypherParser::OC_DoubleLiteralContext *ctx);
    any visitOC_IntegerLiteral(CypherParser::OC_IntegerLiteralContext *ctx);
    any visitOC_BooleanLiteral(CypherParser::OC_BooleanLiteralContext *ctx);
    map<string, vector<pair<InstanceInfo*, jobject>>> *nodeObjectMap;
private:
    Agent* agent;
};
