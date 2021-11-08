#include "CncSerialSpyListCtrl.h"
#include "CncCommon.h"
#include "CncTextCtrl.h"
#include "CncLoggerProxy.h"
#include "CncLoggerView.h"
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
	bool trace(CncLoggerView* ctl) {
		if ( ctl == NULL )
			return false;
		
		// clear
		ctl->clear(LoggerSelection::VAL::STARTUP);
		
		bool containsLogOrErr = false;
		
		//wxTextAttr ta(ctl->GetDefaultStyle());
		wxTextAttr ta;
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
				ctl->changeTextAttr(LoggerSelection::VAL::STARTUP, ta);
				ctl->add(LoggerSelection::VAL::STARTUP, (char)(LoggerStreamBuf::startupBuffer[i]));
			}
		}
		
		ctl->setErrorFlag(LoggerSelection::VAL::STARTUP, containsLogOrErr);
		return containsLogOrErr;
	}
};

///////////////////////////////////////////////////////////
CncCoutBuf::CncCoutBuf(CncTextCtrl* c) 
: LoggerStreamBuf(LoggerStreamBuf::Type::STD, c, wxTextAttr(wxColour(232, 232, 232)))
///////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////
CncCoutBuf::~CncCoutBuf() {
///////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////
CncClogBuf::CncClogBuf(CncTextCtrl* c) 
: LoggerStreamBuf(LoggerStreamBuf::Type::LOG, c, wxTextAttr(wxColour(0, 157, 157))) 
///////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////
CncClogBuf::~CncClogBuf() {
///////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////
CncCerrBuf::CncCerrBuf(CncTextCtrl* c) 
: LoggerStreamBuf(LoggerStreamBuf::Type::ERR, c, wxTextAttr(wxColour(255, 64, 64)))
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncCerrBuf::~CncCerrBuf() {
///////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////
CncCex1Buf::CncCex1Buf(CncTextCtrl* c) 
: LoggerStreamBuf(LoggerStreamBuf::Type::EX1, c, wxTextAttr(wxColour(255, 201, 14)))
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncCex1Buf::~CncCex1Buf() {
///////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////
CncCex2Buf::CncCex2Buf(CncTextCtrl* c) 
: LoggerStreamBuf(LoggerStreamBuf::Type::EX2, c, wxTextAttr(wxColour(255, 101, 14)))
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncCex2Buf:: ~CncCex2Buf() {
///////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////
CncCex3Buf::CncCex3Buf(CncTextCtrl* c) 
: LoggerStreamBuf(LoggerStreamBuf::Type::EX3, c, wxTextAttr(wxColour(255,  11, 14)))
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncCex3Buf::~CncCex3Buf() {
///////////////////////////////////////////////////////////////////
}


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
	textAttr.Apply(ta);
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
void CncCspyBuf::addLine(const wxString& line, const wxString& appendix, int type) {
///////////////////////////////////////////////////////////
	CncSerialSpyListCtrl* c = static_cast<CncSerialSpyListCtrl*>(listCtrl);
	if ( c == NULL )
		return;
		
	c->addLine(line, appendix, (CncSerialSpyListCtrl::LineType)type);
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
	
	c->addLine("RET_OK", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultOk);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_MORE(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("RET_MORE", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultMore);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_ERROR(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("RET_ERROR", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultError);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_LIMIT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("RET_LIMIT", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultLimit);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_INTERRUPT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("RET_INTERRUPT", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultInterrupt);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_HALT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("RET_HALT", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultHalt);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::finalizeRET_QUIT(const char* msg) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("RET_QUIT", ( msg != NULL ? msg : "" ), CncSerialSpyListCtrl::LineType::LT_ResultQuit);
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
void CncSerialSpyStream::addDebugEntry(const wxString& mt) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
		
	c->addLine(mt, CncSerialSpyListCtrl::LineType::LT_DebugEntry);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::enableMessage(const char* additional) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("Serial Spy enabled . . .", ( additional != NULL ? additional : "" ), CncSerialSpyListCtrl::LineType::LT_Enable);
}
///////////////////////////////////////////////////////////
void CncSerialSpyStream::disableMessage(const char* additional) {
///////////////////////////////////////////////////////////
	CncCspyBuf* c = static_cast<CncCspyBuf*>(logStreamBuffer);
	if ( c == NULL )
		return;
	
	c->addLine("Serial Spy disabled . . .", ( additional != NULL ? additional : "" ), CncSerialSpyListCtrl::LineType::LT_Disable);
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


//////////////////////////////////////////////////////////
void CncBasicLogStream::ungregisterTextControl() {
//////////////////////////////////////////////////////////
	if ( logStreamBuffer != NULL )
		logStreamBuffer->ungregisterTextControl();
}
//////////////////////////////////////////////////////////
void CncBasicLogStream::setLogStreamBuffer(LoggerStreamBuf* lsb) {
//////////////////////////////////////////////////////////
	logStreamBuffer = lsb;
}
//////////////////////////////////////////////////////////
void CncBasicLogStream::resetTextAttr() {
//////////////////////////////////////////////////////////
	setTextAttr(LoggerStreamBuf::defaultAttr); 
}
//////////////////////////////////////////////////////////
void CncBasicLogStream::setTextAttr(const wxTextAttr& ta) {
//////////////////////////////////////////////////////////
	if ( logStreamBuffer != NULL )
		logStreamBuffer->setTextAttr(ta);
}
//////////////////////////////////////////////////////////
void CncBasicLogStream::setTextColour(const wxColour& c) {
//////////////////////////////////////////////////////////
	if ( logStreamBuffer != NULL )
		logStreamBuffer->setTextAttr(wxTextAttr(c));
}
//////////////////////////////////////////////////////////
const wxTextAttr& CncBasicLogStream::getTextAttr() const {
//////////////////////////////////////////////////////////
	if ( logStreamBuffer != NULL )
		return logStreamBuffer->getTextAttr();
		
	return LoggerStreamBuf::defaultAttr;
}
//////////////////////////////////////////////////////////
const wxColour& CncBasicLogStream::getTextColour() const { 
//////////////////////////////////////////////////////////
	if ( logStreamBuffer != NULL )
		return logStreamBuffer->getTextAttr().GetTextColour();
		
	return LoggerStreamBuf::defaultAttr.GetTextColour();
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
	CncTempTextAttr<CncTraceLogStream> ta(this, wxTextAttr(infoColour));
	logMessage(m);
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logWarningMessage(const char* m) {
///////////////////////////////////////////////////////////
	CncTempTextAttr<CncTraceLogStream> ta(this, wxTextAttr(warningColour));
	logMessage(m);
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logErrorMessage(const char* m) {
///////////////////////////////////////////////////////////
	CncTempTextAttr<LoggerStreamBuf> x(logStreamBuffer, wxTextAttr(errorColour));
	logMessage(m);
}
///////////////////////////////////////////////////////////
void CncTraceLogStream::logDebugMessage(const char* m) {
///////////////////////////////////////////////////////////
	CncTempTextAttr<CncTraceLogStream> ta(this, wxTextAttr(debugColour));
	logMessage(m);
}
///////////////////////////////////////////////////////////
const char CncTraceLogStream::getCurrentDesignAsChar() {
///////////////////////////////////////////////////////////
	const wxColour& c = getTextColour();
	
	if      ( c == warningColour )	return 'W';
	else if ( c == errorColour )	return 'E';
	else if ( c == debugColour )	return 'D';
	
	return 'I';
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

