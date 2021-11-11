#ifndef CNC_STREAM_BUFFERS_H
#define CNC_STREAM_BUFFERS_H

#include <sstream>
#include <wx/listctrl.h>

///////////////////////////////////////////////////////////////////
class CncTextCtrl;
class CncLoggerView;
class CncStartupLoggerProxy;
class CncStandardLoggerProxy;

class LoggerStreamBuf : public std::streambuf {
	
	private:
		char 				type;
		
	protected:
		CncTextCtrl* 		ctl;
		wxTextAttr 			textAttr;
		
	public:
		enum Type { STD = 'C', LOG = 'L', ERR = 'E', EX1 = '1', EX2 = '2', EX3 = '3', TRC = 'T', MSG = 'M', SPY = 'S' };
		
		static wxString 	startupBuffer;
		static wxTextAttr 	defaultAttr;
		
		///////////////////////////////////////////////////////////
		LoggerStreamBuf(char t, CncTextCtrl* c, const wxTextAttr& ta);
		virtual ~LoggerStreamBuf() {}
		
		virtual int overflow (int c = EOF);
		
		void setTextAttr(const wxTextAttr& ta);
		void setTextControl(CncTextCtrl* c);
		
		const wxTextAttr& getTextAttr() 	const 	{ return textAttr; }
		CncTextCtrl* getTextControl()		const 	{ return ctl; }
		void ungregisterTextControl() 				{ ctl = NULL; }
};

///////////////////////////////////////////////////////////
namespace StartupBuffer {
	
	void append(LoggerStreamBuf::Type t, const char* text);
	bool trace(CncLoggerView* ctl);
	
};

///////////////////////////////////////////////////////////////////
class CncCoutBuf : public LoggerStreamBuf 
{
	public:
		explicit CncCoutBuf(CncTextCtrl* c);
		virtual ~CncCoutBuf();
};

///////////////////////////////////////////////////////////////////
class CncClogBuf : public LoggerStreamBuf 
{
	public:
		explicit CncClogBuf(CncTextCtrl* c);
		virtual ~CncClogBuf() ;
};

///////////////////////////////////////////////////////////////////
class CncCerrBuf : public LoggerStreamBuf
{
	public:
		explicit CncCerrBuf(CncTextCtrl* c);
		virtual ~CncCerrBuf();
};

///////////////////////////////////////////////////////////////////
class CncCex1Buf : public LoggerStreamBuf
{
	public:
		explicit CncCex1Buf(CncTextCtrl* c);
		virtual ~CncCex1Buf();
};

///////////////////////////////////////////////////////////////////
class CncCex2Buf : public LoggerStreamBuf
{
	public:
		explicit CncCex2Buf(CncTextCtrl* c);
		virtual ~CncCex2Buf();
};

///////////////////////////////////////////////////////////////////
class CncCex3Buf : public LoggerStreamBuf
{
	public:
		explicit CncCex3Buf(CncTextCtrl* c);
		virtual ~CncCex3Buf();
};

///////////////////////////////////////////////////////////////////
class CncCtrcBuf : public LoggerStreamBuf {
	public:
		explicit CncCtrcBuf(CncTextCtrl* c) 
		: LoggerStreamBuf(LoggerStreamBuf::Type::TRC, c, LoggerStreamBuf::defaultAttr) {}
		virtual ~CncCtrcBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncCmsgBuf : public LoggerStreamBuf {
	public:
		explicit CncCmsgBuf(CncTextCtrl* c) 
		: LoggerStreamBuf(LoggerStreamBuf::Type::MSG, c, LoggerStreamBuf::defaultAttr) {}
		virtual ~CncCmsgBuf() {}
};

///////////////////////////////////////////////////////////////////
class CncCspyBuf : public LoggerStreamBuf {
	
	private:
		static const unsigned int maxBufferSize = 2048;
		unsigned int bufferIndex;
		char buffer[maxBufferSize];
		
		wxListCtrl* listCtrl;
	
