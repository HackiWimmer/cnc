#ifndef CNC_Z_VIEW
#define CNC_Z_VIEW

#include <wx/panel.h>
#include "CncConfig.h"

class CncZView : public wxPanel  {
	
	protected:
	
		enum RefreshType {RT_ALL, RT_DATA, RT_WATER_MARKS, RT_GRADUATION};
		
		const double balancedGarity = 0.5;
		const double defaultGravity = 0.8;
		const double noScale 		= 1.0;
		const double defaultScale	= 0.4;
		
		double lowWaterMark;
		double highWaterMark;
		double gravity;
		double scale;
		double value;
		double maxValue;
		double durationThickness;
		unsigned int leftMargin;
		unsigned int rightMargin;
		double label;
		wxFont font;
		wxColour valueColour;
		wxColour graduationColour;
		wxColour posColour;
		wxColour negColour;
		wxColour labelColour;
		wxBrush posBrush;
		wxBrush negBrush;
		wxBrush valueBrush;
		
		void OnPaint(wxPaintEvent& event);
		void OnMouse(wxMouseEvent& event);
		void OnKillFocus(wxFocusEvent& event);
		void OnLeaveWindow(wxMouseEvent& event);
		
		void refresh(RefreshType rt);
		
		inline void setLowWaterMark(double val);
		inline void setHighWaterMark(double val);
		
		inline double getYNull();
		inline double convertValue(double val);
		inline double convertPosition(double val);
		
	public:
		CncZView(wxWindow *parent, wxWindowID id=wxID_ANY);
		virtual ~CncZView();
		
		void resetAll();
		
		// ste the given value and consider thee given config
		void updateView(double value, CncConfig& cc);
		// sets the gravity, allowed value > 0.0 .... 1.0, else default 0.5
		void setGravity(double val);
		// sets zero to height/2
		void setBalancedGravity() { setGravity(balancedGarity); }
		// sets the default gravity
		void setDefaultGravity() { setGravity(defaultGravity); }
		// sets the scale, allowed value  > 0.0 .... 1.0, else default 1.0
		void setScale(double val);
		// set the drawing factor without scaling
		void removeScale() { setScale(noScale); }
		// sets the default scale
		void setDefaultScale() { setScale(defaultScale); }
		
		wxDECLARE_EVENT_TABLE();
};

#endif