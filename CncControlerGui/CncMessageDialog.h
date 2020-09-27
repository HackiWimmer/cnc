#ifndef CNCMESSAGEDIALOG_H
#define CNCMESSAGEDIALOG_H

#include "wxCrafterMessageDialog.h"

class CncMessageDialog : public CncMessageDialogBase {
	
	public:
	
		CncMessageDialog(wxWindow* parent, const wxString& msg, wxString headline="", wxString title="");
		virtual ~CncMessageDialog();

		virtual void setMessage(const wxString& msg);
		
		void setTitle(const wxString& t);
		void setHeadline(const wxString& hl);
		void setWordWrap(bool state);

	protected:
		virtual void onInitDialog(wxInitDialogEvent& event);
		virtual void onWordWrap(wxCommandEvent& event);
		virtual void close(wxCommandEvent& event);
		
		void setupDefaultStyle();
};

class CncFileContentDialog : public CncMessageDialog {
	
	public:
	
		CncFileContentDialog(wxWindow* parent, const wxString& fileName, wxString headline="", wxString title="");
		virtual ~CncFileContentDialog();
		
		void setFileName(const wxString& fileName);
		
		void scrollToEOF();
};

#endif // CNCMESSAGEDIALOG_H
