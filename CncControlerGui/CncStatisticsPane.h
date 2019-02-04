#ifndef CNCSTATISTICSPANE_H
#define CNCSTATISTICSPANE_H

#include "wxcrafter.h"

#include "CncVectiesListCtrl.h"
#include "CncStatisticSummaryListCtrl.h"

class CncControl;
class CncMotionMonitor;
class CncMonitorReplayPane;

class CncStatisticsPane : public CncStatisticsPaneBase {
	
	public:
		CncStatisticsPane(wxWindow* parent);
		virtual ~CncStatisticsPane();
		
		void setCncControl(CncControl* c);
		void setMotionMonitor(CncMotionMonitor* mm);
		
		void clear();
		void logStatistics(bool force = true);
		
		void updateReplayPane();
		
	protected:
		virtual void statisticBookChanged(wxNotebookEvent& event);
		virtual void clearMotionMonitorVecties(wxCommandEvent& event);
		virtual void copyMotionMonitorVecties(wxCommandEvent& event);
		virtual void traceMotionMonitorVecties(wxCommandEvent& event);
		
		CncControl* 					cnc;
		CncMotionMonitor* 				motionMonitor;
		CncMonitorReplayPane*			replayControl;
		
		CncStatisticSummaryListCtrl* 	statisticSummaryListCtrl;
		CncVectiesListCtrl* 			vectiesListCtrl;
		
		void updateVectiesList();
		
};
#endif // CNCSTATISTICSPANE_H
