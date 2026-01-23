#include "jni_util.h"
#include "ogoConstants.h"

#include <cstdarg>
#include <jni.h>
#include <jvmti.h>
#include <stdexcept>

using namespace util;
using namespace std;

Agent::Agent(jvmtiEnv *jvmti_env) { this->jvmti_env = jvmti_env; }

void Agent::setJNIEnv(JNIEnv *jni_env) { this->jni_env = jni_env; }

void Agent::setCapabilities(const jvmtiCapabilities &capabilities) {
  (*jvmti_env).functions->AddCapabilities(jvmti_env, &capabilities);
}

void Agent::setEventCallbacks(const jvmtiEventCallbacks &callbacks) {
  (*jvmti_env)
      .functions->SetEventCallbacks(jvmti_env, &callbacks,
                                    sizeof(jvmtiEventCallbacks));
  (*jvmti_env)
      .functions->SetEventNotificationMode(jvmti_env, JVMTI_ENABLE,
                                           JVMTI_EVENT_EXCEPTION, NULL);
}

string Agent::fieldTypeToString(const enum MethodReturnType &type) {
  switch (type) {
  case JBYTE:
    return "JBYTE";
  case JSHORT:
    return "JSHORT";
  case JINT:
    return "JINT";
  case JLONG:
    return "JLONG";
  case JCHAR:
    return "JCHAR";
  case JSTRING:
    return "JSTRING";
  case JFLOAT:
    return "JFLOAT";
  case JDOUBLE:
    return "JDOUBLE";
  case JBOOLEAN:
    return "JBOOLEAN";
  }
}

jlong Agent::getIntegerTypesAsLong(const jvalue &value,
                                   const enum MethodReturnType &type) {
  switch (type) {
  case MethodReturnType::JBYTE:
    return (jlong)value.b;
  case MethodReturnType::JSHORT:
    return (jlong)value.s;
  case MethodReturnType::JINT:
    return (jlong)value.i;
  case MethodReturnType::JLONG:
    return (jlong)value.j;
  default:
    throw std::runtime_error(
        "Error : type must be JBYTE, JSHORT, JINT or JLONG");
  }
}

bool Agent::isIntegerType(const enum MethodReturnType &type) {
  return type == MethodReturnType::JBYTE || type == MethodReturnType::JSHORT ||
         type == MethodReturnType::JINT || type == MethodReturnType::JLONG;
}

jdouble
Agent::getFloatTypesAsDouble(const jvalue &value,
                             const enum util::Agent::MethodReturnType &type) {
  switch (type) {
  case MethodReturnType::JFLOAT:
    return (jfloat)value.f;
  case MethodReturnType::JDOUBLE:
    return (jdouble)value.d;
  default:
    throw std::runtime_error("Error : type must be JFLOAT or JDOUBLE");
  }
}

jdouble Agent::getNumberTypeAsDouble(const jvalue &value,
                                     const enum MethodReturnType &type) {
  if (!isNumberType(type)) {
    throw std::runtime_error("Error : given type must be a number type");
  }
  if (isIntegerType(type)) {
    return (jdouble)getIntegerTypesAsLong(value, type);
  }
  return getFloatTypesAsDouble(value, type);
}

jlong Agent::getNumberTypeAsLong(const jvalue &value,
                                 const enum MethodReturnType &type) {
  if (!isNumberType(type)) {
    throw std::runtime_error("Error : given type must be a number type");
  }
  if (isIntegerType(type)) {
    return (jlong)getIntegerTypesAsLong(value, type);
  }
  return (jlong)getFloatTypesAsDouble(value, type);
}

bool Agent::isFloatType(const enum MethodReturnType &type) {
  return type == MethodReturnType::JFLOAT || type == MethodReturnType::JDOUBLE;
}

bool Agent::isNumberType(const enum MethodReturnType &type) {
  return isIntegerType(type) || isFloatType(type);
}

