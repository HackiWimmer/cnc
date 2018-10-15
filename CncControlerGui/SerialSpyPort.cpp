#include "CncControl.h"
#include "MainFrame.h"
#include "SerialSpyPort.h"

///////////////////////////////////////////////////////////////////
SerialSpyPort::SerialSpyPort(CncControl* cnc)
///////////////////////////////////////////////////////////////////
: Serial(cnc)
{
}
///////////////////////////////////////////////////////////////////
SerialSpyPort::SerialSpyPort(const char *portName)
///////////////////////////////////////////////////////////////////
: Serial(portName)
{
}
///////////////////////////////////////////////////////////////////
SerialSpyPort::~SerialSpyPort() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void SerialSpyPort::spyReadData(int prevRet, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( traceSpyInfo == true ) {
		if ( prevRet <= 0 )
			return;

		if ( spyRead == true ) {
			cnc::spy << wxString::Format("Serial::<< {0x%02X} 0x[ ", getLastFetchResult().ret);
			const unsigned char* b = (const unsigned char*) buffer;
			for ( int i=0; i<prevRet; i++ ) {
				cnc::spy << wxString::Format("%02X ", b[i]);
			}
			cnc::spy << ']' << std::endl;
		}
	}
}
///////////////////////////////////////////////////////////////////
void SerialSpyPort::spyWriteData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( traceSpyInfo == true ) {
		if ( nbByte == 0 )
			return;
			
		if ( spyWrite == true ) {
			cnc::spy << "Serial::>> {0xFF} 0x[ ";
			const unsigned char* b = (const unsigned char*) buffer;
			for ( unsigned int i=0; i<nbByte; i++ ) {
				cnc::spy << wxString::Format("%02X ", b[i]);
			}
			cnc::spy << ']' << std::endl;
		}
	}
	
	// Artificially waste time
	THE_APP->waitActive(cncControl->getStepDelay(), false);
}
///////////////////////////////////////////////////////////////////
int SerialSpyPort::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	int ret = Serial::readData(buffer, nbByte);
	spyReadData(ret, buffer, nbByte);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialSpyPort::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	spyWriteData(buffer, nbByte);
	bool ret = Serial::writeData(buffer, nbByte);
	
	return ret;
}