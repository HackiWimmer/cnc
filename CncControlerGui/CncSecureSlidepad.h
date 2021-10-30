#ifndef CNCSECURESLIDEPAD_H
#define CNCSECURESLIDEPAD_H

#include <vector>
#include "CncSecureGesturesPanel.h"
#include "wxCrafterSecurePanel.h"

class CncSecureSlidepad : public CncSecureSlidepadBase
{
	public:
		
		class CallbackInterface {
		
			public:
				CallbackInterface() {}
				virtual ~CallbackInterface() {}
				
				virtual void sliderValueChanged(int index, int value) {}
		};
		
		enum Resolution{ ResOnes = 1, ResTens = 10, ResHundreds = 100, ResThousands = 1000};
		
		CncSecureSlidepad(wxWindow* parent);
		virtual ~CncSecureSlidepad();
		
		typedef std::vector<int> SliderValues;
		
		int		getValue() const;

		void	setInfo(const wxString& info);
		
		bool	setValues(const SliderValues& list, int index);
		void	setValueByRatio(float ratio) 					{ slider->setValueByRatio(ratio); }
		
		void	setResolution(Resolution r)						{ resolution = r; }
		void	setCallbackInterface(CallbackInterface* inf)	{ caller = inf; }
		
		void	setShowEndButtons(bool show);
		
	protected:
		virtual void onLeftDownResult(wxMouseEvent& event);
		virtual void onSkipToMax(wxCommandEvent& event);
		virtual void onSkipToMin(wxCommandEvent& event);
		
		void onSliderEvent(CncSecureGesturesPanelEvent& event);
		
	private:
	
		static const int		callbackId = 624;
	
		SliderValues			sliderValues;
		CncSecureGesturesPanel*	slider;
		CallbackInterface*		caller;
		Resolution				resolution;
		
		void updateResult(float ratio);
		int findIndex(int value) const;
};

#endif // CNCSECURESLIDEPAD_H
