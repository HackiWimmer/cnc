#ifndef SERIAL_EVENT_H
#define SERIAL_EVENT_H

#include "ArduinoData.h"

////////////////////////////////////////////////////////////////////////////////////
class SerialEvent;
wxDECLARE_EVENT(wxEVT_SERIAL_THREAD, SerialEvent);

class SerialEvent : public wxThreadEvent {
	
	public:

		// --------------------------------------------------------------------
		struct Message {
			
			enum Type {	MT_INFO='I', MT_WARNING='W', MT_DEBUG='D', MT_ERROR='E', MT_SEPERATOR='S' };
			Type 		type 	= MT_INFO;

			wxString 	message;
			wxString 	context;
			
			void setType(const char t) {
				switch ( t ) {
					case 'W':	type = MT_WARNING;		break;
					case 'D':	type = MT_DEBUG;		break;
					case 'E':	type = MT_ERROR;		break;
					case 'S': 	type = MT_SEPERATOR; 	break;
					default:	type = MT_INFO;
				}
			}
			
			void setMessage(const wxString& m) 	{ message.assign(m); }
			void setContext(const wxString& c) 	{ context.assign(c); }

			const wxString& getMessage() 		{ return message;    }
			const wxString& getContext() 		{ return context;    }

			// Info: The message text has to be stored a event.SetString();
			
		} message;
		
		// --------------------------------------------------------------------
		SerialEvent(wxEventType eventType = wxEVT_SERIAL_THREAD, int id = 0)
		: wxThreadEvent(eventType, id)
		{}

		explicit SerialEvent(const SerialEvent& event)
		: wxThreadEvent(event)
		{
			// make sure our string member (which uses COW, aka refcounting) is not
			// shared by other wxString instances:
			message.setMessage(message.getMessage().Clone());
			message.setContext(message.getContext().Clone());
		}
		
		virtual wxEvent *Clone() const {
			return new SerialEvent(*this);
		}
};

#endif
