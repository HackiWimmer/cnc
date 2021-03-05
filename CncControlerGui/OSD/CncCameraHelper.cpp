#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/rawbmp.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "CncCameraHelper.h"

#ifdef __WXMSW__
	#include <windows.h>
	#include <dshow.h>

	//#pragma comment(lib, "strmiids")
#endif

///////////////////////////////////////////////////////////////////////
#ifdef __WXMSW__

	namespace OSD
	{
		class DeviceEnumerator {

			public:
				DeviceEnumerator() = default;
				std::map<int, Device> getDevicesMap(const GUID deviceClass);
				std::map<int, Device> getVideoDevicesMap();
				std::map<int, Device> getAudioDevicesMap();

			private:
				std::string ConvertBSTRToMBS(BSTR bstr);
				std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);

		};

		std::map<int, Device> DeviceEnumerator::getVideoDevicesMap() {
			return getDevicesMap(CLSID_VideoInputDeviceCategory);
		}

		std::map<int, Device> DeviceEnumerator::getAudioDevicesMap() {
			return getDevicesMap(CLSID_AudioInputDeviceCategory);
		}

		// Returns a map of id and devices that can be used
		std::map<int, Device> DeviceEnumerator::getDevicesMap(const GUID deviceClass) {
			std::map<int, Device> deviceMap;

			HRESULT hr = CoInitialize(nullptr);
			if (FAILED(hr)) {
				return deviceMap; // Empty deviceMap as an error
			}

			// Create the System Device Enumerator
			ICreateDevEnum *pDevEnum;
			hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

			// If succeeded, create an enumerator for the category
			IEnumMoniker *pEnum = NULL;
			if (SUCCEEDED(hr)) {
				hr = pDevEnum->CreateClassEnumerator(deviceClass, &pEnum, 0);
				if (hr == S_FALSE) {
					hr = VFW_E_NOT_FOUND;
				}
				pDevEnum->Release();
			}

			// Now we check if the enumerator creation succeeded
			int deviceId = -1;
			if (SUCCEEDED(hr)) {
				// Fill the map with id and friendly device name
				IMoniker *pMoniker = NULL;
				while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {

					IPropertyBag *pPropBag;
					HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
					if (FAILED(hr)) {
						pMoniker->Release();
						continue;
					}

					// Create variant to hold data
					VARIANT var;
					VariantInit(&var);

					std::string deviceName;
					std::string devicePath;

					// Read FriendlyName or Description
					hr = pPropBag->Read(L"Description", &var, 0); // Read description
					if (FAILED(hr)) {
						// If description fails, try with the friendly name
						hr = pPropBag->Read(L"FriendlyName", &var, 0);
					}
					// If still fails, continue with next device
					if (FAILED(hr)) {
						VariantClear(&var);
						continue;
					}
					// Convert to string
					else {
						deviceName = ConvertBSTRToMBS(var.bstrVal);
					}

					VariantClear(&var); // We clean the variable in order to read the next value

										// We try to read the DevicePath
					hr = pPropBag->Read(L"DevicePath", &var, 0);
					if (FAILED(hr)) {
						VariantClear(&var);
						continue; // If it fails we continue with next device
					}
					else {
						devicePath = ConvertBSTRToMBS(var.bstrVal);
					}

					// We populate the map
					deviceId++;
					Device currentDevice;
					currentDevice.id = deviceId;
					currentDevice.deviceName = deviceName;
					currentDevice.devicePath = devicePath;
					deviceMap[deviceId] = currentDevice;

				}
				pEnum->Release();
			}
			
			CoUninitialize();
			return deviceMap;
		}

		/*
		This two methods were taken from
		https://stackoverflow.com/questions/6284524/bstr-to-stdstring-stdwstring-and-vice-versa
		*/

		std::string DeviceEnumerator::ConvertBSTRToMBS(BSTR bstr)
		{
			int wslen = ::SysStringLen(bstr);
			return ConvertWCSToMBS((wchar_t*)bstr, wslen);
		}

		std::string DeviceEnumerator::ConvertWCSToMBS(const wchar_t* pstr, long wslen)
		{
			int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

			std::string dblstr(len, '\0');
			len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
				pstr, wslen /* not necessary NULL-terminated */,
				&dblstr[0], len,
				NULL, NULL /* no default char */);

			return dblstr;
		}
		
		///////////////////////////////////////////////////////////////////
		// Version optimized for Microsoft Windows.
		// matBitmap must be continous and matBitmap.cols % 4 must equal 0
		// as SetDIBits() requires the DIB rows to be DWORD-aligned.
		// Should not be called directly but only from ConvertMatBitmapTowxBitmap()
		// which does all the necessary debug checks.
		bool convertMatBitmapTowxBitmapMSW(const cv::Mat& matBitmap, wxBitmap& bitmap) {
		///////////////////////////////////////////////////////////////////
			const HDC  hScreenDC = ::GetDC(nullptr);
			BITMAPINFO bitmapInfo{0};
			bool       success;

			bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
			bitmapInfo.bmiHeader.biWidth       = bitmap.GetWidth();
			bitmapInfo.bmiHeader.biHeight      = 0 - bitmap.GetHeight();
			bitmapInfo.bmiHeader.biPlanes      = 1;
			bitmapInfo.bmiHeader.biBitCount    = 24;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;

			success = ::SetDIBits(hScreenDC, bitmap.GetHBITMAP(), 0, bitmap.GetHeight(),
								  matBitmap.data, &bitmapInfo, DIB_RGB_COLORS) != 0;
			::ReleaseDC(nullptr, hScreenDC);

			return success;
		}

	} // namespace OSD

