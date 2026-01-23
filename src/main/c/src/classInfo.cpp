#include "classInfo.h"
#include "agent_util.h"
#include "methodInfo.h"

#include <iostream>

using namespace graph;
using namespace std;

ClassInfo::ClassInfo() { clear(); }

ClassInfo::~ClassInfo() { clear(); }

void ClassInfo::clear() {
  name = "";
  fields.erase(fields.begin(), fields.begin() + fields.size());
  methods.erase(methods.begin(), methods.begin() + methods.size());
  instances.erase(instances.begin(), instances.begin() + instances.size());
  instanceTags.clear();
  instanceCount = 0;
  tag = 0;
  writeToGraph = false;
  superClassTag = 0;
  isSorted = true;
}

MethodInfo *
ClassInfo::getMethodMatchingDescriptor(string &methodName,
                                       vector<string> &argDescriptors) {
  for (MethodInfo *mInfo : methods) {
    if (mInfo->name == methodName) {
      if (mInfo->areArgDescriptorSame(argDescriptors)) {
        return mInfo;
      }
    }
  }
  return NULL;
}

/**
 * @fn bool classContainsWritableInstances(const classInfo*)
 * @brief   Checks if any instances belonging to cInfo have
 *          writeToGraph set to true
 *
 * @param cinfo
 * @return  true if any instance of classInfo has writeToGraph set to true
 */
bool ClassInfo::containsWritableInstances() {
  bool writeToGraph = false;

  if (instances.size() == 0) {
    return writeToGraph;
  }

  for (int j = 0; j < instances.size(); j++) {
    writeToGraph |= instances[j]->writeToGraph;
  }
  return writeToGraph;
}

/**
 * @fn      void getInstanceInfoWithTag(long tag,
 *                                      InstanceInfo** iInfo)
 * @brief   Returns the InstanceInfo bearing the given tag
 *
 * @author  1sand0s
 * @param   tag         tag whose instance is to be found
 * @param   iInfo       populated with the instance bearing the queried tag if
 * found
 */
void ClassInfo::getInstanceInfoWithTag(long tag, InstanceInfo **iInfo) {
  int index = -1;

  /*Check if tag was found before retrieving corressponding instanceInfo*/
  if (UTIL::AGENT::searchTag(0, instances.size() - 1, tag, instanceTags,
                             &index)) {
    *iInfo = instances[index];
  }
}

/**
 * @fn void sortElementAscendingTag(classInfo*)
 * @brief   Sorts instanceTags of classInfo by ascending order
 *          of tags value . (NOTE : this does not sort the corressponding
 *          instanceInfos)
 *
 * @param cInfo The classInfo whose instanceTags are to be sorted
 */
void ClassInfo::sortElementAscendingTag() {
  /* If instanceTags are already sorted then return */
  if (isSorted) {
    return;
  }

  /* Sort instanceTags by ascending order*/
  vector<long> tempTags;
  tempTags.reserve(instanceTags.size());
  UTIL::AGENT::mergeSort(0, instanceTags.size() - 1, instanceTags, tempTags);
  isSorted = true;
}
