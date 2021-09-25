#include <iostream>
#include <wx/string.h>
#include "CncUsbPortScanner.h"

#ifdef __WXMSW__
	#include <windows.h>
#endif

#include <vector>
#include <wx/string.h>

#ifdef __WXMSW__
	class CncArduinoRegisteryScanner {
		
		const wxString DEF_REG_START_PATH = "SYSTEM\\CurrentControlSet\\Enum\\USB";
		const wxString DEF_TOKEN_NAME     = "CH340";
		const wxString DEF_COM_TOKEN      = "COM";
		const int      MAX_KEY_LENGTH     =  255;
		
		public:
		
			struct RegEntry {
				HKEY			hKey;
				wxString		regPath			= "";
				wxString		friendlyName	= "";
				wxString		portName		= "";
				int				portNum			= -1;
				
				// -----------------------------------------------------------------
				friend std::ostream &operator<< (std::ostream &ostr, const RegEntry &a) {
					ostr 	<< a.hKey			<< ", " 
							<< a.regPath		<< ", "
							<< a.friendlyName	<< ", " 
							<< a.portName		<< ", "
							<< a.portNum
							;
							
					return ostr;
				}
			};
			
			typedef std::vector<RegEntry> RegResult;
		
			CncArduinoRegisteryScanner()  {}
			~CncArduinoRegisteryScanner() {}
			
			// -----------------------------------------------------------------
			int scan(RegResult& result)  {
				evaluateRegKey(DEF_REG_START_PATH, result);
				return result.size();
			}
			
		private:
			
			// -----------------------------------------------------------------
			bool evaluateRegKey(const wxString& regPath, RegResult& result) {
				
				// --------------------------------------------------------------
				auto query = [&](RegEntry& entry) {
					
					wchar_t buf[MAX_KEY_LENGTH];
					DWORD dwBufSize = sizeof(buf);
					
					// has property "FriendlyName"
					if ( RegQueryValueExW(entry.hKey, L"FriendlyName",0, 0, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS ) {
						
						// if contains the name token
						const wxString fn(wxString::Format("%ls", buf));
						if ( fn.Contains(DEF_TOKEN_NAME) ) {
							
							// searching for the com token
							const int pos =  fn.First(DEF_COM_TOKEN);
							if ( pos != wxNOT_FOUND ) {
								wxString comNum(fn.substr(fn.First(DEF_COM_TOKEN) + DEF_COM_TOKEN.Len()));
								
								// extract the digit part
								for ( size_t i=0; i<comNum.Len(); i++ ) {
									if ( isdigit(comNum[i]) == 0 ) {
										comNum = comNum.substr(0, i);
										break;
									}
								}
								
								long idx; 
								if ( comNum.ToLong(&idx) ) {
									entry.portNum = idx;
								
									// store
									entry.portName.assign(wxString::Format("%s%d", DEF_COM_TOKEN, entry.portNum));
									entry.friendlyName.assign(wxString::Format("%ls", buf));
									return true;
								}
							}
						}
					}
					
					return false;
				};
				
				HKEY     hKey;
				TCHAR    achKey[MAX_KEY_LENGTH];         // buffer for subkey name
				DWORD    cbName;                         // size of name string 
				TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
				DWORD    cchClassName = MAX_PATH;        // size of class string 
				DWORD    cSubKeys=0;                     // number of subkeys 
				DWORD    cbMaxSubKey;                    // longest subkey size 
				DWORD    cchMaxClass;                    // longest class string 
				DWORD    cValues;                        // number of values for key 
				DWORD    cchMaxValue;                    // longest value name 
				DWORD    cbMaxValueData;                 // longest value data 
				DWORD    cbSecurityDescriptor;           // size of security descriptor 
				FILETIME ftLastWriteTime;                // last write time 
		
			if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				
				RegQueryInfoKey(
					hKey,                    // key handle 
					achClass,                // buffer for class name 
					&cchClassName,           // size of class string 
					NULL,                    // reserved 
					&cSubKeys,               // number of subkeys 
					&cbMaxSubKey,            // longest subkey size 
					&cchMaxClass,            // longest class string 
					&cValues,                // number of values for this key 
					&cchMaxValue,            // longest value name 
					&cbMaxValueData,         // longest value data 
					&cbSecurityDescriptor,   // security descriptor 
					&ftLastWriteTime      // last write time 
				);
				
				if ( cSubKeys > 0 ) {
					for ( DWORD i=0; i<cSubKeys; i++ ) {
						
						cbName = MAX_KEY_LENGTH;
						if ( RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime)== ERROR_SUCCESS ) {
							
							const wxString subRegPath(wxString::Format("%s\\%ls", regPath, achKey));
							
							RegEntry entry;
							entry.hKey = hKey;
							entry.regPath.assign(subRegPath);
							 
							if ( query(entry) ) {
								
								result.push_back(entry);
								return true;
							}
							else {
								evaluateRegKey(subRegPath, result);
							}
						}
					}
				}
				else {
					RegEntry entry;
					entry.hKey = hKey;
					entry.regPath.assign(regPath);
					 
					if ( query(entry) ) 
						result.push_back(entry);
				}
			}
			
			return cSubKeys;
		}
	};
	
#endif

