#pragma once
#include "../API.h"
#include "GDICore.h"
#include "../ImageData/ImageData.h"
#include "../MonitorInfo/Monitor.h"
#include <exception>
#include <stdexcept>
#include <vector>

SNAPTUREAPI HBITMAP CaptureDesktopScreen(Monitor*);
SNAPTUREAPI HBITMAP CaptureRegion(Monitor*, int, int, int, int);
SNAPTUREAPI void ReleaseCapturedBitmap(HBITMAP);