#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncSecureRotateModelPanel.h"
#include "wxCrafterSecurePanel.h"

///////////////////////////////////////////////////////////////////
CncSecureRotateModelPanel::CncSecureRotateModelPanel(wxWindow* parent)
: CncSecureRotateModelPanelBase	(parent)
, zoomPanel						(NULL)
, rotateXPanel					(NULL)
, rotateYPanel					(NULL)
, rotateZPanel					(NULL)
///////////////////////////////////////////////////////////////////
{
	const wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	
	zoomPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_zoomPlaceholder, zoomPanel);
	zoomPanel->setCallbackId(CBID_ZOOM);
	zoomPanel->SetBackgroundColour(wxColour(  0, 128, 255));
	zoomPanel->SetFont(font);
	zoomPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	
	rotateXPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_rotateXPlaceholder, rotateXPanel);
	rotateXPanel->setCallbackId(CBID_ROTATE_X);
	rotateXPanel->SetBackgroundColour(wxColour(128, 128, 255));
	rotateXPanel->SetFont(font);
	rotateXPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	
	rotateYPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_rotateYPlaceholder, rotateYPanel);
	rotateYPanel->setCallbackId(CBID_ROTATE_Y);
	rotateYPanel->SetBackgroundColour(wxColour( 128,   0, 255));
	rotateYPanel->SetFont(font);
	rotateYPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	
	rotateZPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_BUTTON, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_rotateZPlaceholder, rotateZPanel);
	rotateZPanel->setCallbackId(CBID_ROTATE_Z);
	rotateZPanel->SetBackgroundColour(wxColour( 255,   0, 255));
	rotateZPanel->SetFont(font);
	rotateZPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	
	Bind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureRotateModelPanel::onModifyModel, this);
}
/////////////////////////////////////////////////////////////////// 
CncSecureRotateModelPanel::~CncSecureRotateModelPanel() {
///////////////////////////////////////////////////////////////////
	Unbind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureRotateModelPanel::onModifyModel, this);
	
	wxDELETE(zoomPanel);
	wxDELETE(rotateXPanel);
	wxDELETE(rotateYPanel);
	wxDELETE(rotateZPanel);
}
///////////////////////////////////////////////////////////////////
void CncSecureRotateModelPanel::onModifyModel(CncSecureGesturesPanelEvent& event) {
///////////////////////////////////////////////////////////////////
	// may be nothing to to
	if ( event.data.isRatioChanged == false )
		return;
	
	switch( event.GetId() )
	{
		//case CncSecureGesturesPanelEvent::Id::CSGP_STARTING:
		//case CncSecureGesturesPanelEvent::Id::CSGP_POS_HELD:
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_CHANGED:
		{
			switch( event.data.cbId )
			{
				case CBID_ZOOM:
				{
					THE_APP->motionMonitorZoom(event.data.ratio);
					break;
				}
				case CBID_ROTATE_X:
				{
					THE_APP->motionMonitorRotateX(event.data.ratio * 360.0 );
					break;
				}
				case CBID_ROTATE_Y:
				{
					THE_APP->motionMonitorRotateY(event.data.ratio * 360.0 );
					break;
				}
				case CBID_ROTATE_Z:
				{
					THE_APP->motionMonitorRotateZ(event.data.ratio * 360.0 );
					break;
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureRotateModelPanel::reset() {
///////////////////////////////////////////////////////////////////
	#warning really necessary?
	zoomPanel->init();
	rotateXPanel->init();
	rotateYPanel->init();
	rotateZPanel->init();
	
	Refresh();
}
