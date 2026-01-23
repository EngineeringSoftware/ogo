
#include "cypher_operators.h"
#include "jni_util.h"

#include <cmath>
#include <jni.h>
#include <map>
#include <stdexcept>

using namespace util;
using namespace util::cypher;
using namespace std;

extern Agent *agent;

/******************************Map Binary Operators to Binary Operator
 * Implementation*************/
const std::map<BinaryExpr::BinaryOperators,
               operations::binary_operations::binOp>
    operations::binary_operations::BIN_OP_FUNC_MAP = {
        {BinaryExpr::BinaryOperators::ADD,
         &operations::binary_operations::arithmetic::ADD},
        {BinaryExpr::BinaryOperators::SUB,
         &operations::binary_operations::arithmetic::SUB},
        {BinaryExpr::BinaryOperators::MUL,
         &operations::binary_operations::arithmetic::MUL},
        {BinaryExpr::BinaryOperators::DIV,
         &operations::binary_operations::arithmetic::DIV},
        {BinaryExpr::BinaryOperators::MDIV,
         &operations::binary_operations::arithmetic::MDIV},
        {BinaryExpr::BinaryOperators::EXP,
         &operations::binary_operations::arithmetic::EXP},
        {BinaryExpr::BinaryOperators::EQ,
         &operations::binary_operations::comparison::EQ},
        {BinaryExpr::BinaryOperators::NEQ,
         &operations::binary_operations::comparison::NEQ},
        {BinaryExpr::BinaryOperators::LT,
         &operations::binary_operations::comparison::LT},
        {BinaryExpr::BinaryOperators::GT,
         &operations::binary_operations::comparison::GT},
        {BinaryExpr::BinaryOperators::LEQ,
         &operations::binary_operations::comparison::LEQ},
        {BinaryExpr::BinaryOperators::GEQ,
         &operations::binary_operations::comparison::GEQ},
        {BinaryExpr::BinaryOperators::AND,
         &operations::binary_operations::boolean::AND},
        {BinaryExpr::BinaryOperators::OR,
         &operations::binary_operations::boolean::OR},
        {BinaryExpr::BinaryOperators::XOR,
         &operations::binary_operations::boolean::XOR}};

/******************************Map Unary Operators to Unary Operator
 * Implementation*************/
const std::map<UnaryExpr::UnaryOperators, operations::unary_operations::unOp>
    operations::unary_operations::UN_OP_FUNC_MAP = {
        {UnaryExpr::UnaryOperators::IS_NOT_NULL,
         &operations::unary_operations::IS_NOT_NULL},
        {UnaryExpr::UnaryOperators::IS_NULL,
         &operations::unary_operations::IS_NULL},
        {UnaryExpr::UnaryOperators::NOT, &operations::unary_operations::NOT}};

static void
util::cypher::operations::binary_operations::getCastedBinaryOperands(
    const jvalue &leftValue, const enum util::Agent::MethodReturnType &leftType,
    const jvalue &rightValue,
    const enum util::Agent::MethodReturnType &rightType,
    jvalue &leftCastedValue, jvalue &rightCastedValue,
    enum util::Agent::MethodReturnType castedType) {
  if (!(Agent::isNumberType(leftType) && Agent::isNumberType(rightType))) {
    throw std::runtime_error(
        "Error : Binary Operand types must be a number literal");
  }
  // If either type is jfloat/jdouble then we cast to jdouble
  if (Agent::isFloatType(leftType) || Agent::isFloatType(rightType)) {
    leftCastedValue.d = Agent::getNumberTypeAsDouble(leftValue, leftType);
    rightCastedValue.d = Agent::getNumberTypeAsDouble(rightValue, rightType);
    castedType = Agent::MethodReturnType::JDOUBLE;
  } else { // cast to jlong
    leftCastedValue.j = Agent::getIntegerTypesAsLong(leftValue, leftType);
    rightCastedValue.j = Agent::getIntegerTypesAsLong(rightValue, rightType);
    castedType = Agent::MethodReturnType::JLONG;
  }
}

