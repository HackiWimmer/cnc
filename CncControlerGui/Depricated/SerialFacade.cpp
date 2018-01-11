#include <iostream>
#include <wx/msgdlg.h>
#include "MainFrame.h"
#include "SerialSimulatorFacade.h"









///////////////////////////////////////////////////////////////////
SerialFacade::SerialFacade(CncControl* cnc)
: SerialSpyPort(cnc)
, serialThread(NULL)
///////////////////////////////////////////////////////////////////
{
	createSerialThread();
}
///////////////////////////////////////////////////////////////////
SerialFacade::SerialFacade(const char *portName)
: SerialSpyPort(portName) 
, serialThread(NULL)
///////////////////////////////////////////////////////////////////
{
	createSerialThread();
}
///////////////////////////////////////////////////////////////////
SerialFacade::~SerialFacade() {
///////////////////////////////////////////////////////////////////
	destroySerialThread();
}
///////////////////////////////////////////////////////////////////
void SerialFacade::createSerialThread() {
///////////////////////////////////////////////////////////////////
	// create the serial thread
	serialThread = new SerialSimulatorThread();
	wxThreadError error = serialThread->Create();

	if ( error != wxTHREAD_NO_ERROR ) {
		wxMessageBox( _("Couldn't create serial thread!") );
		abort();
	}
	
	error = serialThread->Run();
	if ( error != wxTHREAD_NO_ERROR ) {
		wxMessageBox( _("Couldn't run serial thread!") );
		abort();
	}
}
///////////////////////////////////////////////////////////////////
void SerialFacade::destroySerialThread() {
///////////////////////////////////////////////////////////////////
	// destroy the serial thread
	if ( serialThread != NULL ) {
		
		if ( serialThread->Delete() != wxTHREAD_NO_ERROR )
			wxMessageBox("Can't delete the thread!");
		
		while ( true ) {
			// wait for thread completion
			wxThread::This()->Sleep(10);
		
			#warning - todo
			//if ( serialThread == NULL ) 
				break;
		}
	}
}
///////////////////////////////////////////////////////////////////
bool SerialFacade::connect(const char* pn) {
/////////////////////////////////////////////////////////////////// 
	portName = pn;
	connected = ( serialThread != NULL ); 
	return connected; 
}
///////////////////////////////////////////////////////////////////
int SerialFacade::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( serialThread == NULL )
		return 0;
		
	if ( connected == false )
		return false;
		
	int ret = serialThread->readData(buffer, nbByte);
	spyReadData(ret, buffer, nbByte);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialFacade::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( serialThread == NULL )
		return false;
		
	if ( connected == false )
		return false;
		
	spyWriteData(buffer, nbByte);
	return serialThread->writeData(buffer, nbByte);
}
