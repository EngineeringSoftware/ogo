#pragma once

#include <jni_md.h>
#include <jni.h>
#include <jvmti.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdarg>
#include <iostream>

using namespace std;

namespace util {
class Agent {
public:

#define SINGLE_QUOTES(x) ((#x)[0])

#define STRINGIFY(x) #x

    /**
      * @def JVMTI_PRIMITIVE_TABLE_
      * @brief Xmacro for handling printing, malloc'ing etc different
      *        JVMTI Primitive types
      *
      */
#define JVMTI_PRIMITIVE_TABLE_\
  ENTRY(JVMTI_PRIMITIVE_TYPE_BOOLEAN,%x ,  z , char    , Z, BOOLEAN_CLASS, BOOLEAN_CLASS_CONSTRUCTOR) \
    ENTRY(JVMTI_PRIMITIVE_TYPE_BYTE   ,%d ,  b , u_int8_t, B, BYTE_CLASS, BYTE_CLASS_CONSTRUCTOR)       \
    ENTRY(JVMTI_PRIMITIVE_TYPE_CHAR   ,%c ,  c , char    , C, CHARACTER_CLASS, CHARACTER_CLASS_CONSTRUCTOR)	\
    ENTRY(JVMTI_PRIMITIVE_TYPE_SHORT  ,%d ,  s , short   , S, SHORT_CLASS, SHORT_CLASS_CONSTRUCTOR)	\
    ENTRY(JVMTI_PRIMITIVE_TYPE_INT    ,%d ,  i , int     , I, INTEGER_CLASS, INTEGER_CLASS_CONSTRUCTOR)	\
    ENTRY(JVMTI_PRIMITIVE_TYPE_LONG   ,%ld,  j , long    , J, LONG_CLASS, LONG_CLASS_CONSTRUCTOR)	\
    ENTRY(JVMTI_PRIMITIVE_TYPE_FLOAT  ,%f ,  f , float   , F, FLOAT_CLASS, FLOAT_CLASS_CONSTRUCTOR)	\
    ENTRY(JVMTI_PRIMITIVE_TYPE_DOUBLE ,%f ,  d , double  , D, DOUBLE_CLASS, DOUBLE_CLASS_CONSTRUCTOR)	\

    /**
    * @def JVMTI_PRIMITIVE_TABLE_
    * @brief Xmacro for handling printing, malloc'ing etc different
    *        JVMTI Primitive types
    *
    */
#define JAVA_METHOD_RETURN_TYPE_\
  ENTRY(Z, JBOOLEAN)						\
  ENTRY(B, JBYTE)									\
  ENTRY(C, JCHAR)										\
  ENTRY(S, JSHORT)											\
  ENTRY(I, JINT)													\
  ENTRY(J, JLONG)												\
  ENTRY(F, JFLOAT)												\
  ENTRY(D, JDOUBLE)											\
  ENTRY(L, JOBJECT)											\
  ENTRY(V, JVOID)\


#define JAVA_OBJECT_CREATION_TYPE_							\
  ENTRY(Boolean, jboolean, java/lang/Boolean, Z, JBOOLEAN)					\
  ENTRY(Byte, jbyte, java/lang/Byte, B, JBYTE)								\
  ENTRY(Char, jchar, java/lang/Character, C, JCHAR)							\
  ENTRY(Short, jshort, java/lang/Short, S, JSHORT)								\
  ENTRY(Int, jint, java/lang/Integer, I, JINT)								\
  ENTRY(Long, jlong, java/lang/Long, J, JLONG)								\
  ENTRY(Float, jfloat, java/lang/Float, F, JFLOAT)								\
  ENTRY(Double, jdouble, java/lang/Double, D, JDOUBLE)							\


#define JAVA_METHOD_CALLS_							\
  ENTRY(Boolean, jboolean, JBOOLEAN)								\
  ENTRY(Byte, jbyte, JBYTE)												\
  ENTRY(Char, jchar, JCHAR)													\
  ENTRY(Short, jshort, JSHORT)													\
  ENTRY(Int, jint, JINT)														\
  ENTRY(Long, jlong, JLONG)													\
  ENTRY(Float, jfloat, JFLOAT)													\
  ENTRY(Double, jdouble, JDOUBLE)												\
  ENTRY(Object, jobject, JOBJECT)												\
  ENTRY(Void, void, JVOID)													\


#define JAVA_METHOD_OBJECT_CALLS_							\
  ENTRY(Boolean, jboolean, JBOOLEAN, z)									\
  ENTRY(Byte, jbyte, JBYTE, b)											\
  ENTRY(Char, jchar, JCHAR, c)											\
  ENTRY(Short, jshort, JSHORT, s)											\
  ENTRY(Int, jint, JINT, i)												\
  ENTRY(Long, jlong, JLONG, j)											\
  ENTRY(Float, jfloat, JFLOAT, f)											\
  ENTRY(Double, jdouble, JDOUBLE, d)										\

