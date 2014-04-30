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
