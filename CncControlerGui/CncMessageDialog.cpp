#include <fstream>
#include <wx/window.h>
#include "CncMessageDialog.h"

/////////////////////////////////////////////////////////////////////
CncMessageDialog::CncMessageDialog(wxWindow* parent, const wxString& msg, wxString headline, wxString title) 
: CncMessageDialogBase(parent)
{
/////////////////////////////////////////////////////////////////////
	SetReturnCode(wxID_OK);
	
	setMessage(msg);
	setHeadline(headline);
	setTitle(title);
	
	m_btToggleWordWrap->SetValue(false) ;
	m_message->SetReadOnly(true);
	
	setupDefaultStyle();
}
/////////////////////////////////////////////////////////////////////
CncMessageDialog::~CncMessageDialog() {
/////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncMessageDialog::onInitDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	
}
/////////////////////////////////////////////////////////////////////
void CncMessageDialog::setTitle(const wxString& t) {
/////////////////////////////////////////////////////////////////////
	if ( t.IsEmpty() )
		return;
		
	SetTitle(t);
}
/////////////////////////////////////////////////////////////////////
void CncMessageDialog::setHeadline(const wxString& hl) {
/////////////////////////////////////////////////////////////////////
	if ( hl.IsEmpty() ) {
		m_headline->SetLabel("Info:");
		return;
	}

	m_headline->SetLabel(wxString::Format("%s:", hl));
}
/////////////////////////////////////////////////////////////////////
void CncMessageDialog::setMessage(const wxString& msg) {
/////////////////////////////////////////////////////////////////////
	m_message->SetReadOnly(false);
	m_message->ClearAll();
	m_message->AppendText(msg);
	m_message->SetReadOnly(true);
}
/////////////////////////////////////////////////////////////////////
void CncMessageDialog::close(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	Show(false);
}
/////////////////////////////////////////////////////////////////////
void CncMessageDialog::onWordWrap(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	m_message->SetWrapMode(m_btToggleWordWrap->GetValue() == false ? wxSTC_WRAP_NONE : wxSTC_WRAP_WORD);
}
/////////////////////////////////////////////////////////////////////
void CncMessageDialog::setWordWrap(bool state) {
/////////////////////////////////////////////////////////////////////
	m_btToggleWordWrap->SetValue(state);
	m_message->SetWrapMode(m_btToggleWordWrap->GetValue() == false ? wxSTC_WRAP_NONE : wxSTC_WRAP_WORD);
}
///////////////////////////////////////////////////////////////////
void CncMessageDialog::setupDefaultStyle() {
///////////////////////////////////////////////////////////////////
	// Reset all sytles
	m_message->StyleClearAll();
	
	// setup black background as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++)
		m_message->StyleSetBackground (i, wxColour(0, 0, 0));
	
	// setup gray foreground as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++)
		m_message->StyleSetForeground (i, wxColour(175, 175, 175));
	
	// setup default font
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++)
		m_message->StyleSetFont(i, font);

	// Reset folding
	m_message->SetProperty(wxT("xml.auto.close.tags"), 		wxT("0"));
	m_message->SetProperty(wxT("lexer.xml.allow.scripts"), 	wxT("0"));
	m_message->SetProperty(wxT("fold"), 					wxT("0"));
	m_message->SetProperty(wxT("fold.comment"),				wxT("0"));
	m_message->SetProperty(wxT("fold.compact"), 			wxT("0"));
	m_message->SetProperty(wxT("fold.preprocessor"), 		wxT("0"));
	m_message->SetProperty(wxT("fold.html"), 				wxT("0"));
	m_message->SetProperty(wxT("fold.html.preprocessor"), 	wxT("0"));
	
	m_message->SetWrapMode(m_btToggleWordWrap->GetValue() == false ? wxSTC_WRAP_NONE : wxSTC_WRAP_WORD);
	
	m_message->SetMarginMask(0, 			wxSTC_MASK_FOLDERS);
	m_message->SetMarginWidth(0, 			50);
	m_message->SetMarginSensitive(0, 		false);
	m_message->SetFoldMarginColour(true, 	wxColour(73, 73, 73));
	m_message->SetFoldMarginHiColour(true, 	*wxBLACK);
	
	// Configure caret style
	m_message->SetCaretForeground(wxColour(*wxWHITE));
	m_message->SetSelBackground(true, wxColour(64, 64, 64));
	
	// Configure selection colours
	//ctl->SetSelForeground(true, wxColour(255,201,14));
	m_message->SetSelBackground(true, wxColour(83,83,83));
}


/////////////////////////////////////////////////////////////////////
CncFileContentDialog::CncFileContentDialog(wxWindow* parent, const wxString& fileName, wxString headline, wxString title)
: CncMessageDialog(parent, "", headline, title)
{
/////////////////////////////////////////////////////////////////////
	setFileName(fileName);
}
/////////////////////////////////////////////////////////////////////
CncFileContentDialog::~CncFileContentDialog() {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncFileContentDialog::setFileName(const wxString& fileName) {
/////////////////////////////////////////////////////////////////////
	wxString msg;
	if ( wxFileName::Exists(fileName) == true ) {
		std::ifstream ifs (fileName.c_str().AsChar(), std::ifstream::in);
		char c = ifs.get();
		while ( ifs.good() ) {
			msg.append(c);
			c = ifs.get();
		}
		
		ifs.close();
	}
	
	setMessage(msg);
}
