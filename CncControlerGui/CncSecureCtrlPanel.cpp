#include "MainFrame.h"
#include "GlobalStrings.h"
#include "GlobalFunctions.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "wxCrafterImages.h"
#include "CncSecureManuallyMovePanel.h"
#include "CncSecureCtrlPanel.h"

class CncSecurePortListCtrl : public CncLargeScaledListCtrl {
	
	public:
		enum PortListImage {
			PTI_CONNECTED 		=  0,
			PTI_AVAILABLE  		=  1,
			PTI_ACCESS_DENIED 	=  2,
			PTI_UNKNOWN			=  3,
			PTI_EMPTY			= -1
		};
		
		const PortListImage getImgaeIndex(const wxString& name) {
			
			if      (name.IsSameAs(PortSelector::BMP_PS_CONNECTED))     return PTI_CONNECTED;
			else if (name.IsSameAs(PortSelector::BMP_PS_AVAILABLE))     return PTI_AVAILABLE;
			else if (name.IsSameAs(PortSelector::BMP_PS_ACCESS_DENIED)) return PTI_UNKNOWN;
			else if (name.IsSameAs(PortSelector::BMP_PS_UNKNOWN)) 		return PTI_ACCESS_DENIED;
			
			return PTI_EMPTY;
		}
		
		struct PortEntry {
			PortListImage	imageIdx;
			wxString		portName;
			
			PortEntry(const wxString& n, PortListImage i) 
			: imageIdx(i)
			, portName(n)
			{}
		};
		
		typedef std::vector<PortEntry> PortEntryList;
	
	private:
		
		PortEntryList		portEntries;
		CncSecureCtrlPanel*	securePanel;
		
		wxListItemAttr		defaultItemAttr;
		wxListItemAttr		selectedItemAttr;
		
		// --------------------------------------------------------
		virtual wxString OnGetItemText(long item, long column) const {
			if ( isItemValid(item) == false )
				return _("");
		
			if ( column != COL_NAME )
				return _("");
			
			return portEntries.at(item).portName;
		}
		// --------------------------------------------------------
		virtual int OnGetItemColumnImage(long item, long column) const {
			if ( isItemValid(item) == false )
				return -1;
				
			if ( column != COL_STATE )
				return -1;
				
			return portEntries.at(item).imageIdx;
		}
		// --------------------------------------------------------
		virtual wxListItemAttr* OnGetItemAttr(long item) const {
			return (wxListItemAttr*)(item != getLastSelection() ? &defaultItemAttr : &selectedItemAttr);
		}
		// --------------------------------------------------------
		void onSize(wxSizeEvent& event) {
			updateColumnWidth();
			event.Skip(true);
		}
		// --------------------------------------------------------
		void onSelectListItem(wxListEvent& event) {
			//CNC_PRINT_FUNCT;
		}
		// --------------------------------------------------------
		void onActivateListItem(wxListEvent& event) {
			
			long item = event.m_itemIndex;
			if ( item == wxNOT_FOUND )
				return;
			
			if ( isItemValid(item) == false )
				return;
				
			if ( securePanel ) {
				securePanel->setPortSelection(portEntries.at(item).portName);
				setLastSelection(item);
			}
		}
		
	protected:
	
		// --------------------------------------------------------
		virtual bool isItemValid(long item) const {
			
			wxASSERT( GetItemCount() == portEntries.size() );
			
			if( item < 0 || item > (long)(portEntries.size() - 1) )
				return false;
				
			return true;
		}

	public:
		static const int COL_STATE 			= 0;
		static const int COL_NAME 			= 1;
		static const int TOTAL_COL_COUNT	= 2;
		static const int COL_STRECH			= COL_NAME;
		
