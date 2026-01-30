#include "CallClauseVisitor.h"
#include "CreateClauseVisitor.h"
#include "CypherLexer.h"
#include "CypherParser.h"
#include "MatchClauseVisitor.h"
#include "MergeClauseVisitor.h"
#include "ReturnClauseVisitor.h"
#include "classInfo.h"
#include "fieldInfo.h"
#include "instanceInfo.h"
#include "jni_util.h"
#include "ogoConstants.h"
#include "ogoJVMTI.h"
#include "string_utils.h"

#include <antlr4-runtime.h>
#include <cstring>
#include <iostream>
#include <jni.h>
#include <jvmti.h>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace antlr4;
using namespace std;
using namespace graph;
using namespace util;

/**
 * @def DEBUG_PRINT_
 * @brief Uncomment to print debug information
 *
 */
/*#define DEBUG_PRINT_*/

/**
 * @def DEBUG_PRINT_CYPHER_TOKENS_
 * @brief Uncomment to print the lexed Cypher tokens
 *
 */
// #define DEBUG_PRINT_CYPHER_TOKENS_

/**
 * @def DEBUG_PRINT_HEAP_ITERATION_
 * @brief Uncomment to print tag assigned to objects
 *        during heap iteration (see heap_iterationCallback)
 *
 */
// #define DEBUG_PRINT_HEAP_ITERATION_

/**
 * @def DEBUG_PRINT_HEAP_TRAVERSAL_
 * @brief Uncomment to print relationships between instances and
 *        between instances and primitives using tags
 *        (see heap_referenceCallback and heap_primitiveFieldCallback)
 *
 */
// #define DEBUG_PRINT_HEAP_TRAVERSAL_

/**
 * @def DEBUG_PRINT_GRAPH_
 * @brief Uncomment to print object graph to STDOUT
 *
 */
// #define DEBUG_PRINT_GRAPH_

/**
 * @def DEBUG_PRINT_ERROR_
 * @brief Uncomment to print errors encountered during
 *        JVMTI method calls
 *
 */
/*#define DEBUG_PRINT_ERROR_*/

/**
 * @def DEBUG_PRINT_EXCEPTION_CLASS_
 * @brief Uncomment to print execption trigger name
 *
 */
// #define DEBUG_PRINT_EXCEPTION_CLASS_

/**
 * @def PROFILE_PRINT_
 * @brief Uncomment to print execution time of stages of
 *        object graph construction
 *
 */
// #define PROFILE_PRINT_

/**
 * @def PROFILE_PRINT_
 * @brief Uncomment to print execution time of stages of
 *        object graph construction to CSV files
 *
 */
#define PROFILE_PRINT_FILE_

#ifdef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
InstanceInfo *iInfo = NULL;
#endif

/**
 * The only form of exception handling JVMTI offers is error checking
 * of function call returns. If no error then jvmtiError must be
 * JVMTI_ERROR_NONE see:
 *      1.
 * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#functionReturn
 *      2.
 * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#ErrorSection
 *      3.
 * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#jvmtiError
 */
static jvmtiError err;

/**
 *  Holds the JVMTI session handle used to call JVMTI functions
 *  Holds the cpaabilities of the current JVMTI session
 *  Holds the lock for making sections reentrant
 *
 */
Agent *agent;

/**
 *  The object graph
 */
vector<ClassInfo *> classNameList;
map<string, ClassInfo *> className2Info;
// map<string, string> queryClassName2dynamicClassName;
unsigned long classInfoCount = 0;
vector<InstanceInfo *> writeToClassNameListInstances;

/**
 *  Used to tag instance and primtive fields
 *  These tags are used to associate instances with their classes and
 *  to find references between instances and between instances and primitives
 */
static int nodeID = 0;

/**
 *  Number of classes in the classNameList (Not all loaded classes are stored in
 * classNameList Only those used in writing assertions)
 *
 *  Number of elements in classNameList (classInfo*)
 */
static int objectWrittenToClassNameListCount = 0;

/**
 *  JVMTI version used
 */
constexpr int jvmtiVersion = JVMTI_VERSION_11;

/**
 * Contains whitelist of packages. Only instances of classes
 * belonging to these packages will be included in the object classNameList
 */
static vector<string> whitePackageName;

/**
 * Contains blacklist of packages. Instances of classes
 * belonging to these packages will be definitely excluded from the object
 * classNameList
 */
static vector<string> blackPackageName;

/**
 * Contains blacklist of packages. Instances of classes
 * belonging to these packages will be definitely excluded from the object
 * classNameList
 */
static vector<string> excludeblackPackageName;

/**
 *  Header to be used while writing object classNameList to CSV files
 */
const string csvHeader = "Signature,Tag,HashCode";

/**
 * char* containing instanceof name to assign to references between
 * instance and it Class's Class.java instance. Avoids allocating memory to
 * referrerRelation within referenceInfo struct
 */
constexpr char instanceof [] = "instanceof";

jvmtiHeapCallbacks iterationCallbacks;
jvmtiHeapCallbacks iterationSetupCallbacks;
jvmtiHeapCallbacks traversalCallbacks;

/**
 * @fn      void check_jvmti_error(jvmtiError, const char*)
 * @brief   Only means of error handling in jvmti is by checking the error codes
 * returned from jvmti functions. This method is used to check if there was any
 * error
 *
 * @author  1sand0s
 * @param   errnum    The error code returned after jvmti function call
 * @param   str       String to print if there was any error (for debugging)
 * @see
 * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#ErrorSection
 */
/*
static void
check_jvmti_error(jvmtiError errnum,
                  const char *str) {
    if (errnum != JVMTI_ERROR_NONE) {
        char *errnum_str;
        errnum_str = NULL;

        // Get Error codes
        (void) (*agent.jvmti).functions->GetErrorName(agent.jvmti,
                errnum,
                &errnum_str);
#ifdef DEBUG_PRINT_ERROR_
        printf("ERROR: JVMTI: %d(%s): %s\n",
               errnum,
               (errnum_str == NULL ? "Unknown" : errnum_str),
               (str == NULL ? "" : str));
#endif
    }
}*/

/**
 * @fn void getObjectsWithTagscInfo(jobject**,
 *                                  long**,
 *                                  int*,
 *                                  TimeInfo*)
 * @brief Gets all the objects corresponding to instanceInfo of all classInfo
 *        that are writable to CSV
 *
 * @param objects
 * @param tags
 * @param tagCount
 * @param tInfo
 */
static void getObjectsWithTagscInfo(jobject **objects, long **tags,
                                    int *tagCount, TIMER::TimeInfo *tInfo) {
  long *cTags = nullptr;
  int cTagCount = 0;
  *objects = nullptr;
  *tags = nullptr;
  *tagCount = 0;

  for (ClassInfo *cInfo : classNameList) {
    if (cInfo->instances.empty() || !cInfo->containsWritableInstances() ||
        cInfo->name.empty()) {
      continue;
    }

    for (int k = 0; k < cInfo->instances.size(); k++) {
      InstanceInfo *iInfo = cInfo->instances[k];

      if (!iInfo->writeToGraph) {
        continue;
      }

      cTags =
          static_cast<long *>(realloc(cTags, (cTagCount + 1) * sizeof(long)));
      cTags[cTagCount] = cInfo->instanceTags[k];
      cTagCount++;
    }
  }

  TIMER_START_(*tInfo)
  agent->getObjectsWithTags(cTags, cTagCount, objects, tags, tagCount);
  TIMER_ACCUMULATE_(*tInfo)
}

/**
 * @fn      void writeClassNameListToCsv(TimeInfo*)
 * @brief   write object classNameList (cInfo* classNameList) to CSV files
 *
 * @author  1sand0s
 * @param   tInfo   timeInfo array for profiling methods calls
 */
static void writeClassNameListToCsv(TIMER::TimeInfo **tInfo) {

  jobject *objects = nullptr;
  long *tags = nullptr;
  int tagCount = 0;

  getObjectsWithTagscInfo(&objects, &tags, &tagCount, *tInfo);

  for (ClassInfo *cInfo : classNameList) {
    if (!cInfo->containsWritableInstances() || cInfo->name.empty()) {
      continue;
    }

    /* File name as [*classInfo tag*].java.csv */
    string fileName = to_string(cInfo->tag) + ".java.csv";
    ofstream f(fileName, ofstream::out);

    /* Write CSV header to file*/
    f << csvHeader << "\n";

    /* Iterate and write instance and field info to CSV file */
    for (int i = 0; i < tagCount; i++) {
      InstanceInfo *info = nullptr;

      TIMER_START_(*(*tInfo + 1))
      /* Get instanceInfo with tag matching referrer_tag_ptr */
      cInfo->getInstanceInfoWithTag(tags[i], &info);
      TIMER_ACCUMULATE_(*(*tInfo + 1))
      if (info == nullptr) {
        continue;
      }

      if (!info->writeToGraph) {
        continue;
      }

      info->hashCode = 0;
      TIMER_START_(*(*tInfo + 2))
      info->hashCode = agent->getObjectHashCode(objects[i]);
      TIMER_ACCUMULATE_(*(*tInfo + 2))

      objectWrittenToClassNameListCount++;

      TIMER_START_(*(*tInfo + 3))
      /* Write instance information to file */
      info->writeToCsv(f, cInfo->name);
      TIMER_ACCUMULATE_(*(*tInfo + 3))
      TIMER_START_(*(*tInfo + 4))
      /* Write instance relations to file */
      info->writeRelationsToCsv();
      TIMER_ACCUMULATE_(*(*tInfo + 4))
      TIMER_START_(*(*tInfo + 5))
      /* Write instance properties to file */
      info->writePropertiesToCsv(agent);
      TIMER_ACCUMULATE_(*(*tInfo + 5))
    }
    f.close();
  }
}

