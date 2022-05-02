#ifndef CNCAUTOPROGRESSDIALOG_H
#define CNCAUTOPROGRESSDIALOG_H

#include "wxCrafterBaseDialogs.h"

class CncAutoProgressDialog : public CncAutoProgressDialogBase {
	
	public:
		CncAutoProgressDialog(wxWindow* parent, const wxString& context = wxEmptyString);
		virtual ~CncAutoProgressDialog();
		
		static CncAutoProgressDialog* TheProgesssDialog();
		
		void update(const char* info = NULL);
		
	protected:
		virtual void onContinuousTimer(wxTimerEvent& event);
		virtual void onInitDialog(wxInitDialogEvent& event);
		virtual void onShow(wxShowEvent& event);
		
	private:
		static CncAutoProgressDialog* theProgesssDialog;
		
		wxString animationFileName;
		wxString lastInfo;
};

#define UPDATE_PROGRESS_DLG(info) \
	if ( CncAutoProgressDialog::TheProgesssDialog() != NULL ) \
	{ \
		CncAutoProgressDialog::TheProgesssDialog()->update(info); \
	}

#endif // CNCAUTOPROGRESSDIALOG_H
