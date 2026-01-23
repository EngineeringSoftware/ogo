#pragma once

#include "jni_util.h"

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "jni.h"

using namespace std;

namespace graph {
/**
 * @class  MethodInfo
 * @brief  class for storing method information
 *
 * @author 1sand0s
 */
class MethodInfo {
public:
    MethodInfo();
    ~MethodInfo();

    void clear();
    void getArgDescriptors(vector<string>& methodArgDescriptors);
    bool areArgDescriptorSame(vector<string>& givenDescriptors);

    /* Method name
       */
    string name;

    /* Method signature
     */
    string signature;

    /* Method modifiers
     */
    int modifier;

    /* Is Constructor
     */
    bool isConstructor;

    /* MethodId
     */
    jmethodID methodID;

    /* Num Arguments
     */
    int nArgs;

    /* Descriptors of this method arguments */
    vector<string> argDescriptors;
};
}
