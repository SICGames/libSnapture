#include <iostream>
#include "../../libSnapture/Inc/SnaptureCommon.h"

#ifdef _DEBUG
#pragma comment(lib, "../../x64/Debug/libSnapture.lib")
#else 
#pragma comment(lib, "../../x64/Release/libSnapture.lib")
#endif

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
    
    auto collections = displayMonitor->GetMonitors();
    unsigned int sz = collections.size();

    for (auto i = 0; i < sz; i++) {
        Monitor monitor = collections.at(i);

        const char* name = ToMultibyteString(monitor.Name);
        const char* devicename = ToMultibyteString(monitor.DeviceName);
        const char* monitorName = ToMultibyteString(monitor.MonitorName);

        std::cout << "Name: " << name << std::endl;
        std::cout << "Display Name: " << devicename << std::endl;
        std::cout << "Monitor Name: " << monitorName << std::endl;
        const char* isPrimary = monitor.isPrimary == true ? "Yes" : "No";

        std::cout << "Is Primary: " << isPrimary << std::endl;

    }
    collections.clear();
}
