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
		CncExtLoggerListCtrl*	analysisOverall;
		CncExtLoggerListCtrl*	analysisLimit;
		CncExtLoggerListCtrl*	analysisMovement;
		CncExtLoggerListCtrl*	dryRunLogger;
		CncExtLoggerListCtrl*	parsingSynopsis;
		
		void selectPage(CncExtLoggerListCtrl* page) const;
		
	protected:
		virtual void onSaveCurrentList(wxCommandEvent& event);
		virtual void onCopyCurrentList(wxCommandEvent& event);
		
	public:
		CncTemplateContextSummaryPanel(wxWindow* parent);
		virtual ~CncTemplateContextSummaryPanel();
		
		void update();
		void selectSummary()						const { selectPage(summary); }
		void selectDryRun()							const { selectPage(dryRunLogger); }
		void selectParsingSynopsis()				const { selectPage(parsingSynopsis); }
		
		CncExtLoggerListCtrl* getDryRunLogger()		const { return dryRunLogger; }
		CncExtLoggerListCtrl* getParsingSynopsis()	const { return parsingSynopsis; }
		
};

#endif // CNCTEMPLATECONTEXTSUMMARYPANEL_H
