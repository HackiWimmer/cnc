#ifndef CNCSECURESCROLLBUTTON_H
#define CNCSECURESCROLLBUTTON_H

#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/timer.h>
#include "wxCrafterSecurePanel.h"


class CncSecureScrollButton : public wxPanel
{
	public:
		class CallbackInterface;
		typedef CallbackInterface CBI;
		
		class CallbackInterface {
			
			public:
			
				struct Data {
					int id;
					int currValue;
					int prevValue;
					
					Data()						: id(0), currValue(0), prevValue(0) {}
					Data(int i, int c, int p)	: id(i), currValue(c), prevValue(p) {}
					
					bool changed() const { return currValue != prevValue; }
				};
				
				CallbackInterface() {}
				virtual ~CallbackInterface() {}
				
				virtual void notifyValueChange(const CncSecureScrollButton::CBI::Data& d) {}
		};
		
		enum Mode { M_NEGATIVE, M_POSITIVE, M_BOTH };
		
		CncSecureScrollButton(wxWindow* parent, wxOrientation = wxHORIZONTAL, Mode mode = M_BOTH, int sensitivity = 3, wxSize minSize = wxSize(50,50));
		virtual ~CncSecureScrollButton();
		
		void setCallbackInterface(CallbackInterface* inf, int id=0);
		
	protected:
		void onChangedValue(wxScrollEvent& event);
		void onSkipValue(wxScrollEvent& event);
		void onThumbtrack(wxScrollEvent& event);
		void onThumbrelease(wxScrollEvent& event);
		void onLeaveWindow(wxMouseEvent& event);
		void onUpdateTimer(wxTimerEvent& event);
		
	private:
		wxScrollBar*			scrollbar;
		CallbackInterface*		caller;
		wxTimer*				updateTimer;
		Mode					mode;
		int						callbackId;
		int						zeroIndex;
		int						prevValue;
		bool					released;
		bool					skipped;
		
		int		getCurrentValue();
		void	publish(bool continuous);
		void	reset();
};

#endif // CNCSECURESCROLLBUTTON_H
