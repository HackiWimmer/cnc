#ifndef CNCSPEEDSLIDER_H
#define CNCSPEEDSLIDER_H

#include "wxCrafterSpeedMonitor.h"

// -------------------------------------------------------------
class CncValueCtrl {
	
	public:
		
		CncValueCtrl(wxWindow *ctrl) 
		: control(ctrl)
		{}
		
		virtual ~CncValueCtrl()
		{}
		
		bool enable(bool state) {
			if ( control == NULL )
				return false;
				
			return control->Enable(state);
		}
		
		void updateValue(const wxString& value) {
			if ( control == NULL )
				return;
				
			if		( control->IsKindOf(wxCLASSINFO(wxTextCtrl)) )		wxDynamicCast(control, wxTextCtrl)->ChangeValue(value);
			else if ( control->IsKindOf(wxCLASSINFO(wxStaticText)) )	wxDynamicCast(control, wxStaticText)->SetLabel(value);
		}
		
	private:
		wxWindow* control;
};


// -------------------------------------------------------------
class CncSpeedSliderInterface {
	
	protected:
		wxSlider*		slider;
		CncValueCtrl*	sliderValue;
		CncValueCtrl*	sliderUnit;
		wxWindow*		toolTipWindow;
		bool 			bShowvalue;
		
		virtual void updateToolTip();
		virtual void updateControls() = 0;
		
	public:
		CncSpeedSliderInterface(wxSlider* slider, CncValueCtrl* label, CncValueCtrl* unit);
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
		
		void autoConfigure();
		
		int getValueMM_MIN() const;
		int getValueMM_SEC() const;
		
		int getMinValueMM_MIN() const;
		int getMaxValueMM_MIN() const;

		void setToolTipWindow(wxWindow* wnd) { toolTipWindow = wnd; }
};

// -------------------------------------------------------------
class CncDefaultSpeedSlider : public wxEvtHandler, public CncSpeedSliderInterface
{
	protected:
		virtual void onChangedSlider(wxScrollEvent& event);
		virtual void onThumbtrackSlider(wxScrollEvent& event);
		virtual void onThumbreleasekSlider(wxScrollEvent& event);
		
		virtual void updateControls();
		
	public:
		CncDefaultSpeedSlider(wxSlider* slider, CncValueCtrl* label, CncValueCtrl* unit = NULL);
		virtual ~CncDefaultSpeedSlider();
};

#endif // CNCSPEEDSLIDER_H
