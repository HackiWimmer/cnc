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
		
		void setWordWrap(bool state);

	protected:
    virtual void onInitDialog(wxInitDialogEvent& event);
		virtual void onWordWrap(wxCommandEvent& event);
		virtual void close(wxCommandEvent& event);
		
		
		void setupDefaultStyle();
};
#endif // CNCMESSAGEDIALOG_H
