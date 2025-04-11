#pragma once
#include "../API.h"
#include "GDICore.h"
#include "../ImageData/ImageData.h"
#include "../MonitorInfo/Monitor.h"
#include <exception>
#include <stdexcept>
#include <vector>

SNAPTUREAPI HBITMAP CaptureDesktopScreen(int, int, int, int);
SNAPTUREAPI HBITMAP CaptureRegion(int, int, int, int, int, int, int, int);
SNAPTUREAPI void ReleaseCapturedBitmap(HBITMAP);
SNAPTUREAPI RECT GetVirtualScreenSize();