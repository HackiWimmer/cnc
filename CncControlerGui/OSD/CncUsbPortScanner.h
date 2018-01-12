#ifndef CNC_USB_PORT_SCANNER
#define CNC_USB_PORT_SCANNER

class CncUsbPortScanner {
	
	public:
		static int scan();
		static int isComPortAvailable(int port);
};

#endif