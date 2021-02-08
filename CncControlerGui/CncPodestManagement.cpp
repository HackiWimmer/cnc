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
}
///////////////////////////////////////////////////////////////////
CncPodestManagement::~CncPodestManagement() {
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
void CncPodestManagement::onStartupTimer(wxTimerEvent& event) {
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
	Show(false);
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestUpLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	direction = CncPosDir;
	process();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestDownLeftDown(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	direction = CncNegDir;
	process();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestUpLeftUp(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( direction == CncNoneDir )
		return;
		
	direction = CncNoneDir;
	process();
}
///////////////////////////////////////////////////////////////////
void CncPodestManagement::onPodestDownLeftUp(wxMouseEvent& event) {
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