/**
 * @fn          static jint JNICALL heap_iterationCallback(jlong, jlong, jlong*,
 * jint, void*)
 * @brief       Search for class instances and assign unique tag to them. The
 * tag can later be used by followReferences to identify relationships. Unique
 *              tag only assigned to instance types whereas primtives, arrays
 * and string types are by default assigned the same tag as the instance they
 * belong to
 *
 * @author      1sand0s
 * @param       class_tag   Tag of the class of the current instance
 * @param       size        Not used
 * @param       tag_ptr     Tag of the current instance (0 unless instance of
 * java.lang.Class) Assigned to unique tag to be used later during follow
 * references
 * @param       length      Not used
 * @param       user_data   Not used
 *
 */
jint JNICALL heap_iterationCallback(jlong class_tag, jlong size, jlong *tag_ptr,
                                    jint length, void *user_data) {
  if (class_tag > 0 && class_tag <= classInfoCount) {
    if (
        /* 1. This checks if current object is an instance of Class.Class.java
         * 2. We need this object since all instances of a Class hold a
         * reference (instanceof) to their Class's Class.java object
         * 3. We also use this object for getting the Class's meta-data such as
         * fields and methods
         */
        (*tag_ptr > 0) ||

        /* 1. This checks if current object is an instance of a Class which was
         * tagged in getLoadedClasses(). We use this check to limit the size of
         * the Object classNameList.
         * 2. Without this check all objects present in the JVM's heap would be
         * loaded into the object classNameList
         * 3. We need to filter out Class.Class.java objects since even untagged
         * classes will have Class.Class.java objects
         */
        (*tag_ptr == 0)) {

      /* Variable to hold the index of cInfo classNameList element */
      const long classNameListIndex = class_tag - 1;

      /* If its untagged object then its an instance of a Class rather than
       * Class's Class.java and so assign a unique tag to identify relations
       * between instances during followReferences
       *
       * The monotonously increasing tag assigned to objects here assures that
       * all objects inserted into the respective class's classInfo struct
       * happens such that their tags are in ascending order. So, we don't have
       * to do any extra processing
       */
      if (*tag_ptr == 0) {
        *tag_ptr = ++nodeID;
      }
      /* If the object is tagged then its an instance of Class.Class.java and
       * therefore the callback can report this instance in any order (not
       * necessarily ascending order of tags). But since we use binary search to
       * get instances of classes using their tags, we want the instance array
       * to be sorted in ascending order of tags. So set this classInfo element
       * unsorted (Mergesort used later to sort its instances)
       */
      else {
        classNameList[classNameListIndex]->isSorted = false;
      }

#ifdef DEBUG_PRINT_HEAP_ITERATION_
      printf("Class Tag : %ld , Object Tag : %ld\n", class_tag, *tag_ptr);
#endif
      auto *iInfo = new InstanceInfo();
      iInfo->tag = *tag_ptr;
      iInfo->classTag = classNameList[classNameListIndex]->tag;
      iInfo->writeToGraph = classNameList[classNameListIndex]->writeToGraph;

      /* Store instance tags in instanceTags of classInfo(makes searching for
       * instances with specified tags faster)*/
      classNameList[classNameListIndex]->instanceTags.push_back(*tag_ptr);
      classNameList[classNameListIndex]->instances.push_back(iInfo);

      if (iInfo->writeToGraph) {
        writeToClassNameListInstances.push_back(iInfo);
      }
    }
  } else {
    *tag_ptr = 0;
  }
  return JVMTI_VISIT_OBJECTS;
}

/**
 * @fn          static jint JNICALL heap_iterationSetupCallback(jlong, jlong,
 * jlong*, jint, void*)
 * @brief       This sets the tag of all objects in the heap to 0. This serves
 * as an intialization step before we construct the object classNameList
 *
 * @author      1sand0s
 * @param       class_tag   Tag of the class of the current instance
 * @param       size        Not used
 * @param       tag_ptr     Tag of the current instance (assigne to 0)
 * @param       length      Not used
 * @param       user_data   Not used
 */
jint JNICALL heap_iterationSetupCallback(jlong class_tag, jlong size,
                                         jlong *tag_ptr, jint length,
                                         void *user_data) {
  /* For cleaning up and initialization, re-initialize all object tags back to
   * 0*/
  *tag_ptr = 0;
  return JVMTI_VISIT_OBJECTS;
}

bool shouldProceed(const string &name1) {
  if (name1.rfind("java.", 0) == 0) {
    if (name1 == "java.lang.Class" || name1 == "java.lang.Object" ||
        name1.find("java.lang.Byte") != string::npos ||
        name1.find("java.lang.Short") != string::npos ||
        name1.find("java.lang.Integer") != string::npos ||
        name1.find("java.lang.Long") != string::npos ||
        name1.find("java.lang.Character") != string::npos ||
        name1.find("java.lang.Float") != string::npos ||
        name1.find("java.lang.Double") != string::npos ||
        name1.find("java.lang.Boolean") != string::npos ||
        name1.find("java.lang.String") != string::npos ||
        name1.find("java.util.List") != string::npos ||
        name1.find("java.util.LinkedList") != string::npos ||
        name1.find("java.util.ArrayList") != string::npos ||
        name1.find("java.util.ArrayDeque") != string::npos ||
        name1.find("java.util.Vector") != string::npos ||
        name1.find("java.util.HashMap") != string::npos) {

      return true;
    }
    return false;
  }
  return true;
}

/**
 * @fn          jint JNICALL heap_referenceCallback(jvmtiHeapReferenceKind,
 *                                                  const
 * jvmtiHeapReferenceInfo*, jlong, jlong, jlong, jlong*, jlong*, jint, void*)
 * @brief       We use the unique tags assigned to instances inside
 * heap_iterationCallback to identify relations between instances and between
 * instances and their primitives
 *
 * @author      1sand0s
 * @param       reference_kind
 * @param       reference_info
 * @param       class_tag               Tag of the class of the current instance
 * @param       referrer_class_tag      Tag of the class whose instance owns the
 * current instance
 * @param       size                    Not used
 * @param       tag_ptr                 Tag of the current instance
 * @param       referrer_tag_ptr        Tag of the instance who owns the current
 * instance
 * @param       length                  Not used
 * @param       user_data               Not used
 */
