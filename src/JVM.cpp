#include "../include/JVM.h"
#include "../include/Property.h"

JVM::JVM()
{
    util = new Utils();
//    Property* prop;
//    try {
//        prop = new Property("E:/CodeBlocks/JBridge/jbridge.properties", "");
//        prop->load();
//    } catch (string e) {
//        printf("Failed to load configuration file %s\n", e.c_str());
//    }



    //hVM = LoadLibrary(util->getProperty("CONFIG", "jvm.path"));
    hVM = LoadLibrary(JVMHOME);
    if (hVM == NULL)
        throw (JNI_DLL_NOT_FOUND);

    pGetCreatedJavaVMs = (GetCreatedJavaVMs)GetProcAddress(hVM, "JNI_GetCreatedJavaVMs");
    if (pGetCreatedJavaVMs == NULL)
        throw (JNI_GET_CREATED);

    pCreateJavaVM_t = (CreateJavaVM_t)GetProcAddress(hVM, "JNI_CreateJavaVM");
	if (pCreateJavaVM_t == NULL)
		throw (JNI_CREATE);

	//options[0].optionString = (strcat("-Djava.ext.dirs=", util->getProperty("CONFIG", "java.ext.dirs")));
	const char* cPath = CLASSPATH;//prop->getProperty("java.ext.dirs").c_str();
	options[0].optionString = (char*)(cPath);//static_cast<char*>(CLASSPATH);
	//options[1].optionString = "-Xcheck:jni";
	//options[1].optionString = "-verbose:jni";
	vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1; //Numero de parametros passados (JavaVMOption)
    vm_args.options = options;
    vm_args.ignoreUnrecognized = 0;
    //fflush()

    CONNECTION = NULL;
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

void JVM::RedirectOutputs(jobject printStream)
{
    jclass sysClass;
    jmethodID setErr;
    jmethodID setOut;

    if ((sysClass = env->FindClass("java/lang/System")))
    {


        if (!env->ExceptionCheck())
        {
            setErr = env->GetStaticMethodID(sysClass, "setErr", "(Ljava/io/PrintStream;)V");
            setOut = env->GetStaticMethodID(sysClass, "setOut", "(Ljava/io/PrintStream;)V");
            if (setErr && setOut)
            {
                env->CallStaticVoidMethod(sysClass, setErr, printStream);
                env->CallStaticVoidMethod(sysClass, setOut, printStream);
            }
        }

    }

    if (env->ExceptionCheck())
        env->ExceptionDescribe();
}

void JVM::log(const char* level, jthrowable ex, const char* message)
{
    jclass loggerClass = NULL; // = env->FindClass("java/util/logging/Logger");
    jclass levelClass = NULL;
    jfieldID levelField = NULL;
    jobject objLevel = NULL;
    jobject logger = NULL;
    jmethodID getLogger = NULL;
    jmethodID log = NULL;
    jstring sMessage = NULL;

    env->ExceptionClear();

    if ((loggerClass = env->FindClass("java/util/logging/Logger")))
        if ((levelClass = env->FindClass("java/util/logging/Level")))
            if ((getLogger = env->GetStaticMethodID(loggerClass, "getLogger", "(Ljava/lang/String;)Ljava/util/logging/Logger;")))
                if ((levelField = env->GetStaticFieldID(levelClass, level, "Ljava/util/logging/Level;")))
                    if ((objLevel = env->GetStaticObjectField(levelClass, levelField)))
                        if ((logger = env->CallStaticObjectMethod(loggerClass, getLogger, env->NewStringUTF("JBRIDGE LOG"))))
                        {
                            sMessage = env->NewStringUTF(message);
                            if (ex != NULL)
                            {
                                if ((log = env->GetMethodID(loggerClass, "log", "(Ljava/util/logging/Level;Ljava/lang/String;Ljava/lang/Throwable;)V")))
                                {
                                    env->CallVoidMethod(logger, log, objLevel, sMessage, ex);
                                }
                            }
                            else
                            {
                                if ((log = env->GetMethodID(loggerClass, "log", "(Ljava/util/logging/Level;Ljava/lang/String;)V")))
                                {
                                    env->CallVoidMethod(logger, log, objLevel, sMessage);
                                }
                            }

                            //env->DeleteLocalRef(objLevel);
                            //env->DeleteLocalRef(logger);
                            env->DeleteLocalRef(sMessage);
                        }

    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
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

    int jni_error = pGetCreatedJavaVMs(&jvm, 1, NULL);
    if (jvm != NULL)
    {
        jni_error = jvm->AttachCurrentThread((void**) &env, NULL);

        if (jni_error < 0)
            throw (jni_error);

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
    display("formating exception...");
    jthrowable excAux = env->ExceptionOccurred();
    if (excAux) {
        display("initialize...");
        jclass excclsAux = env->GetObjectClass( excAux);
        jclass invocationTargetException = env->FindClass("java/lang/reflect/InvocationTargetException");
        jclass exccls = NULL;
        jthrowable exc = NULL;
        jstring message = NULL; //This is VERY VERY important!!!

        display("check invocationTargetException...");
        if (env->IsInstanceOf(excAux, invocationTargetException)) {
            display("true");
            jmethodID getTargetException = env->GetMethodID(excclsAux, "getTargetException", "()Ljava/lang/Throwable;");
            exc = static_cast<jthrowable>(env->CallObjectMethod(excAux, getTargetException));
            exccls = env->GetObjectClass(exc);
        } else {
           display("false");
           exccls = excclsAux;
           exc = excAux;
        }
        display("get message...");
        jmethodID getMessage = env->GetMethodID( exccls, "getMessage", "()Ljava/lang/String;");
        message = (jstring)(env->CallObjectMethod( exc, getMessage));
        display("get exception class...");
		string className = getClassName(exccls);
		//char* ret;
		//env->ExceptionDescribe();
		//toAscii(message, ret);
		string excMessage(getAsciiString(message));

		display("format message...");
		string result = className+": "+excMessage;
		//Need to be here
        env->ExceptionClear();

        display("save log...");
        jclass c = env->FindClass("com/toth/java/vision/util/ExceptionPrinter");
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

pjava_call JVM::prepareCall(const char* className, const char* methodName, char* param, const char* signature) throw(string)
{
    //USANDO ESSA CHAMADA TA DANDO SHUTDOWN NO VISION. NÃO SEI PORQUE

    if (env == NULL)
        throw(string("Exception in JVM.cpp:\n\tJava Enviroment NULL(JVM::prepareCall)"));

    printf("Start prepare call..\n");
    pjava_call jcall = (pjava_call)malloc(sizeof(pjava_call*));

    jclass clazz = env->FindClass( className);
	if (clazz == NULL)
        throw (getExceptionMessage());

	jmethodID mID = env->GetStaticMethodID( clazz, methodName, signature);
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

    jcall->callClazz      = clazz;
    jcall->callMethodId   = mID;
    jcall->callParameters = args;

    printf("Finish prepare call..\n");
    return jcall;
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

pjava_var JVM::CallMethod(const char* className, const char* methodName, char* param, const char* formatter) throw(string)
{
    display("initialize...");
    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try { env = create_vm();}
    catch (int ex) {throw(string(JVM::getJNIErrorMessage(ex)));}

    jclass clazz = env->FindClass(CALLER_CLASS);
	if (clazz == NULL)
        throw (getExceptionMessage());

	jmethodID mID = env->GetStaticMethodID( clazz, "callJavaFunction", SIMPLE_CALL_SIGNATURE);
	if (mID == NULL)
		throw (getExceptionMessage());

    //log("INFO",NULL,"Method loaded, parameters splitting...");
	jclass objClass = env->FindClass( "java/lang/Object");

    display("spliting parameters");
    display(param);
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
    jstring sFormatter = (formatter != NULL ? env->NewStringUTF(formatter) : NULL);
    display("calling method...");
    jobject ret = env->CallStaticObjectMethod(clazz, mID, classToCall, sMethod, CONNECTION, args, sFormatter);
    env->DeleteLocalRef(sMethod);
    env->DeleteLocalRef(sFormatter);
	display("successful call");
	env->DeleteLocalRef(args);
	//free(jcall);
	//env->CallVoidMethod(printStream, close);

    if (env->ExceptionCheck()) {
        //env->ExceptionDescribe();
        throw (getExceptionMessage());
    }
    //log("INFO",NULL,"Returning values..");

    display("check null return...");

	if (ret == NULL) {
	    //env->ExceptionClear();
	    jvm->DetachCurrentThread();
	    free(env);
	    free(retorno);
	    return NULL;
	}

    display("format return...");

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


/**
 * Tem a mesma funcionalidade do metodo CallMethod, com a diferença
 * que irá passar uma referencia global a um jobjectarray
 * para gravar os resultados da função.
 * Deve ser usada em funções com mais de um retorno.
 *
 * @param const char* className o nome da classe java
 * @param const char* methodName o nome do metodo java
 * @param const char* param os parametros passados
 * @param int results a quantidade de resultados
 * @return pjava_var ponteiro para um struct java_var, que representa uma variavel Java
 * @throw string
 */
pjava_var JVM::CallMultiReturnMethod(const char* className, char* param) throw(string)
{
    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    display("initialize...");
    try { env = create_vm();}
    catch (int ex) {throw(string(JVM::getJNIErrorMessage(ex)));}

    jclass clazz = env->FindClass(CALLER_CLASS);
	if (clazz == NULL)
        throw (getExceptionMessage());

	jmethodID mID = env->GetStaticMethodID(clazz, "callJavaMultiReturnFunction", MULTI_CALL_SIGNATURE);
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

    display("Call...");
	env->CallStaticVoidMethod(clazz, mID, env->FindClass(className), CONNECTION, args);
    display("Sucesfull Call!");
	env->DeleteLocalRef(args);
	//free(jcall);
	//env->CallVoidMethod(printStream, close);
	display("Check Exceptions...");
    if (env->ExceptionCheck())
    {
        throw (getExceptionMessage());
    }

    retorno->type  = "MultiReturn";
    retorno->value = "";

    jvm->DetachCurrentThread();
    free(env);
	return retorno;
}

pjava_var JVM::GetLastResult(const char* attributeName, const char* formatter) throw(string)
{
    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    display("get last result...");
    try { env = create_vm();}
    catch (int ex) {throw(string(JVM::getJNIErrorMessage(ex)));}

    jclass clazz = env->FindClass(CALLER_CLASS);
    if (clazz == NULL)
        throw (getExceptionMessage());

    jmethodID mID = env->GetStaticMethodID(clazz, "getLastResult", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
    if (mID == NULL)
        throw (getExceptionMessage());

    display("Call...");
    jobject ret = env->CallStaticObjectMethod(clazz, mID, env->NewStringUTF(attributeName), env->NewStringUTF(formatter));
    display("Suscessful call!");
    display("Check Exceptions!");
    if (env->ExceptionCheck())
        throw (getExceptionMessage());

    jclass retClass = env->GetObjectClass(ret);
    retorno->type = getClassName(retClass);
    retorno->value = toString(ret);

    env->DeleteLocalRef(ret);

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

    display("transalate utf to ascii...");

    int tam = env->GetStringLength(str);
    char* ret = (char *) malloc(tam + 1);

    if (ret != NULL)
    {
        //display("get unicode char...");
        const jchar* unicode = env->GetStringChars(str, NULL);
        int i;
        //display("mount unicode char...");
        for (i=0;i<tam;i++)
            ret[i] = unicode[i];

        ret[i] = '\0';
        //display("free mem...");
        env->ReleaseStringChars(str, unicode);
    }
    //display("return");
    return ret;
}

/**
 * Cria um objeto de conexão com algum banco de dados e grava na variavel CONNECTION
 * @param const char* className o nome da classe java
 * @param const char* methodName o nome do metodo java
 * @param const char* param os parametros passados
 * @return int 0 sucesso ou -1 erro
 * @throw string
 */
int JVM::StartJavaConnection(const char* className, const char* methodName, char* param) throw(string)
{
    try { env = create_vm(); }
    catch (int ex) { throw(string(JVM::getJNIErrorMessage(ex))); }

    jclass clazz = env->FindClass( className);
	if (clazz == NULL)
        throw (getExceptionMessage());

	jmethodID mID = env->GetStaticMethodID(clazz, methodName, CONNECTION_SIGNATURE);
	if (mID == NULL)
		throw (getExceptionMessage());

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

    jobject ret = env->CallStaticObjectMethod(clazz, mID, args);
	env->DeleteLocalRef(args);

	if (env->ExceptionCheck())
		throw(getExceptionMessage());

    if (ret == NULL)
        return -1;

    CONNECTION = env->NewGlobalRef(ret);
    env->DeleteLocalRef( ret);
    //TODO: Verificar a memória, provavelmente está com lixo.
    jvm->DetachCurrentThread();
    free(env);
	return 0;
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
























