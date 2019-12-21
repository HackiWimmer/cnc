#ifndef CNCCONNECTPROGRESS_H
#define CNCCONNECTPROGRESS_H

#include "wxCrafterBaseDialogs.h"
#include "Codelite/wxPNGAnimation.h"

class CncConnectProgress : public CncConnectProgressBase
{
	public:
		CncConnectProgress(wxWindow* parent);
		virtual ~CncConnectProgress();
		
	protected:
		virtual void startupTimer(wxTimerEvent& event);
		virtual void observeTimer(wxTimerEvent& event);
		virtual void initDialog(wxInitDialogEvent& event);
		virtual void show(wxShowEvent& event);
		
	private:
		wxPNGAnimation* pngAnimation;
		unsigned int 	observeCounter;
		
		void createAnimationControl();
		void startAnimationControl();
		void stopAnimationControl();
};
#endif // CNCCONNECTPROGRESS_H
