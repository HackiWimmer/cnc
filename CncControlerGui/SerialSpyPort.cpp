#include "CncControl.h"
#include "CncUserEvents.h"
#include "SerialSpyPort.h"

#include <wx/frame.h>
extern wxFrame* THE_FRAME;

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
void SerialSpyPort::spyReadData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( traceSpyInfo == true ) {
		if ( nbByte <= 0 )
			return;

		if ( spyRead == true ) {
			lastFetchResult.index += nbByte;
			cnc::spy << wxString::Format(" Serial::<< 0x{%02X %02X %02X %04X} 0x[ ", lastFetchResult.cmd, lastFetchResult.ret, lastFetchResult.pid, lastFetchResult.index);
			
			const unsigned char* b = (const unsigned char*) buffer;
			for ( unsigned int i=0; i<nbByte; i++ ) {
				cnc::spy << wxString::Format("%02X ", b[i]);
			}
			cnc::spy << ']';
			
			// special handling to show text messages
			if ( nbByte == 1 && ( lastFetchResult.pid == PID_TEXT || lastFetchResult.pid == PID_MSG ) ) {
				if ( b[0] >= 32 && b[0] <= 127 ) 
					cnc::spy << wxString::Format(" '%c'", b[0]);
			}
			
			cnc::spy << std::endl;
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
			cnc::spy << wxString::Format(" Serial::>> 0x{%02X FF 01 %04X} 0x[ ",  lastFetchResult.cmd, lastFetchResult.portion + 1);
			
			const unsigned char* b = (const unsigned char*) buffer;
			for ( unsigned int i=0; i<nbByte; i++ ) {
				cnc::spy << wxString::Format("%02X ", b[i]);
			}
			cnc::spy << ']' << std::endl;
		}
	}
	
	// Artificially waste time
	typedef IndividualCommandEvent::EvtMainFrame ID;
	typedef IndividualCommandEvent::ValueName VN;

	IndividualCommandEvent evt(ID::WaitActive);
	evt.setValue(VN::VAL1, cncControl->getStepDelay());
	evt.setValue(VN::VAL2, false);
	wxPostEvent(THE_FRAME, evt);
}
///////////////////////////////////////////////////////////////////
int SerialSpyPort::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	int ret = Serial::readData(buffer, nbByte);
	spyReadData(buffer, ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialSpyPort::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	spyWriteData(buffer, nbByte);
	bool ret = Serial::writeData(buffer, nbByte);
	
	return ret;
}
