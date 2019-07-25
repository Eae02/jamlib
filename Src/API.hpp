#pragma once

#ifdef _WIN32
	#define JM_C_EXPORT extern "C" __declspec(dllexport)
	
	#ifdef JM_BUILDING_LIB
		#define JAPI __declspec(dllexport)
	#else
		#define JAPI __declspec(dllimport)
	#endif
#else
	#define JAPI __attribute__((visibility("default")))
	#define JM_C_EXPORT extern "C" JAPI
#endif