jint JNICALL heap_referenceCallback(
    jvmtiHeapReferenceKind reference_kind,
    const jvmtiHeapReferenceInfo *reference_info, jlong class_tag,
    jlong referrer_class_tag, jlong size, jlong *tag_ptr,
    jlong *referrer_tag_ptr, jint length, void *user_data) {
  if (
      /* Classes of referrer instances limited to tagged class (Will Segfault
         else) */
      referrer_class_tag > 0 && referrer_class_tag <= classInfoCount &&

      /* Classes of referee instances limited to tagged classes to control
         object classNameList size */
      class_tag > 0 && class_tag <= classInfoCount &&

      /* Referee Instances must be tagged (see heap_iterationCallback) */
      *tag_ptr > 0 &&

      /* Referrer Instances must be tagged (see heap_iterationCallback) */
      *referrer_tag_ptr > 0 &&
      (reference_kind == JVMTI_HEAP_REFERENCE_CLASS ||
       reference_kind == JVMTI_HEAP_REFERENCE_FIELD ||
       reference_kind == JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT ||
       reference_kind == JVMTI_HEAP_REFERENCE_STATIC_FIELD ||
       reference_kind == JVMTI_HEAP_REFERENCE_CONSTANT_POOL ||
       reference_kind == JVMTI_HEAP_REFERENCE_SUPERCLASS ||
       reference_kind == JVMTI_HEAP_REFERENCE_INTERFACE)) {
    if (!shouldProceed(classNameList[class_tag - 1]->name)) {
      return 0;
      // return JVMTI_VISIT_ABORT;
    }
#ifdef DEBUG_PRINT_HEAP_TRAVERSAL_
    /*if(classNameList[class_tag -
       1]->name.find("org.ogo.test.korat.BinaryTree") != string::npos ||
            classNameList[referrer_class_tag -
       1]->name.find("org.ogo.test.korat.BinaryTree") != string::npos ) {*/
    cout << "Object Tag : " << *tag_ptr
         << " , Referrer Tag : " << *referrer_tag_ptr
         << " , Class Tag : " << classNameList[class_tag - 1]->name << "  "
         << class_tag << " , Referrer Class Tag : "
         << classNameList[referrer_class_tag - 1]->name << "  "
         << referrer_class_tag << " , Heap Reference Kind : " << reference_kind
         << endl;
    //}
#endif

#ifndef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
    InstanceInfo *iInfo = nullptr;
#else
    if (iInfo) {
      if (iInfo->tag != *referrer_tag_ptr) {
        iInfo = NULL;
        goto GETINSTANCE;
      }
    } else {
#endif
    /* Get instanceInfo with tag matching referrer_tag_ptr */
  GETINSTANCE:
    classNameList[referrer_class_tag - 1]->getInstanceInfoWithTag(
        *referrer_tag_ptr, &iInfo);
#ifdef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
  }
#endif
  /*Check for NULL pointer (if instanceInfo with tag not found)*/
  if (iInfo != nullptr) {

    /* Variable for holding index of the references (for better readability) */
    int referenceIndex = iInfo->references.size();

    auto *rInfo = new ReferenceInfo(reference_kind, *tag_ptr, class_tag,
                                    iInfo->writeToGraph);

    /* If the current instance is a reference field of the referrer instance
     * then store its fieldIndex which can be used to get the name of the
     * current instance from jfieldID (see getFieldInfo()) *ONLY FOR INSTANCES
     * OF TYPE java.lang.String or Primitive Boxed types*
     */
    if (reference_kind == JVMTI_HEAP_REFERENCE_FIELD ||
        reference_kind == JVMTI_HEAP_REFERENCE_STATIC_FIELD) {
      rInfo->fieldIndex = reference_info->field.index;
    }

    if (reference_kind == JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT) {
      rInfo->referrerRelation = std::to_string(reference_info->array.index);
    }

    /* Shallow Copy class's Class.java instance into its classInfo*
     * we need this to get the names of the fields inherited by this class
     */
    if (
        /* Check if reference between referrer and referee instance is of type
         * constant pool
         */
        reference_kind == JVMTI_HEAP_REFERENCE_SUPERCLASS ||
        reference_kind == JVMTI_HEAP_REFERENCE_INTERFACE) {
      if (
          /* Check if referee is a Class.Class.java instance */
          *tag_ptr < classInfoCount &&

          /* Check if referrer is a Class.Class.java instance */
          *referrer_tag_ptr < classInfoCount &&

          /* If both referrer and referee are Class.Class.java instances
           * then their class's must be the same (Class.java) and hence
           * check if their class tags are equal (this condition can perhaps
           * subsume the previous two?)
           */
          class_tag == referrer_class_tag) {

        if (reference_kind == JVMTI_HEAP_REFERENCE_SUPERCLASS)
          /* Shallow copy the Class.java instance into this class's classInfo*
           */
          classNameList[*referrer_tag_ptr - 1]->superClassTag = *tag_ptr;
        else if (reference_kind == JVMTI_HEAP_REFERENCE_INTERFACE)
          classNameList[*referrer_tag_ptr - 1]
              ->implementedInterfaceTags.push_back(*tag_ptr);
      }
    }
    iInfo->references.push_back(rInfo);
  }

#ifdef OPTIMIZATION_FOLLOW_REFERENCES_TERMINATE_IRRELEVANT_PATHS_
  return JVMTI_VISIT_OBJECTS;
}
return JVMTI_VISIT_ABORT;
#else
    }
    return JVMTI_VISIT_OBJECTS;
#endif
}

/**
 * @fn          jint JNICALL heap_primitiveFieldCallback(jvmtiHeapReferenceKind,
 *                                                       const
 * jvmtiHeapReferenceInfo*, jlong, jlong*, jvalue, jvmtiPrimitiveType, void*)
 * @brief       Get primitive fields of an instance during follow references.
 * Used to get values of th primitive fields.
 *
 * @author      1sand0s
 * between owning instance and primitive field
 * @param       object_class_tag    Tag of the class of the instance who owns
 * the current primitive field
 * @param       object_tag_ptr      Tag of the primitive field (by default equal
 * to tag of the owning instance)
 * @param       value               value of the primitive field
 * @param       value_type          type of the primitive field (used to get
 * appropriate field from union value)
 * @param       user_data           Not used
 */
jint JNICALL heap_primitiveFieldCallback(jvmtiHeapReferenceKind kind,
                                         const jvmtiHeapReferenceInfo *info,
                                         jlong object_class_tag,
                                         jlong *object_tag_ptr, jvalue value,
                                         jvmtiPrimitiveType value_type,
                                         void *user_data) {
  if (
      /* Check if the Class of the object that owns this field is tagged */
      object_class_tag > 0 &&

      /* Redundant check but added for readability */
      object_class_tag <= classInfoCount &&

      /* Check if the object that owns this field is tagged */
      *object_tag_ptr > 0) {
#ifdef DEBUG_PRINT_HEAP_TRAVERSAL_
    cout << "Field : Class : " << object_class_tag
         << " , Tag : " << *object_tag_ptr << " , Index : " << info->field.index
         << " , Reference Kind : " << kind
         << " , Field Tag : " << *object_tag_ptr << endl;
    if (value_type == JVMTI_PRIMITIVE_TYPE_INT) {
      cout << "Value : " << value.i << endl;
    }
#endif

#ifndef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
    InstanceInfo *iInfo = nullptr;
#else
      if (iInfo) {
        if (iInfo->tag != *object_tag_ptr) {
          iInfo = NULL;
          goto GETINSTANCE;
        }
      } else {
#endif
    /* Get instanceInfo with tag matching object_tag_ptr */
  GETINSTANCE:
    classNameList[object_class_tag - 1]->getInstanceInfoWithTag(*object_tag_ptr,
                                                                &iInfo);
#ifdef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
  }
#endif

  /* Dynamically allocate memory to fieldInfo */
  if (iInfo != nullptr) {
    auto *fInfo = new FieldInfo();

    /* Primitive fields can only contain 1 jvalue element
     * We use jvalue* to handle Primitive type arrays */
    fInfo->type = agent->getFieldType(value_type);

    /* Primitive fields are all jvalue */
    fInfo->value.push_back(value);
    fInfo->fieldIndex = info->field.index;

    /* See
     * https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#jvmtiHeapReferenceCallback
     * Can only be JVMTI_HEAP_REFERENCE_FIELD (2) or
     * JVMTI_HEAP_REFERENCE_STATIC_FIELD (8) */
    fInfo->referenceKind = kind;

    /* Primitive fields of instances are added to Neo4J as
     * properties of owning instances so assign the same tag
     * as the owning instance
     */
    fInfo->tag = *object_tag_ptr;
    iInfo->fields.push_back(fInfo);
  }

#ifdef OPTIMIZATION_FOLLOW_REFERENCES_TERMINATE_IRRELEVANT_PATHS_
  return JVMTI_VISIT_OBJECTS;
}
return JVMTI_VISIT_ABORT;
#else
      }
      return JVMTI_VISIT_OBJECTS;
#endif
}

/**
 * @fn          jint JNICALL heap_stringFieldCallback(jlong,
 *                                                    jlong,
 *                                                    jlong*,
 *                                                    jchar*,
 *                                                    jint,
 *                                                    void*)
 * @brief       Get string fields of an instance during follow references.
 *
 * @author      1sand0s
 * @param       class_tag    Tag of the class of the instance who owns the
 * current string field
 * @param       tag_ptr      Tag of the string field (by default equal to tag of
 * the owning instance)
 * @param       value               value of the string field
 * @param       value_length  Length of the string
 * @param       user_data           Not used
 */
jint JNICALL heap_stringFieldCallback(jlong class_tag, jlong size,
                                      jlong *tag_ptr, const jchar *value,
                                      jint value_length, void *user_data) {
  if (
      /* Check if the Class of the object that owns this field is tagged */
      class_tag > 0 &&

      /* Redundant check but added for readability */
      class_tag <= classInfoCount &&

      /* Check if the object that owns this field is tagged */
      *tag_ptr > 0) {
#ifdef DEBUG_PRINT_HEAP_TRAVERSAL_
    cout << "String Field : Class Tag : " << class_tag
         << " , Tag : " << *tag_ptr << " , Value : ";
    for (int j = 0; j < value_length; j++) {
      printf("%c", value[j]);
    }
    cout << endl;
#endif

#ifndef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
    InstanceInfo *iInfo = nullptr;
#else
        if (iInfo) {
          if (iInfo->tag != *tag_ptr) {
            iInfo = NULL;
            goto GETINSTANCE;
          }
        } else {
#endif
    /* Get instanceInfo with tag matching object_tag_ptr */
  GETINSTANCE:
    classNameList[class_tag - 1]->getInstanceInfoWithTag(*tag_ptr, &iInfo);
#ifdef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
  }
#endif

  /* Mark instance as string type to embed later */
  if (iInfo != nullptr) {
    iInfo->isStringType = true;
  }

#ifdef OPTIMIZATION_FOLLOW_REFERENCES_TERMINATE_IRRELEVANT_PATHS_
  return JVMTI_VISIT_OBJECTS;
}
return JVMTI_VISIT_ABORT;
#else
        }
        return JVMTI_VISIT_OBJECTS;
