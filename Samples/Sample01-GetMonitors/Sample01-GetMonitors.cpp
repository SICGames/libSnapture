#include <iostream>
#include "../../libSnapture/Inc/MonitorInfo/DisplayMonitor.h"
#include "../../libSnapture/Inc/MonitorInfo/MonitorCollection.h"
#pragma comment(lib, "../../x64/Debug/libSnapture.lib")

#define SNAPTUREAPI_IMPORT

const char* ToMultibyteString(const wchar_t* str) 
{
    auto length = lstrlenW(str) + 1;
    char* result = new char[length];
    size_t i;
    wcstombs_s(&i, result, length,  str, length - 1);
    return result;
}
int main()
{
    DisplayMonitor* displayMonitor = new DisplayMonitor();
    MonitorCollection* collections = new MonitorCollection();
    collections = displayMonitor->GetMonitors();
    unsigned int sz = collections->Count();

    for (auto i = 0; i < sz; i++) {
        Monitor monitor = collections->Get(i);

        const char* name = ToMultibyteString(monitor.Name);
        const char* devicename = ToMultibyteString(monitor.DeviceName);
        const char* monitorName = ToMultibyteString(monitor.MonitorName);

        std::cout << "Name: " << name << std::endl;
        std::cout << "Display Name: " << devicename << std::endl;
        std::cout << "Monitor Name: " << monitorName << std::endl;
        const char* isPrimary = monitor.isPrimary == true ? "Yes" : "No";

        std::cout << "Is Primary: " << isPrimary << std::endl;

    }
    collections->Clear();
    delete collections;

}
