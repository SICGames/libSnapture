#pragma once
#include <Windows.h>
#include <string>
#include "Region.h"
#include "DPI.h"
#pragma pack(4)
typedef struct Monitor
{
	INT ID;
	wchar_t* Name;
	wchar_t* DeviceName;
	wchar_t* MonitorName;
	HMONITOR MonitorHandle;
	HDC hDCMonitor;
	Region ScreenBounds;
	Region WorkAreaBounds;
	DPI Dpi;
	INT isPrimary;
} MONITOR, *LPMONITOR;