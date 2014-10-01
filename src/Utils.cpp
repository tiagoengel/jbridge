#include "../include/Utils.h"

Utils::Utils()
{
    //ctor
}

Utils::~Utils()
{
    //dtor
}

char* Utils::getProperty(const char* section, const char* keyName)
{
    char * value = (char*)malloc(sizeof(char*));
    GetPrivateProfileString(section, keyName, "", value, 256, "E:\\CodeBlocks\\JBridge\\JBridge.ini");
    printf("%s\n",value);
    return value;
}

vector<string> Utils::split(string s, char delim)
{
	stringstream ss(s);
	string buf;
	vector<string> tokens;
	while (getline(ss, buf, delim))
		tokens.push_back(buf);

	return tokens;
}

int Utils::exist(const char *name)
{
    struct stat   buffer;
    return (stat (name, &buffer) == 0);
}

string Utils::normalize(string str) {
    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == '\\')
            str[i] = '/';
    }
    return str;
}

string Utils::getJavaHome() throw (string) {
     char psBuffer[1000];
     FILE *chkdsk;

     string appPath = getPathApp();

     if (exist((appPath+"/../../Programas/Java/jre1.6.0_10").c_str()))
        return appPath+"/../../Programas/Java/jre1.6.0_10";

     if (exist((appPath+"../../Programas/Java/jdk1.6.0_10").c_str()))
        return appPath+"../../Programas/Java/jdk1.6.0_10";

     if (!getenv("JAVA_HOME")) {
        string javaExe = "java.exe";
        string cmd = javaExe + " -cp "+appPath+"/shell/jbridge.jar com.jbridge.JavaHome";

        if( (chkdsk = _popen( cmd.c_str(), "rt" )) == NULL ) {
          throw "Não foi possível encontrar a instalação do JAVA no WINDOWS desse computador";
        }

        fgets( psBuffer, sizeof(psBuffer), chkdsk );
        if (_pclose( chkdsk ) != 0) {
          throw "Não foi possível encontrar a instalação do JAVA no WINDOWS desse computador";
        }

        psBuffer[strlen(psBuffer) - 1] = '\0';
        return (string) psBuffer;

    } else {
        string java_home = getenv("JAVA_HOME");
        java_home = normalize(java_home);
        if (exist((java_home+"/jre").c_str())) {
            java_home = java_home+"/jre";
        }
        return java_home;
    }
}

string Utils::getExtDirs() throw (string) {
    return "-Djava.ext.dirs="+ getPathApp() + "/shell";
}

string Utils::getPathApp() throw (string) {
    if (!getenv(DLL_PATH_VAR)) {
        string error = "Não foi possível encontrar a váriavel de ambiente ";
        error = error + DLL_PATH_VAR;
        error = error+", não será possível iniciar a JVM";
        throw error;
    }

    return normalize(getenv(DLL_PATH_VAR));
}


/**
 *
 */
char ** Utils::split(char *frase, char separador, int* count)
{
    if (strlen(frase) == 0)
    {
        *count = 0;
        return NULL;
    }
    int i, j, k, contsep = 0;

    for(i=0,contsep=0;i<strlen(frase);i++)
    {
       if(frase[i] == separador)
       {
          contsep++;
       }
    }

    if (contsep == 0 )
    {
       //printf("entrei1\n");
       char **result = (char**)malloc(sizeof(char));
       *(result) = (char*)malloc(40*sizeof(char));
       strcpy(*(result), frase);
       *count = 1;
       return result;
    }

    char  aux[contsep][20];
    char **result = (char**)malloc(contsep*sizeof(char));

    if(contsep)
    {
        for(i=0; i<=contsep; i++ )
          *(result + i) = (char*)malloc(40*sizeof(char));

        for(i=0,k=0,j=0; i < strlen(frase); i++)
           if(frase[i] != separador)
           {
              aux[k][j] = frase[i];
              j++;
           }
           else
           {
              aux[k][j] = 0;
              k++;
              j=0;
           }
        aux[k][j] = 0;

        for(i=0;i<=contsep;i++)
          *(result+i) = strcpy(*(result+i), aux[i]);

        *count = contsep+1;
        return result;
    }
    *count = 0;
    return NULL;
}
