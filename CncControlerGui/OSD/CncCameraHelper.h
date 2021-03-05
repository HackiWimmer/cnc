#ifndef CNC_CAMERA_HELPER_H
#define CNC_CAMERA_HELPER_H

#include <vector>
#include <map>
#include <string>
#include <wx/bitmap.h>

namespace cv { 
	class Mat; 
}

#ifdef __WXMSW__
	#include <windows.h>
#endif


namespace OSD {
	
	struct Device {
		int id; 					// This can be used to open the device in OpenCV
		std::string devicePath;
		std::string deviceName;		// This can be used to show the devices to the user
	};
	
	bool evaluateAvailableCameraDevices(std::vector<Device>& ret);
	bool convertMatBitmapTowxBitmap(const cv::Mat& matBitmap, wxBitmap& bitmap);
};

#endif