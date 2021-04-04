#ifndef CNC_GAMMEPAD_DIR_PANEL
#define CNC_GAMMEPAD_DIR_PANEL

#include <wx/panel.h>
#include "CncCommon.h"

class CncGamepadDirectionPanel : public wxPanel {
	
	public:
		enum Mode { M_INACTIVE, M_NAVI, M_STICK };
		
	protected:
		
		const wxColour naviColour  = wxColour(247, 240, 213);
		const wxColour stickColour = wxColour(100, 100, 100);

		CncLinearDirection	xDir;
		CncLinearDirection	yDir;
		int					angle;
		Mode				mode;
		wxColour			bckColour;
		wxColour			fgdColour;
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onEraseBackground(wxEraseEvent& event);
	
	public:
		CncGamepadDirectionPanel(wxWindow* parent);
		virtual ~CncGamepadDirectionPanel();
		
		void setMode(Mode m);
		void setDirection(const CncLinearDirection x, const CncLinearDirection y);
		
		wxDECLARE_NO_COPY_CLASS(CncGamepadDirectionPanel);
		wxDECLARE_EVENT_TABLE();

};

#endif