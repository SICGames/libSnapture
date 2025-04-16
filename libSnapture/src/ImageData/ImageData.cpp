#include "../../Inc/ImageData/ImageData.h"
#include "../../Inc/Helpers/ErrorHandler.h"
/// <summary>
/// Creates ImageData structure from HBITMAP
/// </summary>
/// <param name="bitmap"></param>
/// <returns></returns>
void CreateImageDataFromHBITMAP(_Out_ LPImageData imagedata, const HBITMAP bitmap, const double DPI = 300) 
{
	if (bitmap == NULL) {
		throw new std::exception("Bitmap is null");
	}
	if (imagedata == nullptr) {
		imagedata = reinterpret_cast<ImageData*>(malloc(sizeof(ImageData)));
	}

	if (imagedata != nullptr) 
	{
		const int pixelsPerMeter = static_cast<int>(DPI * 39.3701);
		BITMAP bmp;
		GetObject(bitmap, sizeof(BITMAP), &bmp);

		const int width = bmp.bmWidth;
		const int height = bmp.bmHeight;
		const int channels = bmp.bmBitsPixel / 8;
		const int BitsPerPixel = bmp.bmBitsPixel;

		const int _stride = width * channels;
		int _padding = 0;
		if (BitsPerPixel == 24) 
		{
			_padding = (_stride % 4 == 0) ? 0 : (4 - (_stride % 4));
		}

		const int stride = _stride + _padding; //((((width * BitsPerPixel) + 31) & ~31) >> 3) * channels;
		const int rowsize = ceil(stride / channels);
		const int size = stride * height;

		imagedata->Width = width;
		imagedata->Height = height;
		imagedata->Channels = channels;
		imagedata->BitsPerPixel = BitsPerPixel;
		imagedata->RowSize = rowsize;
		imagedata->Stride = stride;

		BITMAPINFO bmpInfo = { 0 };
		bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
		bmpInfo.bmiHeader.biWidth = width;
		bmpInfo.bmiHeader.biHeight = -abs(height); //-- turn to negative number
		bmpInfo.bmiHeader.biBitCount = BitsPerPixel;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biXPelsPerMeter = pixelsPerMeter;
		bmpInfo.bmiHeader.biYPelsPerMeter = pixelsPerMeter;

		
		if (0 == GetDIBits(GetDC(0), bitmap, 0, 0, nullptr, &bmpInfo, DIB_RGB_COLORS)) {
			throw new std::runtime_error("Unable to obtain bitmap size from screen shot.");
		}

		imagedata->Size = size;
		imagedata->DPI = DPI;

		//Size = bmpInfo.bmiHeader.biSizeImage;
		imagedata->Data = (unsigned char*)malloc(size);

		bmpInfo.bmiHeader.biCompression = BI_RGB;
		if (0 == GetDIBits(GetDC(0), bitmap, 0, -height, imagedata->Data, &bmpInfo, DIB_RGB_COLORS))
		{
			throw new std::runtime_error("Unable to obtain bitmap pixels from screen shot.");
		}
	}
}



/// <summary>
/// Creates Blank Image Data
/// </summary>
/// <param name="data"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="channels"></param>
void CreateImageData(_Out_ LPImageData data, const int width, const int height, const int channels, const void* pixels, const double DPI = 300)
{
	if (data == nullptr)
		data = reinterpret_cast<ImageData*>(malloc(sizeof(ImageData)));

	if (data != nullptr) {
		data->Width = width;
		data->Height = height;
		data->Channels = channels;
		data->DPI = DPI;
		int bitcount = data->Channels * 8;
		int padding = 0;
		int stride = width * channels;

		if (bitcount == 24) {
			padding = (stride % 4 == 0) ? 0 : (4 - (stride % 4));
		}

		data->Stride = stride + padding;		// ((((data->Width * bitcount) + 31) & ~31) >> 3) * data->Channels;
		data->BitsPerPixel = bitcount;
		data->RowSize = ceil(data->Stride / data->Channels);
		data->Size = data->Stride * data->Height;
		if (pixels != nullptr)
		{
			data->Data = (unsigned char*)pixels;
			//data->Data = (unsigned char*)malloc(data->Size);
			//memcpy_s(data->Data, data->Size, pixels, data->Size);
			return;
		}

		data->Data = (unsigned char*)malloc(data->Size);
		if (data->Size > 0)
		{
			try {
				//auto _sz = data->Width * data->Height * data->Channels; //  this is image size.
				for (auto index = 0; index < data->Size; index += data->Channels)
				{
					if (data->Data) {
						for (auto c = 0; c < data->Channels; c++)
						{
							try
							{
								if (c == 3)
								{
									data->Data[index + c] = 255; //-- alpha channel
									continue;
								}
								data->Data[index + c] = 0; //-- C6386 - possible buffer overflow.
							}
							catch (std::exception ex) 
							{
								OutputDebugString(L"CreateImageDataException -> Creating Pixel data\n");
							}
						}
					}
				}
			}
			catch (std::exception ex) {
				throw new std::exception(ex.what());
			}
		}
	}
}

