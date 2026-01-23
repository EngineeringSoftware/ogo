#pragma once

#include "cypher_expression.h"
#include "jni_util.h"

#include <exception>
#include <map>
#include <sstream>
#include <stdexcept>

using namespace util;

namespace util::cypher::operations {

namespace binary_operations {//Open binary_operations
static void getCastedBinaryOperands(const jvalue &leftValue,
                                    const enum Agent::MethodReturnType &leftType,
                                    const jvalue &rightValue,
                                    const enum Agent::MethodReturnType &rightType,
                                    jvalue &leftCastedValue,
                                    jvalue &rightCastedValue,
                                    enum Agent::MethodReturnType castedType);
namespace arithmetic {
LiteralExpr* ADD(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* SUB(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* MUL(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* DIV(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* MDIV(LiteralExpr* left,
                  LiteralExpr* right);
LiteralExpr* EXP(LiteralExpr* left,
                 LiteralExpr* right);
}
namespace comparison {
LiteralExpr* EQ(LiteralExpr* left,
                LiteralExpr* right);
LiteralExpr* NEQ(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* LT(LiteralExpr* left,
                LiteralExpr* right);
LiteralExpr* GT(LiteralExpr* left,
                LiteralExpr* right);
LiteralExpr* LEQ(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* GEQ(LiteralExpr* left,
                 LiteralExpr* right);
}
namespace boolean {
LiteralExpr* AND(LiteralExpr* left,
                 LiteralExpr* right);
LiteralExpr* OR(LiteralExpr* left,
                LiteralExpr* right);
LiteralExpr* XOR(LiteralExpr* left,
                 LiteralExpr* right);
}

typedef LiteralExpr* (*binOp) (LiteralExpr*, LiteralExpr*);
extern const std::map<BinaryExpr::BinaryOperators, binOp> BIN_OP_FUNC_MAP;
}//close binary_operations

namespace unary_operations {
LiteralExpr* IS_NULL(LiteralExpr* litExpr);
LiteralExpr* IS_NOT_NULL(LiteralExpr* litExpr);
LiteralExpr* NOT(LiteralExpr* litExpr);

typedef LiteralExpr* (*unOp) (LiteralExpr*);
extern const std::map<UnaryExpr::UnaryOperators, unOp> UN_OP_FUNC_MAP;
}

//Error class for type mismatch in expression operations
class BadOperandTypeError : public std::exception {
public:
    BadOperandTypeError(const BinaryExpr::BinaryOperators &binOp,
                        const Agent::MethodReturnType &expectedType1,
                        const Agent::MethodReturnType &expectedType2,
                        const Agent::MethodReturnType &foundType1,
                        const Agent::MethodReturnType &foundType2);
    BadOperandTypeError(const UnaryExpr::UnaryOperators &unOp,
                        const Agent::MethodReturnType &expectedType1,
                        const Agent::MethodReturnType &foundType1);
    BadOperandTypeError();
    ~BadOperandTypeError();
    virtual const char* what() const noexcept;
protected:
    stringstream sstream;
};

//Error class for enforcing number type literals in expression operations
class IsNotNumberTypeError : public BadOperandTypeError {
public:
    IsNotNumberTypeError(const BinaryExpr::BinaryOperators &binOp,
                         const Agent::MethodReturnType &foundType1,
                         const Agent::MethodReturnType &foundType2);
    IsNotNumberTypeError(const UnaryExpr::UnaryOperators &unOp,
                         const Agent::MethodReturnType &foundType1);
    ~IsNotNumberTypeError();
    virtual const char* what() const noexcept;
};
}