#endif
}

/**
 * @fn          jint JNICALL heap_arrayFieldCallback(jlong,
 *                                                   jlong,
 *                                                   jlong*,
 *                                                   jint,
 *                                                   jvmtiPrimitiveType,
 *                                                   const void*,
 *                                                   void*)
 * @brief       Get string fields of an instance during follow references.
 *
 * @author      1sand0s
 * @param       class_tag    Tag of the class of the instance who owns the
 * current string field
 * @param       tag_ptr      Tag of the string field (by default equal to tag of
 * the owning instance)
 * @param       user_data           Not used
 */
jint JNICALL heap_arrayFieldCallback(jlong class_tag, jlong size,
                                     jlong *tag_ptr, jint element_count,
                                     jvmtiPrimitiveType element_type,
                                     const void *elements, void *user_data) {
  if (
      /* Check if the Class of the object that owns this field is tagged */
      class_tag > 0 &&

      /* Redundant check but added for readability */
      class_tag <= classInfoCount &&

      /* Check if the object that owns this field is tagged */
      *tag_ptr > 0) {
#ifdef DEBUG_PRINT_HEAP_TRAVERSAL_
    cout << "Array Field : Class Tag : " << class_tag << " , Tag : " << *tag_ptr
         << " Value : ";
    for (int j = 0; j < element_count; j++) {
      switch (element_type) {
#define ENTRY(a, b, c, d, e, f, g)                                             \
  case a:                                                                      \
    printf(STRINGIFY_MACRO_(b), *(((d *)elements) + j));                       \
    break;
        JVMTI_PRIMITIVE_TABLE_
#undef ENTRY
      }
    }
    cout << endl;
#endif

#ifndef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
    InstanceInfo *iInfo = nullptr;
#else
          if (iInfo) {
            if (iInfo->tag != *tag_ptr) {
              iInfo = NULL;
              goto GETINSTANCE;
            }
          } else {
#endif
    /* Get instanceInfo with tag matching object_tag_ptr */
  GETINSTANCE:
    classNameList[class_tag - 1]->getInstanceInfoWithTag(*tag_ptr, &iInfo);
#ifdef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
  }
#endif

  /* Dynamically allocate memory to fieldInfo */
  if (iInfo != nullptr) {
    auto *fInfo = new FieldInfo();
    iInfo->isPrimitiveArrayType = true;
    switch (agent->getFieldType(element_type)) {
#define ENTRY(a, b, c, d)                                                      \
  case Agent::MethodReturnType::c:                                             \
    fInfo->type = Agent::MethodReturnType::c##_ARRAY;                          \
    for (int j = 0; j < element_count; j++) {                                  \
      jvalue value{.d = ((b *)elements)[j]};                                   \
      fInfo->value.push_back(value);                                           \
    }                                                                          \
    break;
      JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
    }
    fInfo->tag = *tag_ptr;
    iInfo->fields.push_back(fInfo);
  }

#ifdef OPTIMIZATION_FOLLOW_REFERENCES_TERMINATE_IRRELEVANT_PATHS_
  return JVMTI_VISIT_OBJECTS;
}
return JVMTI_VISIT_ABORT;
#else
          }
          return JVMTI_VISIT_OBJECTS;
#endif
}

/**
 * @fn void embedStringInstances(const instanceInfo*, instanceInfo*, const int,
 * classInfo*)
 * @brief
 *
 * @param stringInstance
 * @param stringFieldOwningInstance
 * @param stringFieldIndex
 */
static void embedStringInstances(const InstanceInfo *stringInstance,
                                 InstanceInfo *stringFieldOwningInstance,
                                 const int stringFieldIndex,
                                 const vector<FieldInfo *> &inheritedFields) {

  /* We are iterating through all fields defined in java.lang.String to
   * to find the field that contains the Byte array corresponding to the
   * characters in the String
   *
   * This field is of type [B or Byte Array
   */
  for (const auto reference : stringInstance->references) {

    if (reference->referenceKind == JVMTI_HEAP_REFERENCE_FIELD) {

      InstanceInfo *byteArrayInstance = nullptr;

      classNameList[reference->referrerClassTag - 1]->getInstanceInfoWithTag(
          reference->referrerTag, &byteArrayInstance);

      if (byteArrayInstance != nullptr) {
        /* Since the expected field is of type [B we check if the retrieved
         * referee instance is of Array Type and has atleast 1 element
         */
        if (byteArrayInstance->isPrimitiveArrayType) {
          for (FieldInfo *fInfo : byteArrayInstance->fields) {
            if (agent->isFieldArrayType(fInfo->type) && !fInfo->value.empty()) {
              stringFieldOwningInstance->fields.push_back(fInfo);
              if (stringFieldIndex >= 0) {
                fInfo->name = inheritedFields[stringFieldIndex]->name;
                fInfo->type = Agent::MethodReturnType::JSTRING;
              }
              break;
            }
          }
        }
        break;
      }
    }
  }
}

/**
 * @fn void embedBoxedPrimitiveInstances(const instanceInfo*, instanceInfo*,
 * const int, classInfo*)
 * @brief
 *
 * @param boxedPrimitiveInstance
 * @param boxedPrimitiveFieldOwningInstance
 * @param boxedPrimitiveFieldIndex
 */
static void
embedBoxedPrimitiveInstances(InstanceInfo *boxedPrimitiveInstance,
                             InstanceInfo *boxedPrimitiveFieldOwningInstance,
                             const int boxedPrimitiveFieldIndex,
                             const vector<FieldInfo *> &inheritedFields) {

  /* Since the expected field is of type [B we check if the retrieved
   * referee instance is of Array Type and has atleast 1 element
   */
  for (FieldInfo *fInfo : boxedPrimitiveInstance->fields) {
    if (agent->isFieldArrayType(fInfo->type) && !fInfo->value.empty()) {
      fInfo->name = inheritedFields[boxedPrimitiveFieldIndex]->name;
      boxedPrimitiveFieldOwningInstance->fields.push_back(fInfo);
      break;
    }
  }
  boxedPrimitiveInstance->fields.clear();
}

/**
 * @fn          void assignFieldNames(instanceInfo*, int, classInfo* )
 * @brief       Assign field names to objects stored in classNameList for
 *              fields of type JVMTI_HEAP_REFERENCE_FIELD and
 *              JVMTI_HEAP_REFERENCE_STATIC_FIELD
 *
 * @author      1sand0s
 * @param referrer        The referrer instanceInfo in the relation
 * @param referenceIndex  The index of the current referenceInfo element
 * @param classInstance   The Class.Class.java instance of referrer instance
 */
static void
assignFieldNamesToReferenceFields(const InstanceInfo *referrer,
                                  const int referenceIndex,
                                  map<int, FieldInfo *> &inheritedFields) {
  int refereeFieldIndex = referrer->references[referenceIndex]->fieldIndex;

  if ((refereeFieldIndex < 0 &&

       /* String Array elements will have fieldIndex < 0 but still need to be
          embedded */
       referrer->references[referenceIndex]->referenceKind !=
           JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT) ||
      inheritedFields.count(refereeFieldIndex) != 1) {
    return;
  }

  InstanceInfo *referee = nullptr;

  classNameList[referrer->references[referenceIndex]->referrerClassTag - 1]
      ->getInstanceInfoWithTag(
          referrer->references[referenceIndex]->referrerTag, &referee);

  if (referee != nullptr) {

    /* We embed references of type String and primitive boxed types
     * as properties of the owning instance rather than as their own
     * nodes */
    /*if(referee->isStringType) {
          embedStringInstances(referee,
                                                   referrer,
                                                   refereeFieldIndex,
                                                   inheritedFields);
    } else if(referee->isPrimitiveArrayType) {
          if(refereeFieldIndex < 0) {
            return;
          }
          embedBoxedPrimitiveInstances(referee,
                                                                   referrer,
                                                                   refereeFieldIndex,
                                                                   inheritedFields);
                                                                   }*/
    /* Field names become relation names in Neo4J node classNameList
     * for instance fields
     */
    // else
    {
      if (refereeFieldIndex < 0) {
        return;
      }
      referrer->references[referenceIndex]->referrerRelation =
          inheritedFields[refereeFieldIndex]->name;
    }
  }
}