/// <summary>
/// Saves Image Data To File
/// </summary>
/// <param name="data"></param>
/// <param name="filename"></param>
/// <returns></returns>
bool SaveImageDataToFile(LPImageData data, const WCHAR* filename)
{
	bool flipVertically = true;
	
	GDIHelper* gdihelper = new GDIHelper();
	gdihelper->GDIPlus_Startup();
	
	std::wstring filenameStr(filename);
	auto ext = filenameStr.substr(filenameStr.length() - 3);
	HRESULT hr = S_OK;
	CLSID clsid;

	if (ext.compare(L"bmp") == 0)
	{
		hr = CLSIDFromString(L"{557cf400-1a04-11d3-9a73-0000f81ef32e}", &clsid);
	}
	else if (ext.compare(L"jpg") == 0)
	{
		hr = CLSIDFromString(L"{557cf401-1a04-11d3-9a73-0000f81ef32e}", &clsid);
	}
	else if (ext.compare(L"gif") == 0)
	{
		hr = CLSIDFromString(L"{557cf402-1a04-11d3-9a73-0000f81ef32e}", &clsid);
	}
	else if (ext.compare(L"tif") == 0)
	{
		hr = CLSIDFromString(L"{557cf405-1a04-11d3-9a73-0000f81ef32e}", &clsid);
	}
	else if (ext.compare(L"png") == 0)
	{
		hr = CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid);
	}

	const int pixelsPerMeter = static_cast<int>(data->DPI * 39.3701);
	//-- Recreate bitmap from scratch
	BITMAPINFOHEADER bitmapinfoheader;
	ZeroMemory(&bitmapinfoheader, sizeof(BITMAPINFOHEADER));
	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfoheader.biWidth = data->Width;
	bitmapinfoheader.biHeight = flipVertically == true ? -data->Height : -data->Height;
	bitmapinfoheader.biBitCount = data->BitsPerPixel;
	bitmapinfoheader.biCompression = BI_RGB;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biClrImportant = 0;
	bitmapinfoheader.biClrUsed = 0;
	bitmapinfoheader.biSizeImage = data->Stride * data->Height;
	bitmapinfoheader.biXPelsPerMeter = pixelsPerMeter;
	bitmapinfoheader.biYPelsPerMeter = pixelsPerMeter;

	BITMAPINFO bitmapinfo;
	ZeroMemory(&bitmapinfo, sizeof(BITMAPINFO));
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfo.bmiHeader.biWidth = data->Width;
	bitmapinfo.bmiHeader.biHeight = flipVertically == true ? -data->Height : -data->Height;
	bitmapinfo.bmiHeader.biBitCount = data->BitsPerPixel;
	bitmapinfo.bmiHeader.biCompression = BI_RGB;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biSizeImage = data->Stride * data->Height;
	bitmapinfo.bmiHeader.biXPelsPerMeter = pixelsPerMeter;
	bitmapinfo.bmiHeader.biYPelsPerMeter = pixelsPerMeter;

    HBITMAP hBitmap = CreateDIBitmap(GetDC(NULL), &bitmapinfoheader, CBM_INIT, (const void*)data->Data, &bitmapinfo, DIB_RGB_COLORS);

	if (hBitmap == nullptr) {
		DWORD errorCode = GetLastError();
		std::wstring errorMessage = GetErrorMessage(errorCode);
		OutputDebugStringW(errorMessage.c_str());
		throw new std::exception("Unable to create bitmap from memory.");
	}

	/*
	const HBITMAP memBitmap = CreateBitmap(data->Width, data->Height, 1, bitCount, (const void*)data->Data);
	if (memBitmap == nullptr) {
		
	}
	*/

	Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
	auto stat = pBitmap->GetLastStatus();
	if (stat != Gdiplus::Status::Ok) 
	{
		//-- happens only when depth is 24 and less. 
		//-- 32 bit depth works good.
		OutputDebugString(L"Couldn't create Gdi Bitmap from HBITMAP.\n");
	}
	else {
		if (pBitmap != nullptr) {
			Gdiplus::Status status = pBitmap->Save(filename, &clsid, NULL);
			if (status != Gdiplus::Status::Ok)
			{
				auto reason = pBitmap->GetLastStatus();
				throw new std::exception("Issue saving screen shot.");
			}
		}
	}
	pBitmap = nullptr;
	DeleteObject(hBitmap);
	gdihelper->GDIPlus_Shutdown();
	delete gdihelper;
	return true;
}

/// <summary>
/// Releases all Image Data.
/// </summary>
/// <param name="data"></param>
void ReleaseImageData(const LPImageData data)
{
	if (data->Data != nullptr) {
		free(data->Data);
		data->Data = nullptr;
	}
	data->Width = data->Height = data->Channels = data->Stride = data->BitsPerPixel = data->RowSize = data->Size = 0;

}

void CreateWindowsLogo(const LPImageData data) {
	auto sz = data->Size;
	auto halfHeight = data->Height / 2;
	auto halfWidth = data->Width / 2;
	auto stride = data->Stride;
	int BitsPerPixel = data->Channels * 8;
	// int RowSize = ((BitsPerPixel * data->Width + 31) / 32) * data->Channels;
	// int rowSize = data->RowSize; // (data->Width * (BitsPerPixel / 8) + 3) & ~3;
	for (auto y = 0; y < data->Height; y++)
	{
		for (auto x = 0; x < data->Width; x++)
		{
			unsigned char* pixel = &data->Data[y * data->RowSize + x * data->Channels]; // BGR 

			if (x <= halfWidth && y <= halfHeight) //-- top left
			{
				pixel[0] = 0;
				pixel[1] = 0;
				pixel[2] = 255;
			}
			if (x >= halfWidth && y <= halfHeight)  //-- top right
			{
				pixel[0] = 0;
				pixel[1] = 255;
				pixel[2] = 0;
			}
			if (x <= halfWidth && y >= halfHeight) //-- bottom left
			{
				pixel[0] = 255;
				pixel[1] = 128;
				pixel[2] = 64;
			}
			if (x >= halfWidth && y >= halfHeight) //-- Bottom Right
			{
				pixel[0] = 0;
				pixel[1] = 225;
				pixel[2] = 255;
			}

		}
	}
}