Agent::MethodReturnType
Agent::getPriorityType(const enum util::Agent::MethodReturnType &type1,
                       const enum util::Agent::MethodReturnType &type2) {
  if (type1 < type2) {
    return type2;
  }
  return type1;
}

void Agent::createRawMonitor() {
  /* Create raw monitor to enter critical sections */
  (*jvmti_env).functions->CreateRawMonitor(jvmti_env, "agent data", &lock);
}

void Agent::setTag(jobject object, jlong tag) {
  /* Create raw monitor to enter critical sections */
  (*jvmti_env).functions->SetTag(jvmti_env, object, tag);
}

string Agent::getMethodName(jmethodID methodID) {
  char *methodNameC = NULL;
  string methodName = "";
  (*jvmti_env)
      .functions->GetMethodName(jvmti_env, methodID, &methodNameC, NULL, NULL);
  if (methodNameC != NULL) {
    methodName.assign(methodNameC);
  }

  Deallocate((unsigned char **)&methodNameC);
  return methodName;
}

string Agent::getMethodDescriptor(jmethodID methodID) {
  char *methodDescriptorC = NULL;
  string methodDescriptor = "";
  (*jvmti_env)
      .functions->GetMethodName(jvmti_env, methodID, NULL, &methodDescriptorC,
                                NULL);
  if (methodDescriptorC != NULL) {
    methodDescriptor.assign(methodDescriptorC);
  }

  Deallocate((unsigned char **)&methodDescriptorC);
  return methodDescriptor;
}

jclass Agent::getObjectClass(jobject object) {
  return (*jni_env).functions->GetObjectClass(jni_env, object);
}

jclass Agent::getClass(string className) {
  return (*jni_env).functions->FindClass(jni_env, className.c_str());
}

#define ENTRY(a, b, c, d, e)                                                   \
  jobject Agent::create##a##Object(b value) {                                  \
    string descriptor = "(" STRINGIFY(d) ")V";                                 \
    return createObjectV(#c, descriptor, value);                               \
  }
JAVA_OBJECT_CREATION_TYPE_
#undef ENTRY

jobject Agent::createObject(const jvalue &value, const MethodReturnType &type) {
  switch (type) {
#define ENTRY(a, b, c, d)                                                      \
  case c:                                                                      \
    return create##a##Object(value.d);
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
  case JOBJECT:
  case JSTRING:
    return value.l;
  case JNULL:
  case JVOID:
    return NULL;
  }
}

jstring Agent::createNewString(string &str) {
  return (*jni_env).functions->NewStringUTF(jni_env, str.c_str());
}

jobject Agent::createObject(string className, string constructorDescriptor,
                            vector<jvalue> &args) {
  jclass klass = getClass(className);
  jmethodID methodID = getMethodID(klass, "<init>", constructorDescriptor);
  return (*jni_env).functions->NewObject(jni_env, klass, methodID, args.data());
}

jobject Agent::createObjectV(string className, string constructorDescriptor,
                             ...) {
  jclass klass = getClass(className);
  jmethodID methodID = getMethodID(klass, "<init>", constructorDescriptor);
  va_list args;
  jobject createdObject = NULL;
  va_start(args, constructorDescriptor);
  createdObject =
      (*jni_env).functions->NewObjectV(jni_env, klass, methodID, args);
  va_end(args);
  return createdObject;
}

jobject Agent::createObjectV(jclass klass, string constructorDescriptor, ...) {
  jmethodID methodID = getMethodID(klass, "<init>", constructorDescriptor);
  va_list args;
  jobject createdObject = NULL;
  va_start(args, constructorDescriptor);
  createdObject =
      (*jni_env).functions->NewObjectV(jni_env, klass, methodID, args);
  va_end(args);
  return createdObject;
}

