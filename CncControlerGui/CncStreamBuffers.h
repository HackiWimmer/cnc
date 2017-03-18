#ifndef CNC_STREAM_BUFFERS_H
#define CNC_STREAM_BUFFERS_H

#include <sstream>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////
class LoggerStreamBuf : public std::streambuf {
	
	protected:
		wxTextCtrl* ctl;
		wxTextAttr textAttr;
		
	public:
		///////////////////////////////////////////////////////////
		LoggerStreamBuf(wxTextCtrl* c, wxTextAttr ta) 
		: ctl(c)
		, textAttr(ta)
		{}
		virtual ~LoggerStreamBuf() {}
		
		///////////////////////////////////////////////////////////
		virtual int overflow (int c = EOF) {
			
			ctl->SetDefaultStyle(textAttr);
			ctl->AppendText((wxChar)c);
			
			// return something different from EOF
			return 0;
		}
		
		///////////////////////////////////////////////////////////
		void setTextAttr(const wxTextAttr& ta) {
			textAttr = ta;
		}
		
		///////////////////////////////////////////////////////////
		wxTextCtrl* getTextControl() const {
			return ctl;
		}
};

///////////////////////////////////////////////////////////////////
class CncCoutBuf : public LoggerStreamBuf {
	public:
		///////////////////////////////////////////////////////////
		CncCoutBuf(wxTextCtrl* c) 
		: LoggerStreamBuf(c, wxTextAttr(*wxLIGHT_GREY)) {}
		virtual ~CncCoutBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncClogBuf : public LoggerStreamBuf {
	public:
		CncClogBuf(wxTextCtrl* c) 
		: LoggerStreamBuf(c, wxTextAttr(*wxCYAN)) {}
		virtual ~CncClogBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncCerrBuf : public LoggerStreamBuf {
	public:
		CncCerrBuf(wxTextCtrl* c) 
		: LoggerStreamBuf(c, wxTextAttr(*wxRED)) {}
		virtual ~CncCerrBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncCex1Buf : public LoggerStreamBuf {
	public:
		CncCex1Buf(wxTextCtrl* c) 
		: LoggerStreamBuf(c, wxTextAttr(wxColour(255, 201, 14))) {}
		virtual ~CncCex1Buf() {}
};

///////////////////////////////////////////////////////////////////
class CncCtrcBuf : public LoggerStreamBuf {
	public:
		CncCtrcBuf(wxTextCtrl* c) 
		: LoggerStreamBuf(c, wxTextAttr(*wxLIGHT_GREY)) {}
		virtual ~CncCtrcBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncCmsgBuf : public LoggerStreamBuf {
	public:
		CncCmsgBuf(wxTextCtrl* c) 
		: LoggerStreamBuf(c, wxTextAttr(*wxLIGHT_GREY)) {}
		virtual ~CncCmsgBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncBasicLogStream : public std::stringstream {
	
	protected:
		LoggerStreamBuf* logStreamBuffer;
	
	public:
		//////////////////////////////////////////////////////////
		CncBasicLogStream() 
		: std::stringstream() 
		, logStreamBuffer(NULL)
		{}
		
		//////////////////////////////////////////////////////////
		virtual ~CncBasicLogStream() {
			
		}
		
		//////////////////////////////////////////////////////////
		void setLogStreamBuffer(LoggerStreamBuf* lsb) {
			logStreamBuffer = lsb;
		}
		
		//////////////////////////////////////////////////////////
		void resetTextAttr() {
			setTextAttr(wxTextAttr(*wxLIGHT_GREY)); 
		}
		
		//////////////////////////////////////////////////////////
		void setTextAttr(const wxTextAttr& ta) {
			if ( logStreamBuffer != NULL )
				logStreamBuffer->setTextAttr(ta);
		}
		
		//////////////////////////////////////////////////////////
		void setTextColour(const wxColour& c) {
			if ( logStreamBuffer != NULL )
				logStreamBuffer->setTextAttr(c);
		}
		
		//////////////////////////////////////////////////////////
		void red() 		{ setTextColour(*wxRED); }
		void black() 	{ setTextColour(*wxBLACK); }
		void blue() 	{ setTextColour(*wxBLUE); }
		void green() 	{ setTextColour(*wxGREEN); }
		void white() 	{ setTextColour(*wxWHITE); }
		void gray() 	{ setTextColour(*wxLIGHT_GREY); }
};

///////////////////////////////////////////////////////////////////
class CncTraceLogStream : public CncBasicLogStream {
	
	protected:
		///////////////////////////////////////////////////////////
		virtual void logMessage(const char* m) {
			clear();
			
			if ( m == NULL )
				return;
				
			wxString msg(m);
			msg.Replace("\n", " ", true);
			msg.Replace("\r", "", true);
			
			if ( getTextControl() != NULL )
				getTextControl()->AppendText(msg);
		}
		
		///////////////////////////////////////////////////////////
		wxTextCtrl* getTextControl() const {
			if ( logStreamBuffer != NULL )
				return logStreamBuffer->getTextControl();
				
			return NULL;
		}
		
	public:
		///////////////////////////////////////////////////////////
		CncTraceLogStream() 
		: CncBasicLogStream()
		{}

		///////////////////////////////////////////////////////////
		CncTraceLogStream(const CncTraceLogStream& ctb) 
		: CncBasicLogStream()
		{}

		///////////////////////////////////////////////////////////
		virtual ~CncTraceLogStream() 
		{}

		///////////////////////////////////////////////////////////
		void clear() {
			if ( getTextControl() != NULL ) {
				getTextControl()->Clear();
			}
		}
		
		///////////////////////////////////////////////////////////
		void logInfoMessage(const char* m) {
			if ( getTextControl() != NULL ) {
				getTextControl()->SetDefaultStyle(wxTextAttr(*wxLIGHT_GREY));
				logMessage(m);
			}
		}
		
		///////////////////////////////////////////////////////////
		void logWarningMessage(const char* m) {
			if ( getTextControl() != NULL ) {
				getTextControl()->SetDefaultStyle(wxTextAttr(wxColour(255, 201, 14)));
				logMessage(m);
			}
		}
		
		///////////////////////////////////////////////////////////
		void logErrorMessage(const char* m) {
			if ( getTextControl() != NULL ) {
				getTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
				logMessage(m);
			}
		}
		
		///////////////////////////////////////////////////////////
		void logInfo(const char* m) 	{ logInfoMessage(m); }
		void logWarning(const char* m) 	{ logWarningMessage(m); }
		void logError(const char* m) 	{ logErrorMessage(m); }
		
		///////////////////////////////////////////////////////////
		void logInfoMessage(const std::stringstream& ss) 	{ logInfoMessage(ss.str().c_str()); }
		void logWarningMessage(const std::stringstream& ss)	{ logWarningMessage(ss.str().c_str()); }
		void logErrorMessage(const std::stringstream& ss) 	{ logErrorMessage(ss.str().c_str()); }
		void logInfo(const std::stringstream& ss) 			{ logInfoMessage(ss.str().c_str()); }
		void logWarning(const std::stringstream& ss)		{ logWarningMessage(ss.str().c_str()); }
		void logError(const std::stringstream& ss) 			{ logErrorMessage(ss.str().c_str()); }
		
		///////////////////////////////////////////////////////////
		const char* getCurrentMessage() const {
			if ( getTextControl() == NULL)
				return "";
				
			return getTextControl()->GetValue();
		}
};

///////////////////////////////////////////////////////////////////
class CncMsgLogStream : public CncTraceLogStream {
	
	protected:
		///////////////////////////////////////////////////////////
		virtual void logMessage(const char* m) {
			if ( m == NULL )
				return;
				
			if ( getTextControl() != NULL )
				getTextControl()->AppendText(m);
		}
		
	public:
		///////////////////////////////////////////////////////////
		CncMsgLogStream() 
		: CncTraceLogStream()
		{}

		///////////////////////////////////////////////////////////
		CncMsgLogStream(const CncMsgLogStream& cmb) 
		: CncTraceLogStream()
		{}

		///////////////////////////////////////////////////////////
		virtual ~CncMsgLogStream() 
		{}
};

#endif