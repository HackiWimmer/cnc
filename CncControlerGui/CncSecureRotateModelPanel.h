#ifndef CNCSECUREROTATEMODELPANEL_H
#define CNCSECUREROTATEMODELPANEL_H

#include "CncSecureGesturesPanel.h"
#include "wxCrafterSecurePanel.h"

class CncSecureGesturesPanel;

class CncSecureRotateModelPanel : public CncSecureRotateModelPanelBase
{
	public:
		CncSecureRotateModelPanel(wxWindow* parent);
		virtual ~CncSecureRotateModelPanel();
		
		void reset();
		
	protected:
		void onModifyModel(CncSecureGesturesPanelEvent& event);
		
	private:
		
		const static int CBID_ZOOM		= 1110;
		const static int CBID_ROTATE_X	= 1111;
		const static int CBID_ROTATE_Y	= 1112;
		const static int CBID_ROTATE_Z	= 1113;
	
		CncSecureGesturesPanel* zoomPanel;
		CncSecureGesturesPanel* rotateXPanel;
		CncSecureGesturesPanel* rotateYPanel;
		CncSecureGesturesPanel* rotateZPanel;
};

#endif // CNCSECUREROTATEMODELPANEL_H
