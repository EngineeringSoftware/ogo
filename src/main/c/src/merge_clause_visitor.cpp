#include "merge_clause_visitor.hpp"
#include "class_info.hpp"
#include "cypher_expression.hpp"

using namespace std;
using namespace util;
using namespace graph;

extern vector<ClassInfo *> classNameList;

MergeClauseVisitor::MergeClauseVisitor(
        Agent *agent,
        map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap) {
    this->agent = agent;
    this->nodeObjectMap = nodeObjectMap;
}

MergeClauseVisitor::~MergeClauseVisitor() {}

any MergeClauseVisitor::visitOC_SinglePartQuery(
        CypherParser::OC_SinglePartQueryContext *ctx) {
    for (int j = 0; j < ctx->oC_UpdatingClause().size(); j++) {
        ctx->oC_UpdatingClause()[j]->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_MultiPartQuery(
        CypherParser::OC_MultiPartQueryContext *ctx) {
    for (int j = 0; j < ctx->oC_UpdatingClause().size(); j++) {
        ctx->oC_UpdatingClause()[j]->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_UpdatingClause(
        CypherParser::OC_UpdatingClauseContext *ctx) {
    if (ctx->oC_Merge()) {
        ctx->oC_Merge()->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_PatternElement(
        CypherParser::OC_PatternElementContext *ctx) {
    cypher::Expression *node1;
    cypher::Expression *node2;
    if (ctx->oC_NodePattern()) {
        AST_NODE_RETURN(ctx->oC_NodePattern()->accept(this), node1,
                        cypher::Expression *)
        for (int j = 0; j < ctx->oC_PatternElementChain().size(); j++) {
            pair<cypher::Expression *, pair<cypher::Expression *, int>>
                    relation;

            AST_NODE_RETURN(
                    ctx->oC_PatternElementChain()[j]->accept(this), relation,
                    pair<cypher::Expression *, pair<cypher::Expression *, int>>)
            node2 = relation.first;
            string node1Eval = agent->evaluateStringObject(
                    (jstring)((cypher::LiteralExpr *)node1)->getValue().l);
            string node2Eval = agent->evaluateStringObject(
                    (jstring)((cypher::LiteralExpr *)node2)->getValue().l);
            jobject object1 = nodeObjectMap->at(node1Eval)[0].second;
            jobject object2 = nodeObjectMap->at(node2Eval)[0].second;
            string object1ClassName =
                    classNameList[nodeObjectMap->at(node1Eval)[0]
                                          .first->classTag -
                                  1]
                            ->name;
            string object2ClassName =
                    classNameList[nodeObjectMap->at(node2Eval)[0]
                                          .first->classTag -
                                  1]
                            ->name;

            string fieldEval = agent->evaluateStringObject(
                    (jstring)((cypher::LiteralExpr *)relation.second.first)
                            ->getValue()
                            .l);
            if (relation.second.second == 1) {
                agent->setObjectReferenceField(object1, object2, fieldEval,
                                               object2ClassName);
            } else {
                agent->setObjectReferenceField(object2, object1, fieldEval,
                                               object1ClassName);
            }
            node1 = node2;
        }
    }
    return any();
}

any MergeClauseVisitor::visitOC_NodePattern(
        CypherParser::OC_NodePatternContext *ctx) {
    cypher::Expression *nodeVariable;

    /*If the pattern is referenced by any variable then store it*/
    if (ctx->oC_Variable()) {
        AST_NODE_RETURN(ctx->oC_Variable()->accept(this), nodeVariable,
                        cypher::Expression *)
    }
    return nodeVariable;
}

any MergeClauseVisitor::visitOC_PatternElementChain(
        CypherParser::OC_PatternElementChainContext *ctx) {
    cypher::Expression *node2;
    pair<cypher::Expression *, int> relation;
    AST_NODE_RETURN(ctx->oC_NodePattern()->accept(this), node2,
                    cypher::Expression *)
    AST_NODE_RETURN(ctx->oC_RelationshipPattern()->accept(this), relation,
                    pair<cypher::Expression *, int>)
    pair<cypher::Expression *, pair<cypher::Expression *, int>>
            patternElementChain = {node2, relation};
    return patternElementChain;
}

any MergeClauseVisitor::visitOC_RelationshipPattern(
        CypherParser::OC_RelationshipPatternContext *ctx) {
    cypher::Expression *relName = NULL;
    int referrer = 1;
    if (ctx->oC_RelationshipDetail()) {
        AST_NODE_RETURN(ctx->oC_RelationshipDetail()->accept(this), relName,
                        cypher::Expression *)
    }

    if (ctx->oC_LeftArrowHead()) {
        referrer = 2;
    }
    return pair<cypher::Expression *, int>{relName, referrer};
}

any MergeClauseVisitor::visitOC_RelationshipDetail(
        CypherParser::OC_RelationshipDetailContext *ctx) {
    string relName;

    if (ctx->oC_RelationshipTypes()) {
        AST_NODE_RETURN(ctx->oC_RelationshipTypes()->accept(this), relName,
                        string)
    }

    return relName;
}

any MergeClauseVisitor::visitOC_RelationshipTypes(
        CypherParser::OC_RelationshipTypesContext *ctx) {
    if (ctx->oC_RelTypeName().size() > 0) {
        // Only one relationship type allowed per edge
        return ctx->oC_RelTypeName()[0]->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_RelTypeName(
        CypherParser::OC_RelTypeNameContext *ctx) {
    if (ctx->oC_SchemaName()) {
        return ctx->oC_SchemaName()->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_SchemaName(
        CypherParser::OC_SchemaNameContext *ctx) {
    // Only SymbolicName allowed for matching patterns
    if (ctx->oC_SymbolicName()) {
        return ctx->oC_SymbolicName()->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_Variable(
        CypherParser::OC_VariableContext *ctx) {
    if (ctx->oC_SymbolicName()) {
        return ctx->oC_SymbolicName()->accept(this);
    }
    return any();
}

any MergeClauseVisitor::visitOC_SymbolicName(
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

any MergeClauseVisitor::visitOC_Atom(CypherParser::OC_AtomContext *ctx) {
    if (ctx->oC_Variable()) {
        return ctx->oC_Variable()->accept(this);
    }

    return any();
}
