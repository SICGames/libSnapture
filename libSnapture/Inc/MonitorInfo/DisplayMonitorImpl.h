#pragma once
#include "../../Inc/API.h"
#include "DisplayMonitor.h"

SNAPTUREAPI void* CreateDisplayMonitorClass();
SNAPTUREAPI void DestroyDisplayMonitorClass(void*);
SNAPTUREAPI void DisplayMonitorGetMonitors(void*, Monitor**, size_t*);
SNAPTUREAPI void FreeMemoryResource(void*);