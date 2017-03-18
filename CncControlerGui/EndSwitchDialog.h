#ifndef ENDSWITCHDIALOG_H
#define ENDSWITCHDIALOG_H


#include "wxcrafter.h"
#include "CncControl.h"

class EndSwitchDialog : public EndSwitchDialogBase
{
	private:
		bool X;
		bool Y;
		bool Z;
		
		CncControl* cnc;
		
	public:
		EndSwitchDialog(wxWindow* parent, CncControl* c);
		virtual ~EndSwitchDialog();
		
	protected:
		virtual void dissolveX(wxCommandEvent& event);
		virtual void dissolveY(wxCommandEvent& event);
		virtual void dissolveZ(wxCommandEvent& event);
		virtual void selectX(wxCommandEvent& event);
		virtual void selectY(wxCommandEvent& event);
		virtual void selectZ(wxCommandEvent& event);
		virtual void cancle(wxCommandEvent& event);

		void configureControls();
		void evaluateState();
};
#endif // ENDSWITCHDIALOG_H
