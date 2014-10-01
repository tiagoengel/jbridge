#ifndef UTILS_H
#define UTILS_H

#include "jni.h"
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <sys/stat.h>

#define DLL_PATH_VAR "SYSTEX5"

//#define DEBUG_RUNTIME
//#define DEBUG_MESSAGE

#ifdef DEBUG_MESSAGE
    #define display(x)\
    MessageBox(NULL, x, "Debug", MB_OK)
#else
    #define display(x)\
    printf("%s\n",x)
#endif

using namespace std;


class Utils
{
    public:
        Utils();
        virtual ~Utils();

        char **split(char *frase, char separador, int* count);
        vector<string> split(string s, char delim);

        char* getProperty(const char* section, const char* keyName);
        string getJavaHome() throw (string);
        string getExtDirs() throw (string);
        string normalize(string str);
        string getPathApp() throw (string);
        int exist(const char*);

    protected:
    private:
};

#endif // UTILS_H
