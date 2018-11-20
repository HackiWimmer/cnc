#include "CncStreamBuffers.h"

wxTextAttr LoggerStreamBuf::defaultAttr = wxTextAttr(wxColour(195,195,195));
wxTextAttr CncCspyBuf::lineNumberAttr = wxTextAttr(wxColour(205,201,15));
const char* CncSerialSpyStream::hLine	= "----------------------------------------------\n";
const char* CncSerialSpyStream::mLine	= "***********************************************\n";

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