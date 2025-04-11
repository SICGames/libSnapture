#include "../../../Inc/GDI/GDIScreenCapture.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

/// <summary>
/// Captures Desktop from specified monitor
/// </summary>
/// <param name="monitor"></param>
/// <returns></returns>
HBITMAP CaptureDesktopScreen(int x, int y, int width, int height) 
{
    HDC monitorDC = GetDC(NULL);
	HDC compatiableDC = CreateCompatibleDC(monitorDC);
    
	HBITMAP capturedBitmap = CreateCompatibleBitmap(monitorDC, width, height);
	HGDIOBJ originalBitmap = SelectObject(compatiableDC, capturedBitmap);
    
	if (!BitBlt(compatiableDC,
		0, 0,
		width,
		height,
		monitorDC, x,y, SRCCOPY | CAPTUREBLT)) {
		throw new std::exception("Bitblt failed");
	}
    
    DeleteObject(originalBitmap);
    DeleteDC(compatiableDC);
    ReleaseDC(NULL, monitorDC);
    return capturedBitmap;
}

/// <summary>
/// Creates HBitmap from specified monitor and region of that monitor.
/// </summary>
/// <param name="monitor"></param>
/// <param name="X">starting x location of region</param>
/// <param name="Y">starting y location of region</param>
/// <param name="Width"> width of region</param>
/// <param name="Height">height of region</param>
/// <returns></returns>
HBITMAP CaptureRegion(int X, int Y, int Width, int Height, int screenBoundsX, int screenBoundsY, int screenBoundsWidth, int screenBoundsHeight) {
    
    HDC monitorDC = GetDC(0);
    HDC compatiableDC = CreateCompatibleDC(monitorDC);

    int height, width, x, y;
    x = screenBoundsX + X;
    y = screenBoundsY + Y;

    width = screenBoundsWidth;
    height = screenBoundsHeight;

    HBITMAP capturedBitmap = CreateCompatibleBitmap(monitorDC, Width, Height);
    HGDIOBJ originalBitmap = SelectObject(compatiableDC, capturedBitmap);

    if (!BitBlt(compatiableDC,
        0, 0,
        Width,
        Height,
        monitorDC, x, y, SRCCOPY | CAPTUREBLT)) 
    {
        throw new std::exception("Bitblt failed");
    }
   
    DeleteObject(originalBitmap);
    DeleteDC(compatiableDC);
    ReleaseDC(NULL, monitorDC);

    return capturedBitmap;
}

void ReleaseCapturedBitmap(HBITMAP bitmap) 
{
    if (bitmap) {
        DeleteObject(bitmap);
    }
}

RECT GetVirtualScreenSize() 
{
    int vScreenWidth, vScreenHeight = 0;
    int screenX, screenY = 0;
   
    return { 0 };
}