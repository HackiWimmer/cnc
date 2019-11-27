#ifndef CNCAUTOPROGRESSDIALOG_H
#define CNCAUTOPROGRESSDIALOG_H
#include "wxCrafterBaseDialogs.h"

class CncAutoProgressDialog : public CncAutoProgressDialogBase {
	
	public:
		CncAutoProgressDialog(wxWindow* parent);
		virtual ~CncAutoProgressDialog();
		
	protected:
		virtual void onInitDialog(wxInitDialogEvent& event);
		virtual void onShow(wxShowEvent& event);
		
	private:
		wxString animationFileName;
};
#endif // CNCAUTOPROGRESSDIALOG_H
