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
		void flagListItem(CncExtLoggerListCtrl* ctrl, bool flag);
		void resetListFlagging();
		
		wxDECLARE_NO_COPY_CLASS(CncTemplateContextSummaryPanel);
		wxDECLARE_EVENT_TABLE();

	protected:
		
		virtual void onPaint(wxPaintEvent& event);
		virtual void onSaveCurrentList(wxCommandEvent& event);
		virtual void onCopyCurrentList(wxCommandEvent& event);
		
	public:
		CncTemplateContextSummaryPanel(wxWindow* parent);
		virtual ~CncTemplateContextSummaryPanel();
		
		void update(bool force = false);
		
		void selectSummary()						const { selectPage(summary); }
		void selectDryRun()							const { selectPage(dryRunLogger); }
		void selectParsingSynopsis()				const { selectPage(parsingSynopsis); }
		
		CncExtLoggerListCtrl* getDryRunLogger()		const { return dryRunLogger; }
		CncExtLoggerListCtrl* getParsingSynopsis()	const { return parsingSynopsis; }
};

#endif // CNCTEMPLATECONTEXTSUMMARYPANEL_H
