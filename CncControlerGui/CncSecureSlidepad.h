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
		
		enum Resulution { ResOnes = 1, ResTens = 10, ResHundreds = 100, ResThousands = 1000};
		
		CncSecureSlidepad(wxWindow* parent);
		virtual ~CncSecureSlidepad();
		
		typedef std::vector<int> SliderValues;
		
		void	setInfo(const wxString& info);
		
		bool	setValues(const SliderValues& list, int index);
		void	setValueByRatio(float ratio) { slider->setValueByRatio(ratio); }
		int		getValue() const;
		
		void	setCallbackInterface(CallbackInterface* inf) { caller = inf; }
		
	protected:
		
		void onSliderEvent(CncSecureGesturesPanelEvent& event);
		
	private:
	
		static const int		callbackId = 624;
	
		SliderValues			sliderValues;
		CncSecureGesturesPanel*	slider;
		CallbackInterface*		caller;
		Resulution				resolution;
		
		void updateResult(float ratio);
		int findValue(int value) const;
};

#endif // CNCSECURESLIDEPAD_H
