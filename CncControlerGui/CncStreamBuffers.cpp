#include "CncSerialSpyListCtrl.h"
#include "CncLoggerProxy.h"
#include "CncStreamBuffers.h"

wxString   		LoggerStreamBuf::startupBuffer 		= wxString("");
wxTextAttr  	LoggerStreamBuf::defaultAttr 		= wxTextAttr(wxColour(195, 195, 195));
const char* 	CncSerialSpyStream::hLine			= "----------------------------------------------\n";
const char* 	CncSerialSpyStream::mLine			= "***********************************************\n";

///////////////////////////////////////////////////////////
namespace StartupBuffer {
	
	//-------------------------------------------------------------------------
	void append(LoggerStreamBuf::Type t, const char* text) {
		if ( text == NULL )
			return;
			
		wxString msg(text);
		wxString result;
		for ( unsigned int i = 0; i < msg.length(); i++ ) {
			result.append((char)t);
			result.append(msg[i]);
		}
		
		LoggerStreamBuf::startupBuffer.append(result);
	}
	
	//-------------------------------------------------------------------------
	bool trace(CncTextCtrl* ctl) {
		if ( ctl == NULL )
			return false;
		
		bool containsLogOrErr = false;
		
		wxTextAttr ta(ctl->GetDefaultStyle());
		for ( unsigned int i = 0; i < LoggerStreamBuf::startupBuffer.length(); i++ ) {
			
			if ( i % 2 == 0 ) {
				// over all format chars
				switch ( (int)LoggerStreamBuf::startupBuffer[i] ) {
					
					case LoggerStreamBuf::Type::LOG:	containsLogOrErr = true;
														ta.SetTextColour(wxColour(0, 157, 157));
														break;
														
					case LoggerStreamBuf::Type::ERR:	containsLogOrErr = true;
														ta.SetTextColour(wxColour(255, 64, 64));
														break;
														
					default:							ta.SetTextColour(wxColour(192, 192, 192));
				}
				
			} else {
				// over all text chars
				ctl->SetDefaultStyle(ta);
				ctl->AppendText(LoggerStreamBuf::startupBuffer[i]);
			}
		}
		
		return containsLogOrErr;
	}
};


///////////////////////////////////////////////////////////
LoggerStreamBuf::LoggerStreamBuf(char t, CncTextCtrl* c, const wxTextAttr& ta) 
: type(t)
, ctl(c)
, textAttr(ta)
///////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////
void LoggerStreamBuf::setTextAttr(const wxTextAttr& ta) {
///////////////////////////////////////////////////////////
	textAttr = ta;
}
///////////////////////////////////////////////////////////
void LoggerStreamBuf::setTextControl(CncTextCtrl* c) {
	wxASSERT(c);
	ctl = c;
}
///////////////////////////////////////////////////////////
int LoggerStreamBuf::overflow(int c) {
///////////////////////////////////////////////////////////
	if ( ctl != NULL ) {
		ctl->SetDefaultStyle(textAttr);
		ctl->AppendChar((char)c);
		
	} else {
		startupBuffer.append(type);
		startupBuffer.append((wxChar)c);
		
	}
	
	// return something different from EOF
	return 0;
}

///////////////////////////////////////////////////////////
int CncCspyBuf::overflow (int c) {
///////////////////////////////////////////////////////////
	buffer[bufferIndex++] = c;
	
	if ( c == '\n' || maxBufferSize - bufferIndex < 3 ) {
		// remove the "\n"
		bufferIndex--;
		
		flush();
	}
	
	// return something different from EOF
	return 0;
}
///////////////////////////////////////////////////////////
void CncCspyBuf::flush() {
///////////////////////////////////////////////////////////
	if ( listCtrl == NULL )
		return; 
	
	buffer[bufferIndex] = '\0';
	bufferIndex = 0;
	
	CncSerialSpyListCtrl* c = static_cast<CncSerialSpyListCtrl*>(listCtrl);
	if ( c == NULL )
		return;
	
	c->addLine(buffer, CncSerialSpyListCtrl::LineType::LT_Default);
}
///////////////////////////////////////////////////////////
void CncCspyBuf::addLine(const wxString& line, int type) {
///////////////////////////////////////////////////////////
	CncSerialSpyListCtrl* c = static_cast<CncSerialSpyListCtrl*>(listCtrl);
	if ( c == NULL )
		return;
	c->addLine(line, (CncSerialSpyListCtrl::LineType)type);
}


