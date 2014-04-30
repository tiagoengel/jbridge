#include "../include/Property.h"

//Construtores
Property::Property(string fileName, string fileDesc) throw(string)
{
    //std::cout << "teste" << endl;
    propStream = new fstream;
    propStream->open(fileName.c_str(), ios::in);
    if (propStream->fail())
    {
        throw ((string) "Erro ao abrir arquivo " + fileName + " para leitura");
    }
    file = fileName;
    fileDescription = fileDesc;
}
Property::Property(fstream fStream, string fileName, string fileDesc)
{
    propStream = new fstream;
    propStream = &fStream;
    file = fileName;
    fileDescription = fileDesc;


}
Property::Property(string fileDesc)
{
    file = "";
    fileDescription = fileDesc;
}

//Destructor
Property::~Property()
{
    if (propStream->is_open())
        propStream->close();
}


//Metodos
map< string, string>* Property::getMap()
{
    return &propMap;
}

void Property::load() throw(string)
{
    if (file == "")
        throw "Nenhum arquivo foi especificado";
    if (!propStream || !propStream->is_open())
        throw ((string)"Arquivo "+file+" não está pronto para leitura");

    string line="";
    getline(*propStream, line);
    while(*propStream)
    {
        if (line.empty() || line.substr(0,1) == "#")
        {
            continue;
            getline (*propStream, line);
        }

        string key = line.substr(0, strcspn(line.c_str(), "="));
        string value = line.substr(strcspn(line.c_str(), "=") + 1, sizeof(line));
        propMap[key] = value;
        getline (*propStream, line);
    }

}
void Property::store() throw(string)
{
    fstream fOut;
    fOut.open(file.c_str(), ios::out);
    if (fOut.fail())
        throw ("Falha ao abrir arquivo "+file+" para escrita");

    if (strlen(fileDescription.c_str()) > 0)
    {
        fOut << "#"+fileDescription << endl;
    }

    map< string, string >::iterator iter;
    for (iter = propMap.begin(); iter != propMap.end(); iter++)
    {
        fOut << iter->first << "=" << iter->second << endl;
    }
    fOut.flush();
    fOut.close();

}
void Property::setFile(string fileName)
{
    file = fileName;
}
string Property::getProperty(string propertyName)
{
    return propMap[propertyName];
}
void Property::setProperty(string propName, string propValue)
{
    //if (!propMap[propName])
    //throw ((string)"A propriedade "+propName+ " não existe").c_str();

    propMap[propName] = propValue;

}

