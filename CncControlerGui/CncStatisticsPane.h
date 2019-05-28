#ifndef CNCSTATISTICSPANE_H
#define CNCSTATISTICSPANE_H

#include "wxcrafter.h"
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
		
		CncControl* 					cnc;
		CncMotionMonitor* 				motionMonitor;
		CncMonitorReplayPane*			replayControl;
		
		CncStatisticSummaryListCtrl* 	statisticSummaryListCtrl;
		
};
#endif // CNCSTATISTICSPANE_H