jobjectArray Agent::createObjectArray(vector<jobject> &elements) {
  jclass klass = getClass("java/lang/Object");
  jobjectArray array = (*jni_env).functions->NewObjectArray(
      jni_env, elements.size(), klass, NULL);
  for (int j = 0; j < elements.size(); j++) {
    setObjectArrayElement(array, elements[j], j);
  }
  return array;
}

void Agent::setObjectArrayElement(jobjectArray objectArray, jobject element,
                                  jsize index) {
  (*jni_env).functions->SetObjectArrayElement(jni_env, objectArray, index,
                                              element);
}

#define ENTRY(a, b, c, d)                                                      \
  b *Agent::get##a##ArrayElements(b##Array array) {                            \
    return (*jni_env).functions->Get##a##ArrayElements(jni_env, array, NULL);  \
  }
JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY

string Agent::getClassName(jclass klass) {
  char *classNameC = NULL;
  string className = "";
  (*jvmti_env)
      .functions->GetClassSignature(jvmti_env, klass, &classNameC, NULL);
  if (classNameC != NULL) {
    className.assign(classNameC);
  }
  Deallocate((unsigned char **)&classNameC);
  return className;
}

void Agent::getClassName(jclass klass, string &className,
                         string &classSignature) {
  char *classNameC = NULL;
  char *classSignatureC = NULL;
  (*jvmti_env)
      .functions->GetClassSignature(jvmti_env, klass, &classNameC,
                                    &classSignatureC);
  if (classNameC != NULL) {
    className.assign(classNameC);
  }
  if (classSignatureC != NULL) {
    classSignature.assign(classSignatureC);
  }
  Deallocate((unsigned char **)&classNameC);
  Deallocate((unsigned char **)&classSignatureC);
}

string Agent::getClassName(jobject object) {
  return getClassName(getObjectClass(object));
}

void Agent::getLoadedClasses(vector<jclass> &classes) {
  jclass *classesC = NULL;
  jint classCount = 0;
  (*jvmti_env).functions->GetLoadedClasses(jvmti_env, &classCount, &classesC);
  for (int j = 0; j < classCount; j++) {
    classes.push_back(classesC[j]);
  }
}

jfieldID Agent::getFieldID(jclass fieldDeclaringClass, string fieldName,
                           string fieldDescriptor) {
  return (*jni_env).functions->GetFieldID(
      jni_env, fieldDeclaringClass, fieldName.c_str(), fieldDescriptor.c_str());
}

void Agent::getClassDeclaredFields(jclass klass,
                                   vector<jfieldID> &declaredfields) {
  jfieldID *fields = NULL;
  jint fieldCount = 0;
  (*jvmti_env)
      .functions->GetClassFields(jvmti_env, klass, &fieldCount, &fields);
  for (int j = 0; j < fieldCount; j++) {
    declaredfields.push_back(fields[j]);
  }
}

void Agent::getClassDeclaredMethods(jclass klass,
                                    vector<jmethodID> &declaredMethods) {
  jmethodID *methods = NULL;
  jint methodCount = 0;
  (*jvmti_env)
      .functions->GetClassMethods(jvmti_env, klass, &methodCount, &methods);
  for (int j = 0; j < methodCount; j++) {
    declaredMethods.push_back(methods[j]);
  }
}

jmethodID Agent::getMethodID(jclass methodDeclaringClass, string methodName,
                             string methodDescriptor) {
  return (*jni_env).functions->GetMethodID(jni_env, methodDeclaringClass,
                                           methodName.c_str(),
                                           methodDescriptor.c_str());
}

jmethodID Agent::getMethodID(string methodDeclaringClass, string methodName,
                             string methodDescriptor) {
  return getMethodID(getClass(methodDeclaringClass), methodName,
                     methodDescriptor);
}

jmethodID Agent::getStaticMethodID(jclass methodDeclaringClass,
                                   string methodName, string methodDescriptor) {
  return (*jni_env).functions->GetStaticMethodID(jni_env, methodDeclaringClass,
                                                 methodName.c_str(),
                                                 methodDescriptor.c_str());
}

