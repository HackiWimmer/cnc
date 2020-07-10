#ifndef CNCSPEEDSLIDER_H
#define CNCSPEEDSLIDER_H

#include "wxCrafterSpeedMonitor.h"

class CncSpeedSlider : public CncSpeedSliderBase {
	
	private:
		bool 		bShowvalue;
		wxWindow*	toolTipWindow;
		
		void updateToolTip();
		
	protected:
		virtual void onChangedSlider(wxScrollEvent& event);
		virtual void onThumbtrackSlider(wxScrollEvent& event);
		virtual void onChangeSlider(wxScrollEvent& event);
		virtual void onPaint(wxPaintEvent& event);
		
	public:
		CncSpeedSlider(wxWindow* parent);
		virtual ~CncSpeedSlider();
		
		void enable(bool state);
		
		void showUnit(bool state);
		void showValue(bool state);
		
		void setRange(int min, int max);
		void setValue(int value);
		void setValue(float value);
		
		void autoConfigure();
		
		int getValueMM_MIN();
		int getValueMM_SEC();
		
		void setToolTipWindow(wxWindow* wnd) { toolTipWindow = wnd; }
		
};

#endif // CNCSPEEDSLIDER_H
