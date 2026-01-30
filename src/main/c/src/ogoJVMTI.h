#pragma once

#include <map>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>

#include "Timer.h"
#include "agent_util.h"
#include "jni.h"
#include "jni_util.h"
#include "ogoConstants.h"
#include "optimizations.h"
#include "string_utils.h"

#include "classInfo.h"
#include "fieldInfo.h"
#include "instanceInfo.h"
#include "methodInfo.h"
#include "referenceInfo.h"

using namespace graph;
using namespace std;
using namespace util;

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
static void check_jvmti_error(jvmtiError errnum, const char *str);

/**
 * @fn void getObjectsWithTagscInfo(jobject**, long**, int*, timeInfo*)
 * @brief
 *
 * @param objects
 * @param tags
 * @param tagCount
 * @param tInfo
 */
static void getObjectsWithTagscInfo(jobject **objects, long **tags,
                                    int *tagCount, TIMER::TimeInfo *tInfo);

/**
 * @fn      void writeGraphToCsv(timeInfo*)
 * @brief   write object graph (cInfo* graph) to CSV files
 *
 * @author  1sand0s
 * @param   tInfo   timeInfo array for profiling methods calls
 */
static void writeGraphToCsv(TIMER::TimeInfo **tInfo);

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
                                    jint length, void *user_data);

/**
 * @fn          static jint JNICALL heap_iterationSetupCallback(jlong, jlong,
 * jlong*, jint, void*)
 * @brief       This sets the tag of all objects in the heap to 0. This serves
 * as an intialization step before we construct the object graph
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
                                         void *user_data);

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
    jlong *referrer_tag_ptr, jint length, void *user_data);

/**
 * @fn          jint JNICALL heap_primitiveFieldCallback(jvmtiHeapReferenceKind,
 *                                                       const
 * jvmtiHeapReferenceInfo*, jlong, jlong*, jvalue, jvmtiPrimitiveType, void*)
 * @brief       Get primitive fields of an instance during follow references.
 * Used to get values of th primitive fields.
 *
 * @author      1sand0s
 * @param       reference_kind      Enum describing the type of reference
 * between owning instance and primitive field
 * @param       reference_info
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
                                         void *user_data);

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
                                      jint value_length, void *user_data);

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
 * @param       value               value of the string field
 * @param       value_length  Length of the string
 * @param       user_data           Not used
 */
jint JNICALL heap_arrayFieldCallback(jlong class_tag, jlong size,
                                     jlong *tag_ptr, jint element_count,
                                     jvmtiPrimitiveType element_type,
                                     const void *elements, void *user_data);

/**
 * @fn void embedStringInstances(const instanceInfo*, instanceInfo*, const int,
 * classInfo*)
 * @brief
 *
 * @param stringInstance
 * @param stringFieldOwningInstance
 * @param stringFieldIndex
 * @param classInstance
 */
static void embedStringInstances(const InstanceInfo *stringInstance,
                                 InstanceInfo *stringFieldOwningInstance,
                                 const int stringFieldIndex,
                                 vector<FieldInfo *> &inheritedFields);

/**
 * @fn void embedBoxedPrimitiveInstances(const instanceInfo*, instanceInfo*,
 * const int, classInfo*)
 * @brief
 *
 * @param boxedPrimitiveInstance
 * @param boxedPrimitiveFieldOwningInstance
 * @param boxedPrimitiveFieldIndex
 * @param classInstance
 */
static void
embedBoxedPrimitiveInstances(InstanceInfo *boxedPrimitiveInstance,
                             InstanceInfo *boxedPrimitiveFieldOwningInstance,
                             const int boxedPrimitiveFieldIndex,
                             const vector<FieldInfo *> &inheritedFields);

/**
 * @fn          void assignFieldNames(instanceInfo*, int, classInfo* )
 * @brief       Assign field names to objects stored in graph for
 *              fields of type JVMTI_HEAP_REFERENCE_FIELD and
 *              JVMTI_HEAP_REFERENCE_STATIC_FIELD
 *
 * @author      1sand0s
 * @param referrer        The referrer instanceInfo in the relation
 * @param referenceIndex  The index of the current referenceInfo element
 * @param classInstance   The Class.Class.java instance of referrer instance
 */
static void
assignFieldNamesToReferenceFields(const InstanceInfo *referrer, int referenceIndex,
                                  map<int, FieldInfo *> &inheritedFields);

/**
 * @fn          void assignFieldNames()
 * @brief       Assign field names to objects stored in graph
 *
 * @author      1sand0s
 */
static void assignFieldNames();

/**
 * @fn void assignWriteToGraphToReferences()
 * @brief   Marks all references of instance to be written
 *          to graph
 *
 */
static void assignWriteToGraphToReferences();

/**
 * @fn          void printGraph()
 * @brief       Prints the object graph
 *
 * @author      1sand0s
 */
static void printGraph();

/**
 * @fn          void getFieldInfo(const jfieldID, const jclass, char**, char**,
 * int*)
 * @brief       Get class fields, the jfieldID can be used to get field names
 * later on (see method assignFieldNames()). This is used to bypass alternative
 *              implementation using jvmti function GetObjectsWithTag which is
 * extremely inefficient
 *
 * @author      1sand0s
 * @param       field       field ID of field whose information is to be
 * retrieved
 * @param       klass       class where the fields is defined
 * @param       name        name of the field (allocated within function)
 * @param       signature   signature of the field (allocated within function)
 * @param       modifiers   modifiers of the field (allocated within function)
 */
static void getFieldInfo(const jfieldID field, const jclass klass, string &name,
                         string &signature, int &modifiers);

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
                                  int &fieldCount);

/**
 * @fn          void getLoadedClasses()
 * @brief       Get information regarding all loaded classes in the JVM and
 * store in graph. Unique tags are also assigned to Class.java objects
 *
 * @author      1sand0s
 */
static void getLoadedClasses();

/**
 * @fn      void setup()
 * @brief   Cleanup method for initializing variables and freeing memory
 *
 * @author  1sand0s
 */
static void setup();

/**
 * @fn void getBlackLists(JNIEnv*, jmethodID)
 * @brief   Reads file CyphssertBlacklist.txt to find out blacklist
 *          packages. Instances of classes belonging to these packages
 *          will be definitely excluded from the object graph
 */
static void getBlackLists();

/**
 * @fn void getWhiteLists(JNIEnv*, jmethodID)
 * @brief   Reads file CyphssertWhitelist.txt to find out whitelist
 *          packages. Only instances of classes belonging to these packages
 *          will be included in the object graph
 */
static void getWhiteLists();

void computeQuery(const string &query, vector<jobject> &result);

/**
 * @fn          void JNICALL exception_thrown(jvmtiEnv*,
 *                                            JNIEnv*,
 *                                            jthread,
 *                                            jmethodID,
 *                                            jlocation,
 *                                            jobject,
 *                                            jmethodID,
 *                                            jlocation)
 * @brief       Use dummy exception to trigger object graph construction. Much
 * more efficient than using method_entry events
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
                              jmethodID catch_method, jlocation catch_location);

/**
 * @fn          JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM*, char*, void*)
 * @brief       First method called when agent is loaded after staring JVM
 *
 * @author      1sand0s
 * @param       vm
 * @param       options
 * @param       reserved
 */
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved);

/**
 * @fn          JNIEXPORT void JNICALL Agent_OnUnload(JavaVM*)
 * @brief       Called when JVM is killed
 *
 * @author      1sand0s
 * @param       vm
 */
JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm);
