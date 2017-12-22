#include <iostream>
#include <wx/msgdlg.h>
#include "MainFrame.h"
#include "SerialSimulatorDevNull.h"
#include "SerialSimulatorFacade.h"

///////////////////////////////////////////////////////////////////
SerialSimulatorFacade::SerialSimulatorFacade(CncControl* cnc)
: SerialSpyPort(cnc)
, serialThread(NULL)
, serialMutex()
, serialCondition(serialMutex)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialSimulatorFacade::SerialSimulatorFacade(const char *portName)
: SerialSpyPort(portName) 
, serialThread(NULL)
, serialMutex()
, serialCondition(serialMutex)
///////////////////////////////////////////////////////////////////
{
	// the mutex should be initially locked - see wxCondition description
	serialMutex.Lock();
}
///////////////////////////////////////////////////////////////////
SerialSimulatorFacade::~SerialSimulatorFacade() {
///////////////////////////////////////////////////////////////////
	destroySerialThread();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::sleepMilliseconds(unsigned int millis) {
///////////////////////////////////////////////////////////////////
	// if no thread exists wait conventional
	if ( serialThread == NULL ) {
		GBL_CONFIG->getTheApp()->waitActive(millis);
		return;
	}
	
	// sleep in this case mean waiting until the condition is true
	// the mutex should be initially locked - see wxCondition description
	serialMutex.Lock();
	serialCondition.WaitTimeout(millis);
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::onPeriodicallyAppEvent() {
///////////////////////////////////////////////////////////////////
	if ( serialThread == NULL )
		return;

	if ( connected == false )
		return;
		
	if ( serialThread->IsPaused() == false ) {
		if ( (wxDateTime::UNow() - serialThread->getLastLog()).GetMilliseconds() > 3000 ) {
			
			if ( serialThread->readAvailable() > 0 ) {
				cnc::cex1 << "SerialSimulatorFacade::onPeriodicallyAppEvent: Going to pause the serial thread. Warning: More bytes to read availiable: " << serialThread->readAvailable();
				cnc::cex1 << "; Front byte: "  << serialThread->readBufferFrontByte() << std::endl;
				
				// to give the run control a change to release events
				GBL_CONFIG->getTheApp()->dispatchAll();
				
			} else {
				pauseSerialThread();
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::pauseSerialThread() {
///////////////////////////////////////////////////////////////////
	if ( serialThread == NULL )
		return;
		
	if ( serialThread->IsPaused() == false ) {
		wxCriticalSectionLocker enter(serialThreadCS);
		serialThread->Pause();
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::resumeSerialThread() {
///////////////////////////////////////////////////////////////////
	if ( serialThread == NULL )
		return;
		
	if ( serialThread->IsPaused() == true ) {
		wxCriticalSectionLocker enter(serialThreadCS);
		serialThread->Resume();
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::wakeUpOnDemand() {
///////////////////////////////////////////////////////////////////
	resumeSerialThread();
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::createSerialThread() {
///////////////////////////////////////////////////////////////////
	// add further simulator versions on demand here
	if ( wxString(getPortName()) == _portSimulatorNULL )
		serialThread = new SerialSimulatorDevNull(this);
	
	if ( serialThread == NULL ) {
		std::cerr << "SerialSimulatorFacade::createSerialThread: Cant select a SerialSimulator for: " << getPortName() << std::endl;
		return;
	}
	
	// create the serial thread
	wxThreadError error = serialThread->Create();
	if ( error != wxTHREAD_NO_ERROR ) {
		std::cerr << "SerialSimulatorFacade::createSerialThread: Couldn't create serial thread!" << std::endl;
		serialThread = NULL;
		return;
	}
	
	error = serialThread->Run();
	if ( error != wxTHREAD_NO_ERROR ) {
		std::cerr << "SerialSimulatorFacade::createSerialThread: Couldn't run serial thread!" << std::endl;
		serialThread = NULL;
		return;
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::destroySerialThread() {
///////////////////////////////////////////////////////////////////
	// destroy the serial thread
	if ( serialThread != NULL ) {
		wakeUpOnDemand();
		
		{
			wxCriticalSectionLocker enter(serialThreadCS);
			
			if ( serialThread->Delete() != wxTHREAD_NO_ERROR )
				wxMessageBox("SerialSimulatorFacade::destroySerialThread: Can't delete the thread!");
		}
		
		// wait for thread completion
		wxThread::This()->Sleep(10);
		
		// no additional checks for thread completion implemented
		// because TestDestroy() will be called immediately
	}
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorFacade::connect(const char* pn) {
/////////////////////////////////////////////////////////////////// 
	if ( connected == true )
		return true;
	
	// store the port name
	portName = pn;
	
	// check if already a trgead exists
	if ( serialThread != NULL )
		destroySerialThread();
		
	// create a new thread
	createSerialThread();
	
	if ( serialThread == NULL )
		return false;
	
	// wait a portion of time so, that the new thread 
	// was well established and ready to use
	wxThread::This()->Sleep(10);
	
	connected = ( serialThread != NULL && serialThread->IsRunning() ); 
	return connected; 
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorFacade::disconnect(void) {
///////////////////////////////////////////////////////////////////
	if ( connected == false )
		return;
	
	if ( serialThread == NULL )
		return;
		
	// wakeup on demand
	wakeUpOnDemand();
	
	connected = false;
}
///////////////////////////////////////////////////////////////////
int SerialSimulatorFacade::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( connected == false )
		return 0;
	
	if ( serialThread == NULL )
		return 0;
		
	wakeUpOnDemand();
	
	int ret = serialThread->readData(buffer, nbByte);
	spyReadData(ret, buffer, nbByte);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorFacade::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( connected == false )
		return false;
		
	if ( serialThread == NULL )
		return false;
		
	// wakeup on demand
	wakeUpOnDemand();
	
	spyWriteData(buffer, nbByte);
	return serialThread->writeData(buffer, nbByte);
}
