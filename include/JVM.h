#ifndef JVM_H
#define JVM_H
#include "jni.h"
#include <iostream>
#include <windows.h>
#include "../include/Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>

/**
 * Alguns dos possiveis erros na criação da JVM, os outros já estão definidos no jni.h;
 */
#define JNI_DLL_NOT_FOUND    (-7)
#define JNI_CREATE           (-8)
#define JNI_GET_CREATED      (-9)


/**
 * Assinatura padrão para os metodos de criação de conexão e chamada de função do vision
 */
#define CALL_SIGNATURE          "(Ljava/lang/Class;Ljava/lang/String;[Ljava/lang/String;)Ljava/lang/Object;"

#define DISPATCH_CLASS "com/jbridge/Dispatcher"

//#define DEBUG_RUNTIME
//#define DEBUG_MESSAGE

#define DLL_PATH_VAR "SYSTEX5"

#ifdef DEBUG_MESSAGE
    #define display(x)\
    MessageBox(NULL, x, "Debug", MB_OK)
#else
    #define display(x)\
    printf("%s\n",x)
#endif

/**
 * Estrutura de retono de uma função java.
 * type equivale ao nome da classe java, por exemplo String.
 * value é o valor retornado, sempre será retornada convertido para string usando metodo valueOf
 * podendo ser reconvertido utilizando o type.
 */
struct java_var
{
   const char* type;
   const char* value;
};

/**
 * Estrutura de chamada para um metodo java
 */
struct java_call
{
    jclass       callClazz;
    jmethodID    callMethodId;
    jobjectArray callParameters;
};

/**
 * Estrutura de propriedades do sistema
 */
struct SystemProperties {
    const char* java_home;
    const char* ext_dirs;
};

typedef jint (JNICALL *CreateJavaVM_t) (JavaVM**, void**, void*);
typedef jint (JNICALL *GetCreatedJavaVMs)(JavaVM**, jsize, jsize *);
typedef struct java_var*  pjava_var;
typedef struct java_call* pjava_call;

using namespace std;
class JVM
{
    public:
        JVM();
        virtual ~JVM();

        pjava_var CallMethod(const char* className, const char* methodName, char* param) throw(string);
        static const char* getJNIErrorMessage(int);
        const char* getJVMSystemProperty(const char* propName);


    private:
        JNIEnv            *env;
        JavaVM            *jvm;
        HINSTANCE         hVM;
        CreateJavaVM_t    pCreateJavaVM_t;
        GetCreatedJavaVMs pGetCreatedJavaVMs;
        JavaVMInitArgs    vm_args;
        JavaVMOption      options[1];
        Utils             *util;


        JNIEnv* create_vm() throw(int);
        inline string getExceptionMessage();
        const char* getClassName(jclass jClass) throw(string);
        const char* toString(jobject value);
        inline const char* getAsciiString(jstring arg);
        pjava_call prepareCall(const char* className, const char* methodName, char* param, const char* signature) throw(string);
};

#endif // JVM_H
