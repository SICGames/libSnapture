#include "../../../Inc/DesktopDuplication/DesktopDuplicationCapture.h"
#include "../../../Inc/Helpers/ErrorHandler.h"
INT InitDesktopDuplicationCapture(Monitor* monitor) 
{
	CoInitialize(NULL);
	D3D_FEATURE_LEVEL featureLevel;

	UINT CreationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG) 
	CreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DX::ThrowIfFailed(D3D11CreateDevice(0,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		CreationFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&pDevice,
		&featureLevel,
		&pDeviceContext));
	
	
	IDXGIAdapter1* pDXGIAdapter = nullptr;

	DX::ThrowIfFailed(pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice));

	DX::ThrowIfFailed(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void**)&pDXGIAdapter));

	DX::ThrowIfFailed(pDXGIAdapter->EnumOutputs(monitor->ID, &pDXGIOutput));

	SafeRelease(pDXGIAdapter);

	DX::ThrowIfFailed(pDXGIOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&pDXGIOutput1));

	if(pDXGIDevice != 0) 
		DX::ThrowIfFailed(pDXGIOutput1->DuplicateOutput(pDXGIDevice, &pDesktopDuplication));

	return 0;
}
void ReleaseDesktopDuplicationFrame() {
	HRESULT hr = S_OK;
	if (pDesktopDuplication) 
	{
		hr = pDesktopDuplication->ReleaseFrame();
		if (SUCCEEDED(hr)) 
		{
			OutputDebugString(L"DX::ReleaseDesktopDuplicationFrame -> Successfully released frame..\n");
			return;
		}
		else if(FAILED(hr)) 
		{
			if (hr == DXGI_ERROR_INVALID_CALL) {
				OutputDebugString(L"DX::ReleaseDesktopDuplicationFrame -> Invalid Call. Frame Already Released Or Doesn't Exist..\n");
				return;
			}
			else if (hr == DXGI_ERROR_ACCESS_LOST) {
				OutputDebugString(L"DX::ReleaseDesktopDuplicationFrame -> DXGI Device lost.\n");
				SafeRelease(pDesktopDuplication);

				if (pDXGIDevice != 0)
					DX::ThrowIfFailed(pDXGIOutput1->DuplicateOutput(pDXGIDevice, &pDesktopDuplication));

				return;
			}
			//-- got to this point? 
			OutputDebugString(L"Releasing Desktop Duplication Frame Failed.");
		}
	}
}
bool DesktopDuplicationCaptureDesktop(HBITMAP& bitmap)
{
	HRESULT hr = S_OK;
	IDXGIResource* desktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo = { 0 };
	bool bCanProcessFrame = true;
	hr = pDesktopDuplication->AcquireNextFrame(500, &frameInfo, &desktopResource);
	if (SUCCEEDED(hr) && frameInfo.LastPresentTime.QuadPart == 0) {
		//-- no new update. Frame not acquired.
		SafeRelease(desktopResource);
		ReleaseDesktopDuplicationFrame();
		Sleep(1);
		return false;
	}

	if (FAILED(hr))
	{
		//-- why did it fail?
		if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
			OutputDebugString(L"DX::CaptureDesktop -> DXGI WAIT TIMEOUT.\n");
			SafeRelease(desktopResource);
			ReleaseDesktopDuplicationFrame();
			return false;
		}
		else if (hr == DXGI_ERROR_INVALID_CALL) {
			OutputDebugString(L"DX::CaptureDesktop -> Invalid Call. Frame still exists..\n");
			SafeRelease(desktopResource);
			ReleaseDesktopDuplicationFrame();
			return false;
		}
		else if (hr == DXGI_ERROR_ACCESS_LOST) {
			OutputDebugString(L"DX::CaptureDesktop -> DXGI Device lost.\n");
			SafeRelease(desktopResource);
			ReleaseDesktopDuplicationFrame();
			return false;
		}

		//-- got to this point? 
		OutputDebugString(L"Desktop Duplication Failed.");
		SafeRelease(desktopResource);
		ReleaseDesktopDuplicationFrame();
		return false;
	}
	
	OutputDebugString(L"DX::CaptureDesktop => Captured Frame.\n");
	DXGI_OUTDUPL_DESC DuplDesc = {};
	pDesktopDuplication->GetDesc(&DuplDesc);

	ID3D11Texture2D* gpuTexture = nullptr;
	hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&gpuTexture);
	if (FAILED(hr))
	{
		//-- not good.
		DX::ThrowIfFailed(hr);
		return false;
	}

	SafeRelease(desktopResource);

	D3D11_TEXTURE2D_DESC gpuTextureDesc = { 0 };
	gpuTexture->GetDesc(&gpuTextureDesc);
	gpuTextureDesc.Format = DuplDesc.ModeDesc.Format;
	gpuTextureDesc.Width = DuplDesc.ModeDesc.Width;
	gpuTextureDesc.Height = DuplDesc.ModeDesc.Height;
	gpuTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	gpuTextureDesc.Usage = D3D11_USAGE_STAGING;
	gpuTextureDesc.BindFlags = 0;
	gpuTextureDesc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GDI_COMPATIBLE ?

	ID3D11Texture2D* cpuTexture = nullptr;
	hr = pDevice->CreateTexture2D(&gpuTextureDesc, nullptr, &cpuTexture);
	if (FAILED(hr)) {
		//--- bad bad bad
		DX::ThrowIfFailed(hr);
		return false;
	}
	if (cpuTexture != nullptr && gpuTexture != nullptr)
	{
		pDeviceContext->CopyResource(cpuTexture, gpuTexture);

		//-- create hbitmap
		D3D11_TEXTURE2D_DESC cpuTextureDesc = { 0 };
		cpuTexture->GetDesc(&cpuTextureDesc);
		int width = cpuTextureDesc.Width;
		int height = cpuTextureDesc.Height;
		auto format = cpuTextureDesc.Format;
		auto channels = 4; //-- 32 bit ARgb
		int stride = width * channels; //-- since 32-bit, no need for padding.
		int bitsPerPixel = channels * 8;

		D3D11_MAPPED_SUBRESOURCE resource = { 0 };
		hr = pDeviceContext->Map(cpuTexture, 0, D3D11_MAP_READ, 0, &resource);

		if (FAILED(hr)) {
			//-- bad mojo.
			DX::ThrowIfFailed(hr);
			return false;
		}

		unsigned int imageSize = stride * height;
		unsigned char* bytes = reinterpret_cast<unsigned char*>(resource.pData);  //(unsigned char*)malloc(imageSize);

		//errno_t err = memcpy_s(bytes, imageSize, resource.pData, imageSize);

		pDeviceContext->Unmap(cpuTexture, 0);
		/*
		if (err != 0) {
			//--  issue
			throw new std::exception("memcpy_s failed");
			return false;
		}
		*/
		//-- Recreate bitmap from scratch
		BITMAPINFOHEADER bitmapinfoheader;
		ZeroMemory(&bitmapinfoheader, sizeof(BITMAPINFOHEADER));
		bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapinfoheader.biWidth = width;
		bitmapinfoheader.biHeight = -height;
		bitmapinfoheader.biBitCount = bitsPerPixel;
		bitmapinfoheader.biCompression = BI_RGB;
		bitmapinfoheader.biPlanes = 1;
		bitmapinfoheader.biClrImportant = 0;
		bitmapinfoheader.biClrUsed = 0;
		bitmapinfoheader.biSizeImage = imageSize;

		BITMAPINFO bitmapinfo;
		ZeroMemory(&bitmapinfo, sizeof(BITMAPINFO));
		bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapinfo.bmiHeader.biWidth = width;
		bitmapinfo.bmiHeader.biHeight = -height;
		bitmapinfo.bmiHeader.biBitCount = bitsPerPixel;
		bitmapinfo.bmiHeader.biCompression = BI_RGB;
		bitmapinfo.bmiHeader.biPlanes = 1;
		bitmapinfo.bmiHeader.biSizeImage = imageSize;

		bitmap = CreateDIBitmap(GetDC(NULL), &bitmapinfoheader, CBM_INIT, (const void*)bytes, &bitmapinfo, DIB_RGB_COLORS);

		if (bitmap == nullptr) {
			DWORD errorCode = GetLastError();
			std::wstring errorMessage = GetErrorMessage(errorCode);
			OutputDebugStringW(errorMessage.c_str());
			throw new std::exception("Unable to create bitmap from memory.");
		}

		//-- bitmap created. Yay!
		//free(bytes);
		bytes = nullptr;

		SafeRelease(cpuTexture);
		SafeRelease(gpuTexture);
	}
	ReleaseDesktopDuplicationFrame();
	return true;
}
void ReleaseDesktopDuplicationResources() 
{
	SafeRelease(pDesktopDuplication);
	SafeRelease(pDXGIOutput1);
	SafeRelease(pDXGIOutput);
	SafeRelease(pDXGIDevice);
	SafeRelease(pDeviceContext);
	SafeRelease(pDevice);
}