    /**
       * @enum jvmtiExtendedFieldType
       * @brief Enum type for extending jvmtiPrimitiveType to handle embedding of
       *        strings (instances of java.lang.String)
       *
       * see https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#jvmtiPrimitiveType
       * for enum numbering. Assignment of 69 for 'JVMTI_EXTENDED_FIELD_TYPE_STRING' is user
       * defined (recheck for collision)
       */
    enum jvmtiExtendedFieldType {
        JVMTI_EXTENDED_FIELD_TYPE_BOOLEAN = 90, /**< JVMTI_EXTENDED_FIELD_TYPE_BOOLEAN */
        JVMTI_EXTENDED_FIELD_TYPE_BYTE    = 66, /**< JVMTI_EXTENDED_FIELD_TYPE_BYTE    */
        JVMTI_EXTENDED_FIELD_TYPE_CHAR    = 67, /**< JVMTI_EXTENDED_FIELD_TYPE_CHAR    */
        JVMTI_EXTENDED_FIELD_TYPE_SHORT   = 83, /**< JVMTI_EXTENDED_FIELD_TYPE_SHORT   */
        JVMTI_EXTENDED_FIELD_TYPE_INT     = 73, /**< JVMTI_EXTENDED_FIELD_TYPE_INT     */
        JVMTI_EXTENDED_FIELD_TYPE_LONG    = 74, /**< JVMTI_EXTENDED_FIELD_TYPE_LONG    */
        JVMTI_EXTENDED_FIELD_TYPE_FLOAT   = 70, /**< JVMTI_EXTENDED_FIELD_TYPE_FLOAT   */
        JVMTI_EXTENDED_FIELD_TYPE_DOUBLE  = 68, /**< JVMTI_EXTENDED_FIELD_TYPE_DOUBLE  */
        JVMTI_EXTENDED_FIELD_TYPE_STRING  = 69  /**< JVMTI_EXTENDED_FIELD_TYPE_STRING  */
    };

    enum MethodReturnType {
        JBOOLEAN = 0,
        JBYTE = 1,
        JCHAR = 2,
        JSHORT = 3,
        JINT = 4,
        JLONG = 5,
        JFLOAT = 6,
        JDOUBLE = 7,
        JOBJECT = 8,
        JVOID = 9,
        JNULL = 10,
        JSTRING = 11,
        JBOOLEAN_ARRAY = 12,
        JBYTE_ARRAY = 13,
        JCHAR_ARRAY = 14,
        JSHORT_ARRAY = 15,
        JINT_ARRAY = 16,
        JLONG_ARRAY = 17,
        JFLOAT_ARRAY = 18,
        JDOUBLE_ARRAY = 19,
        JSTRING_ARRAY = 20,
    };

    const string DEFAULT_CONSTRUCTOR_DESCRIPTOR = "()V";
    const string DEFAULT_CONSTRUCTOR_NAME = "<init>";

    Agent(jvmtiEnv* jvmti_env);

    static jlong getIntegerTypesAsLong(const jvalue& value,
                                       const enum MethodReturnType& type);
    static string fieldTypeToString(const enum MethodReturnType& type);
    static bool isIntegerType(const enum MethodReturnType& type);
    static bool isFloatType(const enum MethodReturnType& type);
    static bool isNumberType(const enum MethodReturnType& type);
    static jdouble getFloatTypesAsDouble(const jvalue& value, const enum MethodReturnType& type);
    static MethodReturnType getPriorityType(const enum MethodReturnType& type1, const enum MethodReturnType& type2);
    static jdouble getNumberTypeAsDouble(const jvalue& value, const enum MethodReturnType& type);
    static jlong getNumberTypeAsLong(const jvalue& value, const enum MethodReturnType& type);
    template<typename T>
    static jvalue castIntegerType(const T t, const enum MethodReturnType& castedType) {
        jvalue castedValue;
        switch(castedType) {
        case JBYTE:
            castedValue.b = (jbyte) t;
            break;
        case JSHORT:
            castedValue.s = (jshort) t;
            break;
        case JINT:
            castedValue.i = (jint) t;
            break;
        case JLONG:
            castedValue.j = (jlong) t;
            break;
        default:
            throw runtime_error("Error : Must be JBYTE, JSHORT, JINT or JLONG");
            break;
        }
        return castedValue;
    }

