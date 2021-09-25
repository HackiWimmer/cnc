#ifndef CNC_USB_PORT_SCANNER
#define CNC_USB_PORT_SCANNER

#include <vector>
#include <wx/string.h>

class CncUsbPortScanner {
	
	public:
		static int scan();
		static int isComPortAvailable(int port);
};

class CncAvailableArduionPorts {
	
	public:
	
		struct Port {
			wxString desc = "";
			wxString name = "";
			int      num  = -1;
		};
		
		typedef std::vector<Port> PortList;
		
		static PortList evaluate(PortList&);
};

#endif