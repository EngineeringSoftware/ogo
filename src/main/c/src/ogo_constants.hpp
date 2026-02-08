#pragma once

#include <string>

/**
 * @def BOOLEAN_CLASS
 * @brief Fully qualified class name of the Boolean class of Java
 *
 */
#define BOOLEAN_CLASS "java/lang/Boolean"

/**
 * @def BYTE_CLASS
 * @brief Fully qualified class name of the Byte class of Java
 *
 */
#define BYTE_CLASS "java/lang/Byte"

/**
 * @def CHARACTER_CLASS
 * @brief Fully qualified class name of the Character class of Java
 *
 */
#define CHARACTER_CLASS "java/lang/Character"

/**
 * @def SHORT_CLASS
 * @brief Fully qualified class name of the Short class of Java
 *
 */
#define SHORT_CLASS "java/lang/Short"

/**
 * @def INTEGER_CLASS
 * @brief Fully qualified class name of the Integer class of Java
 *
 */
#define INTEGER_CLASS "java/lang/Integer"

/**
 * @def LONG_CLASS
 * @brief Fully qualified class name of the Long class of Java
 *
 */
#define LONG_CLASS "java/lang/Long"

/**
 * @def FLOAT_CLASS
 * @brief Fully qualified class name of the Float class of Java
 *
 */
#define FLOAT_CLASS "java/lang/Float"

/**
 * @def DOUBLE_CLASS
 * @brief Fully qualified class name of the Double class of Java
 *
 */
#define DOUBLE_CLASS "java/lang/Double"

/**
 * @def STRING_CLASS
 * @brief Fully qualified class name of the String class of Java
 *
 */
#define STRING_CLASS "Ljava/lang/String;"

/**
 * @def OBJECT_CLASS
 * @brief Fully qualified class name of Obejct Class of Java
 *
 */
#define OBJECT_CLASS "java/lang/Object"

/**
 * @def OBJECT_ARRAY_CLASS
 * @brief Fully qualified class name of Array of Obejct Class of Java
 *
 */
#define OBJECT_ARRAY_CLASS "[Ljava/lang/Object;"

/**
 * @def BOOLEAN_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Boolean class
 *
 */
#define BOOLEAN_CLASS_CONSTRUCTOR "(Z)V"

/**
 * @def BYTE_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Byte class
 *
 */
#define BYTE_CLASS_CONSTRUCTOR "(B)V"

/**
 * @def CHARACTER_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Character class
 *
 */
#define CHARACTER_CLASS_CONSTRUCTOR "(C)V"

/**
 * @def SHORT_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Short class
 *
 */
#define SHORT_CLASS_CONSTRUCTOR "(S)V"

/**
 * @def INTEGER_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Integer class
 *
 */
#define INTEGER_CLASS_CONSTRUCTOR "(I)V"

/**
 * @def LONG_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Long class
 *
 */
#define LONG_CLASS_CONSTRUCTOR "(J)V"

/**
 * @def FLOAT_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Float class
 *
 */
#define FLOAT_CLASS_CONSTRUCTOR "(F)V"

/**
 * @def DOUBLE_CLASS_CONSTRUCTOR
 * @brief The constructor signature for java/lang/Double class
 *
 */
#define DOUBLE_CLASS_CONSTRUCTOR "(D)V"

/**
 * @def STRING_CLASS_CONSTRUCTOR
 * @brief The constructor signature for Ljava/lang/String class
 *
 */
#define STRING_CLASS_CONSTRUCTOR "(Ljava/lang/String;)V"

namespace CSV_FILES {

const std::string REFERENCE_RELATION_FILE_SUFFIX = "_Neo4JRelations.csv";

const std::string FIELD_PROPERTIES_FILE_SUFFIX = "_Neo4JProperties.csv";
} // namespace CSV_FILES

namespace OGO {
/**
 * @def GRAPH_EXCEPTION_TRIGGER_METHOD_NAME
 * @brief The method in OGO inside which when an exception is
 *        thrown, we start constructing the object graph
 *
 */
const std::string GRAPH_EXCEPTION_TRIGGER_METHOD_NAME = "setupGraph";

/**
 * @def CYPHER_QUERY_STRING_FIELD
 * @brief The String field in GraphTriggerException class
 *        which contains the cypher query
 *
 */
const std::string CYPHER_QUERY_STRING_FIELD = "cQuery";

/**
 * @def CYPHER_QUERY_RESULT_FIELD
 * @brief The Object[] field in GraphTriggerException class
 *        which will contain the result of executing the Query
 *
 */
const std::string CYPHER_QUERY_RESULT_FIELD = "queryResult";

/**
 * @def CYPHER_QUERY_ROOT_OBJECT_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string CYPHER_QUERY_ROOT_OBJECT_FIELD = "root";

/**
 * @def CYPHER_QUERY_IN_MEMORY_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string CYPHER_QUERY_IN_MEMORY_FIELD = "inMemory";

/**
 * @def CYPHER_QUERY_IN_MEMORY_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string CYPHER_QUERY_FORCE_GC_FIELD = "forceGC";

/**
 * @def CYPHER_QUERY_IN_MEMORY_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string CYPHER_QUERY_FOLLOW_ROOT_FIELD = "followRoot";

/**
 * @def CYPHER_QUERY_IN_MEMORY_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string CYPHER_QUERY_WHITELIST_FIELD = "whitelist";

/**
 * @def WHITE_LIST_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string WHITE_LIST_FIELD = "whiteLists";

/**
 * @def BLACK_LIST_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string BLACK_LIST_FIELD = "blackLists";

/**
 * @def BLACK_LIST_FIELD
 * @brief The Object field in GraphTriggerException class
 *        which will contxain the user supplied root object
 *        for graph construction
 *
 */
const std::string EXCLUDE_FROM_BLACK_LIST_FIELD = "excludeBlackListing";
} // namespace OGO
