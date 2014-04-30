jbridge
=======

Simple way to call java functions inside a C or C++  program

Usage
=====

If you want to use inside a DLL file made in C, you can do something like this

			typedef pjava_var   (*pCallMethod)(const char*, const char*, char*, const char* formatter);
			typedef pjava_var   (*pCallMultiReturnMethod)(const char*, const char*);
			typedef pjava_var   (*pGetLastResult)(const char*, const char*);

			HMODULE                myDll;
			pCallMethod            p_CallMethod;
			pCallMultiReturnMethod p_CallMultiReturnMethod;
			pGetLastResult         p_GetLastResult;

			#ifdef __cplusplus
			extern "C"
			{
			#endif
			BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
			                       DWORD reason        /* Reason this function is being called. */ ,
			                       LPVOID reserved     /* Not used. */ )
			{
			    switch (reason)
			    {
			      case DLL_PROCESS_ATTACH:
			      		myDll = NULL;
			      		myDll = LoadLibrary("path_to_dll\JBridge.dll");
			      		if (myDll == NULL)
									MessageBox(NULL, "Unable to load JBridge.dll.", "Fatal Error", MB_ICONERROR);
								else
								{
									p_CallMethod = NULL;
									p_CallMethod = (pCallMethod)GetProcAddress(myDll, "CallMethod");
									p_CallMultiReturnMethod = NULL;
									p_CallMultiReturnMethod = (pCallMultiReturnMethod)GetProcAddress(myDll, "CallMultiReturnMethod");
									p_GetLastResult = NULL;
									p_GetLastResult = (pGetLastResult)GetProcAddress(myDll, "GetLastResult");
									if (p_CallMethod == NULL || p_CallMultiReturnMethod == NULL || p_GetLastResult == NULL)
										MessageBox(NULL, "Unable to load functions to call Java methods.", "Fatal Error", MB_ICONERROR);
								}
			        break;

			      case DLL_PROCESS_DETACH:
			            if (myDll)
			       		   FreeLibrary(myDll);
			        break;

			      case DLL_THREAD_ATTACH:			
			        break;

			      case DLL_THREAD_DETACH:			
			        break;
			    }

			    /* Returns TRUE on success, FALSE on failure */
			    return TRUE;
			}
			#ifdef __cplusplus
			}
			#endif
