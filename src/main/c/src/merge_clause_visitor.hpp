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

class MergeClauseVisitor : public CypherBaseVisitor {

#define AST_NODE_RETURN(VISIT, ASSIGN, ...)                                    \
  {                                                                            \
    any result = VISIT;                                                        \
    if (result.has_value()) {                                                  \
      ASSIGN = any_cast<__VA_ARGS__>(result);                                  \
    }                                                                          \
  }

public:
  MergeClauseVisitor(
      Agent *agent,
      map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap);
  ~MergeClauseVisitor();
  any visitOC_SinglePartQuery(CypherParser::OC_SinglePartQueryContext *ctx);
  any visitOC_MultiPartQuery(CypherParser::OC_MultiPartQueryContext *ctx);
  any visitOC_UpdatingClause(CypherParser::OC_UpdatingClauseContext *ctx);
  any visitOC_PatternElement(CypherParser::OC_PatternElementContext *ctx);
  any visitOC_NodePattern(CypherParser::OC_NodePatternContext *ctx);
  any visitOC_Variable(CypherParser::OC_VariableContext *ctx);
  any visitOC_SymbolicName(CypherParser::OC_SymbolicNameContext *ctx);
  any visitOC_SchemaName(CypherParser::OC_SchemaNameContext *ctx);
  any visitOC_PatternElementChain(
      CypherParser::OC_PatternElementChainContext *ctx);
  any visitOC_RelationshipPattern(
      CypherParser::OC_RelationshipPatternContext *ctx);
  any visitOC_RelationshipDetail(
      CypherParser::OC_RelationshipDetailContext *ctx);
  any visitOC_RelationshipTypes(CypherParser::OC_RelationshipTypesContext *ctx);
  any visitOC_RelTypeName(CypherParser::OC_RelTypeNameContext *ctx);
  // any visitOC_Return(CypherParser::OC_ReturnContext *ctx);
  //  any visitOC_ProjectionItems(CypherParser::OC_ProjectionItemsContext *ctx);
  any visitOC_Atom(CypherParser::OC_AtomContext *ctx);
  map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap;

private:
  Agent *agent;
};
