#include "JBridge.h"
#include <Windows.h>

using namespace std;

JVM* jvm;

pjava_var DLL_EXPORT CallMethod (const char* className, const char* methodName, char* param, const char* formatter)
{
    if (jvm == NULL)
        return NULL;

    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try
    {
    	retorno = jvm->CallMethod(className, methodName, param, formatter);
    }
    catch (string ex)
    {
        retorno->type = "Exception";
        retorno->value = ex.c_str();
    }
    return retorno;
}


pjava_var DLL_EXPORT CallMultiReturnMethod (const char* className, char* param)
{
    if (jvm == NULL)
        return NULL;

    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try
    {
    	retorno = jvm->CallMultiReturnMethod(className,param);
    }
    catch (string ex)
    {
        retorno->type = "Exception";
        retorno->value = ex.c_str();
    }
    return retorno;
}


pjava_var DLL_EXPORT GetLastResult(const char* attributeName, const char* formatter)
{
    if (jvm == NULL)
        return NULL;

    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try
    {
        retorno = jvm->GetLastResult(attributeName, formatter);
    }
    catch (string ex)
    {
        retorno->type = "Exception";
        retorno->value = ex.c_str();
    }
    return retorno;
}

pjava_var DLL_EXPORT InitDBConnection (const char* className, const char* methodName, char* param)
{
    if (jvm == NULL)
        return NULL;
    //MessageBox(NULL, TEXT("Entrou"), TEXT("Info"), MB_ICONINFORMATION);

    int ret;
    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try
    {
        ret = jvm->StartJavaConnection(className, methodName, param);
    }
    catch (string ex)
    {
        retorno->type = "Exception";
        retorno->value = ex.c_str();
        return retorno;
    }

    retorno->type = "Integer";
    char buf[3];
    itoa(ret, buf, 10);
    retorno->value = buf;
    return retorno;
}
const char* DLL_EXPORT GetJVMSystemProperty (const char* propName)
{
    if (jvm == NULL) return "";

    return jvm->getJVMSystemProperty(propName);
}
#ifdef __cplusplus
extern "C"
{
#endif
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            try
            {
            	jvm = new JVM();
            }
            catch (int ex)
            {
                string err(JVM::getJNIErrorMessage(ex));
                string desc = "Erro ao Criar JVM:\n   ->"+err;
            	MessageBox(NULL, desc.c_str(), "Erro Fatal", MB_ICONERROR);
            	jvm = NULL;
            }
            break;

        case DLL_PROCESS_DETACH:
                delete jvm;
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
#ifdef __cplusplus
}
#endif
