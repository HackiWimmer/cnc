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
	// othwerwise this negative effects the leave window callback
	m_btUp	->SetToolTip("");
	m_btDown->SetToolTip("");
}
///////////////////////////////////////////////////////////////////
CncPodestManagement::~CncPodestManagement() {
///////////////////////////////////////////////////////////////////
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
		if ( cnc->processCommand(CMD_DEACTIVATE_PODEST_HW, std::cout) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": Can't deactivate podest hardware!") << std::endl;
		}
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
		case CncPosDir:		ret = cnc->processMovePodest((int32_t)direction);
							break;
							
		case CncNoneDir:	ret = cnc->sendQuitMove();
							break;
	}
	
	if ( ret == false ) 
		std::cerr << CNC_LOG_FUNCT_A(": Cnc processing failed! Direction = %d", (int)direction) << std::endl;
}