void getImplementedInterfaceAndSuperInterfaces(
    std::vector<long> &implementedInterfaceTags,
    unordered_set<long> &allImplementedInterfaceTags) {
  if (implementedInterfaceTags.empty())
    return;
  for (long interfaceTag : implementedInterfaceTags) {
    if (interfaceTag <= 0 || interfaceTag >= classInfoCount)
      continue;
    ClassInfo *interface = classNameList[interfaceTag - 1];
    getImplementedInterfaceAndSuperInterfaces(
        interface->implementedInterfaceTags, allImplementedInterfaceTags);
    allImplementedInterfaceTags.insert(interface->tag);
  }
}

void getAllImplementedInterfaces(
    ClassInfo *cInfo, unordered_set<long> &allImplementedInterfaceTags) {
  if (cInfo == nullptr)
    return;
  getImplementedInterfaceAndSuperInterfaces(cInfo->implementedInterfaceTags,
                                            allImplementedInterfaceTags);
  if (cInfo->superClassTag <= 0 || cInfo->superClassTag >= classInfoCount ||
      cInfo->tag == cInfo->superClassTag)
    return;
  ClassInfo *superClass = classNameList[cInfo->superClassTag - 1];
  getAllImplementedInterfaces(superClass, allImplementedInterfaceTags);
}

/**
 * @fn          void assignFieldNames()
 * @brief       Assign field names to objects stored in classNameList
 *
 * @author      1sand0s
 */
static void assignFieldNames() {
  for (int i = 0; i < classInfoCount; i++) {
    const ClassInfo *cInfo = classNameList[i];
    for (int j = 0; j < cInfo->instances.size(); j++) {

      InstanceInfo *referrer = cInfo->instances[j];

      /* We add String and other Primitive Boxed types as properties rather than
       * as their own nodes hence this exception */
      if (referrer->isPrimitiveArrayType || referrer->isStringType) {
        continue;
      }

      /* This is true for all instances not belonging to Class.java */
      long srcClassIndex = i;

      /* If the tag of the instance is less than classInfoCount then it must
       * be an instance of Class.Class.java and so its fields are declared in
       * Class.Class.java object */
      if (referrer->tag <= classInfoCount) {
        srcClassIndex = cInfo->instanceTags[j] - 1;
      }

      ClassInfo *classInstance = classNameList[srcClassIndex];
      map<int, FieldInfo *> inheritedFields;
      unsigned long fieldCount = 0;
      unordered_set<long> allImplementedInterfaceTags;
      getAllImplementedInterfaces(classInstance, allImplementedInterfaceTags);
      for (const long interfaceTag : allImplementedInterfaceTags) {
        const ClassInfo *interface = classNameList[interfaceTag - 1];
        fieldCount += interface->fields.size();
      }

      /*+TODO : Optimize this */
      /* Get Superclass fields */
      getInheritedFieldInfo(classInstance, inheritedFields, fieldCount);

      /* Get class fields */
      for (FieldInfo *fInfo : classInstance->fields) {
        inheritedFields.insert({fieldCount, fInfo});
        fieldCount++;
      }

      //            fieldCount = inheritedFields.size();
      /* Assign names and signature to primitive fields of instances from
       * Class's Class.java object*/
      for (FieldInfo *referrerField : referrer->fields) {
        /* Check bounds before indexing */
        if (inheritedFields.count(referrerField->fieldIndex) == 1) {
          referrerField->name =
              inheritedFields[referrerField->fieldIndex]->name;
          referrerField->signature =
              inheritedFields[referrerField->fieldIndex]->signature;
        }
        /* We only add name, the signature (Type) and modifier must be accessed
         * through the field's owning instance's Class.java object */
      }

      for (int k = 0; k < referrer->references.size(); k++) {

        if (referrer->references[k]->referenceKind ==
                JVMTI_HEAP_REFERENCE_FIELD ||
            referrer->references[k]->referenceKind ==
                JVMTI_HEAP_REFERENCE_STATIC_FIELD ||
            referrer->references[k]->referenceKind ==
                JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT) {
          assignFieldNamesToReferenceFields(referrer, k, inheritedFields);
        } else if (referrer->references[k]->referenceKind ==
                   JVMTI_HEAP_REFERENCE_CLASS) {
          referrer->references[k]->referrerRelation = instanceof ;
        }
      }
    }
  }
}

/**
 * @fn          void getInheritedFieldInfo(const classInfo*, fieldInfo**)
 * @brief       Get all fields declared in the class and inherited by the class
 * (SHALLOW COPY)
 *
 * @author      1sand0s
 * @param       cInfo       The class whose declared and inehrited fields have
 * to be retrieved
 * @param       fields      Contains the declared and inherited fields
 * @param       fieldCount  Number of elements in fields
 */
static void getInheritedFieldInfo(const ClassInfo *cInfo,
                                  map<int, FieldInfo *> &fields,
                                  unsigned long &fieldCount) {
  if (cInfo == nullptr) {
    return;
  }

  if (cInfo->superClassTag == 0) {
    return;
  }

  ClassInfo *superClass = classNameList[cInfo->superClassTag - 1];
  getInheritedFieldInfo(superClass, fields, fieldCount);
  /* Populate Super Class fields */
  for (auto &field : superClass->fields) {
    fields.insert({fieldCount, field});
    fieldCount++;
  }
  /*for(int j = 0; j < cInfo->classClassInstance->references.size(); j++) {
      if(cInfo->classClassInstance->references[j]->referenceKind ==
     JVMTI_HEAP_REFERENCE_SUPERCLASS &&
                 (cInfo->classClassInstance->references[j]->referrerTag - 1) <
     classInfoCount) { ClassInfo* superClass =
     classNameList[cInfo->classClassInstance->references[j]->referrerTag - 1];

          getInheritedFieldInfo(superClass,
                                fields);
      */
  /* Populate Super Class fields */
  /*       for(int k = 0; k < superClass->fields.size(); k++) {
          fields.push_back(superClass->fields[k]);
      }
  }
          }*/
}

/**
 * @fn void assignWriteToClassNameListToReferences()
 * @brief   Marks all references of instance to be written
 *          to classNameList
 *
 */
static void assignWriteToClassNameListToReferences(InstanceInfo *iInfo) {
  if (iInfo == nullptr) {
    return;
  }

  if (iInfo->isVisited) {
    return;
  }

  if (
      /* Since we embed String and Primitive Array
       * instances as properties/fields in the owning
       * instance, we can skip writing them to classNameList */
      iInfo->isPrimitiveArrayType || iInfo->isStringType) {
    iInfo->isVisited = true;
    iInfo->writeToGraph = false;
    return;
  }

  iInfo->writeToGraph = true;
  iInfo->isVisited = true;
  classNameList[iInfo->classTag - 1]->writeToGraph = true;
  long tag = iInfo->tag;

  for (const auto &reference : iInfo->references) {

    const int referenceKind = reference->referenceKind;

    /* Only static fields, references to other Class.Class.java instances
     * and superclasses will be written to classNameList for Class.Class.java
     * instance*/
    if (tag <= classInfoCount) {
      if (referenceKind != JVMTI_HEAP_REFERENCE_FIELD ||
          referenceKind != JVMTI_HEAP_REFERENCE_CONSTANT_POOL ||
          referenceKind != JVMTI_HEAP_REFERENCE_SUPERCLASS) {
        reference->writeToGraph = false;
        continue;
      }
    }

    const long refClass = reference->referrerClassTag;
    const long refTag = reference->referrerTag;

    InstanceInfo *rinfo = nullptr;

    /* Get the instance corresponding to the reference */
    classNameList[refClass - 1]->getInstanceInfoWithTag(refTag, &rinfo);

    assignWriteToClassNameListToReferences(rinfo);
    reference->writeToGraph = rinfo->writeToGraph;
    if (rinfo->writeToGraph) {
      classNameList[rinfo->classTag - 1]->writeToGraph = true;
    }
  }
}

/**
 * @fn void assignWriteToClassNameListToReferences()
 * @brief   Marks all references of instance to be written
 *          to classNameList
 *
 */
