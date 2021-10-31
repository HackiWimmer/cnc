#include "wxCrafterImages.h"
#include "GlobalFunctions.h"
#include "CncUserEvents.h"
#include "CncExternalViewBox.h"

#include <wx/frame.h>
extern wxFrame* THE_FRAME;

//////////////////////////////////////////////////////////////////
CncExternalViewBoxCluster::CncExternalViewBoxCluster(wxWindow* parent) 
: cluster		()
//////////////////////////////////////////////////////////////////
{
	cluster[EVB_Monitor]	= new CncExternalViewBox(parent);
	cluster[EVB_Config]		= new CncExternalViewBox(parent);
	cluster[EVB_Source]		= new CncExternalViewBox(parent);
	cluster[EVB_Reference]	= new CncExternalViewBox(parent);
	cluster[EVB_Manually]	= new CncExternalViewBox(parent);
	cluster[EVB_Test]		= new CncExternalViewBox(parent);
	cluster[EVB_SetterList]	= new CncExternalViewBox(parent);
	cluster[EVB_CtrlMsg]	= new CncExternalViewBox(parent);
	
	wxASSERT_MSG(
		static_cast<int>(Node::EVB_ENUM_COUNT) == cluster.size(), 
		wxString::Format("Wrong number of cluster nodes. Required=%d, Registered=%d", static_cast<int>(Node::EVB_ENUM_COUNT), (int)cluster.size())
	);
}
//////////////////////////////////////////////////////////////////
CncExternalViewBoxCluster::~CncExternalViewBoxCluster() {
//////////////////////////////////////////////////////////////////
	cluster.clear();
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBoxCluster::setupView1(Node n, wxWindow* wnd, const wxString& title) {
//////////////////////////////////////////////////////////////////
	CncExternalViewBox* evb = getNode(n);
	if ( evb == NULL )
		return false;
		
	if ( wnd == NULL )
		return false;
	
	return evb->setupView(CncExternalViewBox::Default::VIEW1, wnd, title);
}
//////////////////////////////////////////////////////////////////
CncExternalViewBox* CncExternalViewBoxCluster::getNode(Node n) const {
//////////////////////////////////////////////////////////////////
	if ( auto it = cluster.find(n); it != cluster.end() ) {
		return it->second;
	}
	
	wxASSERT_MSG(NULL, wxString::Format("%s: getNode(%d) failed!", CNC_LOG_FUNCT, (int)n));
	return NULL;
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBoxCluster::detachNode(Node n, wxButton* btn) {
//////////////////////////////////////////////////////////////////
	CncExternalViewBox* evb = getNode(n);
	if ( evb == NULL )
		return false;
		
	const bool isExtViewActive = !evb->IsShown();
	
	// prepare and show external preview
	evb->setupSwapButton(CncExternalViewBox::Default::VIEW1, btn);
	evb->selectView(CncExternalViewBox::Default::VIEW1);
	evb->Show(isExtViewActive);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBoxCluster::hideNode(Node n) {
//////////////////////////////////////////////////////////////////
	CncExternalViewBox* evb = getNode(n);
	if ( evb == NULL )
		return false;
	
	return evb->Show(false);
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBoxCluster::hideAll() {
//////////////////////////////////////////////////////////////////
	bool ret = true;
	
	for ( auto it = cluster.begin(); it != cluster.end(); ++it ) {
		if ( hideNode(it->first) == false )
			ret = false;
	}
	
	return ret;
}



//////////////////////////////////////////////////////////////////
CncExternalViewBox::CncExternalViewBox(wxWindow* parent, long style)
: CncExternalViewBoxBase(parent, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(500, 300), wxSTAY_ON_TOP | style)
, guiSensitivity(true)
, moveDelta()
//////////////////////////////////////////////////////////////////
{
	targetCtrl[0] = m_placeholder1;
	targetCtrl[1] = m_placeholder2;
	targetCtrl[2] = m_placeholder3;
	targetCtrl[3] = m_placeholder4;
	
	for (unsigned int i=0; i<MAX_VIEWS; i++) {
		sourceCtrl[i] = NULL;
		swapButton[i] = NULL;
		swapState[i]  = SS_DEFAULT;
		title[i]      = "";
	}
	
	m_viewBook->SetSelection(0);
	
	CenterOnParent();
	this->Bind(wxEVT_CNC_NAVIGATOR_PANEL, 	&CncExternalViewBox::onNavigatorPanel, this);
}
//////////////////////////////////////////////////////////////////
CncExternalViewBox::~CncExternalViewBox() {
//////////////////////////////////////////////////////////////////
	this->Unbind(wxEVT_CNC_NAVIGATOR_PANEL, &CncExternalViewBox::onNavigatorPanel, this);

	for (unsigned int i=0; i<MAX_VIEWS; i++) {
		if ( sourceCtrl[i] != NULL ) {
			if ( swapState[i] == SS_SWAPED )
				swapControls(i);
		}
	}
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBox::isViewAttached(unsigned int idx) const {
//////////////////////////////////////////////////////////////////
	if ( idx > MAX_VIEWS - 1 )
		return false;
		
	return swapState[idx] == SS_DEFAULT;
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBox::isViewDetached(unsigned int idx) const {
//////////////////////////////////////////////////////////////////
	if ( idx > MAX_VIEWS - 1 )
		return false;
		
	return swapState[idx] == SS_SWAPED;
}
//////////////////////////////////////////////////////////////////
const CncExternalViewBox::SwapState CncExternalViewBox::getSwapState(unsigned int idx) const {
//////////////////////////////////////////////////////////////////
	if ( idx > MAX_VIEWS - 1 )
		return SS_DEFAULT;
		
	return swapState[idx];
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBox::selectView(unsigned int idx) {
//////////////////////////////////////////////////////////////////
	if ( idx > MAX_VIEWS - 1 )
		return false;
		
	if ( sourceCtrl[idx] == NULL )
		return false;

	m_viewBook->SetSelection(idx);
	
	SetTitle(title[idx]);
	m_windowTitle->SetLabel(GetTitle());
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBox::setupView(unsigned int idx, wxWindow* source, const wxString& t) {
//////////////////////////////////////////////////////////////////
	if ( idx > MAX_VIEWS - 1 )
		return false;
	
	if ( sourceCtrl[idx] != NULL ) {
		if ( swapState[idx] == SS_SWAPED )
			swapControls(idx);
	}
	
	title[idx] 		= t;
	swapState[idx] 	= SS_DEFAULT;
	sourceCtrl[idx] = source;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncExternalViewBox::setupSwapButton(unsigned int idx, wxButton* btn) {
//////////////////////////////////////////////////////////////////
	if ( idx > MAX_VIEWS - 1 )
		return false;
		
	swapButton[idx] = btn;
	return true;
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::swapControls() {
//////////////////////////////////////////////////////////////////
	for (unsigned int i=0; i<MAX_VIEWS; i++)
		swapControls(i);
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::swapControls(unsigned int idx) {
//////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtMainFrame ID;
	typedef IndividualCommandEvent::ValueName VN;

	if ( idx > MAX_VIEWS - 1 )
		return;
		
	if ( sourceCtrl[idx] == NULL )
		return;

	if ( swapState[idx] == SS_DEFAULT ) {
		GblFunc::swapControls(targetCtrl[idx], sourceCtrl[idx]);
		swapState[idx] = SS_SWAPED;
		
		IndividualCommandEvent evt(ID::ExtViewBoxDetach);
		evt.setValue(VN::VAL1, this);
		evt.setValue(VN::VAL2, idx);
		
		wxPostEvent(THE_FRAME, evt);
		
	} else {
		GblFunc::swapControls(sourceCtrl[idx], targetCtrl[idx]);
		swapState[idx] = SS_DEFAULT;
		
		IndividualCommandEvent evt(ID::ExtViewBoxAttach);
		evt.setValue(VN::VAL1, this);
		evt.setValue(VN::VAL2, idx);
		
		wxPostEvent(THE_FRAME, evt);

	}
	
	if ( swapButton[idx] != NULL ) {
		swapButton[idx]->SetBitmap(ImageLibAui().Bitmap(swapState[idx] == SS_DEFAULT ? "BMP_DETACH"  : "BMP_ATTACH"));
		swapButton[idx]->SetToolTip(                    swapState[idx] == SS_DEFAULT ? "Detach View" : "Attach View");
	}
	
	Update();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onShow(wxShowEvent& event) {
//////////////////////////////////////////////////////////////////
	for (unsigned int i=0; i<MAX_VIEWS; i++) {
		if 		( event.IsShown() == true  && swapState[i] == SS_DEFAULT )	swapControls(i);
		else if ( event.IsShown() == false && swapState[i] == SS_DEFAULT )	; // do nothing
		else if ( event.IsShown() == true  && swapState[i] == SS_SWAPED )	; // do nothing
		else if ( event.IsShown() == false && swapState[i] == SS_SWAPED )	swapControls(i);
	}
	
	decorateMinMaxButton();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onCloseFromButton(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( guiSensitivity == false )
		return;

	Show(false);
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onMinMax(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( guiSensitivity == false )
		return;
	
	if ( IsMaximized() == false )	Maximize();
	else							Restore();
	
	decorateMinMaxButton();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::decorateMinMaxButton() {
//////////////////////////////////////////////////////////////////
	m_btMinMax->SetToolTip( IsMaximized() ? "Minimize" : "Maximize" );
	m_btMinMax->SetBitmap(ImageLibAui().Bitmap(IsMaximized() ? "BMP_AUI_MINIMIZE"  : "BMP_AUI_EXPAND"));
	m_btMinMax->Refresh();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onStartMove(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
	m_moveArea->CaptureMouse();
	
	if ( m_moveArea->HasCapture() ) {
		const wxPoint pos = ::wxGetMousePosition();
		const wxPoint origin = this->GetPosition();
		
		int dx = pos.x - origin.x;
		int dy = pos.y - origin.y;  
		
		moveDelta = wxPoint(dx, dy);
	}
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onStopMove(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( m_moveArea->HasCapture() )
		m_moveArea->ReleaseMouse();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onMotion(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( m_moveArea->HasCapture() ) {
		if ( event.Dragging() && event.LeftIsDown() ) {
			const wxPoint pos = ::wxGetMousePosition();
			Move(wxPoint(pos.x - moveDelta.x, pos.y - moveDelta.y));
		}
	}
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onViewBookChanged(wxNotebookEvent& event) {
//////////////////////////////////////////////////////////////////
	typedef IndividualCommandEvent::EvtMainFrame ID;
	typedef IndividualCommandEvent::ValueName VN;

	IndividualCommandEvent evt(ID::ExtViewBoxChange);
	evt.setValue(VN::VAL1, this);
	evt.setValue(VN::VAL2, event.GetOldSelection());
	evt.setValue(VN::VAL3, event.GetSelection());
	
	wxPostEvent(THE_FRAME, evt);
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onAttachPage() {
//////////////////////////////////////////////////////////////////
	swapControls();
	Show(false);
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onAttachPage1(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	onAttachPage();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onAttachPage2(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	onAttachPage();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onAttachPage3(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	onAttachPage();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::onAttachPage4(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	onAttachPage();
}
//////////////////////////////////////////////////////////////////
void CncExternalViewBox::bringViewOnTop() {
//////////////////////////////////////////////////////////////////
	// Attention: This raises the window on top, no problem, 
	// but the interactive move did not work correctly if the motion 
	// monitor as detached, for whatever reason!
	
	//Raise();
}
///////////////////////////////////////////////////////////////////
void CncExternalViewBox::onNavigatorPanel(CncNavigatorPanelEvent& event) {
///////////////////////////////////////////////////////////////////
	// redirect to main frame . . .
	wxPostEvent(THE_FRAME, event);
}