jmethodID Agent::getStaticMethodID(string methodDeclaringClass,
                                   string methodName, string methodDescriptor) {
  return getStaticMethodID(getClass(methodDeclaringClass), methodName,
                           methodDescriptor);
}

Agent::MethodReturnType Agent::getMethodReturnType(jmethodID methodID) {
  return getMethodReturnType(getMethodDescriptor(methodID));
}

Agent::MethodReturnType Agent::getMethodReturnType(string methodDescriptor) {
  string returnType =
      methodDescriptor.substr(methodDescriptor.find(")") + 1, 1);
  if (returnType == "Ljava/lang/String;") {
    return JSTRING;
  }
  switch (returnType[0]) {
#define ENTRY(a, b)                                                            \
  case SINGLE_QUOTES(a):                                                       \
    return b;
    JAVA_METHOD_RETURN_TYPE_
#undef ENTRY
  default:
    return JVOID;
  }
}

jobject Agent::getInstanceObjectField(jobject instance, string fieldName,
                                      string fieldDescriptor) {
  jclass instanceClass = getObjectClass(instance);
  jfieldID fieldID = getFieldID(instanceClass, fieldName, fieldDescriptor);
  return (*jni_env).functions->GetObjectField(jni_env, instance, fieldID);
}

jvalue Agent::getInstancePrimitiveField(jobject instance, string fieldName,
                                        string fieldDescriptor) {
  jclass instanceClass = getObjectClass(instance);
  jfieldID fieldID = getFieldID(instanceClass, fieldName, fieldDescriptor);
  jvalue value;
  switch (getFieldType(fieldDescriptor)) {
#define ENTRY(a, b, c, d)                                                      \
  case c:                                                                      \
    value.d = (*jni_env).functions->Get##a##Field(jni_env, instance, fieldID); \
    return value;
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
  case JOBJECT:
  case JNULL:
  case JSTRING:
  case JVOID:
    value.l = NULL;
    return value;
  }
}

Agent::MethodReturnType Agent::getFieldType(string fieldDescriptor) {
  if (fieldDescriptor.size() == 1) {
    switch (fieldDescriptor[0]) {
#define ENTRY(a, b, c, d, e)                                                   \
  case SINGLE_QUOTES(d):                                                       \
    return e;
      JAVA_OBJECT_CREATION_TYPE_
#undef ENTRY
    }
  } else if (fieldDescriptor == "Ljava/lang/String;") {
    return Agent::MethodReturnType::JSTRING;
  } else {
    return Agent::MethodReturnType::JOBJECT;
  }
}

Agent::MethodReturnType Agent::getFieldType(jvmtiPrimitiveType type) {
  switch (type) {
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_BOOLEAN:
    return Agent::MethodReturnType::JBOOLEAN;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_BYTE:
    return Agent::MethodReturnType::JBYTE;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_SHORT:
    return Agent::MethodReturnType::JSHORT;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_INT:
    return Agent::MethodReturnType::JINT;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_LONG:
    return Agent::MethodReturnType::JLONG;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_CHAR:
    return Agent::MethodReturnType::JCHAR;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_FLOAT:
    return Agent::MethodReturnType::JFLOAT;
  case jvmtiPrimitiveType::JVMTI_PRIMITIVE_TYPE_DOUBLE:
    return Agent::MethodReturnType::JDOUBLE;
  }
}

bool Agent::isFieldArrayType(MethodReturnType type) {
  switch (type) {
  case JBOOLEAN_ARRAY:
  case JBYTE_ARRAY:
  case JSHORT_ARRAY:
  case JINT_ARRAY:
  case JLONG_ARRAY:
  case JFLOAT_ARRAY:
  case JDOUBLE_ARRAY:
  case JCHAR_ARRAY:
    return true;
  default:
    return false;
  }
}

