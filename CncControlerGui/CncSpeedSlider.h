#ifndef CNCSPEEDSLIDER_H
#define CNCSPEEDSLIDER_H

#include "wxCrafterSpeedMonitor.h"

// -------------------------------------------------------------
class CncSpeedSliderInterface {
	
	protected:
		wxSlider*		slider;
		wxTextCtrl*		sliderValue;
		wxStaticText*	sliderUnit;
		wxWindow*		toolTipWindow;
		bool 			bShowvalue;
		
		virtual void updateToolTip();
		virtual void updateControls() = 0;
		
	public:
		CncSpeedSliderInterface(wxSlider* slider, wxTextCtrl* label, wxStaticText* unit);
		virtual ~CncSpeedSliderInterface();
		
		void enable(bool state);
		
		void showUnit(bool state);
		void showValue(bool state);
		
		void setRange(int min, int max);
		void setValue(int value);
		void setValue(float value);
		void setValue(double value);
		
		void previewValue(int value);
		void previewValue(float value);
		void previewValue(double value);
		void synchronize();
		
		void autoConfigure();
		
		int getValueMM_MIN();
		int getValueMM_SEC();
		
		void setToolTipWindow(wxWindow* wnd) { toolTipWindow = wnd; }
};

// -------------------------------------------------------------
class CncDefaultSpeedSlider : public wxEvtHandler, public CncSpeedSliderInterface
{
	private: 
		int 	loggedSliderValue;
		
	protected:
		virtual void onChangedSlider(wxScrollEvent& event);
		virtual void onThumbtrackSlider(wxScrollEvent& event);
		virtual void onThumbreleasekSlider(wxScrollEvent& event);
		
		virtual void updateControls();
		
	public:
		CncDefaultSpeedSlider(wxSlider* slider, wxTextCtrl* label, wxStaticText* unit = NULL);
		virtual ~CncDefaultSpeedSlider();
		
		void logValue();
		void unlogValue();
		void restoreValue();
};

#endif // CNCSPEEDSLIDER_H
