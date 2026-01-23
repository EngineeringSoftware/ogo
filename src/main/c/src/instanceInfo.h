#pragma once

#include "fieldInfo.h"
#include "jni_util.h"
#include "referenceInfo.h"

#include <string>
#include <fstream>

using namespace util;
using namespace std;

namespace graph {

  /**
   * @class   InstanceInfo
   * @brief   class for storing java class instance information
   *
   * @author  1sand0s
   */
  class InstanceInfo {
  public:

    InstanceInfo();

    ~InstanceInfo();

    void clear();

    /**
     * @fn      void writeInstanceInfoToCsv(FILE*, instanceInfo*)
     * @brief   Write struct instanceInfo to csv file
     *
     * @author  1sand0s
     * @param   f           Pointer to the file to write to
     * @param   info        Information of the instance to write
     */
    void
    writeToCsv(ofstream& f,
	       string className);

    /**
     * @fn      void writeInstancePropertiesToCsv(instanceInfo*)
     * @brief   Write struct instanceInfo properties (primitive fields) to csv file
     *
     * @author  1sand0s
     * @param   info        Information of the instance whose primitive fields are to be written
     */
    void writePropertiesToCsv(Agent* agent);

    /**
     * @fn      void writeInstanceRelationsToCsv(instanceInfo*)
     * @brief   Write struct instanceInfo relations to csv file
     *
     * @author  1sand0s
     * @param   info        Information of the instance whose relations are to be written
     */
    void writeRelationsToCsv();

	FieldInfo* getField(string& fieldName);

    /* Modifier of the instance */
    int modifier;

    /* Primitive fields, String fields and
     * Primitive Array fields owned by this
     * instance
     *
     * Must be freed from the owning(this) instanceInfo
     */
    vector<FieldInfo*> fields;

    /* Tag of the instance */
    long tag;

    /* Contains information of all referee (child)
     * instances
     */
    vector<ReferenceInfo*> references;

    /* This instance will definitely be written to CSV if
     * this flag is true
     *
     * Can be set by user using WhiteList
     */
    bool writeToGraph;

    /* Flag to indicate if this instance is a Primitive Array */
    bool isPrimitiveArrayType;

    /* Flag to indicate if this instance is an instance
     * of java.lang.String
     */
    bool isStringType;

    /* Hash Code of the object corresponding to this instanceInfo */
    int hashCode;

    /* Tag of this instance's class
     */
    long classTag;

	bool isVisited;
  };
}
