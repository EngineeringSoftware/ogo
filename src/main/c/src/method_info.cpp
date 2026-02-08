#include "method_info.hpp"

#include <vector>

using namespace graph;
using namespace std;

MethodInfo::MethodInfo() { clear(); }

MethodInfo::~MethodInfo() { clear(); }

void MethodInfo::clear() {
  modifier = 0;
  name = "";
  signature = "";
  nArgs = -1;
  isConstructor = false;
}

bool MethodInfo::areArgDescriptorSame(vector<string> &givenDescriptors) {
  getArgDescriptors(argDescriptors);

  if (givenDescriptors.size() != argDescriptors.size()) {
    return false;
  }

  bool isMatch = true;
  for (int j = 0; j < givenDescriptors.size() && isMatch; j++) {
    cout << argDescriptors[j] << "\n";
    if (givenDescriptors[j] == "J") {
      isMatch &= (argDescriptors[j] == "I" || argDescriptors[j] == "J" ||
                  argDescriptors[j] == "B" || argDescriptors[j] == "S");
    } else if (givenDescriptors[j] == "D") {
      isMatch &= (argDescriptors[j] == "F" || argDescriptors[j] == "D");
    } else if (givenDescriptors[j][0] == 'L') {
      isMatch &= (argDescriptors[j] == givenDescriptors[j] ||
                  argDescriptors[j] == "Ljava/lang/Object;");
    } else {
      isMatch &= givenDescriptors[j] == argDescriptors[j];
    }
  }
  return isMatch;
}

void MethodInfo::getArgDescriptors(vector<string> &methodArgDescriptors) const {
  if (!argDescriptors.empty()) {
    methodArgDescriptors = argDescriptors;
    return;
  }
  string signature = this->signature.substr(1, this->signature.find(")") - 1);
  for (size_t j = 0; j < signature.size(); j++) {
    switch (signature[j]) {
    case 'Z':
      methodArgDescriptors.emplace_back("Z");
      break;
    case 'B':
      methodArgDescriptors.emplace_back("B");
      break;
    case 'S':
      methodArgDescriptors.emplace_back("S");
      break;
    case 'C':
      methodArgDescriptors.emplace_back("C");
      break;
    case 'I':
      methodArgDescriptors.emplace_back("I");
      break;
    case 'J':
      methodArgDescriptors.emplace_back("J");
      break;
    case 'F':
      methodArgDescriptors.emplace_back("F");
      break;
    case 'D':
      methodArgDescriptors.emplace_back("D");
      break;
    case 'L':
      methodArgDescriptors.push_back(
          signature.substr(j, signature.find(';', j) - j + 1));
      j = signature.find(';', j);
      break;
    default:
      return;
    }
  }
}
