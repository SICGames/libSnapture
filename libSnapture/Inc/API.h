#pragma once
#ifndef SNAPTUREAPI_IMPORT 
#define SNAPTUREAPI extern "C" __declspec(dllexport)
#else
#define SNAPTUREAPI extern "C" __declspec(dllimport) 
#endif

#ifndef SNAPTURE_CLASS_IMPORT
#define SNAPTURECLASS __declspec(dllexport)
#else 
#define SNAPTURECLASS __declspec(dllimport)
#endif