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
			const unsigned char* b = (const unsigned char*) buffer;
			for ( unsigned int i=0; i<nbByte; i++ ) {
				
				lastFetchResult.index++;
				cnc::spy << wxString::Format(" Serial::<< 0x{%02X %02X %02X %04X} 0x[ %02X ]\n", 
												lastFetchResult.cmd, 
												lastFetchResult.ret, 
												lastFetchResult.pid, 
												lastFetchResult.index, 
												b[i]
											);
			}
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
	
	// Artificially waste time - on demand
	const int stepDelay = cncControl->getStepDelay();
	if ( stepDelay > 0 ) {
		
		typedef IndividualCommandEvent::EvtMainFrame ID;
		typedef IndividualCommandEvent::ValueName VN;
		
		IndividualCommandEvent evt(ID::WaitActive);
		evt.setValue(VN::VAL1, cncControl->getStepDelay());
		evt.setValue(VN::VAL2, false);
		wxPostEvent(THE_FRAME, evt);
	}
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