LiteralExpr *util::cypher::operations::binary_operations::arithmetic::ADD(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType;
  LiteralExpr *resultExpr = NULL;

  jvalue leftValue;
  jvalue rightValue;
  enum util::Agent::MethodReturnType castedType;
  try {
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
  } catch (std::runtime_error &e) {
    throw new IsNotNumberTypeError(BinaryExpr::BinaryOperators::ADD, left->type,
                                   right->type);
  }
  resultValueType = Agent::getPriorityType(left->type, right->type);
  if (castedType == util::Agent::MethodReturnType::JLONG) {
    resultValue.j = leftValue.j + rightValue.j;
    resultValue = Agent::castIntegerType<jlong>(resultValue.j, resultValueType);
  } else {
    resultValue.d = leftValue.d + rightValue.d;
    resultValue = Agent::castFloatType<jdouble>(resultValue.d, resultValueType);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::arithmetic::SUB(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType;
  LiteralExpr *resultExpr = NULL;

  jvalue leftValue;
  jvalue rightValue;
  enum util::Agent::MethodReturnType castedType;
  try {
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
  } catch (std::runtime_error &e) {
    throw new IsNotNumberTypeError(BinaryExpr::BinaryOperators::SUB, left->type,
                                   right->type);
  }
  resultValueType = Agent::getPriorityType(left->type, right->type);
  if (castedType == util::Agent::MethodReturnType::JLONG) {
    resultValue.j = leftValue.j - rightValue.j;
    resultValue = Agent::castIntegerType<jlong>(resultValue.j, resultValueType);
  } else {
    resultValue.d = leftValue.d - rightValue.d;
    resultValue = Agent::castFloatType<jdouble>(resultValue.d, resultValueType);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::arithmetic::MUL(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType;
  LiteralExpr *resultExpr = NULL;

  jvalue leftValue;
  jvalue rightValue;
  enum util::Agent::MethodReturnType castedType;
  try {
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
  } catch (std::runtime_error &e) {
    throw new IsNotNumberTypeError(BinaryExpr::BinaryOperators::MUL, left->type,
                                   right->type);
  }
  resultValueType = Agent::getPriorityType(left->type, right->type);
  if (castedType == util::Agent::MethodReturnType::JLONG) {
    resultValue.j = leftValue.j * rightValue.j;
    resultValue = Agent::castIntegerType<jlong>(resultValue.j, resultValueType);
  } else {
    resultValue.d = leftValue.d * rightValue.d;
    resultValue = Agent::castFloatType<jdouble>(resultValue.d, resultValueType);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::arithmetic::DIV(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType;
  LiteralExpr *resultExpr = NULL;

  jvalue leftValue;
  jvalue rightValue;
  enum util::Agent::MethodReturnType castedType;
  try {
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
  } catch (std::runtime_error &e) {
    throw new IsNotNumberTypeError(BinaryExpr::BinaryOperators::DIV, left->type,
                                   right->type);
  }
  resultValueType = Agent::getPriorityType(left->type, right->type);
  if (castedType == util::Agent::MethodReturnType::JLONG) {
    resultValue.j = leftValue.j / rightValue.j;
    resultValue = Agent::castIntegerType<jlong>(resultValue.j, resultValueType);
  } else {
    resultValue.d = leftValue.d / rightValue.d;
    resultValue = Agent::castFloatType<jdouble>(resultValue.d, resultValueType);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::arithmetic::MDIV(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType;
  LiteralExpr *resultExpr = NULL;

  jvalue leftValue;
  jvalue rightValue;
  enum util::Agent::MethodReturnType castedType;
  try {
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
  } catch (std::runtime_error &e) {
    throw new IsNotNumberTypeError(BinaryExpr::BinaryOperators::MDIV,
                                   left->type, right->type);
  }
  resultValueType = Agent::getPriorityType(left->type, right->type);
  if (castedType == util::Agent::MethodReturnType::JLONG) {
    resultValue.j = leftValue.j % rightValue.j;
    resultValue = Agent::castIntegerType<jlong>(resultValue.j, resultValueType);
  } else {
    resultValue.d = (jlong)leftValue.d % (jlong)rightValue.d;
    resultValue = Agent::castFloatType<jdouble>(resultValue.d, resultValueType);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::arithmetic::EXP(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType;
  LiteralExpr *resultExpr = NULL;

  jvalue leftValue;
  jvalue rightValue;
  enum util::Agent::MethodReturnType castedType;
  try {
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
  } catch (std::runtime_error &e) {
    throw new IsNotNumberTypeError(BinaryExpr::BinaryOperators::EXP, left->type,
                                   right->type);
  }
  resultValueType = Agent::getPriorityType(left->type, right->type);
  if (castedType == util::Agent::MethodReturnType::JLONG) {
    resultValue.j = pow(leftValue.j, rightValue.j);
    resultValue = Agent::castIntegerType<jlong>(resultValue.j, resultValueType);
  } else {
    resultValue.d = powf(leftValue.d, rightValue.d);
    resultValue = Agent::castFloatType<jdouble>(resultValue.d, resultValueType);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::comparison::EQ(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (Agent::isNumberType(left->type) && Agent::isNumberType(right->type)) {
    jvalue leftValue;
    jvalue rightValue;
    enum util::Agent::MethodReturnType castedType;
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
    resultValueType = Agent::getPriorityType(left->type, right->type);
    if (castedType == util::Agent::MethodReturnType::JLONG) {
      resultValue.z = leftValue.j == rightValue.j;
    } else {
      resultValue.z = leftValue.d == rightValue.d;
    }
  } else if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
             right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    resultValue.z = left->getValue().z == right->getValue().z;
  } else if (left->type == util::Agent::MethodReturnType::JSTRING &&
             right->type == util::Agent::MethodReturnType::JSTRING) {
    resultValue.z = agent->evaluateStringObject((jstring)left->getValue().l) ==
                    agent->evaluateStringObject((jstring)right->getValue().l);
  } else {
    resultValue.z = false;
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::comparison::NEQ(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (Agent::isNumberType(left->type) && Agent::isNumberType(right->type)) {
    jvalue leftValue;
    jvalue rightValue;
    enum util::Agent::MethodReturnType castedType;
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
    resultValueType = Agent::getPriorityType(left->type, right->type);
    if (castedType == util::Agent::MethodReturnType::JLONG) {
      resultValue.z = leftValue.j != rightValue.j;
    } else {
      resultValue.z = leftValue.d != rightValue.d;
    }
  } else if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
             right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    resultValue.z = left->getValue().z != right->getValue().z;
  } else if (left->type == util::Agent::MethodReturnType::JSTRING &&
             right->type == util::Agent::MethodReturnType::JSTRING) {
    resultValue.z = agent->evaluateStringObject((jstring)left->getValue().l) !=
                    agent->evaluateStringObject((jstring)right->getValue().l);
  } else {
    resultValue.z = true;
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::comparison::LT(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (Agent::isNumberType(left->type) && Agent::isNumberType(right->type)) {
    jvalue leftValue;
    jvalue rightValue;
    enum util::Agent::MethodReturnType castedType;
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
    resultValueType = Agent::getPriorityType(left->type, right->type);
    if (castedType == util::Agent::MethodReturnType::JLONG) {
      resultValue.z = leftValue.j < rightValue.j;
    } else {
      resultValue.z = leftValue.d < rightValue.d;
    }
  } else if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
             right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    if (left->getValue().z == right->getValue().z ||
        left->getValue().z == true) {
      resultValue.z = false;
    } else {
      resultValue.z = true;
    }
  } else if (left->type == util::Agent::MethodReturnType::JSTRING &&
             right->type == util::Agent::MethodReturnType::JSTRING) {
    resultValue.z = agent->evaluateStringObject((jstring)left->getValue().l) <
                    agent->evaluateStringObject((jstring)right->getValue().l);
  } else {
    resultValue.z = false;
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::comparison::GT(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (Agent::isNumberType(left->type) && Agent::isNumberType(right->type)) {
    jvalue leftValue;
    jvalue rightValue;
    enum util::Agent::MethodReturnType castedType;
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
    resultValueType = Agent::getPriorityType(left->type, right->type);
    if (castedType == util::Agent::MethodReturnType::JLONG) {
      resultValue.z = leftValue.j > rightValue.j;
    } else {
      resultValue.z = leftValue.d > rightValue.d;
    }
  } else if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
             right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    if (left->getValue().z == right->getValue().z ||
        left->getValue().z == false) {
      resultValue.z = false;
    } else {
      resultValue.z = true;
    }
  } else if (left->type == util::Agent::MethodReturnType::JSTRING &&
             right->type == util::Agent::MethodReturnType::JSTRING) {
    resultValue.z = agent->evaluateStringObject((jstring)left->getValue().l) >
                    agent->evaluateStringObject((jstring)right->getValue().l);
  } else {
    resultValue.z = false;
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::comparison::LEQ(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (Agent::isNumberType(left->type) && Agent::isNumberType(right->type)) {
    jvalue leftValue;
    jvalue rightValue;
    enum util::Agent::MethodReturnType castedType;
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
    resultValueType = Agent::getPriorityType(left->type, right->type);
    if (castedType == util::Agent::MethodReturnType::JLONG) {
      resultValue.z = leftValue.j <= rightValue.j;
    } else {
      resultValue.z = leftValue.d <= rightValue.d;
    }
  } else if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
             right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    if (left->getValue().z == right->getValue().z ||
        left->getValue().z == false) {
      resultValue.z = true;
    } else {
      resultValue.z = false;
    }
  } else if (left->type == util::Agent::MethodReturnType::JSTRING &&
             right->type == util::Agent::MethodReturnType::JSTRING) {
    resultValue.z = agent->evaluateStringObject((jstring)left->getValue().l) <=
                    agent->evaluateStringObject((jstring)right->getValue().l);
  } else {
    resultValue.z = false;
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *util::cypher::operations::binary_operations::comparison::GEQ(
    LiteralExpr *left, LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (Agent::isNumberType(left->type) && Agent::isNumberType(right->type)) {
    jvalue leftValue;
    jvalue rightValue;
    enum util::Agent::MethodReturnType castedType;
    util::cypher::operations::binary_operations::getCastedBinaryOperands(
        left->getValue(), left->type, right->getValue(), right->type, leftValue,
        rightValue, castedType);
    resultValueType = Agent::getPriorityType(left->type, right->type);
    if (castedType == util::Agent::MethodReturnType::JLONG) {
      resultValue.z = leftValue.j >= rightValue.j;
    } else {
      resultValue.z = leftValue.d >= rightValue.d;
    }
  } else if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
             right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    if (left->getValue().z == right->getValue().z ||
        left->getValue().z == true) {
      resultValue.z = true;
    } else {
      resultValue.z = false;
    }
  } else if (left->type == util::Agent::MethodReturnType::JSTRING &&
             right->type == util::Agent::MethodReturnType::JSTRING) {
    resultValue.z = agent->evaluateStringObject((jstring)left->getValue().l) >=
                    agent->evaluateStringObject((jstring)right->getValue().l);
  } else {
    resultValue.z = false;
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *
util::cypher::operations::binary_operations::boolean::AND(LiteralExpr *left,
                                                          LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
      right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    resultValue.z = left->getValue().z && right->getValue().z;
  } else {
    throw new BadOperandTypeError(
        BinaryExpr::BinaryOperators::AND, Agent::MethodReturnType::JBOOLEAN,
        Agent::MethodReturnType::JBOOLEAN, left->type, right->type);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *
util::cypher::operations::binary_operations::boolean::OR(LiteralExpr *left,
                                                         LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
      right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    resultValue.z = left->getValue().z || right->getValue().z;
  } else {
    throw new BadOperandTypeError(
        BinaryExpr::BinaryOperators::OR, Agent::MethodReturnType::JBOOLEAN,
        Agent::MethodReturnType::JBOOLEAN, left->type, right->type);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *
util::cypher::operations::binary_operations::boolean::XOR(LiteralExpr *left,
                                                          LiteralExpr *right) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (left->type == util::Agent::MethodReturnType::JBOOLEAN &&
      right->type == util::Agent::MethodReturnType::JBOOLEAN) {
    resultValue.z = left->getValue().z != right->getValue().z;
  } else {
    throw new BadOperandTypeError(
        BinaryExpr::BinaryOperators::XOR, Agent::MethodReturnType::JBOOLEAN,
        Agent::MethodReturnType::JBOOLEAN, left->type, right->type);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *
util::cypher::operations::unary_operations::IS_NULL(LiteralExpr *litExpr) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  resultValue.z = litExpr->type == util::Agent::MethodReturnType::JNULL;
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *
util::cypher::operations::unary_operations::IS_NOT_NULL(LiteralExpr *litExpr) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  resultValue.z = litExpr->type != util::Agent::MethodReturnType::JNULL;
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

LiteralExpr *
util::cypher::operations::unary_operations::NOT(LiteralExpr *litExpr) {
  jvalue resultValue;
  enum util::Agent::MethodReturnType resultValueType =
      util::Agent::MethodReturnType::JBOOLEAN;
  LiteralExpr *resultExpr = NULL;

  if (litExpr->type == util::Agent::MethodReturnType::JBOOLEAN) {
    resultValue.z = !(litExpr->getValue().z);
  } else {
    throw new BadOperandTypeError(UnaryExpr::UnaryOperators::NOT,
                                  Agent::MethodReturnType::JBOOLEAN,
                                  litExpr->type);
  }
  resultExpr = new LiteralExpr(resultValue, resultValueType);
  return resultExpr;
}

util::cypher::operations::BadOperandTypeError::BadOperandTypeError(
    const BinaryExpr::BinaryOperators &binOp,
    const Agent::MethodReturnType &expectedType1,
    const Agent::MethodReturnType &expectedType2,
    const Agent::MethodReturnType &foundType1,
    const Agent::MethodReturnType &foundType2) {
  sstream << "Error : " << binOp << " expects " << expectedType1 << " and "
          << expectedType2 << " but found " << foundType1 << " and "
          << foundType2 << endl;
}

util::cypher::operations::BadOperandTypeError::BadOperandTypeError(
    const UnaryExpr::UnaryOperators &unOp,
    const Agent::MethodReturnType &expectedType1,
    const Agent::MethodReturnType &foundType1) {
  sstream << "Error : " << unOp << " expects " << expectedType1 << " but found "
          << foundType1 << endl;
}

util::cypher::operations::BadOperandTypeError::BadOperandTypeError() {}
util::cypher::operations::BadOperandTypeError::~BadOperandTypeError() {}

const char *
util::cypher::operations::BadOperandTypeError::what() const noexcept {
  return sstream.str().c_str();
}

util::cypher::operations::IsNotNumberTypeError::IsNotNumberTypeError(
    const BinaryExpr::BinaryOperators &binOp,
    const Agent::MethodReturnType &foundType1,
    const Agent::MethodReturnType &foundType2) {
  sstream << "Error : " << binOp << " expects a number type operands but found "
          << foundType1 << " and " << foundType2 << endl;
}

util::cypher::operations::IsNotNumberTypeError::IsNotNumberTypeError(
    const UnaryExpr::UnaryOperators &unOp,
    const Agent::MethodReturnType &foundType1) {
  sstream << "Error : " << unOp << " expects a number type operand but found "
          << foundType1 << endl;
}

util::cypher::operations::IsNotNumberTypeError::~IsNotNumberTypeError() {}

const char *
util::cypher::operations::IsNotNumberTypeError::what() const noexcept {
  return sstream.str().c_str();
}