static void assignWriteToClassNameListToReferences() {
  for (int j = 0; j < classInfoCount; j++) {
    for (int i = 0; i < classNameList[j]->instances.size(); i++) {

      /* If an instance is not to be written to classNameList
       * then none of its references will be either */
      if (!classNameList[j]->instances[i]->writeToGraph ||

          /* Since we embed String and Primitive Array
           * instances as properties/fields in the owning
           * instance, we can skip writing them to classNameList */
          classNameList[j]->instances[i]->isPrimitiveArrayType ||
          classNameList[j]->instances[i]->isStringType) {
        continue;
      }

      const long tag = classNameList[j]->instances[i]->tag;

      for (int k = 0; k < classNameList[j]->instances[i]->references.size();
           k++) {

        const int referenceKind =
            classNameList[j]->instances[i]->references[k]->referenceKind;

        /* Only static fields, references to other Class.Class.java instances
         * and superclasses will be written to classNameList for
         * Class.Class.java instance*/
        if (tag <= classInfoCount) {
          if (referenceKind != JVMTI_HEAP_REFERENCE_FIELD ||
              referenceKind != JVMTI_HEAP_REFERENCE_CONSTANT_POOL ||
              referenceKind != JVMTI_HEAP_REFERENCE_SUPERCLASS) {
            continue;
          }
        }

        long refClass =
            classNameList[j]->instances[i]->references[k]->referrerClassTag;
        long refTag =
            classNameList[j]->instances[i]->references[k]->referrerTag;

        InstanceInfo *info = nullptr;

        /* Get the instance corresponding to the reference */
        classNameList[refClass - 1]->getInstanceInfoWithTag(refTag, &info);

        if (info != nullptr) {
          if ((info->isStringType && referenceKind != 3) ||
              info->isPrimitiveArrayType) {
            info->writeToGraph = false;
            classNameList[j]->instances[i]->references[k]->writeToGraph = false;
            continue;
          }
          classNameList[j]->instances[i]->references[k]->writeToGraph = true;
          info->writeToGraph = true;
        }
      }
    }
  }
}

/**
 * @fn          void printClassNameList()
 * @brief       Prints the object classNameList
 *
 * @author      1sand0s
 */

static void printClassNameList() {
  /*
    for (int j = 0; j < classInfoCount; j++) {
        if (classNameList[j].tag > 0) {
            printf("Class Name : %s , Tag : %ld , Num Instances : %d\n",
                   classNameList[j].name == NULL ? "" : classNameList[j].name,
                   classNameList[j].tag,
                   classNameList[j].instanceCount);
            for (int k = 0; k < classNameList[j].instanceCount; k++) {
                printf("Object%d Tag : %ld , Write To classNameList : %d , Array
  Type : %d\n", k, classNameList[j].instances[k].name == NULL ? "" :
  classNameList[j].instances[k].name, *classNameList[j].instances[k].signature,
                       *classNameList[j].instances[k].modifier,

                       classNameList[j].instances[k].tag,
                       classNameList[j].instances[k].writeToClassNameList,
                       classNameList[j].instances[k].isPrimitiveArrayType);
                for (int i = 0; i < classNameList[j].instances[k].fieldCount;
  i++) { printf("\tPrimitive Fields%d Name :  , Signature :  , Modifier :  , Tag
  : %ld\n", i, classNameList[j].instances[k].fields[i].name == NULL ? "" :
  classNameList[j].instances[k].fields[i].name,
                           *classNameList[j].instances[k].fields[i].signature ==
  NULL ? "" : classNameList[j].instances[k].fields[i].signature,
                            *classNameList[j].instances[k].fields[i].modifier,

                           classNameList[j].instances[k].fields[i].tag);
                    if(classNameList[j].instances[k].fields[i].type !=
  Agent::JVMTI_EXTENDED_FIELD_TYPE_STRING) { for(int m = 0; m <
  classNameList[j].instances[k].fields[i].valueCount; m++) {
                            printPrimitiveFieldValue2(classNameList[j].instances[k].fields[i].type,
  classNameList[j].instances[k].fields[i].value.primitive[m]);
                        }
                        printf("\n");
                    }
                }
                for (int i = 0; i <
  classNameList[j].instances[k].referenceCount; i++) { printf("\tReference
  Fields%d Tag : %ld , Class Tag : %ld\n", i,
                           classNameList[j].instances[k].references[i].referrerTag,
                           classNameList[j].instances[k].references[i].referrerClassTag);
                }
                printf("\n");
            }
            printf("\n\n");
        }
  }*/
}

/**
 * @fn          void getLoadedClasses()
 * @brief       Get information regarding all loaded classes in the JVM and
 * store in classNameList. Unique tags are also assigned to Class.java objects
 *
 * @author      1sand0s
 */
static void getLoadedClasses() {
  vector<jclass> classes;

  /* Get all classes loaded into the JVM */
  agent->getLoadedClasses(classes);
  nodeID = 0;

  // cout << "Num Loaded Classes " << classes.size();
  /* Iterate through all loaded classes and get :
   * 1. Source File Name
   * 2. Class Name
   * 3. Class Fields */
  for (jclass klass : classes) {
    vector<jfieldID> fields;
    vector<jmethodID> methods;
    string className;
    string classSignature;
    int tag = 0;
    bool writeToClassNameList = false;

    agent->setTag(klass, 0);

    /* Get class signature/type */
    agent->getClassName(klass, className, classSignature);

    if (className.empty()) {
      continue;
    }

    /* Creating an object classNameList of all objects in JVM heap is expensive
     * So we limit it to objects belonging to either
     * 1. User specified Packages
     * 2. Class.java instances
     * 3. Obect.java instances
     */
    if (STRING::isStringPresentInList(className, blackPackageName) &&
        !STRING::isStringPresentInList(className, excludeblackPackageName)) {
      continue;
    }

    if (whitePackageName.empty()) {
      writeToClassNameList = true;
    } else {
      if (STRING::isStringPresentInList(className, whitePackageName)) {
        writeToClassNameList = true;
      }
    }
    tag = ++nodeID;

    /* Assign a tag to Class.java object for associating objects during heap
     * traversal
     * 1. Every instance of a Class holds a reference to its Class's Class.java
     * object
     * 2. We tag the Class.java object here so we can use this
     * reference/relation to identify the instance's class without additional
     * JNI calls (expensive).
     */
    agent->setTag(klass, tag);

    /* Allocate memory for classInfo struct */
    auto *cInfo = new ClassInfo();
    cInfo->tag = tag;
    cInfo->writeToGraph = writeToClassNameList;
    cInfo->name = STRING::classToCypherLabel2(className);
    cInfo->signature = classSignature;
    cInfo->superClassTag = 0;

    /* Get class fields */
    agent->getClassDeclaredFields(klass, fields);

    /* Get class declared methods */
    // agent->getClassDeclaredMethods(klass,
    //                             methods);
    /* Get field name, signature and modifiers */
    for (jfieldID field : fields) {
      auto *fInfo = new FieldInfo();
      agent->getFieldInfo(field, klass, fInfo->name, fInfo->signature,
                          fInfo->modifier);
      cInfo->fields.push_back(fInfo);
    }

    /* Get method name, signature and modifiers */
    /*        for (jmethodID method : methods) {
        MethodInfo* mInfo = new MethodInfo();
        agent->getMethodInfo(method,
                             mInfo->name,
                             mInfo->signature,
                             mInfo->modifier);
        cInfo->methods.push_back(mInfo);
            }*/

#ifdef DEBUG_PRINT_
    if (STRING::isStringPresentInList(className, whitePackageName)) {
      cout << "\nClass Name : " << cInfo->name
           << " , Class Signature : " << cInfo->signature
           << " , Class Tag : " << cInfo->tag
           << " , Write To ClassNameList : " << cInfo->writeToGraph << "\n";

      /* Print all class fields */
      for (FieldInfo *fInfo : cInfo->fields) {
        cout << "Field Name : " << fInfo->name
             << " , Signature : " << fInfo->signature
             << " , Modifier : " << fInfo->modifier << "\n";
      }
      /* Print all class fields */
      /*  for (MethodInfo* mInfo : cInfo->methods) {
cout << "Method Name : " << mInfo->name << " , Signature : " << mInfo->signature
<< " , Modifier : " << mInfo->modifier << "\n";
              }*/
    }
#endif
    classNameList.push_back(cInfo);
    className2Info.insert({cInfo->name, cInfo});
  }
  classInfoCount = classNameList.size();
}

/**
 * @fn      void setup()
 * @brief   Cleanup method for initializing variables and freeing memory
 *
 * @author  1sand0s
 */
