#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncSecureRotateModelPanel.h"
#include "wxCrafterSecurePanel.h"

///////////////////////////////////////////////////////////////////
CncSecureRotateModelPanel::CncSecureRotateModelPanel(wxWindow* parent)
: CncSecureRotateModelPanelBase	(parent)
, rotateXPanel					(NULL)
, rotateYPanel					(NULL)
, rotateZPanel					(NULL)
///////////////////////////////////////////////////////////////////
{
	const wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
	
	rotateXPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_rotateXPlaceholder, rotateXPanel);
	rotateXPanel->setCallbackId(CBID_ROTATE_X);
	rotateXPanel->SetBackgroundColour(wxColour(128, 128, 255));
	rotateXPanel->SetFont(font);
	rotateXPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	rotateXPanel->init();
	
	rotateYPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_rotateYPlaceholder, rotateYPanel);
	rotateYPanel->setCallbackId(CBID_ROTATE_Y);
	rotateYPanel->SetBackgroundColour(wxColour( 128,   0, 255));
	rotateYPanel->SetFont(font);
	rotateYPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	rotateYPanel->init();
	
	rotateZPanel = new CncSecureGesturesPanel(this, wxVERTICAL, CncSecureGesturesPanel::Type::T_SWITCH, CncSecureGesturesPanel::Mode::M_BOTH, 1);
	GblFunc::replaceControl(m_rotateZPlaceholder, rotateZPanel);
	rotateZPanel->setCallbackId(CBID_ROTATE_Z);
	rotateZPanel->SetBackgroundColour(wxColour( 255,   0, 255));
	rotateZPanel->SetFont(font);
	rotateZPanel->setCenterBitmap(ImageLibSecure().Bitmap("BMP_NAVI_UP_DOWN16"));
	rotateZPanel->init();
	
	Bind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureRotateModelPanel::onModifyModel, this);
}
/////////////////////////////////////////////////////////////////// 
CncSecureRotateModelPanel::~CncSecureRotateModelPanel() {
///////////////////////////////////////////////////////////////////
	Unbind(wxEVT_CNC_SECURE_GESTURES_PANEL, &CncSecureRotateModelPanel::onModifyModel, this);
	
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
		
	//CNC_PRINT_FUNCT_A("eventId=%, callbackId=%d ratio=%lf", event.GetId(), event.data.cbId, event.data.ratio)
	
	switch( event.GetId() )
	{
		//case CncSecureGesturesPanelEvent::Id::CSGP_STARTING:
		//case CncSecureGesturesPanelEvent::Id::CSGP_POS_HELD:
		case CncSecureGesturesPanelEvent::Id::CSGP_POS_CHANGED:
		{
			switch( event.data.cbId )
			{
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
			
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////
void CncSecureRotateModelPanel::reset() {
///////////////////////////////////////////////////////////////////
	rotateXPanel->init();
	rotateYPanel->init();
	rotateZPanel->init();
	
	Refresh();
}
