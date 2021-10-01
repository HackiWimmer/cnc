#ifndef CNCSECURESLIDEPAD_H
#define CNCSECURESLIDEPAD_H

#include <vector>
#include "wxCrafterSecurePanel.h"

class CncSecureSlidepad : public CncSecureSlidepadBase
{
	public:
		
		class Interface {
		
			public:
				Interface() {}
				virtual ~Interface() {}
				
				virtual void sliderValueChanged(int index, int value) {}
		};
		
		CncSecureSlidepad(wxWindow* parent);
		virtual ~CncSecureSlidepad();
		
		typedef std::vector<int> SliderValues;
		
		bool	setValues(const SliderValues& list, int index);
		void	setInfo(const wxString& info);
		
		void	setCallbackInterface(Interface* inf) { caller = inf; }
		
		int		getValue() const;
		
	protected:
		virtual void onScrollChanged(wxScrollEvent& event);
		
	private:
		SliderValues	sliderValues;
		Interface*		caller;
		
		void prepareScrollbar();
		void updateResult();
		
		int findValue(int value) const;
};

#endif // CNCSECURESLIDEPAD_H
