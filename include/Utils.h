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

using namespace std;


class Utils
{
    public:
        Utils();
        virtual ~Utils();

        char **split(char *frase, char separador, int* count);
        vector<string> split(string s, char delim);

        char* getProperty(const char* section, const char* keyName);

    protected:
    private:
};

#endif // UTILS_H
