#pragma once

#include "fieldInfo.h"
#include "instanceInfo.h"
#include "methodInfo.h"

#include <jni.h>
#include <string>
#include <vector>

using namespace util;
using namespace std;

namespace graph {

/**
 * @struct  ClassInfo
 * @brief   class for storing class/Class.java instance information
 *
 * @author  1sand0s
 */
class ClassInfo {
public:
  ClassInfo();

  ~ClassInfo();

  void clear();

  /**
   * @fn void sortElementAscendingTag(classInfo*)
   * @brief   Sorts instanceTags of classInfo by ascending order
   *          of tags value . (NOTE : this does not sort the corressponding
   *          instanceInfos)
   *
   * @param cInfo The classInfo whose instanceTags are to be sorted
   */
  void sortElementAscendingTag();

  bool containsWritableInstances() const;

  void getInstanceInfoWithTag(long tag, InstanceInfo **iInfo) const;

  /* Fully qualified name of the Class*/
  string name;

  string signature;

  /* All the fields declared in the Class
   * (Primitive and Instance types)
   *
   * Must be freed from the owning(this) classInfo
   * variable
   */
  vector<FieldInfo *> fields;

  vector<MethodInfo *> methods;

  MethodInfo *getMethodMatchingDescriptor(string &methodName,
                                          vector<string> &argDescriptors);

  /* Number of elements present in fields */
  int fieldCount;

  /* All instances of this Class */
  vector<InstanceInfo *> instances;

  /* Tags corresponding to the instances
   * of this Class.
   *
   * This array must be bijective w.r.t the
   * instance array
   *
   * Must be sorted in ascending order of tags
   * allows faster retrieval of instances using binary
   * search
   */
  vector<long> instanceTags;

  /* Number of elements in instances or instanceTags */
  int instanceCount;

  /* Tag of the class */
  long tag;

  /* All instances of this Class and its Class.java instance
   * will definitely be written to CSV if
   * this flag is true
   *
   * Can be set by user using WhiteList
   */
  bool writeToGraph;

  /* Shallow Copy of the class's superclass if any, NULL otherwise
   */
  long superClassTag;
  std::vector<long> implementedInterfaceTags;

  /* If this flag is true then the instanceTags of this class
   * has been sorted in ascending order and its instances
   * can safetly be retrieved using getInstanceInfoWithTag
   */
  bool isSorted;

  /* The class object */
  jclass klass;
};
} // namespace graph
