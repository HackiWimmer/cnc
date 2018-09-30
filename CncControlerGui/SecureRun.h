#ifndef SECURERUN_H
#define SECURERUN_H
#include "wxcrafter.h"
#include "MainFrame.h"

class SecureRun : public SecureRunBase
{
	public:
		SecureRun(MainFrame* parent);
		virtual ~SecureRun();
		
		void interrupt();
		
	protected:
		virtual void stop(wxCommandEvent& event);
		virtual void run(wxCommandEvent& event);
		virtual void pause(wxCommandEvent& event);
		
		void updateButtonState();
		
	private:
		MainFrame* parentFrame;
};
#endif // SECURERUN_H
