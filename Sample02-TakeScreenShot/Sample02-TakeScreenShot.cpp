// Sample02-TakeScreenShot.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <iostream>
#include <fstream>

#include "Sample02-TakeScreenShot.h"
#include "../libSnapture/Inc/SnaptureCommon.h"

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/libSnapture.lib")
#else 
#pragma comment(lib, "../x64/Release/libSnapture.lib")
#endif

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::vector<Monitor> monitors;
HBITMAP bitmap;
ULONG_PTR gdiplusToken;

bool captureDesktopViaMonitorsInterlopWay(int monitorIndex) 
{
    DisplayMonitor* display_monitor = reinterpret_cast<DisplayMonitor*>(CreateDisplayMonitorClass());
    Monitor* _monitors = NULL;
    size_t numMonitors = 0;
    DisplayMonitorGetMonitors(display_monitor, &_monitors, &numMonitors);
    bitmap = CaptureDesktopScreen(&_monitors[monitorIndex]);

    FreeMemoryResource(_monitors);
    //delete _monitors;
    numMonitors = 0;
    delete display_monitor;
    display_monitor = nullptr;
    return true;
}
bool CaptureDesktop(int);
bool RenderBitmap(HWND, HBITMAP);
void ReleaseSnapster();


bool CaptureDesktop(int monitorIndex) 
{
    DisplayMonitor* displaymonitor = new DisplayMonitor();

    //-- GetMonitors seems to cause heap corruption in Release builds.
    //-- Debug build is fine.
    monitors = displaymonitor->GetMonitors();
    Monitor* currentMonitor = &monitors.at(monitorIndex);
    bitmap = CaptureDesktopScreen(currentMonitor);
    delete displaymonitor;
    displaymonitor = nullptr;
    return true;
}

bool RenderBitmap(HWND hwnd, HBITMAP bitmap) 
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHBITMAP(bitmap, NULL);
    if (bmp) {
        HDC hDC = GetDC(hwnd);
        if (hDC)
        {
            Gdiplus::Graphics g(hDC);
            if (bmp->GetLastStatus() != S_OK)
            {
                OutputDebugString(L"Issue with loading bitmap.\n");
                Gdiplus::GdiplusShutdown(gdiplusToken);
                return false;
            }
            g.DrawImage(bmp, Gdiplus::Rect(0, 0, bmp->GetWidth(), bmp->GetHeight()));
        }
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
    bmp = nullptr;
    return true;
}
void ReleaseSnapster() 
{
    ReleaseCapturedBitmap(bitmap);
    if (monitors.size() > 0) {
        monitors.clear();
    }
}
using namespace Gdiplus;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SAMPLE02TAKESCREENSHOT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAMPLE02TAKESCREENSHOT));
    MSG msg;
    // Main message loop:
    
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ReleaseSnapster();
    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAMPLE02TAKESCREENSHOT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SAMPLE02TAKESCREENSHOT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   captureDesktopViaMonitorsInterlopWay(1);
   //CaptureDesktop(1);
   ShowWindow(hWnd, nCmdShow);
//   RenderBitmap(hWnd, bitmap);
   UpdateWindow(hWnd);
   return TRUE;
   //-- after exiting function, run-time stack check failure #2 - around variable c is corrupted.
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (bitmap) 
            {
                RenderBitmap(hWnd, bitmap);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
