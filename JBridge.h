#ifndef _JBRIDGE_H_
#define _JBRIDGE_H_

#include <windows.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "include/jni.h"

#include "include/Utils.h"
#include "include/JVM.h"

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

pjava_var DLL_EXPORT CallMethod (const char* className, const char* methodName, char* param);

const char* DLL_EXPORT GetJVMSystemProperty (const char* propName);

#ifdef __cplusplus
}
#endif

#endif // _JBRIDGE_H_
