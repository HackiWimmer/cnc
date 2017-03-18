#ifndef CNC_TOOL_STATE_CONTROL_H
#define CNC_TOOL_STATE_CONTROL_H


#include <wx/bitmap.h>
#include <wx/statbmp.h>

class CncToolStateControl {
	
	public:
		//////////////////////////////////////////////////////////////////
		enum State {neutral, red, yellow, green};
		
	private:
		const char* trafficLightNeutral 	= "BMP_TRAFFIC_LIGHT_DEFAULT";
		const char* trafficLightRed 		= "BMP_TRAFFIC_LIGHT_RED";
		const char* trafficLightYellow 		= "BMP_TRAFFIC_LIGHT_YELLOW";
		const char* trafficLightGreen 		= "BMP_TRAFFIC_LIGHT_GREEN";
		
		CncToolStateControl::State currentState;
		wxStaticBitmap* control;
	
	public:
		//////////////////////////////////////////////////////////////////
		CncToolStateControl()
		: currentState(neutral)
		, control(NULL)
		{}
		~CncToolStateControl() {}
		
		//////////////////////////////////////////////////////////////////
		void setControl(wxStaticBitmap* ctrl) {
			control = ctrl;
		}
		//////////////////////////////////////////////////////////////////
		void setState(CncToolStateControl::State s);
		//////////////////////////////////////////////////////////////////
		CncToolStateControl::State getState() { 
			return currentState; 
		}
	
};

#endif