    template<typename T>
    static jvalue castFloatType(const T t, const enum MethodReturnType& castedType) {
        jvalue castedValue;
        switch(castedType) {
        case JFLOAT:
            castedValue.f = (jfloat) t;
            break;
        case JDOUBLE:
            castedValue.d = (jdouble) t;
            break;
        default:
            throw runtime_error("Error : Must be JFLOAT or JDOUBLE");
            break;
        }
        return castedValue;
    }

    void setJNIEnv(JNIEnv* jni_env);
    void setCapabilities(const jvmtiCapabilities &capabilities);
    void setEventCallbacks(const jvmtiEventCallbacks &callbacks);
    void createRawMonitor();
    void setTag(jobject object,
                jlong tag);
    string getMethodName(jmethodID methodID);
    string getMethodDescriptor(jmethodID methodID);
    MethodReturnType getFieldType(string fieldDescriptor);
    MethodReturnType getFieldType(jvmtiPrimitiveType type);
    bool isFieldArrayType(MethodReturnType type);
    string getClassName(jclass klass);
    string getClassName(jobject object);
    void getClassName(jclass klass, string& className, string& classSignature);
    void getLoadedClasses(vector<jclass>& classes);
    jclass getObjectClass(jobject object);
    jstring createNewString(string& str);
    jobject createObject(string className,
                         string constructorDescriptor,
                         vector<jvalue>& args);
    jobject createObjectV(string className,
                          string constructorDescriptor,
                          ...);
    jobject createObjectV(jclass klass,
                          string constructorDescriptor,
                          ...);
    jobject createObject(const jvalue& value,
                         const MethodReturnType& type);
    jobject createObjectWithoutConstructor(jclass klass);
    jobjectArray createObjectArray(vector<jobject>& elements);
#define ENTRY(a, b, c, d, e) jobject create##a##Object( b value);
    JAVA_OBJECT_CREATION_TYPE_
#undef ENTRY

#define ENTRY(a, b, c) b call##a##MethodV(jobject instance, jmethodID methodID, ...);
    JAVA_METHOD_CALLS_
#undef ENTRY

#define ENTRY(a, b, c) b call##a##MethodA(jobject instance, jmethodID methodID, vector<jvalue>& args);
    JAVA_METHOD_CALLS_
#undef ENTRY

#define ENTRY(a, b, c) b callStatic##a##MethodA(jclass klass, jmethodID methodID, vector<jvalue>& args);
    JAVA_METHOD_CALLS_
#undef ENTRY
    
    jobject getInstanceObjectField(jobject instance,
                                   string fieldName,
                                   string fieldDescriptor);
    jvalue getInstancePrimitiveField(jobject instance,
                                     string fieldName,
                                     string fieldDescriptor);
    void getInstanceObjectArrayField(jobject instance,
                                     string fieldName,
                                     string fieldDescriptor,
                                     vector<jobject>& objectArray);
    void getInstanceStringArrayField(jobject instance,
                                     string fieldName,
                                     string fieldDescriptor,
                                     vector<string>& stringArray);
    void getStringArrayElements(jobjectArray array,
                                vector<string>& stringArray);
    jsize getArrayLength(jarray array);
    jobject getObjectArrayElement(jobjectArray objectArray,
                                  jsize index);
#define ENTRY(a, b, c, d) b* get##a##ArrayElements(b##Array array);
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY

#define ENTRY(a, b, c, d) b##Array create##a##Array(b *array, jint length);
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY

    void setObjectArrayElement(jobjectArray objectArray,
                               jobject element,
                               jsize index);
    jfieldID getFieldID(jclass fieldDeclaringClass,
                        string fieldName,
                        string fieldDescriptor);
    void getClassDeclaredFields(jclass klass,
                                vector<jfieldID>& declaredfields);
    void getClassDeclaredMethods(jclass klass,
                                 vector<jmethodID>& declaredMethods);
    jclass getClass(string className);
    jmethodID getMethodID(jclass methodDeclaringClass,
                          string methodName,
                          string methodDescriptor);
    jmethodID getMethodID(string methodDeclaringClass,
                          string methodName,
                          string methodDescriptor);
    jmethodID getStaticMethodID(jclass methodDeclaringClass,
                          string methodName,
                          string methodDescriptor);
    jmethodID getStaticMethodID(string methodDeclaringClass,
                          string methodName,
                          string methodDescriptor);
    MethodReturnType getMethodReturnType(jmethodID methodID);
    MethodReturnType getMethodReturnType(string methodDescriptor);
    void callInstanceMethod(jobject object,
                            string methodName,
                            string methodDescriptor,
                            vector<jvalue>& args,
                            jvalue& returnValue,
                            MethodReturnType& returnType);
    void callStaticMethod(string className,
                          string methodName,
                          string methodDescriptor,
                          vector<jvalue>& args,
                          jvalue& returnValue,
                          MethodReturnType& returnType);
   
