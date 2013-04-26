#include <string>

#include "face_pipeline.h"

#ifdef CALLBACKPROC_EXPORTS
#define CALLBACKPROC_API __declspec(dllexport)
#else
#define CALLBACKPROC_API __declspec(dllimport)
#endif

// our sample callback will only take 1 string parameter
//typedef void (CALLBACK * fnCallBackFunc)(std::string value);

// marked as extern "C" to avoid name mangling issue
extern "C++"
{
	//this is the export function for subscriber to register the callback function
	//CALLBACKPROC_API void Register_Callback(fnCallBackFunc func);

	CALLBACKPROC_API ISuperClass* Inittt(void);
	
	//CALLBACKPROC_API ISuperClass* Inittt(fnCallBackFunc func);
}

///ISuperClass* Init(void);