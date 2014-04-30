#ifndef PROPERTY_H
#define PROPERTY_H

#include <iostream>
#include <map>
#include <fstream>
#include <string.h>

using namespace std;

class Property
{
public:
    fstream* propStream;
    Property(string fileName, string fileDesc) throw(string);
    Property(fstream fStream, string fileName, string fileDesc);
    Property(string fileDesc);

    //Destructor
    virtual ~Property();

    map<string, string>* getMap();
    void store() throw(string);
    void load()  throw(string);

    void setFile(string fileName);

    void setProperty(string propName, string propValue);
    string getProperty(string propertyName);

protected:
private:
    map<string, string> propMap;
    string file;
    string fileDescription;
};

#endif // PROPERTY_H
