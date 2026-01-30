#pragma once

#include "jni_util.h"

#include <jni.h>
#include <string>
#include <vector>

using namespace std;
using namespace util;

namespace graph {

/**
 * @class  FieldInfo
 * @brief  class for storing primitive field information
 *
 * @author 1sand0s
 */
class FieldInfo {
public:
  FieldInfo();

  ~FieldInfo();

  void clear();

  /* Writes this FieldInfo to string */
  string toString(Agent *agent) const;

  /* Field name as it appears in the declaration
   * inside the field declaring class
   *
   * Always a shallow copy unless the field is owned
   * by the declaring Class's Class.java instance (classInfo)
   *
   * Must only be freed from the owning classInfo variable
   */
  string name;

  /* Field signature as it appears in the declaration
   * inside the field declaring class
   *
   * Always a shallow copy unless the field is owned
   * by the declaring Class's Class.java instance (classInfo)
   *
   * Must only be freed from the owning classInfo variable
   */
  string signature;

  /* Field modifier as it appears in the declaration
   * inside the field declaring class
   */
  int modifier;

  /* Value of the field (jvmti Primitive or
   * java.lang.String)
   *
   * Only valid if the fieldInfo is owned by
   * instanceInfo variable.
   *
   * Must be freed from the owning instanceInfo variable
   */
  vector<jvalue> value;

  /* Type of value contained inside the field
   * using jvmtiExtendedFieldType to handle
   * embedding of boxed primitive types and
   * java.lang.String instances
   */
  Agent::MethodReturnType type;

  /* Index of this field in its declaring class (can be used to
   * query its information from declaring class's class.java
   * instance)
   */
  int fieldIndex;

  /* Tag of the field (Tags are relevant only for instances),
   * check if they can be removed from fields
   */
  long tag;

  /* The kind of reference from the owning instance to this
   * field, can only be JVMTI_HEAP_REFERENCE_FIELD or
   * JVMTI_HEAP_REFERENCE_STATIC_FIELD
   */
  int referenceKind;
};
} // namespace graph
