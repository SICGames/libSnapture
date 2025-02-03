#pragma once
#include "../API.h"
#include "ShellScalingApi.h"
#include <iostream>
#include "../../Inc/MonitorInfo/MonitorCollection.h"
#pragma comment(lib, "Shcore.lib")

class SNAPTURECLASS DisplayMonitor {
public:
	DisplayMonitor();
	~DisplayMonitor();
	std::vector<Monitor> GetMonitors();
	static int mIndex;
private:

	static LPWSTR getMonitorName(HMONITOR& monitor) 
	{
		DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
		MONITORINFOEXW info;
		info.cbSize = sizeof(info);
		GetMonitorInfo(monitor, &info);

		UINT32 requiredPaths, requiredModes;
		GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &requiredPaths, &requiredModes);
		std::vector<DISPLAYCONFIG_PATH_INFO> paths(requiredPaths);
		std::vector<DISPLAYCONFIG_MODE_INFO> modes(requiredModes);
		QueryDisplayConfig(QDC_DATABASE_CURRENT, &requiredPaths, paths.data(), &requiredModes, modes.data(), &currentTopologyId);

		for (auto &p : paths) {
			DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName;
			sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
			sourceName.header.size = sizeof(sourceName);
			sourceName.header.adapterId = p.sourceInfo.adapterId;
			sourceName.header.id = p.sourceInfo.id;

			DisplayConfigGetDeviceInfo(&sourceName.header);
			if (wcscmp(info.szDevice, sourceName.viewGdiDeviceName) == 0)
			{
				DISPLAYCONFIG_TARGET_DEVICE_NAME name;
				ZeroMemory(&name, sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME));
				name.header.size = sizeof(name);
				name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
				name.header.adapterId = p.sourceInfo.adapterId;
				name.header.id = p.targetInfo.id;
				auto result = DisplayConfigGetDeviceInfo(&name.header);
				if (result != ERROR_SUCCESS) 
				{
					OutputDebugString(L"Failed to obtain Monitor name. ");
				}
				OutputDebugString(name.monitorFriendlyDeviceName);
				int len = lstrlenW(name.monitorFriendlyDeviceName);
				LPWSTR monitorName = new WCHAR[len + 1];
				lstrcpyW(monitorName, name.monitorFriendlyDeviceName);
				return monitorName;
				//return (LPWSTR)name.monitorFriendlyDeviceName;
			}
		}
		return NULL;
	}

	

	static BOOL CALLBACK EnumMonitorsCallback(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
	{
		std::vector<Monitor>* pThis = reinterpret_cast<std::vector<Monitor>*>(pData);
		MONITORINFOEX monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMon, &monitorInfo);

		Monitor monitor;
		ZeroMemory(&monitor, sizeof(Monitor));

		monitor.ID = mIndex;
		//monitor.Name = std::wstring(monitorInfo.szDevice);
		
		auto length = lstrlenW(monitorInfo.szDevice);
		monitor.Name = new WCHAR[length + 1];
		lstrcpyW(monitor.Name, monitorInfo.szDevice);

		DISPLAY_DEVICE dd = { 0 };
		dd.cb = sizeof(DISPLAY_DEVICE);

		EnumDisplayDevices(0, mIndex, &dd, 0);

		//monitor.DeviceName = std::wstring(dd.DeviceString);
		
		auto deviceStringLen = lstrlenW(dd.DeviceString);
		monitor.DeviceName = new WCHAR[deviceStringLen + 1];
		lstrcpyW(monitor.DeviceName, dd.DeviceString);
		
		//monitor.MonitorName = std::wstring(getMonitorName(hMon));
		
		LPWSTR mName = getMonitorName(hMon);
		int mNameLen = lstrlenW(mName);
		monitor.MonitorName = new WCHAR[mNameLen];
		lstrcpyW(monitor.MonitorName, mName);

		monitor.MonitorHandle = hMon;
		monitor.hDCMonitor = hdc;
		
		RECT rect = *lprcMonitor;

		monitor.ScreenBounds = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };

		RECT workBounds = monitorInfo.rcWork;
		monitor.WorkAreaBounds = { workBounds.left, workBounds.top, workBounds.right - workBounds.left, workBounds.bottom - workBounds.top };
		
		UINT dpiX, dpiY;
		GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

		monitor.Dpi = DPI() = { dpiX, dpiY };

		INT _isPrimary;
		_isPrimary = monitorInfo.dwFlags & MONITORINFOF_PRIMARY;
		monitor.isPrimary = _isPrimary;

		pThis->push_back(monitor);
		mIndex++;
		
		return TRUE;
	}
	
};




