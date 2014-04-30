#ifndef JVM_H
#define JVM_H
#include <jni.h>
#include <iostream>
#include <windows.h>
#include "../include/Utils.h"

/**
 * Alguns dos possiveis erros na criação da JVM, os outros já estão definidos no jni.h;
 */
#define JNI_DLL_NOT_FOUND    (-7)
#define JNI_CREATE           (-8)
#define JNI_GET_CREATED      (-9)


/**
 * Assinatura padrão para os metodos de criação de conexão e chamada de função do vision
 */
#define VISION_SIGNATURE               "(Ljava/sql/Connection;[Ljava/lang/String;)Ljava/lang/Object;"
#define VISION_MULTI_RETURN_SIGNATURE  "(Ljava/sql/Connection;[Ljava/lang/String;[Ljava/lang/Object;)V"
#define CONNECTION_SIGNATURE           "([Ljava/lang/String;)Ljava/lang/Object;"
#define SIMPLE_CALL_SIGNATURE          "(Ljava/lang/Class;Ljava/lang/String;Ljava/sql/Connection;[Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;"
#define MULTI_CALL_SIGNATURE           "(Ljava/lang/Class;Ljava/sql/Connection;[Ljava/lang/String;)V"

#define CALLER_CLASS "com/toth/java/vision/util/FunctionCaller"

//#define DEBUG_RUNTIME
//#define DEBUG_MESSAGE

#ifdef DEBUG_RUNTIME
    #define CLASSPATH "-Djava.ext.dirs=E:\\Workspaces\\Java\\JNI_CALL"
    #define JVMHOME   "C:\\Program Files (x86)\\Java\\jdk1.6.0_23\\jre\\bin\\client\\jvm.dll"
#else
    #define CLASSPATH "-Djava.ext.dirs=C:\\Systextil\\App\\Systex5\\shell"
    #define JVMHOME   "C:\\Arquivos de programas\\Java\\jre6\\bin\\client\\jvm.dll"
#endif


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

        pjava_var CallMethod(const char* className, const char* methodName, char* param, const char* formatter) throw(string);
        pjava_var CallMultiReturnMethod(const char* className, char* param) throw(string);
        pjava_var GetLastResult(const char* attributeName, const char* formatter) throw(string);
        int StartJavaConnection(const char* className, const char* methodName, char* param) throw(string);
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
        jobject           CONNECTION;
        jobjectArray      LAST_RESULTS; //For vision multi return functions


        JNIEnv* create_vm() throw(int);
        inline string getExceptionMessage();
        const char* getClassName(jclass jClass) throw(string);
        const char* toString(jobject value);
        inline const char* getAsciiString(jstring arg);
        void RedirectOutputs(jobject printStream);
        void log(const char* level, jthrowable ex, const char* message);
        pjava_call prepareCall(const char* className, const char* methodName, char* param, const char* signature) throw(string);
};

#endif // JVM_H
