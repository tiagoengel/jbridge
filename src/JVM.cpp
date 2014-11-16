#include "../include/JVM.h"
#include "../include/Property.h"
#include "../include/Logger.h"

JVM::JVM()
{
    util = new Utils();

    log.info("Trying to load ext dirs...");
    string ext_dirs = util->getExtDirs();
    log.info("Trying to load JAVA HOME...");
    string java_home = util->getJavaHome();

    string jvmDll = java_home + "/bin/client/jvm.dll";
    if (!util->exist(jvmDll.c_str())) {
        jvmDll = java_home + "/bin/server/jvm.dll";
    }
    log.infof("System properties:\n\nJAVA_HOME=%s\n%s\n\n", java_home.c_str(), ext_dirs.c_str());
    log.infof("Using jvm file: %s", jvmDll.c_str());
    log.infof("File %s", (util->exist(jvmDll.c_str()) == 1 ? "exists" : "not exists"));
    hVM = LoadLibrary(jvmDll.c_str());
    if (hVM == NULL) {
        log.errorf("Unable to load jvm.dll. Error code: %d", GetLastError());
        throw (JNI_DLL_NOT_FOUND);
    }

    log.info("JVM successfully loaded");
    pGetCreatedJavaVMs = (GetCreatedJavaVMs)GetProcAddress(hVM, "JNI_GetCreatedJavaVMs");
    if (pGetCreatedJavaVMs == NULL) {
        log.errorf("Unable to find method 'JNI_GetCreatedJavaVMs' in jvm.dll. Error code: %d", GetLastError());
        throw (JNI_GET_CREATED);
    }

    pCreateJavaVM_t = (CreateJavaVM_t)GetProcAddress(hVM, "JNI_CreateJavaVM");
	if (pCreateJavaVM_t == NULL) {
        log.errorf("Unable to find method 'JNI_CreateJavaVM' in jvm.dll. Error code: %d", GetLastError());
		throw (JNI_CREATE);
	}

	const char* cPath = ext_dirs.c_str();
	options[0].optionString = (char*)(cPath);

	//options[1].optionString = "-Xcheck:jni";
	//options[1].optionString = "-verbose:jni";
	vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1; //Numero de parametros passados (JavaVMOption)
    vm_args.options = options;
    vm_args.ignoreUnrecognized = 0;

    jvm = NULL;
    env = NULL;
}

JVM::~JVM()
{
    delete util;
    if (env)
        free(env);

    if (hVM)
        FreeLibrary(hVM);
}

/**
 * Cria nova instancia da jvm, caso não exista nenhuma,
 * e retorna um ponteiro para o JNIEnv criado.
 * Caso já exista uma instancia da JVM criada, "junta"
 * essa thread com a da JVM e retorna um ponteiro para o
 * JNIEnv
 *
 * @return JNIEnv*
 * @throw int - código de erro da criação da JVM
 */
JNIEnv* JVM::create_vm() throw(int)
{
    JNIEnv *env;
    jvm = NULL;

    log.info("Creating JVM");
    int jni_error = pGetCreatedJavaVMs(&jvm, 1, NULL);
    if (jvm != NULL)
    {
        log.info("Found and existing instance, attaching...");
        jni_error = jvm->AttachCurrentThread((void**) &env, NULL);

        if (jni_error < 0) {
            throw (jni_error);
        }

        return env;
    }
    jni_error = pCreateJavaVM_t(&jvm, (void**)&env, &vm_args);
    if(jni_error < 0)
        throw (jni_error);

    return env;
}

const char* JVM::getJNIErrorMessage(int jni_error)
{
    if (jni_error == 0)
       return "";

    switch (jni_error)
    {
       case JNI_ERR           : return "Erro desconhecido"    ; break;
       case JNI_EDETACHED     : return "Thread separada da VM"; break;
       case JNI_EVERSION      : return "Erro de versão"; break;
       case JNI_ENOMEM        : return "Memória insuficiente"; break;
       case JNI_EINVAL        : return "Argumetos inválidos"; break;
       case JNI_EEXIST        : return "VM já criada"; break;
       case JNI_DLL_NOT_FOUND : return "jvm.dll não encontrada"; break;
       case JNI_CREATE        : return "Erro ao localizar metodo JNI_CreateJavaVM"; break;
       case JNI_GET_CREATED   : return "Erro ao localizar metodo JNI_GetCreatedJavaVMs"; break;
       default : return "Erro desconhecido";
    }
}

const char* JVM::toString(jobject value)
{
	jclass stringClass = env->FindClass( "java/lang/String");
	jmethodID valueOf = env->GetStaticMethodID( stringClass, "valueOf", "(Ljava/lang/Object;)Ljava/lang/String;");
	jstring name1 = (jstring)env->CallStaticObjectMethod( stringClass, valueOf, value);

	const char* retName = getAsciiString(name1);

    env->DeleteLocalRef(name1);
	return retName;
}

const char* JVM::getClassName(jclass jClass) throw(string)
{

	jclass clazz = env->FindClass( "java/lang/Class");
    jmethodID getName = env->GetMethodID( clazz, "getSimpleName", "()Ljava/lang/String;");

    jstring name = (jstring)env->CallObjectMethod(jClass, getName);
    const char* ret = env->GetStringUTFChars(name,0);

    env->DeleteLocalRef( name);

	return ret;
}

