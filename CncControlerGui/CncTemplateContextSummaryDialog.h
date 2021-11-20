#ifndef CNCTEMPLATECONTEXTSUMMARYDIALOG_H
#define CNCTEMPLATECONTEXTSUMMARYDIALOG_H

#include "wxCrafterSecurePanel.h"

class CncTemplateContextSummaryPanel;
class CncTemplateContextSummaryDialog : public CncTemplateContextSummaryDialogBase
{
	private:
		
		void swapControl();
		
	protected:
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onCloseFromButton(wxCommandEvent& event);
		
	public:
		CncTemplateContextSummaryDialog(wxWindow* parent);
		virtual ~CncTemplateContextSummaryDialog();
		
		void update();

};

#endif // CNCTEMPLATECONTEXTSUMMARYDIALOG_H
