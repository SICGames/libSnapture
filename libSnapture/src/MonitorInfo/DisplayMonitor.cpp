#include "../../Inc/MonitorInfo/DisplayMonitor.h"

int DisplayMonitor::mIndex = 0;

DisplayMonitor::DisplayMonitor() 
{
}
DisplayMonitor::~DisplayMonitor() {

}

std::vector<Monitor> DisplayMonitor::GetMonitors() 
{
    std::vector<Monitor> monitors;

    EnumDisplayMonitors(0, 0, EnumMonitorsCallback, reinterpret_cast<LPARAM>(&monitors)); //-- seems to be causing issues. Generally heap issues.

    return monitors;
}

