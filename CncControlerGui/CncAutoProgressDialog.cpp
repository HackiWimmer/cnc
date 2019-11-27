#include <wx/evtloop.h>
#include "CncCommon.h"
#include "CncAutoProgressDialog.h"

/////////////////////////////////////////////////////////////
CncAutoProgressDialog::CncAutoProgressDialog(wxWindow* parent)
: CncAutoProgressDialogBase(parent)
, animationFileName("memory:XRC_resource/wxCrafterBaseDialogs_bitmaps.cpp$bitmaps_loading-throb.gif")
/////////////////////////////////////////////////////////////
{
	wxFileSystem fsys;
	wxFSFile* filePointer = fsys.OpenFile(animationFileName);
	if ( filePointer ) {
		if ( m_animationCtrl->Load(*(filePointer->GetStream())) )
			m_animationCtrl->Play();
		else
			std::cerr << CNC_LOG_FUNCT << " Can't load animation resource" << std::endl;
	} else {
		std::cerr << CNC_LOG_FUNCT << " Can't find animation resource" << std::endl;
	}
}
/////////////////////////////////////////////////////////////
CncAutoProgressDialog::~CncAutoProgressDialog() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncAutoProgressDialog::onInitDialog(wxInitDialogEvent& event) {
/////////////////////////////////////////////////////////////
	Update();
}
///////////////////////////////////////////////////////////// 
void CncAutoProgressDialog::onShow(wxShowEvent& event) {
/////////////////////////////////////////////////////////////
	//std::cout << event.IsShown() << std::endl;
}
