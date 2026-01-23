#include "CallClauseVisitor.h"
#include "classInfo.h"

extern vector<ClassInfo *> classNameList;

CallClauseVisitor::CallClauseVisitor(
    Agent *agent,
    map<string, vector<pair<InstanceInfo *, jobject>>> *nodeObjectMap) {
  this->agent = agent;
  this->nodeObjectMap = nodeObjectMap;
}

CallClauseVisitor::~CallClauseVisitor() {}

any CallClauseVisitor::visitOC_SinglePartQuery(
    CypherParser::OC_SinglePartQueryContext *ctx) {
  for (int j = 0; j < ctx->oC_ReadingClause().size(); j++) {
    ctx->oC_ReadingClause()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_MultiPartQuery(
    CypherParser::OC_MultiPartQueryContext *ctx) {
  if (ctx->oC_SinglePartQuery()) {
    return ctx->oC_SinglePartQuery()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_ReadingClause(
    CypherParser::OC_ReadingClauseContext *ctx) {
  if (ctx->oC_InQueryCall()) {
    return ctx->oC_InQueryCall()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_InQueryCall(
    CypherParser::OC_InQueryCallContext *ctx) {
  if (ctx->CALL()) {
    return ctx->oC_ExplicitProcedureInvocation()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_ExplicitProcedureInvocation(
    CypherParser::OC_ExplicitProcedureInvocationContext *ctx) {
  pair<string, string> caller;
  string methodDescriptor;
  if (ctx->oC_ProcedureName()) {
    AST_NODE_RETURN(ctx->oC_ProcedureName()->accept(this), caller,
                    pair<string, string>)
    caller.second = STRING::cypherLabelToClass(caller.second);
    printf("Caller and Callee %s %s\n", caller.first.c_str(),
           caller.second.c_str());
    jobject callerObj = nodeObjectMap->at(caller.first)[0].second;
    jvalue *args = new jvalue[ctx->oC_Expression().size()];
    vector<string> argClass;
    string methodSignature;
    string className =
        classNameList[nodeObjectMap->at(caller.first)[0].first->classTag - 1]
            ->name;

    for (int j = 0; j < ctx->oC_Expression().size(); j++) {
      string arg;
      AST_NODE_RETURN(ctx->oC_Expression()[j]->accept(this), arg, string)
      args[j].l = nodeObjectMap->at(arg)[0].second;
      argClass.push_back(
          classNameList[nodeObjectMap->at(arg)[0].first->classTag - 1]->name);
    }

    for (ClassInfo *cInfo : classNameList) {
      if (cInfo->name == className) {
        /*for(MethodInfo* mInfo : cInfo->methods) {
              if(mInfo->name == caller.second) {
                  int argCount = 0;
                  string str = mInfo->signature.substr(1,
           mInfo->signature.find(")") - 1); while(str.length() > 0) { int ind =
           str.find("L"); if(ind == 0) { str = str.substr(str.find(";") + 1,
           str.length() - str.find(";")); argCount++; } else { argCount = -1;
                          break;
                      }
                  }
                  if(argCount == argClass.size()) {
                      methodSignature = mInfo->signature;
                  }
              }
                }*/
      }
    }

    className = STRING::replacePeriodWithSlash(className);
    cout << className << " " << caller.second << " " << methodSignature
         << " \n";
    /*agent->invokeFunction(callerObj,
                          args,
                          className,
                          caller.second,
                          methodSignature);*/
  }
  return any();
}

any CallClauseVisitor::visitOC_Expression(
    CypherParser::OC_ExpressionContext *ctx) {
  if (ctx->oC_OrExpression()) {
    return ctx->oC_OrExpression()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_OrExpression(
    CypherParser::OC_OrExpressionContext *ctx) {
  for (int j = 0; j < ctx->oC_XorExpression().size(); j++) {
    return ctx->oC_XorExpression()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_XorExpression(
    CypherParser::OC_XorExpressionContext *ctx) {
  for (int j = 0; j < ctx->oC_AndExpression().size(); j++) {
    return ctx->oC_AndExpression()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_AndExpression(
    CypherParser::OC_AndExpressionContext *ctx) {
  for (int j = 0; j < ctx->oC_NotExpression().size(); j++) {
    return ctx->oC_NotExpression()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_NotExpression(
    CypherParser::OC_NotExpressionContext *ctx) {
  if (ctx->oC_ComparisonExpression()) {
    return ctx->oC_ComparisonExpression()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_ComparisonExpression(
    CypherParser::OC_ComparisonExpressionContext *ctx) {
  if (ctx->oC_AddOrSubtractExpression()) {
    return ctx->oC_AddOrSubtractExpression()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_AddOrSubtractExpression(
    CypherParser::OC_AddOrSubtractExpressionContext *ctx) {
  for (int j = 0; j < ctx->oC_MultiplyDivideModuloExpression().size(); j++) {
    return ctx->oC_MultiplyDivideModuloExpression()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_MultiplyDivideModuloExpression(
    CypherParser::OC_MultiplyDivideModuloExpressionContext *ctx) {
  for (int j = 0; j < ctx->oC_PowerOfExpression().size(); j++) {
    return ctx->oC_PowerOfExpression()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_PowerOfExpression(
    CypherParser::OC_PowerOfExpressionContext *ctx) {
  for (int j = 0; j < ctx->oC_UnaryAddOrSubtractExpression().size(); j++) {
    return ctx->oC_UnaryAddOrSubtractExpression()[j]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_UnaryAddOrSubtractExpression(
    CypherParser::OC_UnaryAddOrSubtractExpressionContext *ctx) {
  if (ctx->oC_StringListNullOperatorExpression()) {
    return ctx->oC_StringListNullOperatorExpression()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_StringListNullOperatorExpression(
    CypherParser::OC_StringListNullOperatorExpressionContext *ctx) {
  if (ctx->oC_PropertyOrLabelsExpression()) {
    return ctx->oC_PropertyOrLabelsExpression()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_PropertyOrLabelsExpression(
    CypherParser::OC_PropertyOrLabelsExpressionContext *ctx) {
  if (ctx->oC_Atom()) {
    return ctx->oC_Atom()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_ProcedureName(
    CypherParser::OC_ProcedureNameContext *ctx) {
  string node;
  string method;

  if (ctx->oC_Namespace() && ctx->oC_SymbolicName()) {
    AST_NODE_RETURN(ctx->oC_Namespace()->accept(this), node, string)
    AST_NODE_RETURN(ctx->oC_SymbolicName()->accept(this), method, string)
    return pair<string, string>{node, method};
  }
  return any();
}

any CallClauseVisitor::visitOC_Namespace(
    CypherParser::OC_NamespaceContext *ctx) {
  if (ctx->oC_SymbolicName().size() == 1) {
    return ctx->oC_SymbolicName()[0]->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_Atom(CypherParser::OC_AtomContext *ctx) {
  if (ctx->oC_Variable()) {
    return ctx->oC_Variable()->accept(this);
  } else if (ctx->oC_Literal()) {
    return ctx->oC_Literal()->accept(this);
  }
  return any();
}

any CallClauseVisitor::visitOC_SymbolicName(
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

any CallClauseVisitor::visitOC_Variable(CypherParser::OC_VariableContext *ctx) {
  if (ctx->oC_SymbolicName()) {
    return ctx->oC_SymbolicName()->accept(this);
  }
  return any();
}
