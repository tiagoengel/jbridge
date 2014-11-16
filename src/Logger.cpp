#include "Logger.h"

Logger::~Logger()
{
}

FILE* Logger::openFile() {
    FILE* f = fopen(fileName.c_str(), "a");
    if (!f) {
        printf("Unable to open file %s. Error code: %d", fileName.c_str(), GetLastError());
    }
    return f;
}

void Logger::closeFile(FILE* file) {
    fclose(file);
    free(file);
}

void Logger::clean() {
    remove(fileName.c_str());
}

string Logger::currentTime() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,80,"%x %X",timeinfo);
    string result(buffer);
    return result;
}

void Logger::logf(string level, const char* format, va_list args) {
    string cDate = currentTime();
    string fmt(format);
    FILE *logFile = openFile();
    vfprintf(logFile, (cDate+" | "+level+" | "+fmt+"\n").c_str(), args);
    closeFile(logFile);
}

void Logger::log(string level, const char* format, string str) {
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    FILE *logFile = openFile();
    fprintf(logFile, ("%s | %s | "+((string)format)+"\n").c_str(), currentTime().c_str(), level.c_str(), cstr);
    closeFile(logFile);
    delete [] cstr;
}

void Logger::infof(const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    logf("INFO ", format, arguments);
    va_end(arguments);
}

void Logger::info(string msg) {
    log("INFO ", "%s", msg);
}

void Logger::errorf(const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    logf("ERROR", format, arguments);
    va_end(arguments);
}

void Logger::error(string msg) {
    log("ERROR", "%s", msg);
}
