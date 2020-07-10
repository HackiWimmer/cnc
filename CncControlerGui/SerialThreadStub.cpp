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
	wxASSERT( isSerialThreadInstalled() == true );
	
	wakeUpOnDemand();
	setConnected(isSerialThreadRunning());
	
	if ( isConnected() ) {
		IndividualCommandEvent evt(ID::NotifyConneting);
		wxPostEvent(THE_FRAME, evt);
	}
	return true; 
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::notifySetupSuccesfullyFinsihed() {
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false )
		return;
 
	typedef IndividualCommandEvent::EvtSerialStub ID;
	wxASSERT( isSerialThreadInstalled() == true );
	
	wakeUpOnDemand();
	
	IndividualCommandEvent evt(ID::NotifyConneted);
	wxPostEvent(THE_FRAME, evt);
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::disconnect(void) {
///////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtSerialStub ID;
	
	if ( isConnected() == false ) 
		return;
	
	wxASSERT( isSerialThreadInstalled() == true );
	
	wakeUpOnDemand();
	setConnected(false);
	
	if ( isSerialThreadRunning() == false )
		pauseSerialThread();
	
	IndividualCommandEvent evt(ID::NotifyDisconnected);
	wxPostEvent(THE_FRAME, evt);
}
/////////////////////////////////////////////////////////////////////
bool SerialThreadStub::isSerialThreadInstalled() {
/////////////////////////////////////////////////////////////////////
	return APP_PROXY::getSerialThread(this) != NULL;
}
/////////////////////////////////////////////////////////////////////
bool SerialThreadStub::isSerialThreadRunning() {
/////////////////////////////////////////////////////////////////////
	if ( isSerialThreadInstalled() == false )
		return false;
		
	return APP_PROXY::getSerialThread(this)->IsRunning();
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
	if ( isSerialThreadRunning() == false ) {
		resumeSerialThread();
		
		wxASSERT ( isSerialThreadRunning() );
	}
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::onPeriodicallyAppEvent(bool interrupted) {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
int SerialThreadStub::readData(void *buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false )
		return 0;

	wakeUpOnDemand();
	const bool ret = APP_PROXY::getSerialThread(this)->getSerialAppEndPoint()->readBytes((unsigned char*)buffer, (int)nbByte); 
	
	spyReadData(buffer, ret);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialThreadStub::writeData(void *buffer, unsigned int nbByte) { 
///////////////////////////////////////////////////////////////////
	if ( isConnected() == false )
		return false;
		
	spyWriteData(buffer, nbByte);
	
	wakeUpOnDemand();
	return APP_PROXY::getSerialThread(this)->getSerialAppEndPoint()->write((unsigned char*)buffer, (int)nbByte); 
}
