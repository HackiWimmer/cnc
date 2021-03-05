#ifndef CNC_GAMMEPAD_DIR_PANEL
#define CNC_GAMMEPAD_DIR_PANEL

#include <wx/panel.h>
#include "CncCommon.h"

class CncGamepadDirectionPanel : public wxPanel {
	
	protected:
		
		CncLinearDirection	xDir;
		CncLinearDirection	yDir;
		int					angle;
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onEraseBackground(wxEraseEvent& event);
	
	public:
		CncGamepadDirectionPanel(wxWindow* parent);
		virtual ~CncGamepadDirectionPanel();
		
		void setDirection(const CncLinearDirection x, const CncLinearDirection y);
		
		wxDECLARE_NO_COPY_CLASS(CncGamepadDirectionPanel);
		wxDECLARE_EVENT_TABLE();

};

#endif