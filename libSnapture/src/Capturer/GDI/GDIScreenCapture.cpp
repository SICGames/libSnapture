#include "../../../Inc/GDI/GDIScreenCapture.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

/// <summary>
/// Captures Desktop from specified monitor
/// </summary>
/// <param name="monitor"></param>
/// <returns></returns>
HBITMAP CaptureDesktopScreen(Monitor* monitor) 
{
    HDC monitorDC = GetDC(NULL);
	HDC compatiableDC = CreateCompatibleDC(monitorDC);
    /*
    int height, width, x, y;
    x = monitor->ScreenBounds.X;
    y = monitor->ScreenBounds.Y;
    width = monitor->ScreenBounds.Width;
    height = monitor->ScreenBounds.Height;
    int vScreenWidth, vScreenHeight = 0;
    int screenX, screenY = 0;
    auto dpi = monitor->Dpi;

    screenX = GetSystemMetricsForDpi(SM_XVIRTUALSCREEN, dpi.X);
    screenY = GetSystemMetricsForDpi(SM_YVIRTUALSCREEN, dpi.X);
	vScreenWidth = GetSystemMetricsForDpi(SM_CXVIRTUALSCREEN, dpi.X);
	vScreenHeight = GetSystemMetricsForDpi(SM_CYVIRTUALSCREEN, dpi.Y);
    */

	HBITMAP capturedBitmap = CreateCompatibleBitmap(monitorDC, monitor->ScreenBounds.Width, monitor->ScreenBounds.Height);
	HGDIOBJ originalBitmap = SelectObject(compatiableDC, capturedBitmap);
    
	if (!BitBlt(compatiableDC,
		0, 0,
		monitor->ScreenBounds.Width,
		monitor->ScreenBounds.Height,
		monitorDC, monitor->ScreenBounds.X, monitor->ScreenBounds.Y, SRCCOPY | CAPTUREBLT)) {
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
HBITMAP CaptureRegion(Monitor* monitor, int X, int Y, int Width, int Height) {
    
    HDC monitorDC = GetDC(0);
    HDC compatiableDC = CreateCompatibleDC(monitorDC);

    int height, width, x, y;
    x = monitor->ScreenBounds.X + X;
    y = monitor->ScreenBounds.Y + Y;

    width = monitor->ScreenBounds.Width;
    height = monitor->ScreenBounds.Height;
    /*
    int vScreenWidth, vScreenHeight = 0;
    int screenX, screenY = 0;
    auto dpi = monitor->Dpi;

    screenX = GetSystemMetricsForDpi(SM_XVIRTUALSCREEN, dpi.X);
    screenY = GetSystemMetricsForDpi(SM_YVIRTUALSCREEN, dpi.X);
    vScreenWidth = GetSystemMetricsForDpi(SM_CXVIRTUALSCREEN, dpi.X);
    vScreenHeight = GetSystemMetricsForDpi(SM_CYVIRTUALSCREEN, dpi.X);
    */

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
    /*
    HBITMAP slicedBitmap = CreateCompatibleBitmap(monitorDC, Width, Height);
    HGDIOBJ slicedBmpObj = SelectObject(compatiableDC, slicedBitmap);

    if (!BitBlt(compatiableDC, 0, 0, Width, Height, monitorDC, X + monitor->ScreenBounds.X, Y + monitor->ScreenBounds.Y, SRCCOPY | CAPTUREBLT)) {
        throw new std::exception("Bitblt failed.");
    }
    
    DeleteObject(slicedBmpObj);
    */
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