#ifndef CNCTEMPLATECONTEXTSUMMARYPANEL_H
#define CNCTEMPLATECONTEXTSUMMARYPANEL_H

#include <vector>
#include "wxCrafterSecurePanel.h"

class CncExtLoggerListCtrl;
class CncTemplateContextSummaryPanel : public CncTemplateContextSummaryPanelBase
{
	private:
	
		typedef std::vector<CncExtLoggerListCtrl*> LoggerRegister;
		
		LoggerRegister			loggerRegister;
		CncExtLoggerListCtrl*	summary;
		CncExtLoggerListCtrl*	list;
		CncExtLoggerListCtrl*	analysis;
		CncExtLoggerListCtrl*	tryRunLogger;
		
		
	protected:
		virtual void onSaveCurrentList(wxCommandEvent& event);
		virtual void onCopyCurrentList(wxCommandEvent& event);
		
	public:
		CncTemplateContextSummaryPanel(wxWindow* parent);
		virtual ~CncTemplateContextSummaryPanel();
		
		void update();
		void selectTryRun();
		
		CncExtLoggerListCtrl* getTryRunLogger() { return tryRunLogger; }
		
};

#endif // CNCTEMPLATECONTEXTSUMMARYPANEL_H
