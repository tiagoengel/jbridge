jbridge
=======

Simple way to call java functions inside a C or C++  program

Usage
=====
First load the DLL and the methods you want to use
```c
typedef pjava_var   (*pCallMethod)(const char*, const char*, char*, const char* formatter);
pCallMethod          p_CallMethod;

HMODULE    myDll;
myDll = LoadLibrary("path_to_dll\JBridge.dll");
p_CallMethod = (pCallMethod)GetProcAddress(myDll, "CallMethod");
```
	

Then you can call any static method in your classpath
```c	
pjava_var ret = p_CallMethod("java/lang/Math", "ceil", "Double@1.5");
printf("%s", ret->type); //Integer;
printf("%s", ret->value); //2;
```