		// --------------------------------------------------------
		CncSecurePortListCtrl(wxWindow *parent, CncSecureCtrlPanel* panel, long style)
		: CncLargeScaledListCtrl	(parent, style)
		, portEntries				()
		, securePanel				(panel)
		, defaultItemAttr			()
		, selectedItemAttr			()
		{
			// add colums
			AppendColumn("", wxLIST_FORMAT_LEFT, 30);
			AppendColumn("", wxLIST_FORMAT_LEFT, 30);
			
			// determine styles
			setListType(CncLargeScaledListCtrl::ListType::REVERSE);
			
			wxFont font(16, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
			SetFont(font);
			
			SetTextColour(wxColour(32, 32, 32));
			SetBackgroundColour(wxColour(255, 255, 255));
			
			defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
			defaultItemAttr.SetFont(font);
			defaultItemAttr.SetTextColour(GetTextColour());
			
			selectedItemAttr 	= defaultItemAttr;
			selectedItemAttr	.SetTextColour(wxColour(255, 128, 128));
			selectedItemAttr	.SetFont(font.Bold());
			
			wxImageList* imageList = new wxImageList(16, 16, true);
			imageList->RemoveAll();
			imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_CONNECTED));
			imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_AVAILABLE));
			imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_ACCESS_DENIED));
			imageList->Add(ImageLibPortSelector().Bitmap(PortSelector::BMP_PS_UNKNOWN));
			SetImageList(imageList, wxIMAGE_LIST_SMALL);
		}
		// ---------------------------------------------------------
		virtual ~CncSecurePortListCtrl() {
		}
		// ---------------------------------------------------------
		virtual bool Enable(bool enable=true) {
			GblFunc::freeze(this, !enable);
			Refresh();
			return CncLargeScaledListCtrl::Enable(enable); 
		}
		// ---------------------------------------------------------
		void updateColumnWidth() {
			// avoid flicker
			GblFunc::freeze(this, true);
				
			// try to strech the second (key) column
			const int size = GetSize().GetWidth() - 26; 
			
			if ( size > GetColumnWidth(COL_STRECH) )
				SetColumnWidth(COL_STRECH, size);
				
			GblFunc::freeze(this, false);
		}
		// ---------------------------------------------------------
		void deleteAllEntries() {
			SetItemCount(portEntries.size());
			
			CncLargeScaledListCtrl::clear();
			portEntries.clear();
			
			Refresh();
		}
		// ---------------------------------------------------------
		void addPortEntry(const wxString& name, PortListImage pii) {
			portEntries.push_back(PortEntry(name, pii));
			
			SetItemCount(portEntries.size());
			Refresh();
		}
		// ---------------------------------------------------------
		void addPortEntry(const wxString& name, const wxString& image) {
			addPortEntry(name, getImgaeIndex(image));
		}
		// ---------------------------------------------------------
		bool selectPortInList(const wxString& portName) {
			for ( auto it = portEntries.begin(); it != portEntries.end(); ++it ) {
				
				if ( it->portName == portName ) {
					selectItem(std::distance(portEntries.begin(), it), true);
					return true;
				}
			}
			
			return false;
		}
		
		wxDECLARE_NO_COPY_CLASS(CncSecurePortListCtrl);
		wxDECLARE_EVENT_TABLE();
};

