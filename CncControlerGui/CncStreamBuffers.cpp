#include "CncStreamBuffers.h"

wxString   		LoggerStreamBuf::startupBuffer 		= wxString("");
wxTextAttr  	LoggerStreamBuf::defaultAttr 		= wxTextAttr(wxColour(195, 195, 195));
wxTextAttr  	CncCspyBuf::lineNumberAttr 			= wxTextAttr(wxColour(205, 201, 15));
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
	bool trace(wxTextCtrl* ctl) {
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
LoggerStreamBuf::LoggerStreamBuf(char t, wxTextCtrl* c, const wxTextAttr& ta) 
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
void LoggerStreamBuf::setTextControl(wxTextCtrl* c) {
	wxASSERT(c);
	ctl = c;
}
///////////////////////////////////////////////////////////
int LoggerStreamBuf::overflow(int c) {
///////////////////////////////////////////////////////////
	if ( ctl != NULL ) {
		ctl->SetDefaultStyle(textAttr);
		ctl->AppendText((wxChar)c);
		
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
	if ( ctl == NULL )
		return 0; 
	
	// first insertion
	if ( ctl->GetLastPosition() == 0 )
		return insertLineNumber(true);
	
	// line feed
	if ( c == '\n' )
		return insertLineNumber(false);
	
	//ctl->SetDefaultStyle(textAttr);
	ctl->AppendText((wxChar)c);
	
	// return something different from EOF
	return 0;
}

///////////////////////////////////////////////////////////
int CncCspyBuf::insertLineNumber(bool first) {
///////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return 0; 

	ctl->SetDefaultStyle(CncCspyBuf::lineNumberAttr);
	
	if ( first == true ) 	ctl->AppendText(wxString::Format("%s%06d ", "",   (ctl->GetNumberOfLines())%100000));
	else 					ctl->AppendText(wxString::Format("%s%06d ", "\n", (ctl->GetNumberOfLines()+1)%100000));
	
	ctl->SetDefaultStyle(textAttr);
	
	return 0;
}