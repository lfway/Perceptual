// PerceptualCore.cpp: определяет экспортированные функции для приложения DLL.
//
#include "stdafx.h"
#include "PerceptualCore.h"
#include <string>
#include <math.h>
#include <sstream>


//CALLBACKPROC_API ISuperClass* Inittt(fnCallBackFunc func)
CALLBACKPROC_API ISuperClass* Inittt(void)
{
	return new TheSuperClass();
}

/*CALLBACKPROC_API void Register_Callback(fnCallBackFunc func)
{
	int count = 0;
	while(count < 10)
	{
		std::stringstream msg;
		msg << "Message #" << count;
		func(msg.str());
		count++;
		Sleep(2000);
	}
}*/