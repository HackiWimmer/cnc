#include <iostream>
#include <wx/string.h>
#include "CncUsbPortScanner.h"

#ifdef __WXMSW__
	#include <windows.h>
#endif

// Namespace
using namespace std;

///////////////////////////////////////////////////////////////////
int CncUsbPortScanner::isComPortAvailable(int port) {
///////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	wxString strPort("");
	HANDLE  hPort = NULL;
	DWORD dwErr(0);

	// test the port for availability
	strPort = wxString::Format("\\\\.\\COM%d", port);
	hPort = CreateFile (strPort,        // Pointer to the name of the port
					  GENERIC_READ | GENERIC_WRITE,
									  // Access (read-write) mode
					  0,              // Share mode
					  NULL,           // Pointer to the security attribute
					  OPEN_EXISTING,  // How to open the serial port
					  0,              // Port attributes
					  NULL);          // Handle to port with attribute to copy

	if ( hPort == INVALID_HANDLE_VALUE ) {
		// Invalid Porthandle, port NOT available
		dwErr = GetLastError();
		if (dwErr == ERROR_ACCESS_DENIED || dwErr == ERROR_GEN_FAILURE)
			return 0;   // Port exists but not available
		else
			return -1;  //Port not exists
	}
	else {
		// Valid PortHandle
		CloseHandle(hPort); // Port release
		return 1;  // Port exists
	}
#endif

#ifdef __WXGTK__
	#warning todo GTK implementation
	return 0;
#endif
}
///////////////////////////////////////////////////////////////////
int CncUsbPortScanner::scan() {
///////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
	cout << "USB Device Lister." << endl;
	
	// Get Number Of Devices
	UINT nDevices = 0;
	GetRawInputDeviceList( NULL, &nDevices, sizeof( RAWINPUTDEVICELIST ) );
	
	// Got Any?
	if( nDevices < 1 ) {
		// Exit
		cout << "ERR: 0 Devices?";
		cin.get();
		return 0;
	}
	 
	// Allocate Memory For Device List
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	pRawInputDeviceList = new RAWINPUTDEVICELIST[ sizeof( RAWINPUTDEVICELIST ) * nDevices ];
	
	// Got Memory?
	if( pRawInputDeviceList == NULL ) {
		// Error
		cout << "ERR: Could not allocate memory for Device List.";
		cin.get();
		return 0;
	}
	 
	// Fill Device List Buffer
	int nResult;
	nResult = GetRawInputDeviceList( pRawInputDeviceList, &nDevices, sizeof( RAWINPUTDEVICELIST ) );
	
	// Got Device List?
	if( nResult < 0 ) {
		// Clean Up
		delete [] pRawInputDeviceList;
		
		// Error
		cout << "ERR: Could not get device list.";
		cin.get();
		return 0;
	}
	
	// Loop Through Device List
	for( UINT i = 0; i < nDevices; i++ ) {
		// Get Character Count For Device Name
		UINT nBufferSize = 0;
		nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice, // Device
										 RIDI_DEVICENAME,                // Get Device Name
										 NULL,                           // NO Buff, Want Count!
										 &nBufferSize );                 // Char Count Here!
		
		// Got Device Name?
		if( nResult < 0 ) {
			// Error
			cout << "ERR: Unable to get Device Name character count.. Moving to next device." << endl << endl;
			
			// Next
			continue;
		}
		
		// Allocate Memory For Device Name
		WCHAR* wcDeviceName = new WCHAR[ nBufferSize + 1 ];
		 
		// Got Memory
		if( wcDeviceName == NULL ) {
			// Error
			cout << "ERR: Unable to allocate memory for Device Name.. Moving to next device." << endl << endl;
			
			// Next
			continue;
		}
		
		// Get Name
		nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice, // Device
										 RIDI_DEVICENAME,                // Get Device Name
										 wcDeviceName,                   // Get Name!
										 &nBufferSize );                 // Char Count
		
		// Got Device Name?
		if( nResult < 0 ) {
			// Error
			cout << "ERR: Unable to get Device Name.. Moving to next device." << endl << endl;
			
			// Clean Up
			delete [] wcDeviceName;
			
			// Next
			continue;
		}
		
		// Set Device Info & Buffer Size
		RID_DEVICE_INFO rdiDeviceInfo;
		rdiDeviceInfo.cbSize = sizeof( RID_DEVICE_INFO );
		nBufferSize = rdiDeviceInfo.cbSize;
		
		// Get Device Info
		nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice,
										 RIDI_DEVICEINFO,
										 &rdiDeviceInfo,
										 &nBufferSize );
		
		// Got All Buffer?
		if( nResult < 0 ) {
			// Error
			cout << "ERR: Unable to read Device Info.. Moving to next device." << endl << endl;
			
			// Next
			continue;
		}
		
		// Mouse
		if( rdiDeviceInfo.dwType == RIM_TYPEMOUSE ) {
			// Current Device
			cout << endl << "Displaying device " << i+1 << " information. (MOUSE)" << endl;
			wcout << L"Device Name: " << wcDeviceName << endl;
			cout << "Mouse ID: " << rdiDeviceInfo.mouse.dwId << endl;
			cout << "Mouse buttons: " << rdiDeviceInfo.mouse.dwNumberOfButtons << endl;
			cout << "Mouse sample rate (Data Points): " << rdiDeviceInfo.mouse.dwSampleRate << endl;
			if( rdiDeviceInfo.mouse.fHasHorizontalWheel ) {
				cout << "Mouse has horizontal wheel" << endl;
			} else {
				cout << "Mouse does not have horizontal wheel" << endl;
			}
		}
		
		// Keyboard
		else if( rdiDeviceInfo.dwType == RIM_TYPEKEYBOARD ) {
			// Current Device
			cout << endl << "Displaying device " << i+1 << " information. (KEYBOARD)" << endl;
			wcout << L"Device Name: " << wcDeviceName << endl;
			cout << "Keyboard mode: " << rdiDeviceInfo.keyboard.dwKeyboardMode << endl;
			cout << "Number of function keys: " << rdiDeviceInfo.keyboard.dwNumberOfFunctionKeys << endl;
			cout << "Number of indicators: " << rdiDeviceInfo.keyboard.dwNumberOfIndicators << endl;
			cout << "Number of keys total: " << rdiDeviceInfo.keyboard.dwNumberOfKeysTotal << endl;
			cout << "Type of the keyboard: " << rdiDeviceInfo.keyboard.dwType << endl;
			cout << "Subtype of the keyboard: " << rdiDeviceInfo.keyboard.dwSubType << endl;
		}
		
		// Some HID
		else {
			// (rdi.dwType == RIM_TYPEHID)
			// Current Device
			cout << endl << "Displaying device " << i+1 << " information. (HID)" << endl;
			cout << "Device Name: " << wcDeviceName << endl;
			cout << "Vendor Id:" << rdiDeviceInfo.hid.dwVendorId << endl;
			cout << "Product Id:" << rdiDeviceInfo.hid.dwProductId << endl;
			cout << "Version No:" << rdiDeviceInfo.hid.dwVersionNumber << endl;
			cout << "Usage for the device: " << rdiDeviceInfo.hid.usUsage << endl;
			cout << "Usage Page for the device: " << rdiDeviceInfo.hid.usUsagePage << endl;
		}
		
		// Delete Name Memory!
		delete [] wcDeviceName;
	}
	
	// Clean Up - Free Memory
	delete [] pRawInputDeviceList;
	
	// Exit
	cout << endl << "Finnished.";
	cin.get();
	
	return 0;
#endif

#ifdef __WXGTK__
	#warning todo GTK implementation
	return 0;
#endif
}
