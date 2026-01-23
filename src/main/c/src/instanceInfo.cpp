#include "instanceInfo.h"
#include "fieldInfo.h"
#include "jni_util.h"
#include "ogoConstants.h"
#include <vector>
#include "classInfo.h"

using namespace util;
using namespace graph;
using namespace std;

extern std::vector<ClassInfo*> classNameList;

InstanceInfo::InstanceInfo() {
    clear();
}

InstanceInfo::~InstanceInfo() {
    clear();
}

void InstanceInfo::clear() {
    modifier = 0;
    tag = 0;
    fields.erase(fields.begin(), fields.begin() + fields.size());
    references.erase(references.begin(), references.begin() + references.size());
    isPrimitiveArrayType = false;
    isStringType = false;
    hashCode = 0;
    classTag = 0;
    writeToGraph = false;
}

/**
 * @fn      void writeInstanceInfoToCsv(FILE*, instanceInfo*)
 * @brief   Write struct instanceInfo to csv file
 *
 * @author  1sand0s
 * @param   f           Pointer to the file to write to
 * @param   info        Information of the instance to write
 */
void
InstanceInfo::writeToCsv(ofstream& f,
                         string className) {

    f << className << "," << tag << "," << hashCode << "\n";
}

/**
 * @fn      void writeInstanceRelationsToCsv(instanceInfo*)
 * @brief   Write struct instanceInfo relations to csv file
 *
 * @author  1sand0s
 * @param   info        Information of the instance whose relations are to be written
 */
void
InstanceInfo::writeRelationsToCsv() {

    /* Do nothing if number of refereed instances is zero */
    if (references.size() > 0) {

        string fileName = to_string(tag) + CSV_FILES::REFERENCE_RELATION_FILE_SUFFIX;
        /* Prepend File with the tag of the owner/referrer instance
         * 1. We can then use this naming convention to easily identify the file
         *    that contains this instance's refereed instances
         */
		string relationContent = "";

        /* Iterate through the referrer instance's referee list and add their tags to the
         * [*Referrer_TAG*]_Neo4JRelations.csv file*/
        for (ReferenceInfo* rInfo : this->references) {
		  if(!rInfo->writeToGraph) {
			continue;
		  }
            string relation = rInfo->toString();
            if(relation.size() > 0) {
			  relationContent += relation + "\n";
            }
        }
		if(relationContent.size() > 0){
		  ofstream f(fileName, ofstream::out);
		  f << relationContent;
		  f.close();
		}
    }
}

/**
 * @fn      void writeInstancePropertiesToCsv(instanceInfo*)
 * @brief   Write struct instanceInfo properties (primitive fields) to csv file
 *
 * @author  1sand0s
 * @param   info        Information of the instance whose primitive fields are to be written
 */
void
InstanceInfo::writePropertiesToCsv(Agent* agent) {

    /* Do nothing if number of primitive fields is zero */
    if (fields.size() > 0) {

        string fileName = to_string(tag) + CSV_FILES::FIELD_PROPERTIES_FILE_SUFFIX;
        /* Prepend File with the tag of the owner/referrer instance
         * 1. We can then use this naming convention to easily identify the file
         *    that contains this instance's primitive field information
         */
		string propertyContent="";

        /* Iterate through the referrer instance's primitive field list and add
         * their information to the [*Referrer_TAG*]_Neo4JProperties.csv file*/
        for (FieldInfo* fInfo : this->fields) {
            string properties = fInfo->toString(agent);
            if(properties.size() > 0) {
			  propertyContent+=properties + "\n";
            }
        }
		if(propertyContent.size() > 0){
		  ofstream f(fileName, ofstream::out);
		  f << propertyContent;
		  f.close();
		}
    }
}

FieldInfo* InstanceInfo::getField(string &fieldName) {
    for(FieldInfo* fInfo : fields) {
        if(fInfo->name == fieldName) {
            return fInfo;
        }
    }
    return NULL;
}

