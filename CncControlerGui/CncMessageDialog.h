#ifndef CNCMESSAGEDIALOG_H
#define CNCMESSAGEDIALOG_H

#include "wxcrafter.h"

class CncMessageDialog : public CncMessageDialogBase {
	
	public:
	
		CncMessageDialog(wxWindow* parent, const wxString& msg, wxString headline="", wxString title="");
		virtual ~CncMessageDialog();
		
		void setTitle(const wxString& t);
		void setHeadline(const wxString& hl);
		void setMessage(const wxString& msg);

	protected:
		virtual void onWordWrap(wxCommandEvent& event);
		virtual void close(wxCommandEvent& event);
};
#endif // CNCMESSAGEDIALOG_H
