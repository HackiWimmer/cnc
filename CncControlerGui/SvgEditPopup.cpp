#include "SvgEditPopup.h"

#define svgPathGenItemString								"PGen - Insert last SVG pattern"
MainFrame* SvgEditPopup::_mainFrame							= NULL;
unsigned int SvgEditPopup::_idOffset 						= wxID_HIGHEST;

wxString SvgNodeTemplates::_ret								= _T("");
const char* SvgNodeTemplates::CncParameterBlockNodeName		= "CncParameterBlock";
const char* SvgNodeTemplates::CncBreakBlockNodeName			= "CncBreak";
const char* SvgNodeTemplates::CncPauseBlockNodeName			= "CncPause";

//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getSamplesAsString() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret << "\t<!--samples start /-->\n";
	_ret << "\t\t<!--line x1=\"10\" y1=\"0\" x2=\"20\" y2=\"0\" stroke=\"blue\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--rect x=\"5\" y=\"5\" width=\"10\" height=\"10\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--rect x=\"10\" y=\"10\" rx=\"5\" ry=\"5\" width=\"30\" height=\"30\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--polyline fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\" points=\"10,10 15,15 20,10 25,15\"/-->\n";
	_ret << "\t\t<!--polygon fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\" points=\"10 10 15 15 20 10\"/-->\n";
	_ret << "\t\t<!--circle cx=\"50\" cy=\"40\" r=\"20\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--ellipse cx=\"50\" cy=\"50\" rx=\"40\" ry=\"30\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--path d=\"M 20.5 20.5 l 10 0 0 10 -10 0z \" stroke=\"black\" fill=\"none\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--path d=\"M10,20 C10,10 25,10 25,20 S40,30 40,20\" stroke=\"black\" fill=\"none\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--path d=\"M40,40 Q40,5 60,30 T100,30\"  stroke=\"black\" fill=\"none\" stroke-width=\"0.5\" /-->\n";
	_ret << "\t\t<!--path d=\"M 10 20 a 5 5 0 1 0 10 0 a 5 5 0 1 0 -10 0 \" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t\t<!--path d=\"M 5,5\" stroke=\"black\" fill=\"none\" stroke-width=\"0.5\"/-->\n";
	_ret << "\t<!--samples end /-->\n";
	
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getCncParameterBlock() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret  = wxString::Format("<%s\n depth=\"z-0.0\"\n reverse=\"no\"\n correction=\"none\"/>\n", SvgNodeTemplates::CncParameterBlockNodeName);
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getCncBreakBlock() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret  = wxString::Format("<%s/>\n", SvgNodeTemplates::CncBreakBlockNodeName);
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getCncPauseBlock() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret  = wxString::Format("<%s/>\n", SvgNodeTemplates::CncPauseBlockNodeName);
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getCircleTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<circle cx=\"?\" cy=\"?\" r=\"?\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/>\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getEllipseTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<ellipse cx=\"?\" cy=\"?\" rx=\"?\" ry=\"?\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/>\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getRectTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<rect x=\"?\" y=\"?\" rx=\"?\" ry=\"?\" width=\"?\" height=\"?\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/>\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getLineTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<line x1=\"?\" y1=\"?\" x2=\"?\" y2=\"?\" stroke=\"blue\" stroke-width=\"0.5\"/>\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getPolygonTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<polygon points=\"?,? ?,? ....\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/>\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getPolylineTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<polyline points=\"?,? ?,? ....\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.5\"/>\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getPathTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<path d=\"M ?,? l ?,? z \" stroke=\"blue\" fill=\"none\" stroke-width=\"0.5\"/-->\n";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getEllipticalARCPattern() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = " A ?,? 0 1 0 ?,?";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getQuadraticBezierPattern() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = " Q?,? ?,?";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
const char* SvgNodeTemplates::getCubicBezierPattern() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = " C?,? ?,? ?,? ";
	return _ret.c_str();
}
//////////////////////////////////////////////////////////
void SvgEditPopup::enablePathGeneratorMenuItem(wxMenu* menu) {
//////////////////////////////////////////////////////////
	if ( menu == NULL )
		return;

	int id = menu->FindItem(svgPathGenItemString);
	if ( id == wxNOT_FOUND )
		return;
		
	wxMenuItem* item = menu->FindItem(id);
	if ( item == NULL )
		return;
		
	item->Enable(PathGeneratorFrame::getCurrentGeneratedPath().IsEmpty() == false );
}
//////////////////////////////////////////////////////////
void SvgEditPopup::overAllMenuItems(wxMenu* menu) {
//////////////////////////////////////////////////////////
	if ( menu == NULL )
		return;
		
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
		
		wxMenu* m = mi->GetSubMenu ();
		if ( m != NULL ) {
			overAllMenuItems(m);
		} else {
			//clog << mi->GetItemLabelText() << endl;
		}
	}
}
//////////////////////////////////////////////////////////
void SvgEditPopup::destroyMenu(wxMenu* popup) {
//////////////////////////////////////////////////////////
	if ( popup == NULL )
		return;
		
	overAllMenuItems(popup);
	delete popup;
}
//////////////////////////////////////////////////////////
wxMenu* SvgEditPopup::createMenu(wxStyledTextCtrl* ctl, wxMenu* popup, bool extended) {
//////////////////////////////////////////////////////////
	MainFrame* frame = _mainFrame;
	
	if ( frame == NULL )
		return NULL;
		
	if ( ctl == NULL )
		return NULL;
		
	if ( popup != NULL )
		delete popup;
		
	unsigned int idOffset = getNextIdOffset();
		
	popup = new wxMenu("SVG Tools . . . ");
	popup->Append(idOffset + STC_PM_COMMENT, 				wxT("Comment current SVG Node"));
	popup->Append(idOffset + STC_PM_UNCOMMENT, 				wxT("Uncomment current SVG Node"));
	popup->Append(idOffset + STC_PM_SELECT_NODE,			wxT("Select current SVG Node"));
	popup->Append(idOffset + STC_PM_SELECT_NODE_BLOCK,		wxT("Select current SVG Block"));
	popup->AppendSeparator();
	
	if ( extended == true ) {
		popup->Append(idOffset + STC_PM_PGEN_OPEN, 							wxT("PGen - Open Editor"));
		popup->Append(idOffset + STC_PM_PGEN_INSERT_CURRENT_SVG_FRAGMENT,	wxT(svgPathGenItemString));
		popup->Append(idOffset + STC_PM_PGEN_REGENERATE_CURRENT_SVG_BLOCK,	wxT("PGen - ReGenerate current SVG Pattern"));
		popup->Append(idOffset + STC_PM_PGEN_OPEN_WITH_CURRENT_SVG_BLOCK,	wxT("PGen - Open Editor with current SVG Pattern"));
		popup->AppendSeparator();
	}
	
	wxMenu* cncMenu = new wxMenu("CNC Pattern");
	popup->AppendSubMenu(cncMenu, "CNC Pattern . . .");
	
		cncMenu->Append(idOffset + STC_PM_CNC_PARAM_BLOCK, 		wxT("Insert CncParameterBlock"));
		cncMenu->Append(idOffset + STC_PM_CNC_BREAK_BLOCK, 		wxT("Insert CncBreak"));
		cncMenu->Append(idOffset + STC_PM_CNC_PAUSE_BLOCK, 		wxT("Insert CncPause"));
	
	wxMenu* svgMenu = new wxMenu("SVG Pattern");
	popup->AppendSubMenu(svgMenu, "SVG Pattern . . .");
	
		svgMenu->Append(idOffset + STC_PM_CIRCLE, 				wxT("Insert Circle Node"));
		svgMenu->Append(idOffset + STC_PM_ELLIPSE, 				wxT("Insert Ellipse Node"));
		svgMenu->Append(idOffset + STC_PM_LINE, 				wxT("Insert Line Node"));
		svgMenu->Append(idOffset + STC_PM_POLYGON, 				wxT("Insert Polygon Node"));
		svgMenu->Append(idOffset + STC_PM_POLYLINE, 			wxT("Insert Polyline Node"));
		svgMenu->Append(idOffset + STC_PM_RECT, 				wxT("Insert Rect Node"));
		svgMenu->Append(idOffset + STC_PM_PATH, 				wxT("Insert Path Node"));
		svgMenu->AppendSeparator();
		svgMenu->Append(idOffset + STC_PM_PATH_ELLIPTICALARC, 	wxT("Insert EllipticalARC Pattern"));
		svgMenu->Append(idOffset + STC_PM_PATH_QUADRATICBEZIER, wxT("Insert QuadraticBezier Pattern"));
		svgMenu->Append(idOffset + STC_PM_PATH_CUBICBEZIER, 	wxT("Insert CubicBezier Pattern"));
		
	popup->AppendSeparator();
	popup->Append(idOffset + STC_PM_COPY, 					wxT("Copy"));
	popup->Append(idOffset + STC_PM_PASTE, 					wxT("Paste"));
	popup->Append(idOffset + STC_PM_UNDO, 					wxT("Undo"));
	popup->Append(idOffset + STC_PM_REDO, 					wxT("Redo"));
	popup->AppendSeparator();
	popup->Append(idOffset + STC_PM_SELECT_ALL, 			wxT("Select All"));
	popup->AppendSeparator();
	popup->Append(idOffset + STC_PM_CUT, 					wxT("Cut"));
	popup->Append(idOffset + STC_PM_DELETE, 				wxT("Delete"));
	

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getCncParameterBlock());
	 }, idOffset + STC_PM_CNC_PARAM_BLOCK, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getCncBreakBlock());
	 }, idOffset + STC_PM_CNC_BREAK_BLOCK, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getCncPauseBlock());
	 }, idOffset + STC_PM_CNC_PAUSE_BLOCK, wxID_ANY, ctl);

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getCircleTemplate());
	 }, idOffset + STC_PM_CIRCLE, wxID_ANY, ctl);

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getEllipseTemplate());
	 }, idOffset + STC_PM_ELLIPSE, wxID_ANY, ctl);

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getLineTemplate());
	 }, idOffset + STC_PM_LINE, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getRectTemplate());
	 }, idOffset + STC_PM_RECT, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getPolygonTemplate());
	 }, idOffset + STC_PM_POLYGON, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getPolylineTemplate());
	 }, idOffset + STC_PM_POLYLINE, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getPathTemplate());
	 }, idOffset + STC_PM_PATH, wxID_ANY, ctl);

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getEllipticalARCPattern());
	 }, idOffset + STC_PM_PATH_ELLIPTICALARC, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getQuadraticBezierPattern());
	 }, idOffset + STC_PM_PATH_QUADRATICBEZIER, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getCubicBezierPattern());
	 }, idOffset + STC_PM_PATH_CUBICBEZIER, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->Copy();
	 }, idOffset + STC_PM_COPY, wxID_ANY, ctl);
	
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->Paste();
	 }, idOffset + STC_PM_PASTE, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->Undo();
	 }, idOffset + STC_PM_UNDO, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->Redo();
	 }, idOffset + STC_PM_REDO, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->SelectAll();
	 }, idOffset + STC_PM_SELECT_ALL, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->Cut();
	 }, idOffset + STC_PM_CUT, wxID_ANY, ctl);
	
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->DeleteBack();
	 }, idOffset + STC_PM_DELETE, wxID_ANY, ctl);

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			commentCurrentSvgNode(ctl);
	 }, idOffset + STC_PM_COMMENT, wxID_ANY, ctl);

	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			uncommentCurrentSvgNode(ctl);
	 }, idOffset + STC_PM_UNCOMMENT, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			selectCurrentSvgNode(ctl);
	 }, idOffset + STC_PM_SELECT_NODE, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			selectCurrentSvgNodeBlock(ctl);
	 }, idOffset + STC_PM_SELECT_NODE_BLOCK, wxID_ANY, ctl);
	 
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			ctl->ReplaceSelection(PathGeneratorFrame::getCurrentGeneratedPath());
			
	 }, idOffset + STC_PM_PGEN_INSERT_CURRENT_SVG_FRAGMENT, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			wxASSERT(SvgEditPopup::_mainFrame);
			
			if ( hasSelection(ctl) == false )
				selectCurrentSvgNodeBlock(ctl);
				
			wxString node(ctl->GetSelectedText());
			SvgEditPopup::_mainFrame->regenerateCurrentSvgNodeFromPopup(ctl, node);
			
	 }, idOffset + STC_PM_PGEN_REGENERATE_CURRENT_SVG_BLOCK, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			wxASSERT(SvgEditPopup::_mainFrame);
			
			SvgEditPopup::_mainFrame->openPathGen();
			
	 }, idOffset + STC_PM_PGEN_OPEN, wxID_ANY, ctl);
	 
	//............................................
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED,
	 [](wxCommandEvent& event) {
			wxStyledTextCtrl* ctl = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventUserData());
			wxASSERT(ctl);
			wxASSERT(SvgEditPopup::_mainFrame);
			
			if ( hasSelection(ctl) == false )
				selectCurrentSvgNodeBlock(ctl);
				
			wxString node(ctl->GetSelectedText());
			SvgEditPopup::_mainFrame->openPathGenWithCurrentSvgNodeFromPopup(ctl, node);
			
	 }, idOffset + STC_PM_PGEN_OPEN_WITH_CURRENT_SVG_BLOCK, wxID_ANY, ctl);
	 
	return popup;
}
//////////////////////////////////////////////////////////
bool SvgEditPopup::searchCurrentNode(wxStyledTextCtrl* ctl, EditSearchParam& parameter) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return false;
		
	int cp = ctl->GetCurrentPos();
		
	parameter.out.startPos	= -1;
	parameter.out.EndPos	= -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, parameter.in.searchStart);
	if ( ret != wxNOT_FOUND ) {
		parameter.out.startPos = ret;
		
		// find right >
		ctl->SearchAnchor();
		ret = ctl->SearchNext(0, parameter.in.searchEnd);
		if ( ret != wxNOT_FOUND ) {
			parameter.out.EndPos = ret;
			return true;
		}
	}
	
	cnc::trc.logError(wxString::Format("Cant find '%s' and/or '%s'!", parameter.in.searchStart, parameter.in.searchEnd));
	ctl->SetCurrentPos(cp);
	return false;
}
//////////////////////////////////////////////////////////
bool SvgEditPopup::getCurrentSvgNode(wxStyledTextCtrl* ctl, wxString& node) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return false;
		
	EditSearchParam esp;
	esp.in.searchStart.assign("<");
	esp.in.searchEnd.assign(">");
	
	if ( searchCurrentNode(ctl, esp) == true ) {
		node.assign(ctl->GetTextRange(esp.out.startPos, esp.out.EndPos));
		return true;
	}
	
	node.clear();
	return false;
}
//////////////////////////////////////////////////////////
bool SvgEditPopup::commentCurrentSvgNode(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return false;
		
	int cp = ctl->GetCurrentPos();
	int leftPos  = -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, "<");
	if ( ret != wxNOT_FOUND ) {
		ret++;
		wxString check = ctl->GetTextRange(ret, ret + 1);
		if ( check == "!" ) {
			cnc::trc.logError("Current node is already commented!");
			ctl->SetCurrentPos(cp);
			return false;
		}
		
		leftPos = ret;
		
		// find right >
		ctl->SearchAnchor();
		int ret = ctl->SearchNext(0, ">");
		if ( ret != wxNOT_FOUND ) {
			ctl->InsertText(ret, "--");
			ctl->InsertText(leftPos, "!--");
			return true;
		}
	}
	
	cnc::trc.logError("Cant find '<' and/or '>'!");
	ctl->SetCurrentPos(cp);
	return false;
}
//////////////////////////////////////////////////////////
bool SvgEditPopup::uncommentCurrentSvgNode(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return false;

	int cp = ctl->GetCurrentPos();
	int leftPos  = -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, "<");
	if ( ret != wxNOT_FOUND ) {
		ret++;
		wxString check = ctl->GetTextRange(ret, ret + 1);
		if ( check != "!" ) {
			cnc::trc.logError("Current node isn't commented!");
			ctl->SetCurrentPos(cp);
			return false;
		}
		
		leftPos = ret;
		
		// find right >
		ctl->SearchAnchor();
		int ret = ctl->SearchNext(0, ">");
		if ( ret != wxNOT_FOUND ) {
			ctl->Remove(ret-2, ret);
			ctl->Remove(leftPos, leftPos + 3);
			return true;
		}
	}
	
	cnc::trc.logError("Cant find '<' and/or '>'!");
	ctl->SetCurrentPos(cp);
	return false;
}
//////////////////////////////////////////////////////////
bool SvgEditPopup::selectCurrentSvgNode(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return false;
		
	int cp = ctl->GetCurrentPos();
	int leftPos  = -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, "<");
	if ( ret != wxNOT_FOUND ) {
		leftPos = ret;
		
		// find right >
		ctl->SearchAnchor();
		int ret = ctl->SearchNext(0, ">");
		if ( ret != wxNOT_FOUND ) {
			ctl->SetSelection(leftPos, ret + 1);
			return true;
		}
	}
	
	cnc::trc.logError("Cant find '<' and/or '>'!");
	ctl->SetCurrentPos(cp);
	return false;
}
//////////////////////////////////////////////////////////
bool SvgEditPopup::selectCurrentSvgNodeBlock(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return false;
		
	int cp = ctl->GetCurrentPos();
	
	// find left prev '<' - open node
	int ret = wxNOT_FOUND;
	do {
		ctl->SearchAnchor();
		ret = ctl->SearchPrev(0, "<");
		
	// skip "</"
	} while ( ret != wxNOT_FOUND && ctl->GetTextRange(ret + 1, ret + 2) == "/" );
	
	if ( ret == wxNOT_FOUND ) {
		cnc::trc.logError("No XML start tag '<' available");
		ctl->SetCurrentPos(cp);
		return false;
	}
	
	// start pos of the svg block to select	
	int leftPos = ret;
	
	// determine node name
	ctl->SetCurrentPos(leftPos);
	wxString c;
	do {
		c.assign(ctl->GetTextRange(ret, ret + 1));
		ret++;
		
	// step ret++ until one of the follwing charachters is found	
	} while ( c.IsEmpty() == false && c[0] != '>' && c[0] != ' ' && c[0] != '\t' && c[0] != '\r' && c[0] != '\n');
	
	// store node name
	wxString nodeName(ctl->GetTextRange(leftPos + 1, ret - 1));
	
	// search next right '>'
	ctl->SetCurrentPos(leftPos);
	ctl->SearchAnchor();
	
	ret = ctl->SearchNext(0, ">");
	if ( ret == wxNOT_FOUND ) {
		cnc::trc.logError("Generally no XML end tag '>' available");
		ctl->SetCurrentPos(cp);
		return false;
	}
	
	// check if current node is closed by itself
	wxString check(ctl->GetTextRange(ret - 1, ret));
	if ( check == "/" ) {
		// Success! Select text and return
		ctl->SetSelection(leftPos, ret + 1);
		return true;
	}
	
	// now search </nodename>
	wxString search(wxString::Format("</%s>", nodeName));
	
	ctl->SetCurrentPos(leftPos);
	ctl->SearchAnchor();

	ret = ctl->SearchNext(0, search);
	if ( ret == wxNOT_FOUND ) {
		cnc::trc.logError(wxString::Format("Generally no %s tag available", search));
		ctl->SetCurrentPos(cp);
		return false;
	}

	// Success! Select text and return
	ctl->SetSelection(leftPos, ret + search.length());
	return true;
}