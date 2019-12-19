#include "GlobalFunctions.h"
#include "MainFrame.h"
#include "SerialThread.h"


///////////////////////////////////////////////////////////////////
SerialThread::SerialThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler(handler)
, exit(false)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialThread::~SerialThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pSerialThreadCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	pHandler->serialThread = NULL;
}
///////////////////////////////////////////////////////////////////
void SerialThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode SerialThread::Entry() {
///////////////////////////////////////////////////////////////////
	APPEND_THREAD_IDTO_STACK_TRACE_FILE;
	
	// post complete event
	SerialEvent initEvt(wxEVT_SERIAL_THREAD, MainFrame::EventId::INITIALIZED);
	wxPostEvent(pHandler, initEvt);
	
	while ( !TestDestroy() ) {
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		if ( exit == true )  break;
		
		
		this->Sleep(2000);
		
		SerialEvent hb(wxEVT_SERIAL_THREAD, MainFrame::EventId::SERIAL_HEARTBEAT);
		wxPostEvent(pHandler, hb);

	} // while
	
	// post complete event
	wxQueueEvent(pHandler, new SerialEvent(wxEVT_SERIAL_THREAD, MainFrame::EventId::COMPLETED));
	
	return NULL;
}
