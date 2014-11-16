#ifndef LOGGER_H
#define LOGGER_H

#include <cstdarg>
#include <time.h>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>

using namespace std;

class Logger
{

    public:
        static Logger& getLogger() {
            static Logger instance; // Guaranteed to be destroyed. Instantiated on first use.
            return instance;
        }

        virtual ~Logger();
        void infof(const char* format, ...);
        void info(string msg);
        void errorf(const char* format, ...);
        void error(string msg);
        void clean();

    protected:
    private:
    	Logger() {
            string tmpDir = getenv("temp");
            fileName = tmpDir+"\\jbridgedll.log";
    	};

         //Prevent assignment
        void operator=(Logger const&); // Don't implement
        string fileName;
        FILE *openFile();
        void closeFile(FILE *file);
        void logf(string level, const char* format, va_list args);
        void log(string level, const char* format, string args);
        string currentTime();
};

#endif // LOGGER_H