void Agent::getInstanceObjectArrayField(jobject instance, string fieldName,
                                        string fieldDescriptor,
                                        vector<jobject> &objectArray) {
  jobject objectArrayInstance =
      getInstanceObjectField(instance, fieldName, fieldDescriptor);
  jsize arraySize = getArrayLength((jarray)objectArrayInstance);
  for (jsize i = 0; i < arraySize; i++) {
    objectArray.push_back(
        getObjectArrayElement((jobjectArray)objectArrayInstance, i));
  }
}

void Agent::getInstanceStringArrayField(jobject instance, string fieldName,
                                        string fieldDescriptor,
                                        vector<string> &stringArray) {
  jobjectArray stringArrayInstance = (jobjectArray)getInstanceObjectField(
      instance, fieldName, fieldDescriptor);
  getStringArrayElements(stringArrayInstance, stringArray);
}

void Agent::getStringArrayElements(jobjectArray array,
                                   vector<string> &stringArray) {
  jsize arraySize = getArrayLength(array);
  for (jsize i = 0; i < arraySize; i++) {
    stringArray.push_back(
        evaluateStringObject((jstring)getObjectArrayElement(array, i)));
  }
}

jsize Agent::getArrayLength(jarray array) {
  return (*jni_env).functions->GetArrayLength(jni_env, array);
}

jobject Agent::getObjectArrayElement(jobjectArray objectArray, jsize index) {
  return (*jni_env).functions->GetObjectArrayElement(jni_env, objectArray,
                                                     index);
}

#define ENTRY(a, b, c)                                                         \
  b Agent::call##a##MethodV(jobject instance, jmethodID methodID, ...) {       \
    va_list args;                                                              \
    return (*jni_env).functions->Call##a##MethodV(jni_env, instance, methodID, \
                                                  args);                       \
  }
JAVA_METHOD_CALLS_
#undef ENTRY

#define ENTRY(a, b, c)                                                         \
  b Agent::call##a##MethodA(jobject instance, jmethodID methodID,              \
                            vector<jvalue> &args) {                            \
    return (*jni_env).functions->Call##a##MethodA(jni_env, instance, methodID, \
                                                  args.data());                \
  }
JAVA_METHOD_CALLS_
#undef ENTRY

#define ENTRY(a, b, c)                                                         \
  b Agent::callStatic##a##MethodA(jclass klass, jmethodID methodID,            \
                                  vector<jvalue> &args) {                      \
    return (*jni_env).functions->CallStatic##a##MethodA(                       \
        jni_env, klass, methodID, args.data());                                \
  }
JAVA_METHOD_CALLS_
#undef ENTRY

void Agent::callInstanceMethod(jobject object, string methodName,
                               string methodDescriptor, vector<jvalue> &args,
                               jvalue &returnValue,
                               MethodReturnType &returnType) {
  jclass klass = getObjectClass(object);
  jmethodID methodID = getMethodID(klass, methodName, methodDescriptor);
  returnType = getMethodReturnType(methodDescriptor);
  switch (returnType) {
#define ENTRY(a, b, c, d)                                                      \
  case c:                                                                      \
    returnValue.d = call##a##MethodA(object, methodID, args);                  \
    break;
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
  case JVOID:
    callVoidMethodA(object, methodID, args);
    returnType = MethodReturnType::JVOID;
    break;
  case JSTRING:
  case JOBJECT:
    returnValue.l = callObjectMethodA(object, methodID, args);
    if (returnValue.l == NULL) {
      returnType = MethodReturnType::JNULL;
    }
    break;
  }
}

