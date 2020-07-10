#include <wx/dcmemory.h>
#include "wxCrafterImages.h"
#include "MainFrameProxy.h"
#include "CncGamepadControllerSpy.h"

///////////////////////////////////////////////////////////////////
CncGamepadControllerSpy::CncGamepadControllerSpy(wxWindow* parent)
: CncGamepadControllerSpyBase(parent)
, tsLastUpdate(0)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncGamepadControllerSpy::~CncGamepadControllerSpy() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::onContinuesTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( CncTimeFunctions::getMilliTimestamp() - tsLastUpdate > 6000 )
		Show(false);
}
//////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::update(const GamepadEvent& state) {
//////////////////////////////////////////////////////////////////
	if ( IsShownOnScreen() == false )
		Show(true);
	
	tsLastUpdate = CncTimeFunctions::getMilliTimestamp();

	// Position mode state
	static wxBitmap aBmpNaviXY     = ImageLibGamepadSpy().Bitmap("BMP_NAVI_XY");
	static wxBitmap aBmpNaviZ      = ImageLibGamepadSpy().Bitmap("BMP_NAVI_Z");
	static wxBitmap aBmpStickLeft  = ImageLibGamepadSpy().Bitmap("BMP_STICK_LEFT");
	static wxBitmap aBmpStickRight = ImageLibGamepadSpy().Bitmap("BMP_STICK_RIGHT");
	static wxBitmap dBmpNaviXY     = ImageLibGamepadSpy().Bitmap("BMP_NAVI_XY").ConvertToDisabled();
	static wxBitmap dBmpNaviZ      = ImageLibGamepadSpy().Bitmap("BMP_NAVI_Z").ConvertToDisabled();
	static wxBitmap dBmpStickLeft  = ImageLibGamepadSpy().Bitmap("BMP_STICK_LEFT").ConvertToDisabled();
	static wxBitmap dBmpStickRight = ImageLibGamepadSpy().Bitmap("BMP_STICK_RIGHT").ConvertToDisabled();
	
	switch ( state.data.posCtrlMode ) {
		case GamepadEvent::PCM_STICKS:
			GetGpBmp1()->SetBitmap(dBmpNaviXY);
			GetGpBmp2()->SetBitmap(dBmpNaviZ);
			GetGpBmp3()->SetBitmap(aBmpStickLeft);
			GetGpBmp4()->SetBitmap(aBmpStickRight);
			break;
			
		case GamepadEvent::PCM_NAV_XY:
			GetGpBmp1()->SetBitmap(aBmpNaviXY);
			GetGpBmp2()->SetBitmap(dBmpNaviZ);
			GetGpBmp3()->SetBitmap(dBmpStickLeft);
			GetGpBmp4()->SetBitmap(dBmpStickRight);
			break;
			
		case GamepadEvent::PCM_NAV_Z:
			GetGpBmp1()->SetBitmap(dBmpNaviXY);
			GetGpBmp2()->SetBitmap(aBmpNaviZ);
			GetGpBmp3()->SetBitmap(dBmpStickLeft);
			GetGpBmp4()->SetBitmap(dBmpStickRight);
			break;
			
		default:
			GetGpBmp1()->SetBitmap(dBmpNaviXY);
			GetGpBmp2()->SetBitmap(dBmpNaviZ);
			GetGpBmp3()->SetBitmap(dBmpStickLeft);
			GetGpBmp4()->SetBitmap(dBmpStickRight);
	}
	
	// Sensitivity
	if ( state.data.buttonA == true ) {
		
		unsigned int sel = m_chStepsSensitivity->GetSelection();
		unsigned int cnt = m_chStepsSensitivity->GetCount();
		
		// select it at the ref pos dlg
		if ( sel + 1 >= cnt ) 	m_chStepsSensitivity->SetSelection(0);
		else					m_chStepsSensitivity->SetSelection(sel +1);
	}
	
	// Position Management
	static wxBitmap bmpCompass_Null = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_NULL");
	static wxBitmap bmpCompass_000  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_000"); 
	static wxBitmap bmpCompass_045  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_045"); 
	static wxBitmap bmpCompass_090  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_090"); 
	static wxBitmap bmpCompass_135  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_135"); 
	static wxBitmap bmpCompass_180  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_180"); 
	static wxBitmap bmpCompass_225  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_225"); 
	static wxBitmap bmpCompass_270  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_270"); 
	static wxBitmap bmpCompass_315  = ImageLibGamepadSpy().Bitmap("BMP_COMPASS_315"); 
	
	if      ( state.data.dx  > 0 && state.data.dy == 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_000);
	else if ( state.data.dx  < 0 && state.data.dy == 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_180);
	else if ( state.data.dx == 0 && state.data.dy  > 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_090);
	else if ( state.data.dx == 0 && state.data.dy  < 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_270);
	else if ( state.data.dx  > 0 && state.data.dy  > 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_045);
	else if ( state.data.dx  < 0 && state.data.dy  > 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_135);
	else if ( state.data.dx  > 0 && state.data.dy  < 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_315);
	else if ( state.data.dx  < 0 && state.data.dy  < 0 )	m_bmpCompassXY->SetBitmap(bmpCompass_225);
	else 													m_bmpCompassXY->SetBitmap(bmpCompass_Null);
	
	if      ( state.data.dz > 0 )							m_bmpCompassZ->SetBitmap(bmpCompass_090);
	else if ( state.data.dz < 0 )							m_bmpCompassZ->SetBitmap(bmpCompass_270);
	else 													m_bmpCompassZ->SetBitmap(bmpCompass_Null);
	
	m_bmpCompassXY->Refresh();
	m_bmpCompassZ->Refresh();
}
//////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::selectSensitivity(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////
	if ( APP_PROXY::isAppPointerAvailable() == true )
		APP_PROXY::GetRbStepSensitivity()->SetSelection(m_chStepsSensitivity->GetSelection());
}
//////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::dclickLeftStick(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::dclickNaviXY(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::dclickNaviZ(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void CncGamepadControllerSpy::dclickRightStick(wxMouseEvent& event) {
//////////////////////////////////////////////////////////////////
}
