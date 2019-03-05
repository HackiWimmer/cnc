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
#include "CncBaseEditor.h"

// ----------------------------------------------------------------------------
// CncBaseEditor Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncBaseEditor, wxStyledTextCtrl)
	EVT_STC_MARGINCLICK	(wxID_ANY, CncBaseEditor::onMarginClick)
	EVT_STC_CHANGE		(wxID_ANY, CncBaseEditor::onChange)
	EVT_KEY_DOWN		(CncBaseEditor::onKeyDown)
	EVT_KEY_UP			(CncBaseEditor::onKeyUp)
	EVT_LEFT_DOWN		(CncBaseEditor::onLeftDown)
	EVT_LEFT_UP			(CncBaseEditor::onLeftUp)
	EVT_LEFT_DCLICK		(CncBaseEditor::onLeftDClick)
	EVT_RIGHT_DOWN		(CncBaseEditor::onRightDown)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncBaseEditor::CncBaseEditor(wxWindow *parent) 
: wxStyledTextCtrl(parent)
, styles()
, flags()
, fileInfo()
, svgPopupMenu(NULL)
, ctlEditMode(NULL)
, ctlColunmPostion(NULL)
, ctlStatus(NULL)
, fileLoadingActive(false)
///////////////////////////////////////////////////////////////////
{
	setupStyle();
	Enable(hasEditMode());
}
///////////////////////////////////////////////////////////////////
CncBaseEditor::~CncBaseEditor() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onMarginClick(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	//folding
	if ( event.GetMargin() == MARGIN_FOLD ) {
		int lineClick  = LineFromPosition(event.GetPosition());
		int levelClick = GetFoldLevel(lineClick);
		
		if ( ( levelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0) {
			ToggleFold(lineClick);
		}
	}
	
	// break points
	if ( flags.handleBreakpoints == true ) {
		if ( event.GetMargin() == MARGIN_BREAKPOINT || event.GetMargin() == MARGIN_LINE_NUMBERS ) {
			int lineClick = LineFromPosition(event.GetPosition());

			if ( MarginGetText(lineClick) == "B" ) {
				MarginSetText(lineClick, wxT(" "));
				MarginSetStyle(lineClick, TE_DEFAULT_STYLE);
			} else {
				MarginSetText(lineClick, wxT("B"));
				MarginSetStyle(lineClick, TE_BREAKPOINT_STYLE);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onChange(wxStyledTextEvent& event) {
///////////////////////////////////////////////////////////////////
	// todo currently not tested
	event.Skip();
	
	bool isInsert = event.GetModificationType() & wxSTC_MOD_INSERTTEXT;
	bool isDelete = event.GetModificationType() & wxSTC_MOD_DELETETEXT;
	
	if ( isInsert || isDelete) {
		int numlines(event.GetLinesAdded());
		
		// ignore this event incase we are in the middle of file reloading
		if ( fileLoadingActive == false/*GetReloadingFile() == false && GetMarginWidth(EDIT_TRACKER_MARGIN_ID */) /* margin is visible */ {
			int curline(LineFromPosition(event.GetPosition()));
			
			if ( numlines == 0 ) {
				// probably only the current line was modified
				MarginSetText(curline, wxT(" "));
				MarginSetStyle(curline, TE_LINE_MODIFIED_STYLE);
			} else {
				for ( int i = 0; i <= numlines; i++ ) {
					MarginSetText(curline + i, wxT(" "));
					MarginSetStyle(curline + i, TE_LINE_MODIFIED_STYLE);
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onKeyDown(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	onUpdateFilePosition();
	
	if ( flags.handleKeyCommands == false )
		return;
	
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
void CncBaseEditor::onKeyUp(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	onUpdateFilePosition();
	
	// update tab label - on demand
	decorateParentTabName(IsModified());
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
	onUpdateFilePosition();
	
	/*
	wxASSERT(pathGenerator);
	pathGenerator->updateEditControlCanReplaceState(true);
	*/
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip(true);
	onUpdateFilePosition();
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onLeftDClick(wxMouseEvent& event)  {
///////////////////////////////////////////////////////////////////
	// currently nothing to do
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onRightDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( fileInfo.format == TplSvg ) {
		// Show popupmenu at position
		if ( svgPopupMenu != NULL ) {
			SvgEditPopup::enablePathGeneratorMenuItem(svgPopupMenu);
			PopupMenu(svgPopupMenu);
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::onUpdateFilePosition() {
///////////////////////////////////////////////////////////////////
	long x, y;
	PositionToXY(GetInsertionPoint(), &x, &y);
	
	wxString label(wxString::Format("Column: %ld", x + 1));
	if ( getCtlColumnPos() != NULL )
		getCtlColumnPos()->SetLabel(label);
	
	// try to select current line as client id
	THE_APP->updateFileContentPosition(x, y + 1);
	
	// display gcode help hint
	if ( getCtlStatus() != NULL )
		getCtlStatus()->SetValue("");
	
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
		
		if ( getCtlStatus() != NULL )
			getCtlStatus()->SetValue(GCodeCommands::getGCodeHelpHint(cl));
	}
	
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::Enable(bool enable) {
///////////////////////////////////////////////////////////////////
	if ( hasEditMode() == false ) {
		SetReadOnly(true);
		
	} else {
		
		if ( fileInfo.format == TplBinary ) 	SetReadOnly(true);
		else									SetReadOnly(!enable);
	
	}
	
	if ( getCtlEditMode() != NULL ) {
		if ( IsEditable() == true ) {
			getCtlEditMode()->SetForegroundColour(wxColour(0, 0, 0));
			getCtlEditMode()->SetLabel("Editable  ");
		} else {
			getCtlEditMode()->SetForegroundColour(wxColour(255, 64, 64));
			getCtlEditMode()->SetLabel("Read only ");
		}
	}
	
	setupStyle();
	
	return IsEditable();
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::gotoBegin() {
///////////////////////////////////////////////////////////////////
	GotoPos(0);
	SetSelection(0, 0);
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::gotoEnd() {
///////////////////////////////////////////////////////////////////
	GotoLine(GetNumberOfLines() - 1);
	LineEnd();
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::selectLineNumber(unsigned long ln, const char* searchKey) {
///////////////////////////////////////////////////////////////////
	switch ( fileInfo.format ) {
		case TplSvg:		return selectLineSvg(ln, searchKey);
		case TplBinary:		return selectLineBinary(ln);
		default:			;
	}
	
	return selectLineDefault(ln);
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::selectLineDefault(unsigned long ln) {
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
bool CncBaseEditor::selectLineSvg(unsigned long ln, const char* searchKey) {
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
	if ( currentNodeName.IsEmpty() || GetLine(ln).Contains(currentNodeName) == false ) {
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
bool CncBaseEditor::selectLineBinary(unsigned long ln) {
///////////////////////////////////////////////////////////////////
	auto it = fileInfo.lineNumberTranslater.find(ln);
	if ( it != fileInfo.lineNumberTranslater.end() )
		return selectLineDefault(it->second);
	
	return selectLineDefault(ln);
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::setupDefaultStyle() {
///////////////////////////////////////////////////////////////////
	// Reset all sytles
	StyleClearAll();
	
	// setup black background as default
	for ( unsigned int i=0; i<wxSTC_STYLE_MAX; i++) {
		if ( IsEditable() == true )	StyleSetBackground (i, styles.clDefaultBck);
		else						StyleSetBackground (i, styles.clDisabledBck);
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
void CncBaseEditor::setupStyle() {
	switch ( fileInfo.format ) {
		case TplText:	setupTextStyle();		break;
		case TplSvg:	setupSvgStyle();		break;
		case TplGcode:	setupGcodeStyle();		break;
		case TplBinary:	setupBinaryStyle();		break;
		
		default: 		setupDefaultStyle();
	}
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::setupTextStyle() {
///////////////////////////////////////////////////////////////////
	setupSvgStyle();
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::setupSvgStyle() {
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
void CncBaseEditor::setupGcodeStyle() {
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
void CncBaseEditor::setupBinaryStyle() {
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
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::clearContent() {
///////////////////////////////////////////////////////////////////
	Enable(true);
	ClearAll();
	Enable(hasEditMode());
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::prepareNewTemplateFile(TemplateFormat tf) {
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
void CncBaseEditor::setupModelType() {
///////////////////////////////////////////////////////////////////
	TemplateFormat tf = fileInfo.format;
	if ( tf == TplBinary )
		tf = fileInfo.binaryOrigFomat;
	
	#warning  GLContextBase::ModelType::MT_LEFT_HAND ???
	switch ( tf ) {
		case TplSvg:		fileInfo.modelType 		= GLContextBase::ModelType::MT_RIGHT_HAND;
							//fileInfo.modelType 		= GLContextBase::ModelType::MT_LEFT_HAND;
							fileInfo.modelViewType	= CncDimensions::CncDimension2D; 
							break;
							
		default:			fileInfo.modelType 		= GLContextBase::ModelType::MT_RIGHT_HAND; 
							fileInfo.modelViewType	= CncDimensions::CncDimension3D; 
	}
}
///////////////////////////////////////////////////////////////////
void CncBaseEditor::changeBinaryViewType(const BinaryFileParser::ViewType bvt) {
///////////////////////////////////////////////////////////////////
	fileInfo.binaryViewType = bvt;
	
	if ( fileInfo.format != TplBinary )
		return;
	
	if ( openBinaryFile() == false )
		std::cerr << "CncBaseEditor::changeBinaryViewType(): Error while changing view type" << std::endl;
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::openFile(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	if ( wxFileName::Exists(fileName) == false ) {
		std::cerr << "CncBaseEditor::openFile(): File '" 
		          << fileName
		          << "' not found!" << std::endl;
		return false;
	}
	
	fileInfo.fileName.Assign(fileName);
	fileInfo.format = cnc::getTemplateFormatFromFileName(fileName);
	
	bool ret = false;
	switch ( fileInfo.format ) {
		case TplText:		ret = openTextFile();		break;
		case TplSvg:		ret = openTextFile();		break;
		case TplGcode:		ret = openTextFile();		break;
		case TplBinary:		ret = openBinaryFile();		break;
		default: 			ret = false;
	}
	
	if ( ret == true ) {
		setupModelType();
		setupStyle();
		DiscardEdits();
		decorateParentTabName(false);
		EmptyUndoBuffer();
		
		return true;
	}
	
	fileInfo.reset();
	std::cerr << "CncBaseEditor::openFile(): Not registered template format: '" 
	          << cnc::getTemplateFormatAsString(fileInfo.format)
	          << "'" << std::endl;
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::openTextFile() {
///////////////////////////////////////////////////////////////////
	#define RETURN(val) \
		Enable(hasEditMode()); \
		fileLoadingActive = false; \
		return(val);
	
	fileLoadingActive = true;
	
	//Enable(true);
	SetReadOnly(false);
	
	if ( LoadFile(fileInfo.fileName.GetFullPath()) == true ) 
		RETURN(true);
		
	std::cerr << "CncBaseEditor::openTextFile(): Error while open file: '" 
	          << fileInfo.fileName.GetFullPath()
	          << "'" << std::endl;
	
	RETURN(false);
	#undef RETURN
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::openBinaryFile() {
///////////////////////////////////////////////////////////////////
	#define RETURN(val) \
		Enable(hasEditMode()); \
		fileLoadingActive = false; \
		return(val);

	fileLoadingActive = true;
	
	//Enable(true);
	SetReadOnly(false);
	ClearAll();
	
	BinaryFileParser::ViewInfo vi;
	if ( BinaryFileParser::extractViewInfo(fileInfo.binaryViewType, fileInfo.fileName.GetFullPath(), vi) == false ) {
		
		std::cerr << "CncBaseEditor::openBinaryFile(): Error while open file: '" 
				  << fileInfo.fileName.GetFullPath()
				  << "'" << std::endl;
				
		RETURN(false);
	}
	
	fileInfo.binaryOrigFomat 		= cnc::getTemplateFormatFromExtention(vi.type);
	fileInfo.lineNumberTranslater	= vi.lineNumberTranslater;
	
	AppendText(vi.viewContent);
	DiscardEdits();
	
	THE_APP->GetNestedDataHeader()->ChangeValue(vi.dataHeader);
	THE_APP->GetNestedSource()->ChangeValue(vi.sourceContent);

	RETURN(true);
	#undef RETURN
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::save() {
///////////////////////////////////////////////////////////////////
	if ( hasEditMode() == false )
		return false;

	bool ret = false;
	switch ( fileInfo.format ) {
		case TplSvg:		
		case TplGcode:		ret = SaveFile(fileInfo.fileName.GetFullPath()); 
							break;
							
		default: 			ret = false;
	}
	
	cnc::trc.logInfo(wxString::Format("File '%s' saved . . . ", fileInfo.fileName.GetFullPath()));
	decorateParentTabName(false);
	DiscardEdits();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::saveFile() {
///////////////////////////////////////////////////////////////////
	if ( IsModified() == false ) {
		cnc::trc.logInfo(wxString::Format("Nothing to do. File '%s' is unchanged. . . ", fileInfo.fileName.GetFullPath()));
		return true;
	}
		
	return save();
}
///////////////////////////////////////////////////////////////////
bool CncBaseEditor::saveFileAs(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxFileName fn(fileName);
	if ( fn.DirExists() == false ) {
		cnc::trc.logError(wxString::Format("Invalid directory for: '%s'", fileInfo.fileName.GetFullPath()));
		return false;
	}
	
	fileInfo.fileName.Assign(fileName);
	return save();
}
