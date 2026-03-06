#include "cypher_expression.hpp"
#include "class_info.hpp"
#include "cypher_operators.hpp"
#include "jni_util.hpp"
#include "method_info.hpp"

#include <cmath>
#include <exception>
#include <jni.h>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace util::cypher;
using namespace util::cypher::operations;
using namespace util::cypher::native_function_expr;
using namespace graph;
using namespace std;

extern vector<ClassInfo *> classNameList;
extern map<string, ClassInfo *> className2Info;
extern Agent *agent;

/**************static variable definitions***************/
const map<Agent::MethodReturnType, string> LiteralExpr::mapToDescriptor = {
        {Agent::MethodReturnType::JBOOLEAN, "Z"},
        {Agent::MethodReturnType::JBYTE, "B"},
        {Agent::MethodReturnType::JSHORT, "S"},
        {Agent::MethodReturnType::JINT, "I"},
        {Agent::MethodReturnType::JLONG, "J"},
        {Agent::MethodReturnType::JFLOAT, "F"},
        {Agent::MethodReturnType::JDOUBLE, "D"},
        {Agent::MethodReturnType::JCHAR, "C"},
        {Agent::MethodReturnType::JSTRING, "Ljava/lang/String"},
        {Agent::MethodReturnType::JOBJECT, "Ljava/lang/Object"},
        {Agent::MethodReturnType::JNULL, "Ljava/lang/Object"}};

/*********************Begin Class LiteralExpr*************************/
LiteralExpr::LiteralExpr(string value, enum Agent::MethodReturnType type) {
    this->value = value;
    this->type = type;
    this->isResolved = false;
}
LiteralExpr::LiteralExpr(jvalue &value, enum Agent::MethodReturnType type) {
    this->resolvedValue = value;
    this->type = type;
    this->isResolved = true;
}
LiteralExpr::~LiteralExpr() {}
jvalue LiteralExpr::getValue() {
    if (isResolved) {
        return resolvedValue;
    } else {
        jvalue value;
        isResolved = true;
        switch (this->type) {
        case Agent::MethodReturnType::JBOOLEAN:
            value.z = (this->value == "TRUE" | this->value == "true") ? true
                                                                      : false;
            return value;
        case Agent::MethodReturnType::JBYTE:
            value.b = (jbyte)stoi(this->value);
            return value;
        case Agent::MethodReturnType::JSHORT:
            value.s = (jshort)stoi(this->value);
            return value;
        case Agent::MethodReturnType::JINT:
            value.i = (jint)stoi(this->value);
            return value;
        case Agent::MethodReturnType::JLONG:
            value.j = (jlong)stol(this->value);
            return value;
        case Agent::MethodReturnType::JFLOAT:
            value.f = (jfloat)stof(this->value);
            return value;
        case Agent::MethodReturnType::JDOUBLE:
            value.d = (jdouble)stod(this->value);
            return value;
        case Agent::MethodReturnType::JSTRING:
            value.l = (jobject)agent->createNewString(this->value);
            return value;
        }
        throw std::runtime_error(
                "Unsupported literal type in LiteralExpr::getValue");
    }
}
Expression *LiteralExpr::evaluate(
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    if (type == Agent::MethodReturnType::JOBJECT) {
        if (pattern->find(value) != pattern->end()) {
            jvalue newValue;
            newValue.l = pattern->at(value)[0].second;
            return new LiteralExpr(newValue, Agent::MethodReturnType::JOBJECT);
        }
    }
    return this;
}
/*********************End Class LiteralExpr*****************************/

/*****************Begin Class FunctionInvocationExpr********************/
FunctionInvocationExpr::FunctionInvocationExpr(string callerNode,
                                               string calledFunction,
                                               vector<LiteralExpr *> &args) {
    this->callerNode = callerNode;
    this->calledFunction = calledFunction;
    this->args = args;
}
FunctionInvocationExpr::FunctionInvocationExpr(string callerNode,
                                               string calledFunction) {
    this->callerNode = callerNode;
    this->calledFunction = calledFunction;
}
FunctionInvocationExpr::~FunctionInvocationExpr() {
    args.erase(args.begin(), args.begin() + args.size());
}
void FunctionInvocationExpr::addArg(LiteralExpr *litExpr) {
    args.push_back(litExpr);
}
Expression *FunctionInvocationExpr::evaluate(
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    jobject callerObject = pattern->at(callerNode)[0].second;
    string callerObjectClass =
            classNameList[pattern->at(callerNode)[0].first->classTag - 1]->name;
    vector<jvalue> argValues;
    vector<string> argDescriptors;

    for (LiteralExpr *litExpr : args) {
        string argClass = LiteralExpr::mapToDescriptor.at(litExpr->type);
        jvalue value;
        if (litExpr->type == Agent::MethodReturnType::JOBJECT) {
            value.l = pattern->at(litExpr->value)[0].second;
        } else {
            value = litExpr->getValue();
        }
        argDescriptors.push_back(argClass);
        argValues.push_back(value);
    }

    MethodInfo *invokedMethod = className2Info.at(callerObjectClass)
                                        ->getMethodMatchingDescriptor(
                                                calledFunction, argDescriptors);
    jvalue functionReturn;
    if (invokedMethod != NULL) {
        Agent::MethodReturnType returnType;
        agent->callInstanceMethod(callerObject, invokedMethod->name,
                                  invokedMethod->signature, argValues,
                                  functionReturn, returnType);
        return new LiteralExpr(functionReturn, returnType);
    }
    return new LiteralExpr(functionReturn, Agent::MethodReturnType::JNULL);
}
/*******************End Class FunctionInvocationExpr***********************/

