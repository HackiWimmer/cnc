#ifndef SECURERUN_H
#define SECURERUN_H
#include "wxcrafter.h"
#include "MainFrame.h"

class SecureRun : public SecureRunBase
{
	public:
		SecureRun(MainFrame* parent);
		virtual ~SecureRun();
		
		void enableControls(bool state);
		
	protected:
		virtual void blinkTimer(wxTimerEvent& event);
		virtual void startupTimer(wxTimerEvent& event);
		
		virtual void initDialog(wxInitDialogEvent& event);
		
		virtual void show(wxShowEvent& event);
		
		virtual void play(wxCommandEvent& event);
		virtual void emergengy(wxCommandEvent& event);
		virtual void stop(wxCommandEvent& event);
		
	private:
		MainFrame* parentFrame;
		bool       isPause;
		bool       headerFlag;
		
		void hideDialog(int retValue);
};
#endif // SECURERUN_H
