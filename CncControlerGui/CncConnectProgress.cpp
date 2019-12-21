#include <iostream>
#include "wxCrafterImages.h"
#include "MainFrameProxy.h"
#include "CncConnectProgress.h"

/////////////////////////////////////////////////////////////////////////
CncConnectProgress::CncConnectProgress(wxWindow* parent)
: CncConnectProgressBase(parent)
, pngAnimation		(NULL)
, observeCounter	(0)
/////////////////////////////////////////////////////////////////////////
{
	m_observeTimer->Stop();
	createAnimationControl();
}
/////////////////////////////////////////////////////////////////////////
CncConnectProgress::~CncConnectProgress() {
/////////////////////////////////////////////////////////////////////////
	stopAnimationControl();
}
/////////////////////////////////////////////////////////////////////////
void CncConnectProgress::initDialog(wxInitDialogEvent& event) {
/////////////////////////////////////////////////////////////////////////
	startAnimationControl();
}
/////////////////////////////////////////////////////////////////////////
void CncConnectProgress::show(wxShowEvent& event) {
/////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////
void CncConnectProgress::startupTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////////
	m_startupTimer->Stop();
	m_observeTimer->Start(800);

	bool ret = APP_PROXY::connectSerialPort();
	EndModal(ret == true ? wxID_OK : wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncConnectProgress::observeTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	m_observeCounterLabel->SetLabel(wxString::Format("%d ", observeCounter));
	
	if ( observeCounter++ > 10 ) {
		m_observeTimer->Stop();
	
		std::cerr << " Connect progress aborted by observer. Timeout reached!" << std::endl;
		EndModal(wxID_CANCEL);
		
	} else {
		long oc = -1;
		m_observeCounterLabel->GetLabel().ToLong(&oc);
		if ( observeCounter != (unsigned int)oc )
			APP_PROXY::dispatchAll();
	}
}
///////////////////////////////////////////////////////////////////
void CncConnectProgress::createAnimationControl() {
///////////////////////////////////////////////////////////////////
	if ( pngAnimation != NULL )
		delete pngAnimation;
		
	pngAnimation = NULL;
	
	wxBitmap bmp = ImageLib16().Bitmap("ANIMATION_SPRITE");
	if ( bmp.IsOk() ) {
		pngAnimation = new wxPNGAnimation(m_connectActivityPanel, bmp, wxHORIZONTAL, wxSize(120,7), 0);
	}
}
///////////////////////////////////////////////////////////////////
void CncConnectProgress::startAnimationControl() {
///////////////////////////////////////////////////////////////////
	if ( pngAnimation != NULL ) {
		if ( pngAnimation->IsRunning() )
			pngAnimation->Stop();
			
		pngAnimation->Start(70);
		pngAnimation->Refresh();
		pngAnimation->Update();
	}
}
///////////////////////////////////////////////////////////////////
void CncConnectProgress::stopAnimationControl() {
///////////////////////////////////////////////////////////////////
	if ( pngAnimation != NULL ) {
		if ( pngAnimation->IsRunning() ) {
			pngAnimation->Stop();
			pngAnimation->Refresh();
			pngAnimation->Update();
		}
	}
}