///////////////////////////////////////////////////////////
void CncSerialSpyStream::logCommand(const wxString& cmd) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine(cmd, CncSerialSpyListCtrl::LineType::LT_Command);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::initializeResult(const char* msg) {
///////////////////////////////////////////////////////////	
	wxDateTime now = wxDateTime::UNow();
	
	if ( msg != NULL )	logCommand(wxString::Format("[%s]: %s", now.Format("%H:%M:%S.%l"), msg));
	else				logCommand(wxString::Format("[%s]:",    now.Format("%H:%M:%S.%l")));
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_OK(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultOk);
		
	c->addLine("RET_OK", CncSerialSpyListCtrl::LineType::LT_ResultOk);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_MORE(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultMore);
		
	c->addLine("RET_MORE", CncSerialSpyListCtrl::LineType::LT_ResultMore);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_ERROR(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultError);
		
	c->addLine("RET_ERROR", CncSerialSpyListCtrl::LineType::LT_ResultError);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_LIMIT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultLimit);
		
	c->addLine("RET_LIMIT", CncSerialSpyListCtrl::LineType::LT_ResultLimit);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_INTERRUPT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultInterrupt);
		
	c->addLine("RET_INTERRUPT", CncSerialSpyListCtrl::LineType::LT_ResultInterrupt);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_HALT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultHalt);
		
	c->addLine("RET_HALT", CncSerialSpyListCtrl::LineType::LT_ResultHalt);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_QUIT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	if ( msg != NULL )
		c->addLine(msg, CncSerialSpyListCtrl::LineType::LT_ResultQuit);
		
	c->addLine("RET_QUIT", CncSerialSpyListCtrl::LineType::LT_ResultQuit);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::addMarker(const wxString& mt) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
		
	c->addLine(mt, CncSerialSpyListCtrl::LineType::LT_Marker);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::enableMessage(const char* additional) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;

	c->addLine("Serial Spy enabled . . .", CncSerialSpyListCtrl::LineType::LT_Enable);
	
	if ( additional != NULL )
		c->addLine(additional, CncSerialSpyListCtrl::LineType::LT_Enable);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::disableMessage(const char* additional) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;

	c->addLine("Serial Spy disabled . . .", CncSerialSpyListCtrl::LineType::LT_Disable);
	
	if ( additional != NULL )
		c->addLine(additional, CncSerialSpyListCtrl::LineType::LT_Disable);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::logMessage(const char* m) {
///////////////////////////////////////////////////////////
	if ( m == NULL )
		return;
	
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	c->addLine(m, CncSerialSpyListCtrl::LineType::LT_Default);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::logTime() {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	wxDateTime now = wxDateTime::UNow();
	c->addLine(now.Format("Time: %H:%M:%S.%l: "), CncSerialSpyListCtrl::LineType::LT_Time);
}



///////////////////////////////////////////////////////////
void CncTraceLogStream::logMessage(const char* m) {
///////////////////////////////////////////////////////////
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
void CncTraceLogStream::clear() {
///////////////////////////////////////////////////////////
	if ( getTextControl() != NULL ) {
		getTextControl()->Clear();
	}
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logInfoMessage(const char* m) {
///////////////////////////////////////////////////////////
	if ( getTextControl() != NULL ) {
		getTextControl()->SetDefaultStyle(wxTextAttr(infoColour));
		logMessage(m);
	}
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logWarningMessage(const char* m) {
///////////////////////////////////////////////////////////
	if ( getTextControl() != NULL ) {
		getTextControl()->SetDefaultStyle(wxTextAttr(warningColour));
		logMessage(m);
	}
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logErrorMessage(const char* m) {
///////////////////////////////////////////////////////////
	if ( getTextControl() != NULL ) {
		getTextControl()->SetDefaultStyle(wxTextAttr(errorColour));
		logMessage(m);
	}
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logDebugMessage(const char* m) {
///////////////////////////////////////////////////////////
	if ( getTextControl() != NULL ) {
		getTextControl()->SetDefaultStyle(wxTextAttr(debugColour));
		logMessage(m);
	}
}
///////////////////////////////////////////////////////////
const char* CncTraceLogStream::getCurrentMessage() const {
///////////////////////////////////////////////////////////
	if ( getTextControl() == NULL)
		return "";
		
	return getTextControl()->GetValue();
}


///////////////////////////////////////////////////////////
void CncMsgLogStream::logMessage(const char* m) {
///////////////////////////////////////////////////////////
	if ( m == NULL )
		return;
		
	if ( getTextControl() != NULL )
		getTextControl()->AppendText(m);
}

