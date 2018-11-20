#ifdef __WXMSW__
	#define CNC_GCODE_LEXER
#endif

#include <wx/wfstream.h>
#include <wx/datstrm.h>
#include <wx/txtstrm.h>
#include "OSD/CncAsyncKeyboardState.h"
#include "SvgEditPopup.h"
#include "MainFrame.h"
#include "GCodeCommands.h"
#include "CncConfig.h"
#include "CncSourceEditor.h"

// ----------------------------------------------------------------------------
// CncSourceEditor Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncSourceEditor, wxStyledTextCtrl)
	//EVT_STC_MARGINCLICK(CncSourceEditor::onMarginClick)
	//EVT_STC_CHNAGE(CncSourceEditor::onChange)
	EVT_KEY_DOWN(CncSourceEditor::onKeyDown)
	EVT_KEY_UP(CncSourceEditor::onKeyUp)
	EVT_LEFT_DOWN(CncSourceEditor::onLeftDown)
	EVT_LEFT_UP(CncSourceEditor::onLeftUp)
	EVT_LEFT_DCLICK(CncSourceEditor::onLeftDClick)
	EVT_RIGHT_DOWN(CncSourceEditor::onRightDown)
wxEND_EVENT_TABLE()


///////////////////////////////////////////////////////////////////
CncSourceEditor::CncSourceEditor(wxWindow *parent) 
: wxStyledTextCtrl(parent)
, styles()
, fileInfo()
///////////////////////////////////////////////////////////////////
{
	setupStyle();
}
///////////////////////////////////////////////////////////////////
CncSourceEditor::~CncSourceEditor() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onMarginClick(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onChange(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onKeyDown(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	onUpdateFilePosition();
	
	// update tab label
	wxString name(THE_APP->GetTemplateNotebook()->GetPageText(TemplateBookSelection::VAL::SOURCE_PANEL));
	if ( name.StartsWith("*") == false ) {
		name.Prepend("*");
		THE_APP->GetTemplateNotebook()->SetPageText(TemplateBookSelection::VAL::SOURCE_PANEL, name);
	}
	
	bool shtKey = CncAsyncKeyboardState::isShiftPressed();
	bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	int c       = event.GetUnicodeKey();
	
	wxString find(GetSelectedText());
	
	// run
	if      ( c == 'R' && ctlKey == true && shtKey == true) {
		wxCommandEvent dummy;
		THE_APP->rcRun(dummy);
		
	// debug
	} if      ( c == 'D' && ctlKey == true && shtKey == true) {
		wxCommandEvent dummy;
		THE_APP->rcDebug(dummy);
		
	// find
	} else if ( c == 'F' && ctlKey == true ) {
		event.Skip(false);
		
		if ( find.IsEmpty() == false ) 
			THE_APP->GetSourceEditSearch()->SetValue(find);
			
		THE_APP->GetSourceEditSearch()->SetFocus();
		return;
		
	// goto line
	} else if ( c == 'G' && ctlKey == true ) {
		
		wxTextEntryDialog dlg(this, "Line Number:", "Go to line . . .", "");
		dlg.SetMaxLength(8);
		dlg.SetTextValidator(wxFILTER_DIGITS);
		
		if ( dlg.ShowModal() == wxID_OK  ) {
			
			wxString s = dlg.GetValue();
			s.Trim(true).Trim(false);
			if ( s.IsEmpty() == false ) {
				long ln; s.ToLong(&ln);
				
				if ( ln >= 0 && ln <= GetNumberOfLines() )	GotoLine(ln-1);
				else										std::clog << "Source Editor: Invalid line numer: " << ln << std::endl;
			}
		}
		
	// Undo
	} else if ( c == 'Z' && ctlKey == true ) {
		Undo();
	
	// Redo
	} else if ( c == 'Y' && ctlKey == true ) {
		Redo();
		
	// save
	} else if ( c == 'S' && ctlKey == true ) {
		THE_APP->saveFile();
		
	// goto home
	} else if ( c == WXK_HOME && ctlKey == true ) {
		GotoLine(0);
		Home();
		
	// goto end
	} else if ( c == WXK_END && ctlKey == true ) {
		GotoLine(GetLineCount() - 1);
		LineEnd();
		
	// select cur pos to home
	} else if ( c == WXK_HOME && ctlKey == true && shtKey == true ) {
		SetSelection(GetCurrentPos(), 0);
		
	// select cur pos to end
	} else if ( c == WXK_HOME && ctlKey == true && shtKey == true ) {
		SetSelection(GetCurrentPos(), GetLastPosition());
	}
	
	event.Skip(true);

}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onKeyUp(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	onUpdateFilePosition();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
	onUpdateFilePosition();
	
	/*
	#warning
	wxASSERT(pathGenerator);
	pathGenerator->updateEditControlCanReplaceState(true);
	*/
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
	onUpdateFilePosition();
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onLeftDClick(wxMouseEvent& event)  {
///////////////////////////////////////////////////////////////////
	#warning support decodeSvgFragment again
	//decodeSvgFragment(event, sourceEditor)
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onRightDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	#warning support svg popup again
	/*
	if ( getCurrentTemplateFormat() != TplSvg )
		return;
	
	// Show popupmenu at position
	if ( stcFileContentPopupMenu != NULL ) {
		SvgEditPopup::enablePathGeneratorMenuItem(stcFileContentPopupMenu);
		sourceEditor->PopupMenu(stcFileContentPopupMenu);
	}
	 */
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::onUpdateFilePosition() {
///////////////////////////////////////////////////////////////////
	long x, y;
	PositionToXY(GetInsertionPoint(), &x, &y);
	
	wxString label(wxString::Format("Column: %ld", x + 1));
	THE_APP->GetFilePosition()->SetLabel(label);
	
	// try to select current  line as client id
	THE_APP->updateFileContentPosition(x, y);
	
	// display gcode help hint
	THE_APP->GetSourceEditStatus()->SetValue("");
	
	if ( fileInfo.format == TplGcode ) {
		int col = GetColumn(GetCurrentPos());
		wxString cl = GetLine(y);
		
		// find first blank on left
		for ( int i = col - 1; i >= 0; i-- ) {
			if ( cl[i] == ' ' || cl[i] == '\t' ) {
				cl = cl.Mid(i+1);
				break;
			}
		}
		
		THE_APP->GetSourceEditStatus()->SetValue(GCodeCommands::getGCodeHelpHint(cl));
	}
	
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::Enable(bool enable) {
///////////////////////////////////////////////////////////////////
	if ( fileInfo.format == TplBinary ) 	SetReadOnly(true);
	else									SetReadOnly(!enable);
	
	return enable;
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::gotoBegin() {
///////////////////////////////////////////////////////////////////
	GotoPos(0);
	SetSelection(0, 0);
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::gotoEnd() {
///////////////////////////////////////////////////////////////////
	#warning implement CncSourceEditor::gotoEnd()
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::selectLineNumber(unsigned long ln, const char* searchKey) {
///////////////////////////////////////////////////////////////////
	switch ( fileInfo.format ) {
		case TplSvg:		return selectLineSvg(ln, searchKey);
		case TplBinary:		return selectLineBinary(ln);
		default:			;
	}
	
	return selectLineDefault(ln);
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::selectLineDefault(unsigned long ln) {
///////////////////////////////////////////////////////////////////
	const int total = GetNumberOfLines() - 1;
	if ( total <= 0 )
		return false;
		
	if ( ln > (unsigned long)total )
		return false;
	
	GotoLine(ln);
	
	if ( ln == 0 ) {
		SetSelectionStart(0);
		SetSelectionEnd(0);
	} else {
		SetSelectionStart(GetCurrentPos());
		SetSelectionEnd(GetLineEndPosition(ln));
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::selectLineSvg(unsigned long ln, const char* searchKey) {
///////////////////////////////////////////////////////////////////
	wxString currentNodeName;
	if ( searchKey != NULL )
		currentNodeName.assign(searchKey);
	
	// debug only
	bool debug = false;
	if ( debug ) {
		std::clog << ln << ": " 	 << currentNodeName << std::endl;
		std::clog << GetCurrentPos() << std::endl;
		std::clog << GetLine(ln) 	 << std::endl;
	}
	
	bool ok = false;
	
	long backStep = 1;
	wxString searchStart(currentNodeName);
	
	// only if the currentNodeName exists use it as serach start point
	// this is only the case if the parser runs preprocess() or spool()
	if ( GetLine(ln).Contains(currentNodeName) == false ) {
		backStep = 0;
		searchStart.assign("<");
	}
	
	// sets the position to the start of the given line
	GotoLine(ln);
	Home();
	SearchAnchor();
	
	// find start
	long sp = SearchNext(0, searchStart);
	long ep = wxNOT_FOUND;
	
	if ( sp != wxNOT_FOUND ) {
		SetCurrentPos(sp);
		SearchAnchor();
		
		// find end
		ep = SearchNext(0, ">");
		
		if ( ep != wxNOT_FOUND ) {
			// make the end visible
			GotoPos(ep);
			// select
			SetSelection(sp - backStep, ep + 1);
			ok = true;
		}
	}
	
	// debug only
	if ( debug )
		std::clog << sp << ", " << ep << "\n";
	
	// on error use the default handling
	if ( ok == false )
		return selectLineDefault(ln);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::selectLineBinary(unsigned long ln) {
///////////////////////////////////////////////////////////////////
	auto it = fileInfo.lineNumberTranslater.find(ln);
	if ( it != fileInfo.lineNumberTranslater.end() )
		return selectLineDefault(it->second);
	
	return selectLineDefault(ln);
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::setupDefaultStyle() {
///////////////////////////////////////////////////////////////////
	// Reset all sytles
	StyleClearAll();
	
	// setup black background as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		StyleSetBackground (i, styles.clDefaultBck);
	}
	
	// setup gray foreground as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		StyleSetForeground (i, styles.clDefaultFgd);
	}
	
	// setup default font
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		StyleSetFont(i, styles.defaultFont);
	}

	// Reset folding
	SetProperty(wxT("xml.auto.close.tags"), 		wxT("0"));
	SetProperty(wxT("lexer.xml.allow.scripts"), 	wxT("0"));
	SetProperty(wxT("fold"), 						wxT("0"));
	SetProperty(wxT("fold.comment"),				wxT("0"));
	SetProperty(wxT("fold.compact"), 				wxT("0"));
	SetProperty(wxT("fold.preprocessor"), 			wxT("0"));
	SetProperty(wxT("fold.html"), 					wxT("0"));
	SetProperty(wxT("fold.html.preprocessor"), 	wxT("0"));
	
	SetMarginMask(MARGIN_FOLD, 			wxSTC_MASK_FOLDERS);
	SetMarginWidth(MARGIN_FOLD, 		0);
	SetMarginSensitive(MARGIN_FOLD, 	false);
	SetFoldMarginColour(true, 			wxColour(73, 73, 73));
	SetFoldMarginHiColour(true, 		*wxBLACK);
	
	// Set default styles 
	StyleSetForeground(TE_DEFAULT_STYLE, 		styles.clDefaultFgd);
	StyleSetBackground(TE_DEFAULT_STYLE, 		wxColour(73, 73, 73));
	StyleSetBackground(TE_BREAKPOINT_STYLE, 	wxColour(128, 0, 0));
	StyleSetForeground(TE_BREAKPOINT_STYLE, 	*wxWHITE);
	StyleSetBackground(TE_LINE_SAVED_STYLE, 	wxColour(wxT("FOREST GREEN")));
	StyleSetBackground(TE_LINE_MODIFIED_STYLE, 	wxColour(wxT("ORANGE")));
	StyleSetForeground(wxSTC_STYLE_LINENUMBER, 	styles.clDefaultFgd);
	StyleSetBackground(wxSTC_STYLE_LINENUMBER, 	wxColour(73, 73, 73));
	SetTabWidth(4);
	SetWrapMode(wxSTC_WRAP_NONE);
	SetReadOnly(false);
	
	// Enable line numbers
	SetMarginWidth(MARGIN_LINE_NUMBERS, 		35);
	SetMarginType(MARGIN_LINE_NUMBERS, 			wxSTC_MARGIN_NUMBER);
	SetMarginSensitive(MARGIN_LINE_NUMBERS, 	true);
	
	// Enable breakpoint
	SetMarginWidth(MARGIN_BREAKPOINT, 			8);
	SetMarginType(MARGIN_BREAKPOINT, 			wxSTC_MARGIN_TEXT);
	SetMarginMask(MARGIN_BREAKPOINT, 			0);
	SetMarginSensitive(MARGIN_BREAKPOINT, 		true);
	
	// Enable edit style - file content marker
	SetMarginWidth(MARGIN_EDIT_TRACKER, 		3);
	SetMarginType(MARGIN_EDIT_TRACKER, 			wxSTC_MARGIN_SYMBOL); 
	SetMarginMask(MARGIN_EDIT_TRACKER, 			0);
	
	// Configure caret style
	SetCaretForeground(styles.clCaretFgd);
	SetSelBackground(true, 						styles.clCaretBck);
	
	// Configure selection colours
	//ctl->SetSelForeground(true, wxColour(255,201,14));
	SetSelBackground(true, 						wxColour(83,83,83));
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::setupStyle() {
	switch ( fileInfo.format ) {
		case TplSvg:	setupSvgStyle();		break;
		case TplGcode:	setupGcodeStyle();		break;
		case TplBinary:	setupBinaryStyle();		break;
		
		default: 		setupDefaultStyle();
	}
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::setupSvgStyle() {
///////////////////////////////////////////////////////////////////
	setupDefaultStyle();
	
	SetLexer(wxSTC_LEX_HTML);
	
	// setup highlight colours
	StyleSetForeground(wxSTC_H_DOUBLESTRING,		wxColour(255, 	205, 	139));
	StyleSetForeground(wxSTC_H_SINGLESTRING,		wxColour(255,	205, 	139));
	StyleSetForeground(wxSTC_H_ENTITY,				wxColour(255,	0, 		0));
	StyleSetForeground(wxSTC_H_TAGUNKNOWN,			wxColour(0,		150, 	0));
	StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN,	wxColour(0,		0, 		150));
	StyleSetForeground(wxSTC_H_ATTRIBUTE,			styles.clAttribute);
	StyleSetForeground(wxSTC_H_TAG,					styles.clIdentifier);
	StyleSetForeground(wxSTC_H_COMMENT,				styles.clComment);
	
	// setup folding
	SetProperty(wxT("xml.auto.close.tags"), 		wxT("1"));
	SetProperty(wxT("lexer.xml.allow.scripts"), 	wxT("1"));
	SetProperty(wxT("fold"), 						wxT("1"));
	SetProperty(wxT("fold.comment"),				wxT("1"));
	SetProperty(wxT("fold.compact"), 				wxT("1"));
	SetProperty(wxT("fold.preprocessor"), 			wxT("1"));
	SetProperty(wxT("fold.html"), 					wxT("1"));
	SetProperty(wxT("fold.html.preprocessor"), 	wxT("1"));
	
	MarkerDefine(wxSTC_MARKNUM_FOLDER,        		wxSTC_MARK_BOXPLUS, 			styles.clDefaultBck, styles.clDefaultFgd);
	MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    		wxSTC_MARK_BOXMINUS, 			styles.clDefaultBck, styles.clDefaultFgd);
	MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,    		wxSTC_MARK_VLINE,    			styles.clDefaultBck, styles.clDefaultFgd);
	MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     		wxSTC_MARK_BOXPLUSCONNECTED,	styles.clDefaultBck, styles.clDefaultFgd);
	MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, 		wxSTC_MARK_BOXMINUSCONNECTED, 	styles.clDefaultBck, styles.clDefaultFgd);
	MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL,		wxSTC_MARK_TCORNER,     		styles.clDefaultBck, styles.clDefaultFgd);
	MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    		wxSTC_MARK_LCORNER,     		styles.clDefaultBck, styles.clDefaultFgd);

	SetMarginMask(MARGIN_FOLD, 						wxSTC_MASK_FOLDERS);
	SetMarginWidth(MARGIN_FOLD, 					32);
	SetMarginSensitive(MARGIN_FOLD, 				true);
	
	SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED | 16);
	
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::setupGcodeStyle() {
///////////////////////////////////////////////////////////////////
	setupDefaultStyle();
	
	#ifdef CNC_GCODE_LEXER
		SetLexer(wxSTC_LEX_GCODE);
		
		// setup highlight colours
		StyleSetForeground (wxSTC_GCODE_OPERATOR,		styles.clOperator);
		StyleSetForeground (wxSTC_GCODE_NUMBER,			styles.clNumber);
		StyleSetForeground (wxSTC_GCODE_IDENTIFIER,		styles.clIdentifier);
		StyleSetForeground (wxSTC_GCODE_PARAM,			styles.clAttribute);
		StyleSetForeground (wxSTC_GCODE_COMMENT,		styles.clComment);
		StyleSetForeground (wxSTC_GCODE_COMMENT_LINE,	styles.clComment);
		StyleSetForeground (wxSTC_GCODE_DIRECTIVE,		styles.clComment);
		
		wxFont font;
		font = (StyleGetFont(wxSTC_GCODE_IDENTIFIER)).Bold();
		StyleSetFont(wxSTC_GCODE_IDENTIFIER, font);
		
		font = (StyleGetFont(wxSTC_GCODE_PARAM)).Bold();
		StyleSetFont(wxSTC_GCODE_PARAM, font);
		
	#endif
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::setupBinaryStyle() {
///////////////////////////////////////////////////////////////////
	setupDefaultStyle();
	
	SetLexer(wxSTC_LEX_CPP);
			
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++)
		StyleSetFont(i, styles.staticFont);
		
	StyleSetForeground(wxSTC_C_COMMENT, 		styles.clComment);
	StyleSetForeground(wxSTC_C_COMMENTLINE, 	styles.clComment);
	StyleSetForeground(wxSTC_C_COMMENTDOC, 		styles.clComment);
	StyleSetFont(wxSTC_C_COMMENT, 				styles.defaultFont);
	StyleSetFont(wxSTC_C_COMMENTLINE, 			styles.defaultFont);
	StyleSetFont(wxSTC_C_COMMENTDOC, 			styles.defaultFont);
	
	StyleSetForeground(wxSTC_C_NUMBER, 			styles.clNumber);
	StyleSetFont(wxSTC_C_NUMBER, 				styles.staticFont);
	
	StyleSetForeground(wxSTC_C_NUMBER, 			styles.clIdentifier);
	StyleSetFont(wxSTC_C_IDENTIFIER, 			styles.defaultFont);
	StyleSetForeground(wxSTC_C_CHARACTER,		wxColour(255, 255, 128));
	
	wxFont fontWord(styles.defaultFont);
	fontWord.MakeBold();

	wxFont fontWord2(styles.defaultFont);
	fontWord2.MakeBold();
	
	StyleSetForeground(wxSTC_C_WORD,			wxColour(155, 185, 151));
	StyleSetFont(wxSTC_C_WORD,		 			fontWord);
	
	StyleSetForeground(wxSTC_C_WORD2,			wxColour(112, 146, 190));
	StyleSetFont(wxSTC_C_WORD2,		 			fontWord2);
	
	SetKeyWords(0, "cnc size");
	SetKeyWords(1, "int exec");
	
	SetReadOnly(true);
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::prepareNewTemplateFile(TemplateFormat tf) {
///////////////////////////////////////////////////////////////////
	ClearAll();
	
	if ( tf == TplSvg ) {
		AppendText("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\r\n");
		AppendText("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"\r\n");
		AppendText("\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\r\n");
		AppendText("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100mm\" height=\"100mm\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\r\n");
		AppendText("<title>CNC Woodworking standard template</title>\r\n");
		AppendText("<desc>....</desc>\r\n");
		
		AppendText(SvgNodeTemplates::getSamplesAsString());
	
		AppendText("\r\n");
		AppendText("</svg>\r\n");
		
	} else if ( tf == TplGcode ) {
		AppendText("(<1: Programm start>)\n");
		AppendText("G17 G40\n");
		AppendText("G80\n");
		AppendText("G90\n");
		
		AppendText("T01 M6\n");
		AppendText("S0 F700 M3\n");
		AppendText("G43 H07\n");
		AppendText("\n\n\n");
		AppendText("M2\n");
		
	} else {
		//Curently do nothing
	}
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::setupModelType() {
///////////////////////////////////////////////////////////////////
	TemplateFormat tf = fileInfo.format;
	if ( tf == TplBinary )
		tf = fileInfo.binaryOrigFomat;
	
	switch ( tf ) {
		case TplSvg:		fileInfo.modelType = GLContextBase::ModelType::MT_LEFT_HAND; 
							break;
							
		default:			fileInfo.modelType = GLContextBase::ModelType::MT_RIGHT_HAND; 
	}
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::changeBinaryViewType(const BinaryFileParser::ViewType bvt) {
///////////////////////////////////////////////////////////////////
	fileInfo.binaryViewType = bvt;
	
	if ( fileInfo.format != TplBinary )
		return;
	
	if ( openBinaryFile() == false )
		std::cerr << "CncSourceEditor::changeBinaryViewType(): Error while changing view type" << std::endl;
}
///////////////////////////////////////////////////////////////////
TemplateFormat CncSourceEditor::evaluateTemplateFromExtention(const wxString& ext) {
///////////////////////////////////////////////////////////////////
	wxString e(ext);
	e.MakeUpper();
	
	if      ( e == "SVG" )		return TplSvg;
	else if ( e == "GCODE") 	return TplGcode;
	else if ( e == "NGC") 		return TplGcode;
	else if ( e == "BCT") 		return TplBinary;
	
	return TplUnknown;
}
///////////////////////////////////////////////////////////////////
TemplateFormat CncSourceEditor::evaluateTemplateFormatFromFileName(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxFileName fn(fileName);
	wxString ext(fn.GetExt());
	
	return evaluateTemplateFromExtention(ext);
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::openFile(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	if ( wxFileName::Exists(fileName) == false ) {
		std::cerr << "CncSourceEditor::openFile(): File '" 
		          << fileName
		          << "' not found!" << std::endl;
		return false;
	}
	
	fileInfo.fileName.Assign(fileName);
	fileInfo.format = CncSourceEditor::evaluateTemplateFormatFromFileName(fileName);
	
	bool ret = false;
	switch ( fileInfo.format ) {
		case TplSvg:		ret = openTextFile();		break;
		case TplGcode:		ret = openTextFile();		break;
		case TplBinary:		ret = openBinaryFile();		break;
		default: 			ret = false;
	}
	
	if ( ret == true ) {
		setupModelType();
		setupStyle();
		DiscardEdits();
		EmptyUndoBuffer();
		
		return true;
	}
	
	fileInfo.reset();
	std::cerr << "CncSourceEditor::openFile(): Not registered template format: '" 
	          << cnc::getTemplateFormatAsString(fileInfo.format)
	          << "'" << std::endl;
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::openTextFile() {
///////////////////////////////////////////////////////////////////
	wxFileInputStream input(fileInfo.fileName.GetFullPath());
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	
	#define RETURN(val) \
		SetReadOnly(false); \
		return(val);

	if ( input.IsOk() ) {
		SetReadOnly(false);
		ClearAll();
		
		while( input.IsOk() && !input.Eof() ) {
			wxString line=text.ReadLine();
			AppendText(line);
			AppendText(_T("\r\n"));
		}
		
		RETURN(true);
	}
	
	std::cerr << "CncSourceEditor::openTextFile(): Error while open file: '" 
	          << fileInfo.fileName.GetFullPath()
	          << "'" << std::endl;
	
	RETURN(false);
	#undef RETURN
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::openBinaryFile() {
///////////////////////////////////////////////////////////////////
	SetReadOnly(false);
	ClearAll();
	
	#define RETURN(val) \
		SetReadOnly(true); \
		return(val);
	
	BinaryFileParser::ViewInfo vi;
	if ( BinaryFileParser::extractViewInfo(fileInfo.binaryViewType, fileInfo.fileName.GetFullPath(), vi) == false ) {
		
		std::cerr << "CncSourceEditor::openBinaryFile(): Error while open file: '" 
				  << fileInfo.fileName.GetFullPath()
				  << "'" << std::endl;
				
		RETURN(false);
	}
	
	fileInfo.binaryOrigFomat 		= CncSourceEditor::evaluateTemplateFromExtention(vi.type);
	fileInfo.lineNumberTranslater	= vi.lineNumberTranslater;
	
	AppendText(vi.content);
	DiscardEdits();

	RETURN(true);
	#undef RETURN
}