/******************Begin Class PropertyLookupExpr**************************/
PropertyLookupExpr::PropertyLookupExpr(string &node, string property) {
    this->node = node;
    this->property = property;
}
PropertyLookupExpr::~PropertyLookupExpr() {}
Expression *PropertyLookupExpr::evaluate(
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    jobject callerObject = pattern->at(node)[0].second;
    string callerObjectClass =
            classNameList[pattern->at(node)[0].first->classTag - 1]->name;
    FieldInfo *fInfo = pattern->at(node)[0].first->getField(property);
    jvalue fieldValue;
    if (fInfo != NULL) {
        Agent::MethodReturnType fieldType =
                agent->getFieldType(fInfo->signature);
        if (fieldType == Agent::MethodReturnType::JSTRING) {
            fieldValue.l = agent->getInstanceObjectField(
                    callerObject, fInfo->name, fInfo->signature);
        } else {
            fieldValue = agent->getInstancePrimitiveField(
                    callerObject, fInfo->name, fInfo->signature);
        }
        return new LiteralExpr(fieldValue, fieldType);
    }
    return new LiteralExpr(fieldValue, Agent::MethodReturnType::JNULL);
}
/**********************End Class PropertyLookupExpr********************/

/******************Begin Class BinaryExpr******************************/
BinaryExpr::BinaryExpr(Expression *left, Expression *right,
                       enum BinaryExpr::BinaryOperators op) {
    this->left = left;
    this->right = right;
    this->op = op;
}
BinaryExpr::~BinaryExpr() {
    delete left;
    delete right;
}
Expression *BinaryExpr::evaluate(
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    LiteralExpr *leftExpr = (LiteralExpr *)left->evaluate(pattern);
    LiteralExpr *rightExpr = (LiteralExpr *)right->evaluate(pattern);
    return binary_operations::BIN_OP_FUNC_MAP.at(op)(leftExpr, rightExpr);
}
/********************End Class BinaryExpr******************************/

/*********************Begin Class UnaryExpr****************************/
UnaryExpr::UnaryExpr(Expression *expr, enum UnaryExpr::UnaryOperators op) {
    this->expr = expr;
    this->op = op;
}
UnaryExpr::~UnaryExpr() { delete expr; }
Expression *UnaryExpr::evaluate(
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    LiteralExpr *litExpr = (LiteralExpr *)expr->evaluate(pattern);
    return unary_operations::UN_OP_FUNC_MAP.at(op)(litExpr);
}
/*************************End Class UnaryExpr**************************/

COUNT::COUNT() : FunctionInvocationExpr("", "COUNT") { this->expr = NULL; }
COUNT::COUNT(Expression *expr) : FunctionInvocationExpr("", "COUNT") {
    this->expr = expr;
}
Expression *COUNT::evaluate(
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    if (LiteralExpr *litExpr = dynamic_cast<LiteralExpr *>(expr);
        litExpr != NULL) {
        return evaluate(litExpr, pattern);
    } else if (PropertyLookupExpr *propExpr =
                       dynamic_cast<PropertyLookupExpr *>(expr);
               propExpr != NULL) {
        return evaluate(propExpr, pattern);
    }
    return NULL;
}
LiteralExpr *COUNT::evaluate(
        LiteralExpr *litExpr,
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    if (litExpr->type == Agent::MethodReturnType::JOBJECT) {
        jvalue count{.j = (jlong)pattern->at(litExpr->value).size()};
        return new LiteralExpr(count, Agent::MethodReturnType::JLONG);
    }
    throw std::runtime_error("Unsupported operand type for COUNT");
}
LiteralExpr *COUNT::evaluate(
        PropertyLookupExpr *propExpr,
        const map<string, vector<pair<InstanceInfo *, jobject>>> *pattern) {
    jlong count = 0;
    throw std::runtime_error("Error : not implemented");
}
