#ifndef CNC_SPEED_VIEW
#define CNC_SPEED_VIEW

#include <wx/panel.h>
#include <wx/dcclient.h>
#include "CncConfig.h"

class CncSpeedView : public wxPanel  {
	
	protected:
	
		enum RefreshType {RT_ALL, RT_X, RT_Y, RT_Z};
		
		const unsigned int topMargin = 4;
		const unsigned int bottomMargin = 10;
		const unsigned int space = 2;
		
		unsigned int maxSpeed;
		unsigned int speedX;
		unsigned int speedY;
		unsigned int speedZ;
		
		wxFont font;
		
		wxColour colourX;
		wxColour colourY;
		wxColour colourZ;
		
		wxBrush brushX;
		wxBrush brushY;
		wxBrush brushZ;
		
		inline unsigned int convertValue(unsigned int value);
		
		void refresh(RefreshType rt);
		
		inline unsigned int getWidth() 				{ return GetClientSize().GetWidth(); }
		inline unsigned int getGraduationWidth() 	{ return 20; }
		inline unsigned int getWidthX() 			{ return (getWidth() - getGraduationWidth()) / 3 - 2 * space; }
		inline unsigned int getWidthY() 			{ return (getWidth() - getGraduationWidth()) / 3 - 2 * space; }
		inline unsigned int getWidthZ() 			{ return (getWidth() - getGraduationWidth()) / 3 - 2 * space; }
		
		
		inline unsigned int getGraduationOffset()	{ return 0; }
		inline unsigned int getOffsetX()			{ return getGraduationOffset()		+ getGraduationWidth() + space; }
		inline unsigned int getOffsetY()			{ return getOffsetX()				+ getWidthX() + space; }
		inline unsigned int getOffsetZ()			{ return getOffsetY()				+ getWidthY() + space; }

		void setMaxSpeed(unsigned int value);
		void updateToolTip();
		
		void OnPaint(wxPaintEvent& event);
		
	public:
		CncSpeedView(wxWindow *parent, wxWindowID id=wxID_ANY);
		virtual ~CncSpeedView();
		
		void setMaxSpeedX(unsigned int value) 		{ setMaxSpeed(value); refresh(RT_ALL); }
		void setMaxSpeedY(unsigned int value) 		{ setMaxSpeed(value); refresh(RT_ALL); }
		void setMaxSpeedZ(unsigned int value) 		{ setMaxSpeed(value); refresh(RT_ALL); }
		
		void setCurrentSpeedX(unsigned int value)	{ speedX = value; updateToolTip(); refresh(RT_X); }
		void setCurrentSpeedY(unsigned int value)	{ speedY = value; updateToolTip(); refresh(RT_Y); }
		void setCurrentSpeedZ(unsigned int value)	{ speedZ = value; updateToolTip(); refresh(RT_Z); }
		
		
		wxDECLARE_EVENT_TABLE();
};


#endif