void Agent::callStaticMethod(string className, string methodName,
                             string methodDescriptor, vector<jvalue> &args,
                             jvalue &returnValue,
                             MethodReturnType &returnType) {
  jclass klass = getClass(className);
  jmethodID methodID = getStaticMethodID(klass, methodName, methodDescriptor);
  returnType = getMethodReturnType(methodDescriptor);
  switch (returnType) {
#define ENTRY(a, b, c, d)                                                      \
  case c:                                                                      \
    returnValue.d = callStatic##a##MethodA(klass, methodID, args);             \
    break;
    JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY
  case JVOID:
    callStaticVoidMethodA(klass, methodID, args);
    returnType = MethodReturnType::JVOID;
    break;
  case JSTRING:
  case JOBJECT:
    returnValue.l = callStaticObjectMethodA(klass, methodID, args);
    if (returnValue.l == NULL) {
      returnType = MethodReturnType::JNULL;
    }
    break;
  }
}

// printf("Array %p\n", obj);(*jni_env).functions->Set##a##ArrayRegion(jni_env,
// obj, 0, length, array);
#define ENTRY(a, b, c, d)                                                      \
  b##Array Agent::create##a##Array(b *array, jint length) {                    \
    printf("Length %d\n", length);                                             \
    b##Array obj = (*jni_env).functions->New##a##Array(jni_env, length + 1);   \
    return obj;                                                                \
  }
JAVA_METHOD_OBJECT_CALLS_
#undef ENTRY

bool Agent::evaluateBooleanObject(jobject booleanObject) {
  jmethodID method = getMethodID("java/lang/Boolean", "booleanValue", "()Z");
  return callBooleanMethodV(booleanObject, method);
}

string Agent::evaluateStringObject(jstring stringObject) {
  string value = "";
  const char *cString =
      (*jni_env).functions->GetStringUTFChars(jni_env, stringObject, NULL);
  if (cString != NULL) {
    value.assign(cString);
  }
  (*jni_env).functions->ReleaseStringUTFChars(jni_env, stringObject, cString);
  return value;
}

int Agent::getObjectHashCode(jobject object) {
  jint hashCode = 0;
  (*jvmti_env).functions->GetObjectHashCode(jvmti_env, object, &hashCode);
  return (int)hashCode;
}

void Agent::getObjectsWithTags(long *givenTags, int givenTagsCount,
                               jobject **objects, long **returnedTags,
                               int *returnedTagsCount) {
  (*jvmti_env)
      .functions->GetObjectsWithTags(jvmti_env, givenTagsCount, givenTags,
                                     returnedTagsCount, objects, returnedTags);
}

void Agent::forceGC() {
  (*jvmti_env).functions->ForceGarbageCollection(jvmti_env);
}

void Agent::rawMonitorEnter() {
  (*jvmti_env).functions->RawMonitorEnter(jvmti_env, lock);
}

void Agent::rawMonitorExit() {
  (*jvmti_env).functions->RawMonitorExit(jvmti_env, lock);
}

void Agent::iterateThroughHeap(jvmtiHeapCallbacks *heapOperationCallback,
                               int jvmtiHeapFilter) {
  (*jvmti_env)
      .functions->IterateThroughHeap(jvmti_env, jvmtiHeapFilter, NULL,
                                     heapOperationCallback, NULL);
}

void Agent::followReferences(jvmtiHeapCallbacks *heapOperationCallback,
                             int jvmtiHeapFilter, jobject root) {
  (*jvmti_env)
      .functions->FollowReferences(jvmti_env, jvmtiHeapFilter, NULL, root,
                                   heapOperationCallback, NULL);
}

void Agent::Deallocate(unsigned char **mem) {
  if (*mem == NULL) {
    return;
  }
  (*jvmti_env).functions->Deallocate(jvmti_env, *mem);
  *mem = NULL;
}

jobject Agent::createObjectWithoutConstructor(jclass klass) {
  return (*jni_env).functions->AllocObject(jni_env, klass);
}

