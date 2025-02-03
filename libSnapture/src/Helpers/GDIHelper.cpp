#include "../../inc/Helpers/GDIHelper.h"

void GDIHelper::GDIPlus_Startup() 
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}
void GDIHelper::GDIPlus_Shutdown() {
	Gdiplus::GdiplusShutdown(gdiplusToken);
}