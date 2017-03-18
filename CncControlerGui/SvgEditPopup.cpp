#include "SvgEditPopup.h"

wxString SvgNodeTemplates::_ret;
unsigned int SvgEditPopup::_idOffset = 0;

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
const char* SvgNodeTemplates::getCNCTemplate() {
//////////////////////////////////////////////////////////
	_ret.clear();
	_ret = "<CNC reverse=\"no\" correction=\"none\"/>";
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
wxMenu* SvgEditPopup::createMenu(MainFrame* frame, wxStyledTextCtrl* ctl, wxMenu* popup) {
//////////////////////////////////////////////////////////
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
	popup->Append(idOffset + STC_PM_CNC, 					wxT("Insert CNC Node"));
	popup->Append(idOffset + STC_PM_CIRCLE, 				wxT("Insert Circle Node"));
	popup->Append(idOffset + STC_PM_ELLIPSE, 				wxT("Insert Ellipse Node"));
	popup->Append(idOffset + STC_PM_LINE, 					wxT("Insert Line Node"));
	popup->Append(idOffset + STC_PM_POLYGON, 				wxT("Insert Polygon Node"));
	popup->Append(idOffset + STC_PM_POLYLINE, 				wxT("Insert Polyline Node"));
	popup->Append(idOffset + STC_PM_RECT, 					wxT("Insert Rect Node"));
	popup->Append(idOffset + STC_PM_PATH, 					wxT("Insert Path Node"));
	popup->AppendSeparator();
	popup->Append(idOffset + STC_PM_PATH_ELLIPTICALARC, 	wxT("Insert EllipticalARC pattern"));
	popup->Append(idOffset + STC_PM_PATH_QUADRATICBEZIER, 	wxT("Insert QuadraticBezier pattern"));
	popup->Append(idOffset + STC_PM_PATH_CUBICBEZIER, 		wxT("Insert CubicBezier pattern"));
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
			ctl->InsertText(ctl->GetCurrentPos(), SvgNodeTemplates::getCNCTemplate());
	 }, idOffset + STC_PM_CNC, wxID_ANY, ctl);

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
	 
	return popup;
}
//////////////////////////////////////////////////////////
void SvgEditPopup::commentCurrentSvgNode(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;
		
	int cp = ctl->GetCurrentPos();
		
	int leftPos  = -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, "<");
	if ( ret != wxNOT_FOUND ) {
		ret++;
		wxString check = ctl->GetTextRange(ret, ret + 1);
		if ( check == "!" ) {
			ctl->SetCurrentPos(cp);
			return;
		}
		
		leftPos = ret;
		
		// find right >
		ctl->SearchAnchor();
		int ret = ctl->SearchNext(0, ">");
		if ( ret != wxNOT_FOUND ) {
			ctl->InsertText(ret, "--");
			ctl->InsertText(leftPos, "!--");
			return;
		}
	}
	ctl->SetCurrentPos(cp);
}
//////////////////////////////////////////////////////////
void SvgEditPopup::uncommentCurrentSvgNode(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;

	int cp = ctl->GetCurrentPos();
	
	int leftPos  = -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, "<");
	if ( ret != wxNOT_FOUND ) {
		ret++;
		wxString check = ctl->GetTextRange(ret, ret + 1);
		if ( check != "!" ) {
			ctl->SetCurrentPos(cp);
			return;
		}
		
		leftPos = ret;
		
		// find right >
		ctl->SearchAnchor();
		int ret = ctl->SearchNext(0, ">");
		if ( ret != wxNOT_FOUND ) {
			ctl->Remove(ret-2, ret);
			ctl->Remove(leftPos, leftPos + 3);
			return;
		}
	}
	ctl->SetCurrentPos(cp);
}
//////////////////////////////////////////////////////////
void SvgEditPopup::selectCurrentSvgNode(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;
		
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
			return;
		}
	}
	ctl->SetCurrentPos(cp);
}
//////////////////////////////////////////////////////////
void SvgEditPopup::selectCurrentSvgNodeBlock(wxStyledTextCtrl* ctl) {
//////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;
		
	int cp = ctl->GetCurrentPos();
		
	int leftPos  = -1;
	
	// find left <
	ctl->SearchAnchor();
	int ret = ctl->SearchPrev(0, "<");
	if ( ret == wxNOT_FOUND ) {
		ctl->SetCurrentPos(cp);
		return;
	}
		
	leftPos = ret;
		
	// find rightnode name end 
	ctl->SetCurrentPos(leftPos);
	wxString c;
	do {
		c = ctl->GetTextRange(ret, ret + 1);
		ret++;
	} while ( c.IsEmpty() == false && c[0] != ' ' && c[0] != '\t' && c[0] != '\r' && c[0] != '\n');
	
	// store node name
	wxString nodeName = ctl->GetTextRange(leftPos + 1, ret - 1);
	
	// search right >
	ctl->SetCurrentPos(leftPos);
	ctl->SearchAnchor();
	ret = ctl->SearchNext(0, ">");
	if ( ret == wxNOT_FOUND ) {
		ctl->SetCurrentPos(cp);
		return;
	}
	
	// check if current node is closed by itself
	wxString check = ctl->GetTextRange(ret - 1, ret);
	
	if ( check == "/" ) {
		ctl->SetSelection(leftPos, ret + 1);
		return;
	}
	
	// search </nodeName>
	wxString search("</");
	search += nodeName;
	search += ">";
	std::clog << search << std::endl;
	
	ctl->SetCurrentPos(leftPos);
	ctl->SearchAnchor();
	ret = ctl->SearchNext(0, search);
	if ( ret == wxNOT_FOUND ) {
		ctl->SetCurrentPos(cp);
		return;
	}

	ctl->SetSelection(leftPos, ret + search.length());
}