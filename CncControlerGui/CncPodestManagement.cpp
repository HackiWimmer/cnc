#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include "MainFrameProxy.h"
#include "CncControl.h"
#include "CncPodestManagement.h"

///////////////////////////////////////////////////////////////////
CncPodestManagement::CncPodestManagement(wxWindow* parent)
: CncPodestManagementBase	(parent)
, direction					(CncNoneDir)
///////////////////////////////////////////////////////////////////
{
	// otherwise this negative effects the leave window callback
	m_btManuallyUp	->SetToolTip("");
	m_btManuallyDown->SetToolTip("");
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0.0, THE_CONFIG->getMaxDimensionH());
	val.SetPrecision(3);
	m_moveRelative->SetValidator(val);
	
	m_moveRelative->ChangeValue("0.000");
}
///////////////////////////////////////////////////////////////////
CncPodestManagement::~CncPodestManagement() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::enable(bool state) {
///////////////////////////////////////////////////////////////////
	m_btManuallyUp	->Enable(state);
	m_btManuallyDown->Enable(state);
	
	m_btRelativeUp	->Enable(state);
	m_btRelativeDown->Enable(state);
	
	m_btClose		->Enable(state);
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::showInfo() {
///////////////////////////////////////////////////////////////////
	wxRichToolTip tip(	"Additional information", 
						"While this dialog is shown, the corresponding hardware buttons are active."
					  );
	
	tip.SetIcon(wxICON_INFORMATION);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor(m_info);
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onLefDownInfo(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	showInfo();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onShow(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onInit(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		
		Show(false);
		return;
	}

	if ( cnc->processCommand(CMD_ACTIVATE_PODEST_HW, std::cout) == false ) {
		std::cerr << CNC_LOG_FUNCT_A(": Can't activate podest hardware!") << std::endl;
		
		Show(false);
		return;
	}
	
	showInfo();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onClose(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
	}
	else {
		
		// remove the remains form the asysnc move commands
		if ( cnc->popSerial() == false )
			std::cerr << CNC_LOG_FUNCT_A(": Call popSerial() failed!") << std::endl;
		
		// deactivate the hardware again
		if ( cnc->processCommand(CMD_DEACTIVATE_PODEST_HW, std::cout) == false )
			std::cerr << CNC_LOG_FUNCT_A(": Can't deactivate podest hardware!") << std::endl;
	}
	
	Show(false);
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestUpLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	direction = CncPosDir;
	process();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestDownLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	direction = CncNegDir;
	process();
	event.Skip();
}

///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestUpLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	reset();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestDownLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	reset();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestDownLeave(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	reset();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestUpLeave(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	reset();
	event.Skip();
}
///////////////////////////////////////////////////////////////////
double CncPodestManagement::evaluateMillimeterToMove() {
///////////////////////////////////////////////////////////////////
	double dh;
	if ( m_moveRelative->GetValue().ToDouble(&dh) )
		return dh;
	
	std::cerr << CNC_LOG_FUNCT_A(" conversion to double failed\n");
	return 0.0;
}
///////////////////////////////////////////////////////////////////
int32_t CncPodestManagement::evaluateStepsToMove() {
///////////////////////////////////////////////////////////////////
	const double mmtm = evaluateMillimeterToMove();
	return THE_CONFIG->convertMetricToStepsH(mmtm);
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::reset() {
///////////////////////////////////////////////////////////////////
	if ( direction == CncNoneDir )
		return;
		
	direction = CncNoneDir;
	process();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::process() {
///////////////////////////////////////////////////////////////////
	CncControl* cnc = APP_PROXY::getCncControl();
	if ( cnc == NULL ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid cnc control!") << std::endl;
		return;
	}
	
	//std::cout << direction<< std::endl;
	
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
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestRelativeUp(wxCommandEvent& event) {
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
void CncPodestManagement::onPodestRelativeDown(wxCommandEvent& event) {
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
