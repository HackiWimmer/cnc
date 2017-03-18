#ifndef INTERRUPT_THREAD_H
#define INTERRUPT_THREAD_H

#include <wx/thread.h>


class MainFrame;
class InterruptThread : public wxThread {
	public:
		InterruptThread(MainFrame *handler)
		: wxThread(wxTHREAD_DETACHED) 
		{ 
			pHandler = handler;
		}
		~InterruptThread();
		
protected:
		// frame handler
		MainFrame* pHandler;
		virtual ExitCode Entry();
};

#endif