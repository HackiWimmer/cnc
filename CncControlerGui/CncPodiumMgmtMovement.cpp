#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncControl.h"
#include "CncContext.h"
#include "CncSecureNumpadDialog.h"
#include "CncPodiumMgmtMovement.h"

///////////////////////////////////////////////////////////////////
CncPodiumMgmtMovement::CncPodiumMgmtMovement(wxWindow* parent)
: CncPodiumMgmtMovementBase					(parent)
, direction									(CncNoneDir)
, interactiveMove							(NULL)
, caller									(NULL)
///////////////////////////////////////////////////////////////////
{
	interactiveMove = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_intactiveMovePlaceholder, interactiveMove);
	interactiveMove->SetBackgroundColour(*wxYELLOW);
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0.0, THE_CONFIG->getMaxDimensionH());
	val.SetPrecision(3);
	
	m_moveRelative->SetValidator(val);
	m_moveRelative->ChangeValue("0.000");
	
	Bind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncPodiumMgmtMovement::onInteractiveMove,	this);
}
///////////////////////////////////////////////////////////////////
CncPodiumMgmtMovement::~CncPodiumMgmtMovement() {
///////////////////////////////////////////////////////////////////
	Unbind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncPodiumMgmtMovement::onInteractiveMove,	this);
	
	wxDELETE(interactiveMove);
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::onInteractiveMove(CncSecureGesturesPanelEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( IsShownOnScreen() == false )
		return;

	const CncLinearDirection prevDirection = direction;
	
	// stop ....
	if ( event.data.isZero() )
	{
		if ( prevDirection != CncNoneDir )
		{		
			direction = CncNoneDir;
			return process();
		}
	}
	
	switch ( event.GetId() )
	{
		case CncSecureGesturesPanelEvent::Id::CSGP_STARTING:
		{
			if ( prevDirection != CncNoneDir )
			{
				// stop ....
				direction = CncNoneDir;
				process();
			}
			
			break;
		}
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_CHANGED:
		{
			auto determineDir = [&]() {
				if ( event.data.range == 0 )	direction = CncNoneDir;
				else							direction = event.data.range < 0 ? CncNegDir : CncPosDir;
			};
			
			if ( prevDirection != CncNoneDir )
			{
				determineDir();
				
				if ( prevDirection != direction )
				{
					// stop ....
					direction = CncNoneDir;
					process();
				}
			}
			
			determineDir();
			if ( direction != prevDirection )
				process();
			
			break;
		}
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_HELD:
		{
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::enable(bool state) {
///////////////////////////////////////////////////////////////////
	m_btRelativeUp	->Enable(state);
	m_btRelativeDown->Enable(state);
	interactiveMove	->Enable(state);
	
	if ( caller )
		caller->podiumNotifyEnable(state);
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::reset() {
///////////////////////////////////////////////////////////////////
	if ( direction == CncNoneDir )
		return;
		
	direction = CncNoneDir;
	process();
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::process() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = THE_APP->getCncControl();
	if ( cnc == NULL ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	if ( cnc->isConnected() == false )
	{
		std::cerr << CNC_LOG_FUNCT_A(": Not connected!") << std::endl;
		return;
	}
	
	const bool testOnly = false;
	if ( testOnly ) 
	{
		switch ( direction )
		{
			case CncNegDir:		std::cout << "Neg:   " << (int32_t)direction << std::endl; break;
			case CncPosDir:		std::cout << "Pos:   " << (int32_t)direction << std::endl; break;
			case CncNoneDir:	std::cout << "Quite: " << std::endl; break;
		}
	}
	else
	{
		const bool exact = false;
		bool ret = true;
		
		switch ( direction ) {
			case CncNegDir:
			case CncPosDir:		ret = cnc->processMovePodium((int32_t)direction, exact);
								break;
								
			case CncNoneDir:	ret = cnc->sendQuitMove();
								
								// remove all inbound remnants to have a clear situation 
								// before the next movement
								THE_APP->waitActive(500);
								cnc->popSerial(true);
								
								break;
		}
		
		if ( ret == false ) 
			std::cerr << CNC_LOG_FUNCT_A(": Cnc processing failed! Direction = %d", (int)direction) << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
bool CncPodiumMgmtMovement::init() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = THE_APP->getCncControl();
	bool ret = true;
	
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		ret = false;
	}

	if ( ret == true ) {
		if ( cnc->processCommand(CMD_ACTIVATE_PODIUM_HW, std::cout) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Can't activate podium hardware!") << std::endl;
			ret = false;
		}
	}
	
	if ( caller )
		caller->podiumNotifyInit(ret);
			
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncPodiumMgmtMovement::close() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = THE_APP->getCncControl();
	bool ret = true;
	
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		ret = false;
	}
	else
 {
		
		// remove the remains form the async move commands
		if ( cnc->popSerial() == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Call popSerial() failed!") << std::endl;
			ret = false;
		}
		
		// deactivate the hardware again
		if ( cnc->processCommand(CMD_DEACTIVATE_PODIUM_HW, std::cout) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Can't deactivate podium hardware!") << std::endl;
			ret = false;
		}
	}
	
	if ( caller )
		caller->podiumNotifyClose(ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
double CncPodiumMgmtMovement::evaluateMillimeterToMove() {
///////////////////////////////////////////////////////////////////
	double dh;
	if ( m_moveRelative->GetValue().ToDouble(&dh) )
		return dh;
	
	std::cerr << CNC_LOG_FUNCT_A(" conversion to double failed\n");
	return 0.0;
}
///////////////////////////////////////////////////////////////////
int32_t CncPodiumMgmtMovement::evaluateStepsToMove() {
///////////////////////////////////////////////////////////////////
	const double mmtm = evaluateMillimeterToMove();
	const int32_t ret = THE_CONFIG->convertMetricToStepsH(mmtm);
	
	if ( false )
	{
		std::cout 	<< "calcFactH      : " << THE_CONFIG->getCalculationFactH()	<< std::endl
					<< "getStepsH()    : " << THE_CONFIG->getStepsH()			<< std::endl
					<< "getPitchH()    : " << THE_CONFIG->getPitchH()			<< std::endl
					<< "mm2Move        : " << mmtm								<< std::endl
					<< "steps2Move     : " << ret								<< std::endl
		; 
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::onPodiumRelativeUp(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const int32_t stm = evaluateStepsToMove();
	if ( stm == 0 )
	{
		wxRichToolTip tip("Information missing", "No distance information available!");
		
		tip.SetIcon(wxICON_ERROR);
		tip.SetTipKind(wxTipKind_TopLeft);
		tip.ShowFor(m_moveRelative);
		
		return;
	}
	
	CncControl* cnc = THE_APP->getCncControl();
	if ( cnc == NULL )
	{
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	enable(false);
	
		const bool exact = true;
		if ( cnc->processMovePodium(stm * (+1), exact) == false )
			std::cerr  << CNC_LOG_FUNCT_A(": processMovePodium() failed !") << std::endl;
	
	enable(true);
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::onPodiumRelativeDown(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	const int32_t stm = evaluateStepsToMove();
	if ( stm == 0 )
	{
		wxRichToolTip tip("Information missing", "No distance information available!");
		
		tip.SetIcon(wxICON_ERROR);
		tip.SetTipKind(wxTipKind_TopLeft);
		tip.ShowFor(m_moveRelative);
		
		return;
	}
	
	CncControl* cnc = THE_APP->getCncControl();
	if ( cnc == NULL ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	enable(false);
	
		const bool exact = true;
		if ( cnc->processMovePodium(stm * (-1), exact) == false )
			std::cerr  << CNC_LOG_FUNCT_A(": processMovePodium() failed !") << std::endl;
	
	enable(true);
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::onPodiumRelativeStop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	//CNC_PRINT_LOCATION
	
	CncControl* cnc = THE_APP->getCncControl();
	if ( cnc == NULL ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	if ( cnc->isConnected() == false )
	{
		std::cerr << CNC_LOG_FUNCT_A(": Not connected!") << std::endl;
		return;
	}
	
	cnc->sendQuitMove();
	
	// remove all inbound remnants to have a clear situation 
	// before the next movement
	THE_APP->waitActive(500);
	cnc->popSerial(true);
								
	enable(true);
}
///////////////////////////////////////////////////////////////////
void CncPodiumMgmtMovement::onLeftDownDistance(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->secureModeInfo.isActive == true )
	{
		event.Skip(false);
		
		CncSecureNumpadDialog dlg(this, CncSecureNumpad::Type::DOUBLE, 3, -100.0, 100.0);
		dlg.setValue(m_moveRelative->GetValue());
		dlg.setInfo("Distance:");
		dlg.Center(wxCENTRE_ON_SCREEN);
		
		if ( dlg.ShowModal() == wxID_OK ) 
			m_moveRelative->ChangeValue(wxString::Format("%.3lf", dlg.getValueAsDouble()));
			
		return;
	}
	
	event.Skip();
}

