jbridge
=======

This lib provide a simple way to interact with java programs from within a C or C++ program.
As for now, this lib is only suitable for use on windows. 

This project is been used for a very specific purpose and in turn is not very "generic" if
you want to use it and need some help feel free to contact me.

Usage
=====
First load the DLL and the `CallMethod` function.
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

Support for types is very basic, all `native` types are supported as well as String, Integer, Double, Float, Boolean.
The syntax for passing values to java function is `type@val`. Ex: "String@this is a string"