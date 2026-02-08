#pragma once

#include <string>

using namespace std;

namespace graph {

/**
 * @class  ReferenceInfo
 * @brief  class for storing child object/instance information
 *

 */
class ReferenceInfo {
public:
  ReferenceInfo();
  ReferenceInfo(int referenceKind, long referrerTag, long referrerClassTag,
                bool writeToGraph);
  ~ReferenceInfo();
  void clear();

  /* Writes this ReferenceInfo to string */
  string toString() const;

  /* Defines the kind of reference between the referrer and
   * the referee instance
   * see
   * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#jvmtiHeapReferenceCallback
   * for types of reference kinds
   */
  int referenceKind;

  /* Tag of the referee instance */
  long referrerTag;

  /* Tag of the Class of the referee instance */
  long referrerClassTag;

  /* This corresponds to the field name of this reference instance
   * which shows up as the relation between the referrer and referee
   * in the node graph in Neo4J
   */
  string referrerRelation;

  /* If true then the referee relation is written to graph */
  bool writeToGraph;

  /* Valid only if the reference kind is of type
   * JVMTI_HEAP_REFERENCE_FIELD or JVMTI_HEAP_REFERENCE_STATIC_FIELD
   *
   * Can be used to index into the instance's Class.Class.java instance to
   * obtain information of the instance's declaration
   *
   * Used for identifying name of String and Boxed type instance fields to
   * embed them as properties in their owning instances
   */
  int fieldIndex;
};
} // namespace graph