static void setup() {
  /*
  // Iterate through classNameList and free memory
  for (int j = 0; j < classInfoCount; j++) {

    // Free memory of class qualified name
    if (classNameList[j].name != NULL) {
      free(classNameList[j].name);
      classNameList[j].name = NULL;
    }

    // Free memory for instances of classes
    if (classNameList[j].instanceCount > 0) {
      for (int k = 0; k < classNameList[j].instanceCount; k++) {

  // Free memory for references of instances
  if (classNameList[j].instances[k].referenceCount > 0) {
    if (classNameList[j].instances[k].references != NULL){
      free(classNameList[j].instances[k].references);
    }
    classNameList[j].instances[k].references = NULL;
    classNameList[j].instances[k].referenceCount = 0;
  }

  // Free memory for fields of instances
  if (classNameList[j].instances[k].fieldCount > 0) {
    for(int i = 0; i < classNameList[j].instances[k].fieldCount; i++){
      if(classNameList[j].instances[k].fields[i].valueCount > 0){
        if(classNameList[j].instances[k].fields[i].type ==
  Agent::JVMTI_EXTENDED_FIELD_TYPE_STRING){ for(int m = 0; m <
  classNameList[j].instances[k].fields[i].valueCount; m++){
        free(classNameList[j].instances[k].fields[i].value.string[m]);
      }
      free(classNameList[j].instances[k].fields[i].value.string);
      classNameList[j].instances[k].fields[i].value.string = NULL;
        }
        else{
      free(classNameList[j].instances[k].fields[i].value.primitive);
      classNameList[j].instances[k].fields[i].value.primitive = NULL;
        }
        classNameList[j].instances[k].fields[i].valueCount = 0;
      }
    }

    free(classNameList[j].instances[k].fields);
    classNameList[j].instances[k].fields = NULL;
    classNameList[j].instances[k].fieldCount = 0;
  }
      }
      free(classNameList[j].instances);
      free(classNameList[j].instanceTags);
      classNameList[j].instances = NULL;
      classNameList[j].instanceTags = NULL;
      classNameList[j].instanceCount = 0;
    }

    // Free memory of fields of class
    if (classNameList[j].fieldCount > 0) {
      for (int k = 0; k < classNameList[j].fieldCount; k++) {
  // Free memory of field name
  if (classNameList[j].fields[k].name != NULL){
    free(classNameList[j].fields[k].name);
  }
  classNameList[j].fields[k].name = NULL;

  // Free memory of field signature
  if (classNameList[j].fields[k].signature != NULL){
    free(classNameList[j].fields[k].signature);
  }
  classNameList[j].fields[k].signature = NULL;
      }
      if (classNameList[j].fields != NULL){
  free(classNameList[j].fields);
      }
      classNameList[j].fields = NULL;
      classNameList[j].fieldCount = 0;
    }
  }
  */
  /*Free memory of the object classNameList */
  /*if (classNameList != NULL){
    free(classNameList);
    }*/

  /* Free memory for whitelist of packages */
  /*if(whitePackageNameListCount > 0){
    for(int j = 0; j < whitePackageNameListCount; j++){
      free(whitePackageName[j]);
    }
    free(whitePackageName);
  }
  */
  /* Free memory for blacklist of packages */
  /*if(blackPackageNameListCount > 0){
    for(int j = 0; j < blackPackageNameListCount; j++){
      free(blackPackageName[j]);
    }
    free(blackPackageName);
    }*/

  /* Initialize to default values */
  classNameList.clear();
  writeToClassNameListInstances.clear();
  className2Info.clear();
#ifdef OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
  iInfo = NULL;
#endif
  nodeID = 0;
  classInfoCount = 0;
  objectWrittenToClassNameListCount = 0;
  whitePackageName.clear();
  blackPackageName.clear();
}

/**
 * @fn          void JNICALL exception_thrown(jvmtiEnv*,
 *                                            JNIEnv*,
 *                                            jthread,
 *                                            jmethodID,
 *                                            jlocation,
 *                                            jobject,
 *                                            jmethodID,
 *                                            jlocation)
 * @brief       Use dummy exception to trigger object classNameList
 * construction. Much more efficient than using method_entry events
 *
 * @author      1sand0s
 * @param       jvmti_env       The handle to JVMTI environment
 * @param       jni_env         The handle to JNI environment
 * @param       thread          The java thread which threw the exception
 * @param       method          The java method in which the exception was
 * thrown
 * @param       location
 * @param       exception       The type of exception
 * @param       catch_method    The java method in which the exceptionw as
 * caught
 * @param       catch_location
 */
void JNICALL exception_thrown(jvmtiEnv *jvmti_env, JNIEnv *jni_env,
                              jthread thread, jmethodID method,
                              jlocation location, jobject exception,
                              jmethodID catch_method,
                              jlocation catch_location) {
  string exceptionMethodName;
  jobject rootObject = nullptr;
  vector<jobject> results;
  jfieldID resultField;
  string cQuery;
  bool inMemory;
  bool forceGC;
  bool followRoot;
  bool whiteList;

  /* Variables to hold running time of different steps */
  TIMER::TimeInfo total_, setup_, forceGC_, getWhiteLists_, getBlackLists_,
      iterateHeap_Init_, getLoadedClasses_, iterateHeap_Assign_,
      sortElementAscendingTag_, followReferences_, assignSuperClass_,
      assignFieldNames_, assignWriteToClassNameListToReferences_,
      writeClassNameListToCsv_, *writeClassNameListToCsv_Internal_;
  /* Get name of method throwing exception */

  /* If exception is thrown from method "setupClassNameList" then proceed with
   * classNameList construction */
  if (agent->getMethodName(method) ==
      OGO::GRAPH_EXCEPTION_TRIGGER_METHOD_NAME) {
    agent->setJNIEnv(jni_env);
#ifdef DEBUG_PRINT_EXCEPTION_CLASS_
    cout << "Debug : Exception Class Name : " << agent->getClassName(exception)
         << "\n";
#endif
    TIMER_START_(total_)
    inMemory = agent->evaluateBooleanObject(agent->getInstanceObjectField(
        exception, OGO::CYPHER_QUERY_IN_MEMORY_FIELD, "Ljava/lang/Boolean;"));
    forceGC = agent->evaluateBooleanObject(agent->getInstanceObjectField(
        exception, OGO::CYPHER_QUERY_FORCE_GC_FIELD, "Ljava/lang/Boolean;"));
    followRoot = agent->evaluateBooleanObject(agent->getInstanceObjectField(
        exception, OGO::CYPHER_QUERY_FOLLOW_ROOT_FIELD, "Ljava/lang/Boolean;"));
    whiteList = agent->evaluateBooleanObject(agent->getInstanceObjectField(
        exception, OGO::CYPHER_QUERY_WHITELIST_FIELD, "Ljava/lang/Boolean;"));
    // cout << "In memory " << inMemory << endl;
    rootObject = agent->getInstanceObjectField(
        exception, OGO::CYPHER_QUERY_ROOT_OBJECT_FIELD, "Ljava/lang/Object;");
    TIMER_START_(setup_)
    /* Frees memory if classNameList(classInfo*) is non-NULL and initializes
     * all globals*/
    setup();
    TIMER_END_(setup_)
    if (inMemory) {
      cQuery =
          agent->evaluateStringObject((jstring)agent->getInstanceObjectField(
              exception, OGO::CYPHER_QUERY_STRING_FIELD, "Ljava/lang/String;"));
      // cout << "Cypher Query String " << cQuery << endl;
    }
    /* Force GC to reduce objects in the heap, this is an optimization that
     * improves the performance of heapIteration and FollowReferences callbacks
     */
    TIMER_START_(forceGC_)
    if (forceGC) {
      agent->forceGC();
    }
    TIMER_END_(forceGC_)

    TIMER_START_(getWhiteLists_)
    if (whiteList) {
      /* Read Whitelist of Packages */
      agent->getInstanceStringArrayField(exception, OGO::WHITE_LIST_FIELD,
                                         "[Ljava/lang/Object;",
                                         whitePackageName);
    }
    //        cout << "WhiteLists : " << whitePackageName[0] << endl;
    TIMER_END_(getWhiteLists_)

    TIMER_START_(getBlackLists_)
    /* Read Blacklist of Packages */
    agent->getInstanceStringArrayField(exception, OGO::BLACK_LIST_FIELD,
                                       "[Ljava/lang/Object;", blackPackageName);
    agent->getInstanceStringArrayField(
        exception, OGO::EXCLUDE_FROM_BLACK_LIST_FIELD, "[Ljava/lang/Object;",
        excludeblackPackageName);

    TIMER_END_(getBlackLists_)

    /* Use raw monitor to ensure single threaded execution */
    agent->rawMonitorEnter();
    TIMER_START_(iterateHeap_Init_)
    /* Iterate through heap and assign 0 tag to every instances
     * (Initialization Step) */
    agent->iterateThroughHeap(&iterationSetupCallbacks,
                              JVMTI_HEAP_FILTER_UNTAGGED);
    TIMER_END_(iterateHeap_Init_)

    TIMER_START_(getLoadedClasses_)
    /* Populate classNameList with all loaded classes barring the ones
     * whose fully qualified class name matches part of those in
     * the blacklist of packages */
    getLoadedClasses();
    TIMER_END_(getLoadedClasses_)

    TIMER_START_(iterateHeap_Assign_)
    /* Iterate through heap and assign tags to encountered instances */
    agent->iterateThroughHeap(&iterationCallbacks,
                              JVMTI_HEAP_FILTER_CLASS_UNTAGGED);
    TIMER_END_(iterateHeap_Assign_)

    TIMER_START_(sortElementAscendingTag_)
    /* Sort instances according to ascending order of instanceTags
     * This enables using binary search to retrieve instance with a
     * particular tag */
    for (int j = 0; j < classInfoCount; j++) {
      if (!classNameList[j]->isSorted) {
        classNameList[j]->sortElementAscendingTag();
      }

      for (int i = 0; i < classNameList[j]->instances.size(); i++) {
        classNameList[j]->instances[i]->tag = classNameList[j]->instanceTags[i];
      }
    }
    TIMER_END_(sortElementAscendingTag_)
    if (!followRoot) {
      rootObject = nullptr;
    }

    TIMER_START_(followReferences_)
    /* Follow instance references and use tags to identify relations */
    agent->followReferences(&traversalCallbacks, JVMTI_HEAP_FILTER_UNTAGGED,
                            rootObject);
    TIMER_END_(followReferences_)

    TIMER_START_(assignWriteToClassNameListToReferences_)
    if (!inMemory) {
      for (InstanceInfo *iInfo : writeToClassNameListInstances) {
        assignWriteToClassNameListToReferences(iInfo);
        iInfo->isVisited = true;
      }
    }
    TIMER_END_(assignWriteToClassNameListToReferences_)

    TIMER_START_(assignFieldNames_)
    assignFieldNames();
    TIMER_END_(assignFieldNames_)

#ifdef DEBUG_PRINT_CLASSNAMELIST_
    printClassNameList();
#endif

    if (inMemory) {
      computeQuery(cQuery, results);
      if (!results.empty()) {
        agent->setObjectReferenceField(
            exception, agent->createObjectArray(results),
            OGO::CYPHER_QUERY_RESULT_FIELD, "[Ljava/lang/Object;");
      }
    } else {
      writeClassNameListToCsv_Internal_ = static_cast<TIMER::TimeInfo *>(
          calloc(6, sizeof(struct TIMER::TimeInfo)));
      TIMER_START_(writeClassNameListToCsv_)
      writeClassNameListToCsv(&writeClassNameListToCsv_Internal_);
      TIMER_END_(writeClassNameListToCsv_)
    }
    TIMER_END_(total_)

#ifdef PROFILE_PRINT_FILE_
    FILE *pFile;
    char profileFile[40];
    struct tm *timenow;
    time_t now = time(nullptr);
    timenow = gmtime(&now);
    strftime(profileFile, sizeof(profileFile),
             "PROFILE_HOQ_C_%Y-%m-%d_%H:%M:%S", timenow);
    pFile = fopen(profileFile, "w");
    if (pFile == nullptr) {
      printf("Failed to create file for writing profile data %s\n",
             profileFile);
      exit(1);
    }
    fprintf(pFile,
            "Setup, %f\n"
            "Forcing_GC, %f\n"
            "Iterate_Heap_Initialization, %f\n"
            "Get_Loaded_Classes, %f\n"
            "Iterate_Heap_and_Assign_Tags, %f\n"
            "Sort_Instances_by_ascending_tag_order, %f\n"
            "Follow_References, %f\n"
            "Assign_SuperClass_Field_Names, %f\n"
            "Assign_Field_Names, %f\n"
            "Write_ClassNameList_To_CSV, %f\n"
            "Number_of_loaded_classes, %lu\n"
            "Number_of_Objects_in_the_heap, %d\n"
            "Number_of_Objects_written_to_graph, %d\n"
            "Total, %f\n",
            DURATION_(setup_), DURATION_(forceGC_),
            DURATION_(iterateHeap_Init_), DURATION_(getLoadedClasses_),
            DURATION_(iterateHeap_Assign_), DURATION_(sortElementAscendingTag_),
            DURATION_(followReferences_), DURATION_(assignSuperClass_),
            DURATION_(assignFieldNames_), DURATION_(writeClassNameListToCsv_),
            classInfoCount, nodeID, objectWrittenToClassNameListCount,
            DURATION_(total_));
    fclose(pFile);
#endif

    /* Free memory of object classNameList (classInfo*)*/
    setup();

    // free(writeClassNameListToCsv_Internal_);

    /* Exit critical section */
    agent->rawMonitorExit();
  }
}