	public:
		//////////////////////////////////////////////////////////
		explicit CncCspyBuf(wxListCtrl* c) 
		: LoggerStreamBuf(LoggerStreamBuf::Type::SPY, NULL, LoggerStreamBuf::defaultAttr) 
		, bufferIndex(0)
		, listCtrl(c)
		{}
		
		virtual ~CncCspyBuf() 
		{}
		
		///////////////////////////////////////////////////////////
		virtual int overflow (int c = EOF);
		virtual void flush();
		
		void addLine(const wxString& line, int type);
		void addLine(const wxString& line, const wxString& appendix, int type);
};

///////////////////////////////////////////////////////////////////
class CncBasicLogStream : public std::ostream { 
	
	public:
		//////////////////////////////////////////////////////////
		CncBasicLogStream() 
		: std::ostream() 
		{}
		
		//////////////////////////////////////////////////////////
		virtual ~CncBasicLogStream() {
		}
		
		//////////////////////////////////////////////////////////
		void ungregisterTextControl();
		void resetTextAttr();
		void setTextAttr(const wxTextAttr& ta);
		void setTextColour(const wxColour& c);
		const wxTextAttr& getTextAttr() const;
		const wxColour& getTextColour() const;
			
		//////////////////////////////////////////////////////////
		void red() 		{ setTextColour(wxColour(255, 128, 128)); }
		void black() 	{ setTextColour(*wxBLACK);                }
		void blue() 	{ setTextColour(wxColour(0,   162, 232)); }
		void green() 	{ setTextColour(wxColour(0,   128,  64)); }
		void white() 	{ setTextColour(*wxWHITE);                }
		void gray() 	{ setTextColour(*wxLIGHT_GREY);           }
		void yellow()	{ setTextColour(wxColour(255, 242,   0)); }
};

///////////////////////////////////////////////////////////////////
class CncTraceLogStream : public CncBasicLogStream {
	
	protected:
		
		wxColour infoColour;
		wxColour warningColour;
		wxColour errorColour;
		wxColour debugColour;
		
		///////////////////////////////////////////////////////////
		virtual void logMessage(const char* m);
		
	public:
		///////////////////////////////////////////////////////////
		CncTraceLogStream() 
		: CncBasicLogStream()
		, infoColour		(*wxWHITE)
		, warningColour		(255, 201, 14)
		, errorColour		(wxColour(255, 128, 128))
		, debugColour		(128, 128,  0)
		{}

		///////////////////////////////////////////////////////////
		CncTraceLogStream(const CncTraceLogStream& ctb) 
		: CncBasicLogStream()
		, infoColour		(*wxWHITE)
		, warningColour		(255, 201, 14)
		, errorColour		(wxColour(255, 128, 128))
		, debugColour		(128, 128,  0)
		{}

		///////////////////////////////////////////////////////////
		virtual ~CncTraceLogStream() 
		{}

		///////////////////////////////////////////////////////////
		CncTextCtrl* getTextControl() const 
		{
			LoggerStreamBuf* logStreamBuffer = static_cast<LoggerStreamBuf*>(rdbuf());
			if ( logStreamBuffer != NULL )
				return logStreamBuffer->getTextControl();
				
			return NULL;
		}
		
		///////////////////////////////////////////////////////////
		const wxColour getInfoColour()    const { return infoColour;    }
		const wxColour getWarningColour() const { return warningColour; }
		const wxColour getErrorColour()   const { return errorColour;   }
		const wxColour getDebugColour()   const { return debugColour;   }
		
		const char getCurrentDesignAsChar();
		
		///////////////////////////////////////////////////////////
		void clear();
		void logInfoMessage		(const char* m);
		void logWarningMessage	(const char* m);
		void logErrorMessage	(const char* m);
		void logDebugMessage	(const char* m);

		///////////////////////////////////////////////////////////
		void setInfoColour		(const wxColour& c)				{ infoColour 	= c; }
		void setWarningColour	(const wxColour& c)				{ warningColour = c; }
		void setErrorColour		(const wxColour& c)				{ errorColour 	= c; }
		void setDebugColour		(const wxColour& c)				{ debugColour 	= c; }
		
