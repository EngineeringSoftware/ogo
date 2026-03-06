#pragma once

#include "agent_util.hpp"
#include "instance_info.hpp"

#include <jni.h>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace util;
using namespace graph;

namespace util::cypher {
class Expression {
public:
  virtual ~Expression() = 0;
  virtual Expression *evaluate(
      const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) = 0;
};

class LiteralExpr : public Expression {
public:
  LiteralExpr(string value, enum Agent::MethodReturnType type);
  LiteralExpr(jvalue &jvalue, enum Agent::MethodReturnType type);
  ~LiteralExpr();

  Expression *
  evaluate(const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);
  jvalue getValue();

  string value;
  jvalue resolvedValue;
  bool isResolved;
  enum Agent::MethodReturnType type;
  static const map<Agent::MethodReturnType, string> mapToDescriptor;
};

class UnaryExpr : public Expression {
public:
  enum UnaryOperators {
    // NULL check operators
    IS_NULL,
    IS_NOT_NULL,

    // Boolean Operators
    NOT
  };
  typedef LiteralExpr *(*unaryOp)(LiteralExpr *);

  UnaryExpr(Expression *exp, enum UnaryExpr::UnaryOperators op);
  ~UnaryExpr();

  Expression *
  evaluate(const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);

  Expression *expr;
  enum UnaryExpr::UnaryOperators op;
};

class FunctionInvocationExpr : public Expression {
public:
  FunctionInvocationExpr(string callerNode, string calledFunction,
                         vector<LiteralExpr *> &args);
  FunctionInvocationExpr(string callerNode, string calledFunction);
  ~FunctionInvocationExpr();

  virtual Expression *
  evaluate(const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);
  void addArg(LiteralExpr *litExpr);

  string callerNode;
  string calledFunction;
  vector<LiteralExpr *> args;
};

class PropertyLookupExpr : public Expression {
public:
  PropertyLookupExpr(string &node, string property);
  ~PropertyLookupExpr();

  Expression *
  evaluate(const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);

  string node;
  string property;
};

class BinaryExpr : public Expression {
public:
  enum BinaryOperators {
    // Arithemtic Operators
    ADD,
    SUB,
    MUL,
    DIV,
    MDIV,
    EXP,

    // Comparison Operators
    EQ,
    NEQ,
    LT,
    GT,
    LEQ,
    GEQ,

    // Boolean Operators
    AND,
    OR,
    XOR
  };

  BinaryExpr(Expression *left, Expression *right,
             enum BinaryExpr::BinaryOperators op);
  ~BinaryExpr();

  Expression *
  evaluate(const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);

  Expression *left;
  Expression *right;
  enum BinaryExpr::BinaryOperators op;
};

namespace native_function_expr {
class COUNT : public FunctionInvocationExpr {
public:
  COUNT();
  COUNT(Expression *);

  Expression *
  evaluate(const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);

private:
  LiteralExpr *
  evaluate(LiteralExpr *litExpr,
           const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);
  LiteralExpr *
  evaluate(PropertyLookupExpr *propExpr,
           const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern);

public:
  Expression *expr;
};
} // namespace native_function_expr
} // namespace util::cypher
