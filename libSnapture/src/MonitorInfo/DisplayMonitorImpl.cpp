#include "../../inc/MonitorInfo/DisplayMonitorImpl.h"

void* CreateDisplayMonitorClass() {
	return new DisplayMonitor();
}
void DestroyDisplayMonitorClass(void* displaymonitor) 
{
	if (displaymonitor != nullptr) {
		delete reinterpret_cast<DisplayMonitor*>(displaymonitor);
		displaymonitor = nullptr;
	}
}

void DisplayMonitorGetMonitors(void* displaymonitor, Monitor** monitorArray, size_t* numMonitors) {
	if (displaymonitor != nullptr)
	{
		std::vector<Monitor> monitors = ((DisplayMonitor*)(displaymonitor))->GetMonitors();

		*numMonitors = monitors.size();
		size_t sz = *numMonitors * sizeof(Monitor);
		*monitorArray = new Monitor[monitors.size()];
		//*monitorArray = reinterpret_cast<Monitor*>(malloc(sz));
		
		if (*numMonitors > 0) 
		{
			for (int x = 0; x < monitors.size(); x++)
			{
				if (*monitorArray)
				{
					const Monitor monitor = monitors.at(x);
					(*monitorArray)[x].ID = monitor.ID;
					(*monitorArray)[x].Name = monitor.Name;
					(*monitorArray)[x].DeviceName = monitor.DeviceName;
					(*monitorArray)[x].MonitorName = monitor.MonitorName;
					(*monitorArray)[x].MonitorHandle = monitor.MonitorHandle;
					(*monitorArray)[x].hDCMonitor = monitor.hDCMonitor;
					(*monitorArray)[x].Dpi = monitor.Dpi;
					(*monitorArray)[x].isPrimary = monitor.isPrimary;
					(*monitorArray)[x].ScreenBounds = monitor.ScreenBounds;
					(*monitorArray)[x].WorkAreaBounds = monitor.WorkAreaBounds;
				}
			}
		}

		monitors.clear();
	}
}
void FreeMemoryResource(void* ptr) 
{
	if (ptr != nullptr) 
	{
		delete[] ptr;
		//free(ptr);
		//ptr = nullptr;
	}
}