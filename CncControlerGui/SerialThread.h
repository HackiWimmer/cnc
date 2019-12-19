#ifndef SerialThread_H
#define SerialThread_H

#include <wx/thread.h>
#include <wx/event.h>

////////////////////////////////////////////////////////////////////////////////////
class MainFrame;

////////////////////////////////////////////////////////////////////////////////////
class SerialEvent;
wxDECLARE_EVENT(wxEVT_SERIAL_THREAD, SerialEvent);

class SerialEvent : public wxThreadEvent {
	
	public:
		SerialEvent(wxEventType eventType = wxEVT_SERIAL_THREAD, int id = 0)
		: wxThreadEvent(eventType, id)
		{}
		
		explicit SerialEvent(const SerialEvent& event)
		: wxThreadEvent(event)
		{}

		virtual wxEvent *Clone() const {
			return new SerialEvent(*this);
		}
};

typedef void (wxEvtHandler::*SerialEventFunction)(SerialEvent&);
#define SerialEventHandler(func) wxEVENT_HANDLER_CAST(SerialEventFunction, func)

////////////////////////////////////////////////////////////////////////////////////
class SerialThread : public wxThread {
	
	protected:
		MainFrame* pHandler;
		bool exit;
		
		virtual ExitCode Entry();
		
	public:
		SerialThread(MainFrame *handler);
		virtual ~SerialThread();
		
		// thread interface
		void stop();
};

#endif