#endif // #ifndef __WXMSW__

namespace OSD
{
	///////////////////////////////////////////////////////////////////
	/*
		@param matBitmap
			Its data must be encoded as BGR CV_8UC3, which is
			probably the most common format for OpenCV images.
		@param bitmap
			It must be initialized to the same width and height as matBitmap
			and its depth must be 24.
			
		@return @true if the conversion succeeded, @false otherwise.
		On MS Windows, a MSW-optimized version is used if possible,
		the portable one otherwise. In my testing on MSW with
		3840x2160 image in the Release build, the optimized version
		was about 25% faster then the portable one. MSW-optimized version
		is used when bitmap is a DIB and its width modulo 4 is 0.
		In my testing on MSW with MSVS using 3840x2160 image, the portable
		version of conversion function in the Debug build was more then
		60 times slower than in the Release build.
		At least on MSW, initializing a wxBitmap takes quite some
		time. If you are processing images of the same size in a loop
		(e.g., frames of a video file), I recommend initializing the
		wxBitmap outside the loop and reusing it in the loop instead
		of creating it every time inside the loop.
	*/
	bool convertMatBitmapTowxBitmap(const cv::Mat& matBitmap, wxBitmap& bitmap) {
	///////////////////////////////////////////////////////////////////
		wxCHECK(!matBitmap.empty(), false);
		wxCHECK(matBitmap.type() == CV_8UC3, false);
		wxCHECK(matBitmap.dims == 2, false);
		wxCHECK(bitmap.IsOk(), false);
		wxCHECK(bitmap.GetWidth() == matBitmap.cols && bitmap.GetHeight() == matBitmap.rows, false);
		wxCHECK(bitmap.GetDepth() == 24, false);

	#ifdef __WXMSW__
		if (  bitmap.IsDIB()
			  && matBitmap.isContinuous()
			  && matBitmap.cols % 4 == 0 )
		{
			return OSD::convertMatBitmapTowxBitmapMSW(matBitmap, bitmap);
		}
	#endif

		wxNativePixelData           pixelData(bitmap);
		wxNativePixelData::Iterator pixelDataIt(pixelData);

		if ( matBitmap.isContinuous() )
		{
			const uchar* bgr = matBitmap.data;
			
			for ( int row = 0; row < pixelData.GetHeight(); ++row )
			{
				pixelDataIt.MoveTo(pixelData, 0, row);

				for ( int col = 0;
					  col < pixelData.GetWidth();
					  ++col, ++pixelDataIt )
				{
					pixelDataIt.Blue()  = *bgr++;
					pixelDataIt.Green() = *bgr++;
					pixelDataIt.Red()   = *bgr++;
				}
			}
		}
		else // Is it even possible for Mat with image to be not continous?
		{
			auto matBitmapIt = matBitmap.begin<cv::Vec3b>();

			for ( int row = 0; row < pixelData.GetHeight(); ++row )
			{
				pixelDataIt.MoveTo(pixelData, 0, row);

				for ( int col = 0;
					  col < pixelData.GetWidth();
					  ++col, ++pixelDataIt, ++matBitmapIt )
				{
					pixelDataIt.Blue()  = (*matBitmapIt)[0];
					pixelDataIt.Green() = (*matBitmapIt)[1];
					pixelDataIt.Red()   = (*matBitmapIt)[2];
				}
			}
		}

		return bitmap.IsOk();
	}
	
	///////////////////////////////////////////////////////////////////
	bool evaluateAvailableCameraDevices(std::vector<OSD::Device>& ret) {
	///////////////////////////////////////////////////////////////////
		ret.clear();
		
		DeviceEnumerator de;
		
		// Video Devices
		std::map<int, Device> devices = de.getVideoDevicesMap();
		for (auto const &device : devices)
			ret.push_back(device.second);
	
		return ret.size() > 0;
	}

} // OSD