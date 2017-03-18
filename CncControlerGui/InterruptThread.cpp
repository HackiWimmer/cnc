
#include "MainFrame.h"
#include "InterruptThread.h"

///////////////////////////////////////////////////////////////////
InterruptThread::~InterruptThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	pHandler->pThread = NULL;
}

///////////////////////////////////////////////////////////////////
wxThread::ExitCode InterruptThread::Entry() {
///////////////////////////////////////////////////////////////////
	while ( !TestDestroy() ) {
		// ... do a bit of work...
	
		wxQueueEvent(pHandler, new wxThreadEvent(wxEVT_COMMAND_MYTHREAD_UPDATE));
	}
	
	// signal the event handler that this thread is going to be destroyed
	// NOTE: here we assume that using the m_pHandler pointer is safe,
	//       (in this case this is assured by the MyFrame destructor)
	wxQueueEvent(pHandler, new wxThreadEvent(wxEVT_COMMAND_MYTHREAD_COMPLETED));
	return (wxThread::ExitCode)0;
}