    bool evaluateBooleanObject(jobject booleanObject);
    int getObjectHashCode(jobject object);
    void getObjectsWithTags(long* givenTags,
                            int givenTagsCount,
                            jobject** objects,
                            long** returnedTags,
                            int* returnedTagsCount);
    string evaluateStringObject(jstring stringObject);
    void Deallocate(unsigned char** mem);
    void forceGC();
    void rawMonitorEnter();
    void rawMonitorExit();
    void iterateThroughHeap(jvmtiHeapCallbacks* heapOperationCallback,
                            int jvmtiHeapFilter);

    void followReferences(jvmtiHeapCallbacks* heapOperationCallback,
                          int jvmtiHeapFilter,
                          jobject root);

    /**
     * @fn      void setObjectPrimitiveField(JNIEnv* jniEnv,
     *                                          string className,
     *                                          jobject object,
     *                                          string fieldName,
     *                                          jvmtiExtendedFieldType type,
     *                                          string fieldValue);
     * @brief   Sets a primitive field of the specified object to
     *          the given value
     *
     * @author  1sand0s
     * @param   jniEnv           Pointer to JNI Environment
     * @param   object           Instance whose primitive field is to be set
     * @param   fieldName        Name of the primitive field to be set
     * @param   type             Type of the primitive field to be set
     * @param   fieldValue       Value to set the primitive field to
     */
    void setObjectPrimitiveField(string className,
                                 jobject object,
                                 string fieldName,
                                 jvmtiExtendedFieldType type,
                                 string fieldValue);
    void setObjectPrimitiveField(string className,
                                 jobject object,
                                 string fieldName,
				 string fieldDescriptor,
                                 MethodReturnType type,
                                 jvalue fieldValue);

    /**
     * @fn      void setObjectReferenceField(JNIEnv* jniEnv,
     *                                          jobject referrer,
     *                                          jobject referee,
     *                                          string fieldName,
     *                                          string fieldDescriptor);
     * @brief   Sets a reference field of the specified object to
     *          the given object
     *
     * @author  1sand0s
     * @param   jniEnv           Pointer to JNI Environment
     * @param   referrer         Referrer object
     * @param   referee          Referee object
     * @param   fieldName        Name of the reference field
     * @param   fieldDescriptor  Descriptor of the reference field
     */
    void setObjectReferenceField(jobject referrer,
                                 jobject referee,
                                 string fieldName,
                                 string fieldDescriptor);

    /**
     * @fn      string getUnboxedDescriptorForType(jvmtiExtendedFieldType type);
     * @brief   Gets the field Descriptor for jvmtiExtendedFieldType
     *
     * @author  1sand0s
     * @param   type     a jvmtiExtendedFieldType to get the field decriptor of
     */
    string getUnboxedDescriptorForType(jvmtiExtendedFieldType type);

    /**
    * @fn      string getUnboxedDescriptorForType(jvmtiExtendedFieldType type);
    * @brief   Gets the field Descriptor for jvmtiExtendedFieldType
    *
    * @author  1sand0s
    * @param   type     a jvmtiExtendedFieldType to get the field decriptor of
    */
    void getFieldInfo(const jfieldID field,
                      const jclass klass,
                      string& name,
                      string& signature,
                      int& modifiers);
    /**
    * @fn      string getUnboxedDescriptorForType(jvmtiExtendedFieldType type);
    * @brief   Gets the field Descriptor for jvmtiExtendedFieldType
    *
    * @author  1sand0s
    * @param   type     a jvmtiExtendedFieldType to get the field decriptor of
    */
    void getMethodInfo(const jmethodID method,
                       string& name,
                       string& signature,
                       int& modifiers);

private:
    JNIEnv* jni_env;

    /* JVMTI agent handle for calling
       * JVMTI methods
       */
    jvmtiEnv* jvmti_env;

    /* Raw monitor to make methods
     * reentrant
       */
    jrawMonitorID lock;

    /**
    * Used to enable only required capabilities to improve performance (JVMTI vs JVM(P&D)I)
    * see https://docs.oracle.com/en/java/javase/11/docs/specs/jvmti.html#jvmtiCapabilities
    */
    jvmtiCapabilities capabilities;
};
} // namespace UTIL