void acceptVisitor(const string &query,
                   antlr4::tree::ParseTreeVisitor *visitor) {
  ANTLRInputStream input(query);
  CypherLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  tokens.fill();
#ifdef DEBUG_PRINT_CYPHER_TOKENS_
  for (auto token : tokens.getTokens()) {
    printf("%s\n", token->toString().c_str());
  }
#endif
  CypherParser parser(&tokens);
  parser.oC_Cypher()->accept(visitor);
}

/**
 * @fn          void computeQuery(const string& query,
 *                                vector<jobject>& result)
 *
 * @author      1sand0s
 * @param       query      The given Cypher Query
 * @param       result     The result returned by RETURN clause
 */
void computeQuery(const string &query, vector<jobject> &result) {
  map<string, vector<pair<InstanceInfo *, jobject>>> nodeObjectMap;

  MatchClauseVisitor matchClauseVisitor(agent, &nodeObjectMap);
  acceptVisitor(query, &matchClauseVisitor);
  cout << "After matching " << nodeObjectMap.size() << "\n";

  CreateClauseVisitor createClauseVisitor(agent, &nodeObjectMap);
  acceptVisitor(query, &createClauseVisitor);
  cout << "After creating " << nodeObjectMap.size() << "\n";

  CallClauseVisitor callClauseVisitor(agent, &nodeObjectMap);
  acceptVisitor(query, &callClauseVisitor);
  cout << "After calling " << nodeObjectMap.size() << "\n";

  MergeClauseVisitor mergeClauseVisitor(agent, &nodeObjectMap);
  acceptVisitor(query, &mergeClauseVisitor);
  cout << "After merging " << nodeObjectMap.size() << "\n";

  ReturnClauseVisitor returnClauseVisitor(agent, &nodeObjectMap, &result);
  acceptVisitor(query, &returnClauseVisitor);
  cout << "After returning " << result.size() << "\n";
}

/**
 * @fn          static void setupCallbacks()
 * @brief       Allocates memory to jvmti callback structs
 *
 * @author      1sand0s
 */
static void setupCallbacks() {
  /* Allocate memory for struct defining callback functions */
  (void)memset(&iterationCallbacks, 0, sizeof(iterationCallbacks));
  iterationCallbacks.heap_iteration_callback = &heap_iterationCallback;

  /* Allocate memory for struct defining callback functions */
  (void)memset(&iterationSetupCallbacks, 0, sizeof(iterationSetupCallbacks));
  iterationSetupCallbacks.heap_iteration_callback =
      &heap_iterationSetupCallback;

  /* Allocate memory for struct defining callback functions */
  (void)memset(&traversalCallbacks, 0, sizeof(traversalCallbacks));
  traversalCallbacks.heap_reference_callback = &heap_referenceCallback;
  traversalCallbacks.primitive_field_callback = &heap_primitiveFieldCallback;
  traversalCallbacks.string_primitive_value_callback =
      &heap_stringFieldCallback;
  traversalCallbacks.array_primitive_value_callback = &heap_arrayFieldCallback;
}

/**
 * @fn          JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM*,
 *                                                  char*,
 *						    void*)
 * @brief       First method called when agent is loaded after staring JVM
 *
 * @author      1sand0s
 * @param       vm
 * @param       options
 * @param       reserved
 */
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  jint rc;
  jvmtiEventCallbacks callbacks;
  jvmtiCapabilities capabilities;
  jvmtiEnv *jvmti_env;

  /* Get JVMTI environment */
  rc = vm->functions->GetEnv(vm, reinterpret_cast<void **>(&jvmti_env),
                             jvmtiVersion);
  if (rc != JNI_OK || jvmti_env == nullptr) {
    printf("ERROR: Unable to create jvmtiEnv, GetEnv failed, error=%d\n", rc);
    return -1;
  }

  /* Define all capabilities going to be used */
  (void)memset(&capabilities, 0, sizeof(jvmtiCapabilities));
  capabilities.can_generate_exception_events = 1;
  capabilities.can_tag_objects = 1;
  capabilities.can_access_local_variables = 1;
  capabilities.can_get_source_file_name = 1;

  /* Set callbacks for JVMTI events */
  (void)memset(&callbacks, 0, sizeof(jvmtiEventCallbacks));
  callbacks.Exception = &exception_thrown;

  /* Create and setup agent */
  agent = new Agent(jvmti_env);
  agent->setCapabilities(capabilities);
  agent->setEventCallbacks(callbacks);

  setupCallbacks();

  agent->createRawMonitor();
  return 0;
}

/**
 * @fn          JNIEXPORT void JNICALL Agent_OnUnload(JavaVM*)
 * @brief       Called when JVM is killed
 *
 * @author      1sand0s
 * @param       vm
 */
JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm) {
  /* No point freeing memory since VM is killed anyway, but doing it for good
   * practice */
  setup();
}