///////////////////////////////////////////////////////////////////
CncAvailableArduionPorts::PortList CncAvailableArduionPorts::evaluate(PortList& pl) {
///////////////////////////////////////////////////////////////////
#ifdef __WXMSW__

	CncArduinoRegisteryScanner ars;
	CncArduinoRegisteryScanner::RegResult result;
	ars.scan(result);
	
	for ( auto it = result.begin(); it != result.end(); ++it ) {
		
		CncAvailableArduionPorts::Port port;
		port.desc.assign(it->friendlyName);
		port.name.assign(it->portName);
		port.num = it->portNum;
		
		pl.push_back(port);
	}
	
	return pl;
	
#else

	for ( int i=0; i<32; i++ ) {
		CncAvailableArduionPorts::Port port;
		port.desc.assign("Unknown");
		port.name.assign(wxString::Format("COM%d", i));
		port.num = i;
		
		pl.push_back(port);
	}
	
	return pl;

#endif
}

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
	std::cout << "USB Device Lister." << std::endl;
	
	// Get Number Of Devices
	UINT nDevices = 0;
	GetRawInputDeviceList( NULL, &nDevices, sizeof( RAWINPUTDEVICELIST ) );
	
	// Got Any?
	if( nDevices < 1 ) {
		// Exit
		std::cout << "ERR: 0 Devices?";
		std::cin.get();
		return 0;
	}
	 
	// Allocate Memory For Device List
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	pRawInputDeviceList = new RAWINPUTDEVICELIST[ sizeof( RAWINPUTDEVICELIST ) * nDevices ];
	
	// Got Memory?
	if( pRawInputDeviceList == NULL ) {
		// Error
		std::cout << "ERR: Could not allocate memory for Device List.";
		std::cin.get();
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
		std::cout << "ERR: Could not get device list.";
		std::cin.get();
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
			std::cout << "ERR: Unable to get Device Name character count.. Moving to next device." << std::endl << std::endl;
			
			// Next
			continue;
		}
		
		// Allocate Memory For Device Name
		WCHAR* wcDeviceName = new WCHAR[ nBufferSize + 1 ];
		 
		// Got Memory
		if( wcDeviceName == NULL ) {
			// Error
			std::cout << "ERR: Unable to allocate memory for Device Name.. Moving to next device." << std::endl << std::endl;
			
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
			std::cout << "ERR: Unable to get Device Name.. Moving to next device." << std::endl << std::endl;
			
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
			std::cout << "ERR: Unable to read Device Info.. Moving to next device." << std::endl << std::endl;
			
			// Next
			continue;
		}
		
		// Mouse
		if( rdiDeviceInfo.dwType == RIM_TYPEMOUSE ) {
			// Current Device
			std::cout << std::endl << "Displaying device " << i+1 << " information. (MOUSE)" << std::endl;
			std::wcout << L"Device Name: " << wcDeviceName << std::endl;
			std::cout << "Mouse ID: " << rdiDeviceInfo.mouse.dwId << std::endl;
			std::cout << "Mouse buttons: " << rdiDeviceInfo.mouse.dwNumberOfButtons << std::endl;
			std::cout << "Mouse sample rate (Data Points): " << rdiDeviceInfo.mouse.dwSampleRate << std::endl;
			if( rdiDeviceInfo.mouse.fHasHorizontalWheel ) {
				std::cout << "Mouse has horizontal wheel" << std::endl;
			} else {
				std::cout << "Mouse does not have horizontal wheel" << std::endl;
			}
		}
		
		// Keyboard
		else if( rdiDeviceInfo.dwType == RIM_TYPEKEYBOARD ) {
			// Current Device
			std::cout << std::endl << "Displaying device " << i+1 << " information. (KEYBOARD)" << std::endl;
			std::wcout << L"Device Name: " << wcDeviceName << std::endl;
			std::cout << "Keyboard mode: " << rdiDeviceInfo.keyboard.dwKeyboardMode << std::endl;
			std::cout << "Number of function keys: " << rdiDeviceInfo.keyboard.dwNumberOfFunctionKeys << std::endl;
			std::cout << "Number of indicators: " << rdiDeviceInfo.keyboard.dwNumberOfIndicators << std::endl;
			std::cout << "Number of keys total: " << rdiDeviceInfo.keyboard.dwNumberOfKeysTotal << std::endl;
			std::cout << "Type of the keyboard: " << rdiDeviceInfo.keyboard.dwType << std::endl;
			std::cout << "Subtype of the keyboard: " << rdiDeviceInfo.keyboard.dwSubType << std::endl;
		}
		
		// Some HID
		else {
			// (rdi.dwType == RIM_TYPEHID)
			// Current Device
			std::cout << std::endl << "Displaying device " << i+1 << " information. (HID)" << std::endl;
			std::cout << "Device Name: " << wcDeviceName << std::endl;
			std::cout << "Vendor Id:" << rdiDeviceInfo.hid.dwVendorId << std::endl;
			std::cout << "Product Id:" << rdiDeviceInfo.hid.dwProductId << std::endl;
			std::cout << "Version No:" << rdiDeviceInfo.hid.dwVersionNumber << std::endl;
			std::cout << "Usage for the device: " << rdiDeviceInfo.hid.usUsage << std::endl;
			std::cout << "Usage Page for the device: " << rdiDeviceInfo.hid.usUsagePage << std::endl;
		}
		
		// Delete Name Memory!
		delete [] wcDeviceName;
	}
	
	// Clean Up - Free Memory
	delete [] pRawInputDeviceList;
	
	// Exit
	std::cout << std::endl << "Finnished.";
	std::cin.get();
	
	return 0;
#endif

#ifdef __WXGTK__
	#warning todo GTK implementation
	return 0;
#endif
}
