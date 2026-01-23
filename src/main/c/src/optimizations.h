#ifndef OPTIMIZATIONS_JVMTI_H_
#define OPTIMIZATIONS_JVMTI_H_

/**
 * @def define OPTIMIZATION_FORCE_GC_
 * @brief Uncomment to turn on forcing GC
 *        before object graph construction
 *
 */
#define OPTIMIZATION_FORCE_GC_

/**
 * @def define OPTIMIZATION_INSTANCEINFO_REALLOC_
 * @brief Uncomment to realloc instanceInfo* of classInfo
 *        inside heap_IterationCallback.
 *
 *        Having 2 reallocs (instanceInfo* and instanceTags*)
 *        inside heap_IterationCallback degrades performance
 *        hence this optimization
 */
//#define OPTIMIZATION_INSTANCEINFO_REALLOC_

/**
 * @def define OPTIMIZATION_TRAVERSE_PASSED_OBJECTS_ONLY_
 * @brief Uncomment to follow objects from GC roots
 *
 *        If no object is provided to followReferences
 *        then the roots for GC are used to traverse the object
 *        graph but this is expensive so use the hashes contained
 *        in the thrown exception object (GraphTriggerException) to
 *        traverse instead
 */
#define OPTIMIZATION_TRAVERSE_PASSED_OBJECTS_ONLY_

/**
 * @def define OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_
 * @brief Uncomment to enable caching of retrieved instanceInfo
 *        inside heap_referenceCallback to avoided repeated retrievals
 *
 *
 */
/*#define OPTIMIZATION_CACHE_RETRIEVED_INSTANCE_INFO_*/

/**
 * @def define OPTIMIZATION_WHITELIST_INSTANCE_CLASSES_
 * @brief Uncomment to write every instance of every class
 *        to CSV
 *
 *        The primary bottleneck in cyphssert end to end run is
 *        running queries on Neo4J database. If too many irrelevant
 *        objects are written to CSV then the total size of the database
 *        can increase dramatically making running queries extremely inefficient
 */
#define OPTIMIZATION_WHITELIST_INSTANCE_CLASSES_

/**
 * @def define OPTIMIZATION_FOLLOW_REFERENCES_TERMINATE_IRRELEVANT_PATHS_
 * @see https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#jvmtiHeapVisitControl
 * @brief Uncomment to follow references of all objects in the JVM's heap
 *
 *        Although we do filter out objects that are untagged/or whose
 *        classes are untagged, they are still reported inside the
 *        follow references callback. This degrades performance since
 *        irrelevant object references are reported. Use this to selectively
 *        terminates such paths using JVMTI_VISIT_OBJECTS and JVMTI_VISIT_ABORT
 */
//#define OPTIMIZATION_FOLLOW_REFERENCES_TERMINATE_IRRELEVANT_PATHS_

#endif
