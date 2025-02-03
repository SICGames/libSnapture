#pragma once
#include <Windows.h>
#include <stdexcept>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

class GDIHelper
{
public:
	void GDIPlus_Startup();
	void GDIPlus_Shutdown();
private:
	ULONG_PTR gdiplusToken;
};