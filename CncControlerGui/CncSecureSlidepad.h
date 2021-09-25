#ifndef CNCSECURESLIDEPAD_H
#define CNCSECURESLIDEPAD_H

#include <vector>
#include "wxCrafterSecurePanel.h"

class CncSecureSlidepad : public CncSecureSlidepadBase
{
	public:
		CncSecureSlidepad(wxWindow* parent);
		virtual ~CncSecureSlidepad();
		
		typedef std::vector<int> SliderValues;
		
		bool	setValues(const SliderValues& list, int index);
		void	setInfo(const wxString& info);
		
		int		getValue() const;
		
	protected:
		virtual void onScrollChanged(wxScrollEvent& event);
		
	private:
		SliderValues sliderValues;
		
		void prepareScrollbar();
		void updateResult();
		
		int findValue(int value) const;
};

#endif // CNCSECURESLIDEPAD_H