/////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CncSecurePortListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE				(			CncSecurePortListCtrl::onSize				)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncSecurePortListCtrl::onSelectListItem		)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY, 	CncSecurePortListCtrl::onActivateListItem	)
wxEND_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
CncSecureCtrlPanel::CncSecureCtrlPanel(wxWindow* parent)
: CncSecureCtrlPanelBase						(parent)
, CncSecureSlidepad::CallbackInterface			()
, CncReferenceEvaluation::CallbackInterface		()
, CncPodestMgmtMovement::CallbackInterface		()
, CncSecureGesturesPanel::CallbackInterface		()
, portSelectorList								(NULL)
, manuallyMovePanel								(NULL)
, interactiveMoveX								(NULL)
, interactiveMoveY								(NULL)
, interactiveMoveZ								(NULL)
, interactiveTouchpadXYZ						(NULL)
, podestPanel									(NULL)
, referencePanel								(NULL)
, speedpad										(NULL)
, pageVector									()
/////////////////////////////////////////////////////////////////////
{
	// ---------------------------------------------------------------
	auto setPageWidth = [&](wxWindow* page, int width){
		int pos = m_rightBook->FindPage(page);
		if ( pos >=0 && pos < (int)pageVector.size() )
			pageVector.at(pos).width = width;
	};

	// setup additional page information
	wxASSERT( m_leftBook->GetPageCount() ==  m_rightBook->GetPageCount() );
	for (unsigned int i = 0; i < m_leftBook->GetPageCount(); i++ ) {
		PageInfo pi;
		pageVector.push_back(pi);
	}
	
	setPageWidth(m_rpManually,	800);
	setPageWidth(m_rpRef,		800);
	
	portSelectorList = new CncSecurePortListCtrl(this, this, wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_NO_HEADER); 
	GblFunc::replaceControl(m_portSelectorPlaceholder, portSelectorList);
	
	manuallyMovePanel = new CncSecureManuallyMovePanel(this); 
	GblFunc::replaceControl(m_manuallyMovePlaceholder, manuallyMovePanel);
	
	interactiveMoveX = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveMoveXPlaceholder, interactiveMoveX);
	interactiveMoveX->setCallbackInterface(this, CallbackID_SPX);
	interactiveMoveX->SetBackgroundColour(wxColour(255, 128, 128));
	
	interactiveMoveY = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveMoveYPlaceholder, interactiveMoveY);
	interactiveMoveY->setCallbackInterface(this, CallbackID_SPY);
	interactiveMoveY->SetBackgroundColour(wxColour(  0, 120, 215));
	
	interactiveMoveZ = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveMoveZPlaceholder, interactiveMoveZ);
	interactiveMoveZ->setCallbackInterface(this, CallbackID_SPZ);
	interactiveMoveZ->SetBackgroundColour(wxColour(  0, 128,  0));
	
	interactiveTouchpadXYZ = new CncSecureGesturesPanel(this, wxBOTH, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 5);
	GblFunc::replaceControl(m_interactiveTouchpadXYZ, interactiveTouchpadXYZ);
	interactiveTouchpadXYZ->setCallbackInterface(this, CallbackID_TPXY);
	interactiveTouchpadXYZ->SetBackgroundColour(wxColour(255, 255, 184));
	
	
	
				CncSecureGesturesPanel* panelQ1 = new CncSecureGesturesPanel(this, wxHORIZONTAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_POSITIVE, 10);
				GblFunc::replaceControl(m_panelQ1, panelQ1);
				panelQ1->setCallbackInterface(this, CallbackID_TPZ+6);
				panelQ1->SetBackgroundColour(wxColour(  0, 128,  0));
				
				CncSecureGesturesPanel* panelQ2 = new CncSecureGesturesPanel(this, wxHORIZONTAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_NEGATIVE, 10);
				GblFunc::replaceControl(m_panelQ2, panelQ2);
				panelQ2->setCallbackInterface(this, CallbackID_TPZ+6);
				panelQ2->SetBackgroundColour(wxColour(  0, 128,  0));
				
				CncSecureGesturesPanel* panelQ3 = new CncSecureGesturesPanel(this, wxHORIZONTAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_BOTH, 10);
				GblFunc::replaceControl(m_panelQ3, panelQ3);
				panelQ3->setCallbackInterface(this, CallbackID_TPZ+6);
				panelQ3->SetBackgroundColour(wxColour(  0, 128,  0));
				
				
	
	podestPanel = new CncPodestMgmtMovement(this); 
	GblFunc::replaceControl(m_podestPlaceholder, podestPanel);
	podestPanel->setCallbackInterface(this);
	
	referencePanel = new CncReferenceEvaluation(this); 
	GblFunc::replaceControl(m_evaluateReferencePlaceholder, referencePanel);
	referencePanel->setCallbackInterface(this);
	
	/*
	speedpad = new CncSecureSlidepad(this); 
	GblFunc::replaceControl(m_speedSliderPlaceholder, speedpad);
	speedpad->setInfo("Speed F:");
	CncSecureSlidepad::SliderValues values;
	{
		values.push_back(cnc::getSpeedValue(FINEST));
		values.push_back(cnc::getSpeedValue(FINE));
		values.push_back(cnc::getSpeedValue(MEDIUM));
		values.push_back(cnc::getSpeedValue(ROUGH));
		values.push_back(cnc::getSpeedValue(ROUGHEST));
	}
	speedpad->setValues(values, cnc::getSpeedValue(FINE));
	speedpad->setCallbackInterface(this);
	speedpad->GetInfoText()->SetForegroundColour(*wxWHITE);
	*/
	
	THE_APP->GetSecureVersionInfo()->SetLabel(CNC_VERSION_STR);
}
/////////////////////////////////////////////////////////////////////
CncSecureCtrlPanel::~CncSecureCtrlPanel() {
/////////////////////////////////////////////////////////////////////
	wxDELETE(portSelectorList);
	wxDELETE(manuallyMovePanel);
	wxDELETE(podestPanel);
	wxDELETE(referencePanel);
	wxDELETE(speedpad);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::notifyStarting(const CncSecureGesturesPanel::State s) {
/////////////////////////////////////////////////////////////////////
	const int pos = cnc::getSpeedStepSensitivityIndex(FINE);
	
	THE_APP->selectStepSensitivity(pos);
	THE_APP->startInteractiveMove(CncInteractiveMoveDriver::IMD_NAVIGATOR); 
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::notifyPositionHeld(const CncSecureGesturesPanel::Data& d) {
/////////////////////////////////////////////////////////////////////
	CNC_PRINT_FUNCT
	THE_APP->updateInteractiveMove();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::notifyPositionChanged(const CncSecureGesturesPanel::Data& d) {
/////////////////////////////////////////////////////////////////////
	// stop ....
	if ( d.isZero() )
	{
		THE_APP->stopInteractiveMove();
		return;
	}
	
	//std::cout << d << std::endl;
	//return;
	
	// may be nothing to to
	if ( d.isChanged() == false )
		return;
	
	// determine direction(s)
	CncLinearDirection dx = CncNoneDir;
	CncLinearDirection dy = CncNoneDir;
	CncLinearDirection dz = CncNoneDir;
	
	switch ( d.cbId ) 
	{
		case CallbackID_SPX:	dx = ( d.range != 0 ? ( d.range < 0  ? CncNegDir : CncPosDir ) : CncNoneDir ); 
								break;
								
		case CallbackID_SPY:	dy = ( d.range != 0 ? ( d.range < 0  ? CncNegDir : CncPosDir ) : CncNoneDir );
								break;
								
		case CallbackID_SPZ:
		case CallbackID_TPZ:	dz = ( d.range != 0 ? ( d.range < 0  ? CncNegDir : CncPosDir ) : CncNoneDir );
								break;
								
		case CallbackID_TPXY:	dx = ( d.xVal ? ( ( d.xVal  < 0 ) ? CncNegDir : CncPosDir ) : CncNoneDir );
								dy = ( d.yVal ? ( ( d.yVal  < 0 ) ? CncNegDir : CncPosDir ) : CncNoneDir );
								break;
								
		default:				return;
	}
	
	// move . . . 
	const int modifySpeed = abs(d.range);
	std::cout << wxString::Format("updateInteractiveMove(%+d, %+d, %+d, %d)\n", (int)dx, (int)dy, (int)dz, abs(d.range));
	THE_APP->updateInteractiveMove(dx, dy, dz, modifySpeed);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::sliderValueChanged(int pos, int value) {
/////////////////////////////////////////////////////////////////////
	// Notification from navigator speed slider
	THE_APP->selectStepSensitivity(pos);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onLeftBookPageChanged() {
/////////////////////////////////////////////////////////////////////
	wxASSERT(m_leftBook->GetPageCount() == m_rightBook->GetPageCount() );
	
	const int sel = m_leftBook->GetSelection();
	m_rightBook->SetSelection(sel);
	
	if ( sel >=0 && sel < (int)pageVector.size() )
		THE_APP->GetSecureSplitterMainV()->SetSashPosition(pageVector.at(sel).width);
	
	performRightHeadline();
	
	switch (sel) {
		case PAGE_PODEST:	podestPanel->init();
							break;
							
		case PAGE_REF:		referencePanel->init();
							break;
							
		default:			podestPanel->close();
							
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onLeftBookPageChanged(wxListbookEvent& event) {
/////////////////////////////////////////////////////////////////////
	onLeftBookPageChanged();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onEmergencySec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->emergencyStop(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onRunSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcRun(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onPauseSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcPause(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onStopSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcStop(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onConnectSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->connectSec(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onEvaluateHardwareReference(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->onEvaluateHardwareReference(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onResetSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->rcReset(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onDetermineAnchorPositionsSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->setAnchorPosition(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::lockSelection(bool b) {
/////////////////////////////////////////////////////////////////////
	m_leftBook->Enable(b == false);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::performRightHeadline() {
/////////////////////////////////////////////////////////////////////
	const int sel = m_leftBook->GetSelection();
	wxString hl(wxString::Format("%s:\n", m_leftBook->GetPageText(sel)));
	hl.Replace("\n", " ", true);
	
	m_rightHeadline->SetLabel(hl);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::tryToProvideTemplate() {
/////////////////////////////////////////////////////////////////////
	wxFileName fn(THE_APP->getCurrentTemplateFileName());
	if ( fn.Exists() == false ) {
		
		wxString firstLruFile;
		if ( THE_APP->getFirstLruFile(firstLruFile) )
			fn.Assign(firstLruFile);
			
		bool fileOpened = false;
		if ( fn.Exists() == true ) {
			THE_APP->setTemplateName(fn);
			fileOpened = THE_APP->openFile();
		}
		
		if ( fileOpened == false )
			THE_APP->openTemplate();
	}
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::setPortSelection(const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	THE_APP->GetPortSelector()->SetStringSelection(portName);
	THE_APP->selectPort();
	
}/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::updatePortSelection(const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	portSelectorList->selectPortInList(portName);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::addPortName(const wxString& portName, const wxString& imageName) {
/////////////////////////////////////////////////////////////////////
	portSelectorList->addPortEntry(portName, imageName);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::clearPortSelection() {
/////////////////////////////////////////////////////////////////////
	portSelectorList->deleteAllEntries();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::notifyConnection(bool state, const wxString& portName) {
/////////////////////////////////////////////////////////////////////
	const wxBitmap bmpC = ImageLib16().Bitmap("BMP_OK16");
	const wxBitmap bmpD = ImageLib16().Bitmap("BMP_WARNING16");
	
	m_portName->SetLabel(portName);
	m_btHardwareRefSec->Enable(THE_CONTEXT->hasHardware() && state == true );
	m_bmpConnectionStateSecure->SetBitmap( state == true ? bmpC : bmpD);
	m_bmpConnectionStateSecure->Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onOpenTemplateSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->openTemplate(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onSessionDirSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->openSessionDialog(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onTestFunctionSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->testFunction1(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onStackTraceStoreSec(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->showStacktraceStore(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::requestResolveLimitStates(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	THE_APP->requestResolveLimitStates(event);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::referenceNotifyMessage(const wxString& msg, int flags) {
//////////// /////////////////////////////////////////////////////////
	wxString m(msg);
	while ( m.EndsWith("\n") )
		m.RemoveLast();
	
	m_referenceInfobar->ShowMessage(m, flags);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::referenceDismissMessage() {
/////////////////////////////////////////////////////////////////////
	m_referenceInfobar->Dismiss();
	Layout();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::cameraNotifyPreview(bool show) {
/////////////////////////////////////////////////////////////////////
	if ( show == true )	THE_APP->GetSecurePreviewBook()->SetSelection(SecurePreviewBookSelection::VAL::CAMERA_PREVIEW);
	else				THE_APP->GetSecurePreviewBook()->SetSelection(SecurePreviewBookSelection::VAL::RIGHT_PREVIEW);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::podestNotifyEnable(bool state)  {
/////////////////////////////////////////////////////////////////////
	m_leftBook->Enable(state);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::podestNotifyInit(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state == false )
		CNC_PRINT_FUNCT
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::podestNotifyClose(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( state == false )
		CNC_PRINT_FUNCT
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onReferenceSet(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	referencePanel->set();
	
	RefPosResult parameter;
	referencePanel->getResult(parameter);
	THE_APP->updateReferencePosition(&parameter);
	
	cameraNotifyPreview(false);
	m_leftBook->SetSelection(PAGE_RUN);
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::onToggleTouchpadPane(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	interactiveTouchpadXYZ->init();
	
	const int id = interactiveTouchpadXYZ->getCallbackId();
	switch ( id ) 
	{
		case CallbackID_TPXY:
			((wxToggleButton*)event.GetEventObject())->SetLabel("Z Axis");
			interactiveTouchpadXYZ->setCallbackId(CallbackID_TPZ);
			interactiveTouchpadXYZ->setOrientation(wxVERTICAL);
			interactiveTouchpadXYZ->SetBackgroundColour(wxColour(  0, 128,  0));
			break;
			
		case CallbackID_TPZ:
			((wxToggleButton*)event.GetEventObject())->SetLabel("XY Pane");
			interactiveTouchpadXYZ->setCallbackId(CallbackID_TPXY);
			interactiveTouchpadXYZ->setOrientation(wxBOTH);
			interactiveTouchpadXYZ->SetBackgroundColour(wxColour(255, 255, 184));
			break;
	}
	
	interactiveTouchpadXYZ->update();
}
/////////////////////////////////////////////////////////////////////
void CncSecureCtrlPanel::activate(bool b) {
/////////////////////////////////////////////////////////////////////
	THE_APP->navigatorPanel->setSecureMode(b);
	
	if ( b ) {
		if ( THE_CONTEXT->secureModeInfo.isActivatedByStartup == true ) {
			tryToProvideTemplate();
		}
		
		const int sel = PAGE_CONNECT;
		m_leftBook->SetSelection(sel);
		m_rightBook->SetSelection(sel);
		THE_APP->GetSecureSplitterMainV()->SetSashPosition(pageVector.at(sel).width);
		performRightHeadline();
	}
	else {

	}
	
	//..
	
}