		///////////////////////////////////////////////////////////
		void logInfo			(const char* m)					{ logInfoMessage(m);    }
		void logWarning			(const char* m)					{ logWarningMessage(m); }
		void logError			(const char* m)					{ logErrorMessage(m);   }
		void logDebug			(const char* m)					{ logDebugMessage(m);   }
		
		///////////////////////////////////////////////////////////
		void logInfoMessage		(const std::stringstream& ss)	{ logInfoMessage(ss.str().c_str()); }
		void logWarningMessage	(const std::stringstream& ss)	{ logWarningMessage(ss.str().c_str()); }
		void logErrorMessage	(const std::stringstream& ss)	{ logErrorMessage(ss.str().c_str()); }
		void logDebugMessage	(const std::stringstream& ss)	{ logDebugMessage(ss.str().c_str()); }
		
		void logInfo			(const std::stringstream& ss)	{ logInfoMessage(ss.str().c_str()); }
		void logWarning			(const std::stringstream& ss)	{ logWarningMessage(ss.str().c_str()); }
		void logError			(const std::stringstream& ss)	{ logErrorMessage(ss.str().c_str()); }
		void logDebug			(const std::stringstream& ss)	{ logDebugMessage(ss.str().c_str()); }
		
		const char* getCurrentMessage() const;
};

///////////////////////////////////////////////////////////////////
class CncMsgLogStream : public CncTraceLogStream {
	
	protected:
		///////////////////////////////////////////////////////////
		virtual void logMessage(const char* m);
		
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

///////////////////////////////////////////////////////////////////
class CncSerialSpyStream : public CncTraceLogStream {
	
	protected:
		///////////////////////////////////////////////////////////
		virtual void logMessage(const char* m);
		virtual void logTime();
		
	public:
	
		static const char* hLine;
		static const char* mLine;
		///////////////////////////////////////////////////////////
		CncSerialSpyStream() 
		: CncTraceLogStream()
		{}

		///////////////////////////////////////////////////////////
		CncSerialSpyStream(const CncSerialSpyStream& cmb) 
		: CncTraceLogStream()
		{}

		///////////////////////////////////////////////////////////
		virtual ~CncSerialSpyStream() {
		}
		
		///////////////////////////////////////////////////////////
		void logCommand				(const wxString& cmd);
		void addMarker				(const wxString& mt);
		void addDebugEntry			(const wxString& mt);
		void enableMessage			(const char* additional = NULL);
		void disableMessage			(const char* additional = NULL);
		
		///////////////////////////////////////////////////////////
		void initializeResult(const char* msg = NULL);
		
		void finalizeRET_OK			(const char* msg = NULL);
		void finalizeRET_MORE		(const char* msg = NULL);
		void finalizeRET_ERROR		(const char* msg = NULL );
		void finalizeRET_LIMIT		(const char* msg = NULL );
		void finalizeRET_INTERRUPT	(const char* msg = NULL );
		void finalizeRET_HALT		(const char* msg = NULL );
		void finalizeRET_QUIT		(const char* msg = NULL );

};

template <class T> 
class CncTempTextAttr {
	
	private:
		T*			stream;
		wxTextAttr	prvAttr;
		
	public:
		CncTempTextAttr(T* s, const wxTextAttr& tmpAttr) 
		: stream	(s)
		, prvAttr	(s ? s->getTextAttr() : tmpAttr)
		{
			if  ( stream ) {
				stream->setTextAttr(tmpAttr);
				
				if ( stream->getTextControl() != NULL )
					stream->getTextControl()->SetDefaultStyle(tmpAttr);
			}
		}
		
		~CncTempTextAttr() 
		{
			if  ( stream ) {
				stream->setTextAttr(prvAttr);
				
				if ( stream->getTextControl() != NULL )
					stream->getTextControl()->SetDefaultStyle(prvAttr);
			}
		}
};

#endif

