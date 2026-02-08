#include "field_info.hpp"
#include "jni.h"
#include "jni_util.hpp"

#include <sstream>
#include <string>
#include <vector>

using namespace graph;
using namespace util;
using namespace std;

FieldInfo::FieldInfo() { clear(); }

FieldInfo::~FieldInfo() { clear(); }

void FieldInfo::clear() {
  name = "value";
  signature = "";
  modifier = 0;
  fieldIndex = -1;
  tag = 0;
  type = Agent::MethodReturnType::JNULL;
}

string FieldInfo::toString(Agent *agent) const {
  if (type == Agent::MethodReturnType::JNULL) {
    return "";
  }
  stringstream sstream;
  string valueString;
  switch (type) {
#define ENTRY(a, b, c, d)                                                      \
  case Agent::MethodReturnType::c:                                             \
    valueString = std::to_string(value[0].d);                                  \
    break;
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
  case Agent::MethodReturnType::JSTRING: {
    for (int j = 0; j < this->value.size(); j++) {
      valueString += "x" + std::to_string((int)this->value[j].b);
    }
  } break;
#define ENTRY(a, b, c, d)                                                      \
  case Agent::MethodReturnType::c##_ARRAY: {                                   \
    for (int j = 0; j < this->value.size(); j++) {                             \
      valueString += std::to_string(this->value[j].d);                         \
      if (j < this->value.size() - 1) {                                        \
        valueString += ",";                                                    \
      }                                                                        \
    }                                                                          \
  } break;
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
    /*
case Agent::MethodReturnType::JSTRING_ARRAY: {
    vector<string> elements;
    agent->getStringArrayElements((jobjectArray) this->value.l, elements);
    sstream << this->name << ",JSTRING_ARRAY";
    for(string element : elements) {
        sstream << ",";
        for(int j = 0; j < element.size(); j++) {
            sstream << "x" << ((int) element.c_str()[j]);
        }
    }
            }
            break;*/
  }
  if (!name.empty() && !valueString.empty()) {
    sstream << name << "," << Agent::fieldTypeToString(type) << ","
            << valueString;
  } else {
    sstream << "";
  }
  return sstream.str();
}
