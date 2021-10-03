#ifndef CNCSECURESLIDEPAD_H
#define CNCSECURESLIDEPAD_H

#include <vector>
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
		
		CncSecureSlidepad(wxWindow* parent);
		virtual ~CncSecureSlidepad();
		
		typedef std::vector<int> SliderValues;
		
		bool	setValues(const SliderValues& list, int index);
		void	setInfo(const wxString& info);
		
		void	setCallbackInterface(CallbackInterface* inf) { caller = inf; }
		
		int		getValue() const;
		
	protected:
		virtual void onScrollChanged(wxScrollEvent& event);
		
	private:
		SliderValues		sliderValues;
		CallbackInterface*	caller;
		
		void prepareScrollbar();
		void updateResult();
		
		int findValue(int value) const;
};

#endif // CNCSECURESLIDEPAD_H
