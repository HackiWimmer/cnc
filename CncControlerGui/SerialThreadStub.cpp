#include "MainFrameProxy.h"
#include "CncConfig.h"
#include "CncUserEvents.h"
#include "SerialCircularBuffer.h"
#include "SerialThread.h"
#include "SerialEndPoint.h"
#include "SerialThreadStub.h"
#include "CncUserEvents.h"
#include <wx/frame.h>
extern wxFrame* THE_FRAME;

///////////////////////////////////////////////////////////////////
SerialThreadStub::SerialThreadStub(CncControl* cnc)
: SerialSpyPort(cnc)
///////////////////////////////////////////////////////////////////
{
	wxASSERT( isSerialThreadInstalled() == true );
}
///////////////////////////////////////////////////////////////////
SerialThreadStub::~SerialThreadStub() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialThreadStub::connect(const char* portName) { 
///////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;
	const bool ok = isSerialThreadInstalled();
	wxASSERT( ok == true );
	
	if ( ok == true ) {
		wakeUpOnDemand();
		
		// first set connected to support clearRemainingBytes() below
		setConnected(isSerialThreadRunning());
		
		// set connected if clearing remaining bytes was successfully
		setConnected(clearRemainingBytes(true));
		
		if ( isConnected() ) {
			IndividualCommandEvent evt(ID::NotifyConneting);
			wxPostEvent(THE_FRAME, evt);
		}
	}
	
	return ok; 
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::notifySetupSuccesfullyFinsihed() {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false )
		return;
 
	typedef IndividualCommandEvent::EvtSerialStub ID;
	const bool ok = isSerialThreadInstalled();
	wxASSERT( ok == true );
	
	if ( ok == true ) {
		wakeUpOnDemand();
		
		IndividualCommandEvent evt(ID::NotifyConneted);
		wxPostEvent(THE_FRAME, evt);
	}
	else {
		IndividualCommandEvent evt(ID::NotifyFatalError);
		evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
		wxPostEvent(THE_FRAME, evt);
	}
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::disconnect(void) {
///////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;
	
	if ( isConnected() == false ) 
		return;
	
	const bool ok = isSerialThreadInstalled();
	wxASSERT( ok == true );
	
	if ( ok == true ) {
		wakeUpOnDemand();
		
		clearRemainingBytes(true);
		
		setConnected(false);
		
		if ( isSerialThreadRunning() == false )
			pauseSerialThread();
		
		IndividualCommandEvent evt(ID::NotifyDisconnected);
		wxPostEvent(THE_FRAME, evt);
	}
	else {
		IndividualCommandEvent evt(ID::NotifyFatalError);
		evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
		wxPostEvent(THE_FRAME, evt);
	}
}
/////////////////////////////////////////////////////////////////////
bool SerialThreadStub::isSerialThreadInstalled() {
/////////////////////////////////////////////////////////////////////
	return APP_PROXY::getSerialThread(this) != NULL;
}
/////////////////////////////////////////////////////////////////////
bool SerialThreadStub::isSerialThreadRunning() {
/////////////////////////////////////////////////////////////////////
	return ( isSerialThreadInstalled() == true ? APP_PROXY::getSerialThread(this)->IsRunning() : false );
}
/////////////////////////////////////////////////////////////////////
void SerialThreadStub::resumeSerialThread() {
/////////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;

	if ( isSerialThreadInstalled() == true ) {
		IndividualCommandEvent evtB(ID::NotifyResumeBefore);
		wxPostEvent(THE_FRAME, evtB);
		
		APP_PROXY::getSerialThread(this)->Resume();
		
		IndividualCommandEvent evtA(ID::NotifyResumeAfter);
		wxPostEvent(THE_FRAME, evtA);
	}
}
/////////////////////////////////////////////////////////////////////
void SerialThreadStub::pauseSerialThread(){
/////////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;
	
	if ( isSerialThreadInstalled() == true ) {
		
		IndividualCommandEvent evtB(ID::NotifyPauseBefore);
		wxPostEvent(THE_FRAME, evtB);
		
		APP_PROXY::getSerialThread(this)->Pause();
		
		IndividualCommandEvent evtA(ID::NotifyPauseAfter);
		wxPostEvent(THE_FRAME, evtA);
	}
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::wakeUpOnDemand() {
///////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;
	
	if ( isSerialThreadRunning() == false ) {
		resumeSerialThread();
		
		wxASSERT ( isSerialThreadRunning() );
		
		IndividualCommandEvent evt(ID::NotifyFatalError);
		evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
		wxPostEvent(THE_FRAME, evt);
	}
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::onPeriodicallyAppEvent(bool interrupted) {
///////////////////////////////////////////////////////////////////
	// nothing to do
}
#include "CncControl.h"
///////////////////////////////////////////////////////////////////
int SerialThreadStub::readData(void *buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;

	if ( isConnected() == false )
		return 0;
		
	if ( cncControl && cncControl->isInterrupted() )
		return 0;
	
	wakeUpOnDemand();
	
	// first, read buffered data
	int bytesRead         = readBufferedData(buffer, nbByte);
	const int bytesToRead = nbByte - bytesRead;
	
	if ( APP_PROXY::getSerialThread(this) != NULL) {
		if ( APP_PROXY::getSerialThread(this)->getSerialAppEndPoint() != NULL ) {
			// then, read from serial
			if ( bytesToRead > 0 )
				bytesRead += APP_PROXY::getSerialThread(this)->getSerialAppEndPoint()->readBytes((unsigned char*)buffer, bytesToRead); 
		}
		else {
			IndividualCommandEvent evt(ID::NotifyFatalError);
			evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
			wxPostEvent(THE_FRAME, evt);
		}
	}
	else {
		IndividualCommandEvent evt(ID::NotifyFatalError);
		evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
		wxPostEvent(THE_FRAME, evt);
	}
	
	// last, support spy
	spyReadData(buffer, bytesRead);
	return bytesRead;
}
///////////////////////////////////////////////////////////////////
bool SerialThreadStub::writeData(void *buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;
	
	if ( isConnected() == false )
		return false;
		
	if ( cncControl && cncControl->isInterrupted() )
		return false;

	spyWriteData(buffer, nbByte);
	
	wakeUpOnDemand();
	
	if ( APP_PROXY::getSerialThread(this) == NULL) {
		
		IndividualCommandEvent evt(ID::NotifyFatalError);
		evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
		wxPostEvent(THE_FRAME, evt);
		
		return false;
	}
		
	if ( APP_PROXY::getSerialThread(this)->getSerialAppEndPoint() == NULL ) {
		
		IndividualCommandEvent evt(ID::NotifyFatalError);
		evt.setValue(IndividualCommandEvent::ValueName::VAL1, CNC_LOG_LOCATION);
		wxPostEvent(THE_FRAME, evt);
		
		return false;
	}
	
	return APP_PROXY::getSerialThread(this)->getSerialAppEndPoint()->write((unsigned char*)buffer, (int)nbByte); 
}
