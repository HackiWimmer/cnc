#include <wx/evtloop.h>
#include "MainFrame.h"
#include "CncCommon.h"
#include "CncAutoProgressDialog.h"

/////////////////////////////////////////////////////////////
CncAutoProgressDialog* CncAutoProgressDialog::theProgesssDialog	= NULL;
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
CncAutoProgressDialog::CncAutoProgressDialog(wxWindow* parent, const wxString& context)
: CncAutoProgressDialogBase		(parent)
, animationFileName				("memory:XRC_resource/wxCrafterBaseDialogs_bitmaps.cpp$bitmaps_loading-throb.gif")
, lastInfo						()
/////////////////////////////////////////////////////////////
{
	wxFileSystem fsys;
	wxFSFile* filePointer = fsys.OpenFile(animationFileName);
	
	if ( filePointer ) 
	{
		if ( m_animationCtrl->Load(*(filePointer->GetStream())) )
			m_animationCtrl->Play();
		else
			std::cerr << CNC_LOG_FUNCT << " Can't load animation resource" << std::endl;
	} 
	else
	{
		std::cerr << CNC_LOG_FUNCT << " Can't find animation resource" << std::endl;
	}
	
	if ( context.IsEmpty() == false )	m_context->SetValue(context);
	else								m_context->Destroy();
	
	// the dialogue width as a function of context
	if ( context.IsEmpty() == false )
	{
		wxClientDC dc(this);
		dc.SetFont(m_context->GetFont());
		const wxSize size = dc.GetTextExtent(context);
		
		SetClientSize(std::min(size.GetWidth() + 10, 600), GetClientSize().GetHeight());
	}
	
	m_infoText->SetValue("");
	theProgesssDialog = this;
	
	Layout();
}
/////////////////////////////////////////////////////////////
CncAutoProgressDialog::~CncAutoProgressDialog() {
/////////////////////////////////////////////////////////////
	theProgesssDialog = NULL;
}
///////////////////////////////////////////////////////////// 
CncAutoProgressDialog* CncAutoProgressDialog::TheProgesssDialog() {
///////////////////////////////////////////////////////////// 
	return theProgesssDialog;
}
///////////////////////////////////////////////////////////// 
void CncAutoProgressDialog::onShow(wxShowEvent& event) {
/////////////////////////////////////////////////////////////
	if ( event.IsShown() == true )	m_continuousTimer->Start(-1);
	else							m_continuousTimer->Stop();
}
/////////////////////////////////////////////////////////////
void CncAutoProgressDialog::onInitDialog(wxInitDialogEvent& event) {
/////////////////////////////////////////////////////////////
	update();
}
/////////////////////////////////////////////////////////////
void CncAutoProgressDialog::onContinuousTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////
	update();
}
/////////////////////////////////////////////////////////////
void CncAutoProgressDialog::update(const char* info) {
/////////////////////////////////////////////////////////////
	if ( info != NULL )
		lastInfo = info;

	m_infoText->ChangeValue(lastInfo);
	Update();
	
	THE_APP->dispatchAll();
}
