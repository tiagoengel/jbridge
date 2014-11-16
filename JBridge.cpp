#include "JBridge.h"
#include <Windows.h>

using namespace std;

JVM* jvm;

pjava_var DLL_EXPORT CallMethod (const char* className, const char* methodName, char* param)
{
    if (jvm == NULL) {
        log.info("JVM not created, returning NULL");
        return NULL;
    }

    pjava_var retorno = (pjava_var)malloc(sizeof(pjava_var*));
    try
    {
    	retorno = jvm->CallMethod(className, methodName, param);
    }
    catch (string ex)
    {
        retorno->type = "Exception";
        retorno->value = ex.c_str();
    }
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
            log.clean();
            log.info("JBridge.dll attached. Trying to create a JVM...");
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
            catch (string e)
            {
                MessageBox(NULL, e.c_str(), "Erro Fatal", MB_ICONERROR);
                jvm = NULL;
            }
            break;

        case DLL_PROCESS_DETACH:
            delete jvm;
            break;

        case DLL_THREAD_ATTACH:
            //printf("Using JVM version:%s\n", jvm->getJVMSystemProperty("java.version"));
            break;
        case DLL_THREAD_DETACH:

            break;
    }
    return TRUE; // succesful
}
#ifdef __cplusplus
}
#endif
