#ifndef CNCCONNECTPROGRESS_H
#define CNCCONNECTPROGRESS_H

#include "wxcrafter.h"
#include "Codelite/wxPNGAnimation.h"
#include "MainFrame.h"

class CncConnectProgress : public CncConnectProgressBase
{
	public:
		CncConnectProgress(MainFrame* parent);
		virtual ~CncConnectProgress();
		
	protected:
		virtual void startupTimer(wxTimerEvent& event);
		virtual void observeTimer(wxTimerEvent& event);
		virtual void initDialog(wxInitDialogEvent& event);
		virtual void show(wxShowEvent& event);
		
	private:
		MainFrame*	parentFrame;
		wxPNGAnimation* pngAnimation;
		
		void createAnimationControl();
		void startAnimationControl();
		void stopAnimationControl();
};
#endif // CNCCONNECTPROGRESS_H
