#pragma once
#include "DirectXHeaders.h"
#include "../Helpers/DirectXErrorHandler.h"

#include "../MonitorInfo/Monitor.h"
#include "../Helpers/DirectXMacros.h"
#include "../ImageData/ImageData.h"

#include <atlbase.h>
#include <memory>
#include <vector>
#include "../API.h"

using namespace ATL;

#pragma region Declarations
ID3D11Device* pDevice = 0;
ID3D11DeviceContext* pDeviceContext = 0;
IDXGIOutput* pDXGIOutput;
IDXGIOutput1* pDXGIOutput1;
IDXGIDevice* pDXGIDevice = nullptr;
IDXGIOutputDuplication* pDesktopDuplication = 0;
bool hasFrameLocked;
#pragma endregion

SNAPTUREAPI INT InitDesktopDuplicationCapture(Monitor*);
SNAPTUREAPI bool DesktopDuplicationCaptureDesktop(HBITMAP&);
SNAPTUREAPI void ReleaseDesktopDuplicationResources();
SNAPTUREAPI void ReleaseDesktopDuplicationFrame();
