#include "MainFrameProxy.h"
#include "GlobalFunctions.h"
#include "CncControl.h"
#include "CncContext.h"
#include "CncSecureNumpadDialog.h"
#include "CncPodestMgmtMovement.h"

///////////////////////////////////////////////////////////////////
CncPodestMgmtMovement::CncPodestMgmtMovement(wxWindow* parent)
: CncPodestMgmtMovementBase					(parent)
, CncSecureScrollButton::CallbackInterface	()
, direction									(CncNoneDir)
, interactiveMove							(NULL)
, caller									(NULL)
///////////////////////////////////////////////////////////////////
{
	interactiveMove = new CncSecureScrollButton(this, wxVERTICAL,CncSecureScrollButton::Mode::M_BOTH, 1, wxSize(-1,200));
	GblFunc::replaceControl(m_intactiveMovePlaceholder, interactiveMove);
	interactiveMove->setCallbackInterface(this);
	interactiveMove->SetBackgroundColour(*wxYELLOW);
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0.0, THE_CONFIG->getMaxDimensionH());
	val.SetPrecision(3);
	m_moveRelative->SetValidator(val);
	
	m_moveRelative->ChangeValue("0.000");
}
///////////////////////////////////////////////////////////////////
CncPodestMgmtMovement::~CncPodestMgmtMovement() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::notifyValueChange(const CncSecureScrollButton::CBI::Data& d) {
///////////////////////////////////////////////////////////////////
	// interactiveMove callback
	const CncLinearDirection prevDirection = direction;
	
	if ( d.currValue == 0 )	direction = CncNoneDir;
	else					direction = d.currValue < 0 ? CncNegDir : CncPosDir;
	
	if ( prevDirection != direction )
		process();
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::enable(bool state) {
///////////////////////////////////////////////////////////////////
	m_btRelativeUp	->Enable(state);
	m_btRelativeDown->Enable(state);
	
	if ( caller )
		caller->podestNotifyEnable(state);
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::reset() {
///////////////////////////////////////////////////////////////////
	if ( direction == CncNoneDir )
		return;
		
	direction = CncNoneDir;
	process();
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::process() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	const bool testOnly = false;
	if ( testOnly ) {
		switch ( direction ) {
			case CncNegDir:		std::cout << "Neg:   " << (int32_t)direction << std::endl; break;
			case CncPosDir:		std::cout << "Pos:   " << (int32_t)direction << std::endl; break;
			case CncNoneDir:	std::cout << "Quite: " << std::endl; break;
		}
	}
	else
	{
		bool ret = true;
		
		switch ( direction ) {
			case CncNegDir:
			case CncPosDir:		ret = cnc->processMovePodest((int32_t)direction, false);
								break;
								
			case CncNoneDir:	ret = cnc->sendQuitMove();
								break;
		}
		
		if ( ret == false ) 
			std::cerr << CNC_LOG_FUNCT_A(": Cnc processing failed! Direction = %d", (int)direction) << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
bool CncPodestMgmtMovement::init() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	bool ret = true;
	
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		ret = false;
	}

	if ( ret == true ) {
		if ( cnc->processCommand(CMD_ACTIVATE_PODEST_HW, std::cout) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Can't activate podest hardware!") << std::endl;
			ret = false;
		}
	}
	
	if ( caller )
		caller->podestNotifyInit(ret);
			
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncPodestMgmtMovement::close() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	bool ret = true;
	
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		ret = false;
	}
	else {
		
		// remove the remains form the async move commands
		if ( cnc->popSerial() == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Call popSerial() failed!") << std::endl;
			ret = false;
		}
		
		// deactivate the hardware again
		if ( cnc->processCommand(CMD_DEACTIVATE_PODEST_HW, std::cout) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Can't deactivate podest hardware!") << std::endl;
			ret = false;
		}
	}
	
	if ( caller )
		caller->podestNotifyInit(ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
double CncPodestMgmtMovement::evaluateMillimeterToMove() {
///////////////////////////////////////////////////////////////////
	double dh;
	if ( m_moveRelative->GetValue().ToDouble(&dh) )
		return dh;
	
	std::cerr << CNC_LOG_FUNCT_A(" conversion to double failed\n");
	return 0.0;
}
///////////////////////////////////////////////////////////////////
int32_t CncPodestMgmtMovement::evaluateStepsToMove() {
///////////////////////////////////////////////////////////////////
	const double mmtm = evaluateMillimeterToMove();
	return THE_CONFIG->convertMetricToStepsH(mmtm);
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::onPodestRelativeUp(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	enable(false);
	
		if ( cnc->processMovePodest(evaluateStepsToMove() * (+1), true) == false )
			std::cerr  << CNC_LOG_FUNCT_A(": processMovePodest() failed !") << std::endl;
	
	enable(true);
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::onPodestRelativeDown(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	enable(false);
	
		if ( cnc->processMovePodest(evaluateStepsToMove() * (-1), true) == false )
			std::cerr  << CNC_LOG_FUNCT_A(": processMovePodest() failed !") << std::endl;
	
	enable(true);
}
///////////////////////////////////////////////////////////////////
void CncPodestMgmtMovement::onLeftDownDistance(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->secureModeInfo.isActive == true ) {
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
