#pragma once
#include <Windows.h>
#include <stdexcept>
#include "../API.h"
#include "../Helpers/GDIHelper.h"

typedef struct tagImageData
{
	int Width;
	int Height;
	int Channels;
	int Stride;
	int BitsPerPixel;
	int RowSize;
	int Size;
	unsigned char* Data;
} ImageData, *LPImageData;


//-- prototypes
//-- C# Pinvoking. Does not work in C++ application without &. Thus, screwing up C# pinvokes.
SNAPTUREAPI void CreateImageDataFromHBITMAP(_Out_ LPImageData imagedata, const HBITMAP bitmap);
SNAPTUREAPI void CreateImageData(_Out_ LPImageData imagedata, const int width, const int height, const int channels,const void* pixels);
SNAPTUREAPI bool SaveImageDataToFile(LPImageData imagedata,const WCHAR* filename);

SNAPTUREAPI void ReleaseImageData(const LPImageData imagedata);
SNAPTUREAPI void CreateWindowsLogo(const LPImageData imagedata);