/**
 * @fn      void setObjectPrimitiveField(AgentEnv* jni_env,
 *                                          string className,
 *                                          jobject object,
 *                                          string fieldName,
 *                                          jvmtiExtendedFieldType type,
 *                                          string fieldValue);
 * @brief   Sets a primitive field of the specified object to
 *          the given value
 *
 * @author  1sand0s
 * @param   jni_env           Pointer to Agent Environment
 * @param   object           Instance whose primitive field is to be set
 * @param   fieldName        Name of the primitive field to be set
 * @param   type             Type of the primitive field to be set
 * @param   fieldValue       Value to set the primitive field to
 */
void Agent::setObjectPrimitiveField(string className, jobject object,
                                    string fieldName,
                                    jvmtiExtendedFieldType type,
                                    string fieldValue) {
  jclass cls = (*jni_env).functions->FindClass(jni_env, className.c_str());
  jfieldID fieldID = (*jni_env).functions->GetFieldID(
      jni_env, cls, fieldName.c_str(),
      getUnboxedDescriptorForType(type).c_str());
  switch (type) {
  case JVMTI_EXTENDED_FIELD_TYPE_BOOLEAN:
    (*jni_env).functions->SetBooleanField(jni_env, object, fieldID,
                                          fieldValue == "True" ? true : false);
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_BYTE:
    (*jni_env).functions->SetByteField(jni_env, object, fieldID,
                                       (jbyte)stoi(fieldValue));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_CHAR:
    (*jni_env).functions->SetCharField(jni_env, object, fieldID,
                                       (jchar)(fieldValue.c_str()[0]));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_SHORT:
    (*jni_env).functions->SetShortField(jni_env, object, fieldID,
                                        (jshort)stoi(fieldValue));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_INT:
    (*jni_env).functions->SetIntField(jni_env, object, fieldID,
                                      (jint)stoi(fieldValue));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_LONG:
    (*jni_env).functions->SetLongField(jni_env, object, fieldID,
                                       (jlong)stol(fieldValue));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_FLOAT:
    (*jni_env).functions->SetFloatField(jni_env, object, fieldID,
                                        (jfloat)stod(fieldValue));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_DOUBLE:
    (*jni_env).functions->SetDoubleField(jni_env, object, fieldID,
                                         (jdouble)stod(fieldValue));
    break;
  case JVMTI_EXTENDED_FIELD_TYPE_STRING:
    break;
  default:
    exit(0);
  }
}

void Agent::setObjectPrimitiveField(string className, jobject object,
                                    string fieldName, string fieldDescriptor,
                                    MethodReturnType type, jvalue fieldValue) {
  jclass cls = (*jni_env).functions->FindClass(jni_env, className.c_str());
  jfieldID fieldID = getFieldID(cls, fieldName, fieldDescriptor);
  switch (type) {
  case JBOOLEAN:
    (*jni_env).functions->SetBooleanField(jni_env, object, fieldID,
                                          fieldValue.z);
    break;
  case JBYTE:
    (*jni_env).functions->SetByteField(jni_env, object, fieldID, fieldValue.b);
    break;
  case JCHAR:
    (*jni_env).functions->SetCharField(jni_env, object, fieldID, fieldValue.c);
    break;
  case JSHORT:
    (*jni_env).functions->SetShortField(jni_env, object, fieldID, fieldValue.s);
    break;
  case JINT:
    (*jni_env).functions->SetIntField(jni_env, object, fieldID, fieldValue.i);
    break;
  case JLONG:
    (*jni_env).functions->SetLongField(jni_env, object, fieldID, fieldValue.j);
    break;
  case JFLOAT:
    (*jni_env).functions->SetFloatField(jni_env, object, fieldID, fieldValue.f);
    break;
  case JDOUBLE:
    (*jni_env).functions->SetDoubleField(jni_env, object, fieldID,
                                         fieldValue.d);
    break;
  case JSTRING:
    (*jni_env).functions->SetObjectField(jni_env, object, fieldID,
                                         fieldValue.l);
    break;
  default:
    exit(0);
  }
}
/**
 * @fn      void setObjectReferenceField(AgentEnv* jni_env,
 *                                          jobject referrer,
 *                                          jobject referee,
 *                                          string fieldName,
 *                                          string fieldDescriptor);
 * @brief   Sets a reference field of the specified object to
 *          the given object
 *
 * @author  1sand0s
 * @param   jni_env           Pointer to Agent Environment
 * @param   referrer         Referrer object
 * @param   referee          Referee object
 * @param   fieldName        Name of the reference field
 * @param   fieldDescriptor  Descriptor of the reference field
 */