inline string JVM::getExceptionMessage()
{
    jthrowable excAux = env->ExceptionOccurred();
    if (excAux) {
        log.info("Trying to get last exception message...");
        jclass excclsAux = env->GetObjectClass( excAux);
        jclass invocationTargetException = env->FindClass("java/lang/reflect/InvocationTargetException");
        jclass exccls = NULL;
        jthrowable exc = NULL;
        jstring message = NULL; //This is VERY VERY important!!!

        if (env->IsInstanceOf(excAux, invocationTargetException)) {
            log.info("Found an InvocationTargetException");
            jmethodID getTargetException = env->GetMethodID(excclsAux, "getTargetException", "()Ljava/lang/Throwable;");
            exc = static_cast<jthrowable>(env->CallObjectMethod(excAux, getTargetException));
            exccls = env->GetObjectClass(exc);
        } else {
           exccls = excclsAux;
           exc = excAux;
        }
        log.info("Calling method 'getMessage'");
        jmethodID getMessage = env->GetMethodID( exccls, "getMessage", "()Ljava/lang/String;");
        message = (jstring)(env->CallObjectMethod( exc, getMessage));
		string className = getClassName(exccls);

		string excMessage(getAsciiString(message));


		string result = excMessage;

        env->ExceptionClear();

        log.info("Printing exception...");
        jclass c = env->FindClass("com/jbridge/ExceptionPrinter");
        jmethodID mId = env->GetStaticMethodID(c, "print", "(Ljava/lang/Throwable;)V");
        env->CallStaticVoidMethod(c,mId,exc);

		env->DeleteLocalRef( message);
		env->DeleteLocalRef( exc);

        if (env->ExceptionCheck())
            env->ExceptionClear(); //Not forget this

		return result;
    }

	return "";
}

/**
 * Chama uma função java a partir dos parametros passados.
 * Caso a função utilize alguma conexão com o banco de dados
 * a função StartJavaConnection deve ser chamada pelo menos 1 vez
 * antes de qualquer chamada desta.
 * A função java deve obrigatoriamente ter a seguinte assinatura
 * public static Object functionName(Connection conn, String args[]);
 *
 * @param const char* className o nome da classe java
 * @param const char* methodName o nome do metodo java
 * @param const char* param os parametros passados
 * @return pjava_var ponteiro para um struct java_var, que representa uma variavel Java
 * @throw string
 */

pjava_var JVM::CallMethod(const char* className, const char* methodName, char* param) throw(string)
{
    log.infof("JVM::CallMethod(%s, %s, %s)", className, methodName, param);
    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try { env = create_vm();}
    catch (int ex) {throw(string(JVM::getJNIErrorMessage(ex)));}

    jclass clazz = env->FindClass(DISPATCH_CLASS);
	if (clazz == NULL)
        throw (getExceptionMessage());

	jmethodID mID = env->GetStaticMethodID( clazz, "dispatch", CALL_SIGNATURE);
	if (mID == NULL)
		throw (getExceptionMessage());

    //log("INFO",NULL,"Method loaded, parameters splitting...");
	jclass objClass = env->FindClass( "java/lang/Object");

    string s(param);
    vector<string> sParam = util->split(s, ';');
    jobjectArray args = env->NewObjectArray(sParam.size(),objClass, NULL);


    unsigned int i;
    for (i=0;i<sParam.size();i++)
    {
        jobject objt = env->NewStringUTF(sParam[i].c_str());
        env->SetObjectArrayElement(args, i, objt);

        env->DeleteLocalRef( objt);
    }

    if (env->ExceptionCheck())
        throw (getExceptionMessage());


    jclass classToCall = env->FindClass(className);

    if (classToCall == NULL)
        throw (getExceptionMessage());

    jstring sMethod = env->NewStringUTF(methodName);
    log.info("Calling Java method...");
    jobject ret = env->CallStaticObjectMethod(clazz, mID, classToCall, sMethod, args);
    env->DeleteLocalRef(sMethod);
	log.info("Method successfully called");
	env->DeleteLocalRef(args);

    if (env->ExceptionCheck()) {
        throw (getExceptionMessage());
    }

	if (ret == NULL) {
	    jvm->DetachCurrentThread();
	    free(env);
	    free(retorno);
	    return NULL;
	}

    log.info("Formating result");

    jclass retClass = env->GetObjectClass(ret);
    retorno->type = getClassName(retClass);
    retorno->value = toString(ret);

	if (env->ExceptionCheck())
        throw (getExceptionMessage());
    //TODO: Verificar a memória, provavelmente está com lixo.
    jvm->DetachCurrentThread();
    free(env);
	return retorno;
}

inline const char* JVM::getAsciiString(jstring str)
{
    if (str == NULL || !str) return "";

    log.info("transalate utf to ascii...");

    int tam = env->GetStringLength(str);
    char* ret = (char *) malloc(tam + 1);

    if (ret != NULL)
    {
        const jchar* unicode = env->GetStringChars(str, NULL);
        int i;

        for (i=0;i<tam;i++)
            ret[i] = unicode[i];

        ret[i] = '\0';
        env->ReleaseStringChars(str, unicode);
    }
    return ret;
}

const char* JVM::getJVMSystemProperty(const char* propName)
{
    try {env = create_vm();}
    catch (int ex) { return "";}

    jclass clazz = env->FindClass("java/lang/System");
    if (clazz == NULL) return "";

    jmethodID mID = env->GetStaticMethodID(clazz, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
    if (mID == NULL) return "";

    jstring par = env->NewStringUTF(propName);
    jstring prop = static_cast<jstring>(env->CallStaticObjectMethod(clazz, mID, par));

    const char* ret = getAsciiString(prop);
    env->DeleteLocalRef(prop);
    env->DeleteLocalRef(par);

    if (env->ExceptionOccurred())
    {
        env->ExceptionClear();
        return "";
    }

    jvm->DetachCurrentThread();
    free(env);
    return ret;
}
