void Agent::setObjectReferenceField(jobject referrer, jobject referee,
                                    string fieldName, string fieldDescriptor) {
  jclass referrerClass =
      (*jni_env).functions->GetObjectClass(jni_env, referrer);
  jclass refereeClass = (*jni_env).functions->GetObjectClass(jni_env, referee);
  jfieldID fieldID = (*jni_env).functions->GetFieldID(
      jni_env, referrerClass, fieldName.c_str(), fieldDescriptor.c_str());
  (*jni_env).functions->SetObjectField(jni_env, referrer, fieldID, referee);
}

/**
 * @fn      string getUnboxedDescriptorForType(jvmtiExtendedFieldType type);
 * @brief   Gets the field Descriptor for jvmtiExtendedFieldType
 *
 * @author  1sand0s
 * @param   type     a jvmtiExtendedFieldType to get the field decriptor of
 */
string Agent::getUnboxedDescriptorForType(jvmtiExtendedFieldType type) {
  switch (type) {
  case JVMTI_EXTENDED_FIELD_TYPE_BOOLEAN:
    return "Z";
  case JVMTI_EXTENDED_FIELD_TYPE_BYTE:
    return "B";
  case JVMTI_EXTENDED_FIELD_TYPE_CHAR:
    return "C";
  case JVMTI_EXTENDED_FIELD_TYPE_SHORT:
    return "S";
  case JVMTI_EXTENDED_FIELD_TYPE_INT:
    return "I";
  case JVMTI_EXTENDED_FIELD_TYPE_LONG:
    return "J";
  case JVMTI_EXTENDED_FIELD_TYPE_FLOAT:
    return "F";
  case JVMTI_EXTENDED_FIELD_TYPE_DOUBLE:
    return "D";
  case JVMTI_EXTENDED_FIELD_TYPE_STRING:
    return STRING_CLASS;
  default:
    exit(0);
  }
}

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
void Agent::getFieldInfo(const jfieldID field, const jclass klass, string &name,
                         string &signature, int &modifiers) {
  char *cName = NULL;
  char *cSignature = NULL;
  jint fModifiers; /* Use jint to remove warnings */

  /* Get field name and signature */
  (*jvmti_env)
      .functions->GetFieldName(jvmti_env, klass, field, &cName, &cSignature,
                               NULL);

  /* Get field modifiers */
  (*jvmti_env)
      .functions->GetFieldModifiers(jvmti_env, klass, field, &fModifiers);

  name.assign(cName);
  signature.assign(cSignature);
  modifiers = fModifiers;

  /* Free memory allocated for cStrings*/
  free(cName);
  free(cSignature);
}

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
void Agent::getMethodInfo(const jmethodID method, string &name,
                          string &signature, int &modifiers) {
  char *cName = NULL;
  char *cSignature = NULL;
  jint fModifiers; /* Use jint to remove warnings */

  /* Get method name and signature */
  (*jvmti_env)
      .functions->GetMethodName(jvmti_env, method, &cName, &cSignature, NULL);

  /* Get method modifiers */
  (*jvmti_env).functions->GetMethodModifiers(jvmti_env, method, &fModifiers);

  name.assign(cName);
  signature.assign(cSignature);
  modifiers = fModifiers;

  /* Free memory allocated for cStrings*/
  free(cName);
  free(cSignature);
}
