#include <wx/clipbrd.h>

#include "wxCrafterMotionMonitor.h"
#include "GlobalFunctions.h"
#include "CncNumberFormatter.h"
#include "CncControl.h"
#include "CncMotionMonitor.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncMonitorReplayPane.h"
#include "CncStatisticsPane.h"

///////////////////////////////////////////////////////////////////
CncStatisticsPane::CncStatisticsPane(wxWindow* parent) 
: CncStatisticsPaneBase(parent)
, cnc(NULL)
, motionMonitor(NULL)
, replayControl(NULL)
, statisticSummaryListCtrl(NULL)
///////////////////////////////////////////////////////////////////
{
	// statistic summary
	statisticSummaryListCtrl = new CncStatisticSummaryListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_statisticSummaryListCtrl, statisticSummaryListCtrl);
	
	// replay control
	replayControl = new CncMonitorReplayPane(this);
	GblFunc::replaceControl(m_replayPlaceholder, replayControl);
}
///////////////////////////////////////////////////////////////////
CncStatisticsPane::~CncStatisticsPane() {
///////////////////////////////////////////////////////////////////
	delete statisticSummaryListCtrl;
	delete replayControl;
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::setCncControl(CncControl* c) {
///////////////////////////////////////////////////////////////////
	cnc 			= c;
	wxASSERT(cnc != NULL);
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::setMotionMonitor(CncMotionMonitor* mm) {
///////////////////////////////////////////////////////////////////
	motionMonitor	= mm;
	
	wxASSERT(motionMonitor != NULL);
	wxASSERT(replayControl != NULL);
	replayControl->setMotionMonitor(motionMonitor);
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::updateReplayPane() {
///////////////////////////////////////////////////////////////////
	wxASSERT(replayControl != NULL);
	replayControl->updateControls();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::clear() {
///////////////////////////////////////////////////////////////////
	statisticSummaryListCtrl->resetValues();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::logStatistics(bool force) {
///////////////////////////////////////////////////////////////////
	if ( force == false ) {
		if ( m_checkBoxStatisticUpdate->IsChecked() == false )
			return;
			
		if ( statisticSummaryListCtrl->IsShownOnScreen() == false )
			return;
	}
		
	if ( cnc == NULL )
		return;
		
	CncDoublePosition min(cnc->getMinPositionsMetric());
	CncDoublePosition max(cnc->getMaxPositionsMetric());
	CncNanoTimespan measurementTimeSpan = cnc->getMeasurementNanoTimeSpanTotal();
	
	double elapsedTimeMSEC	= 0.0;
	double elapsedTimeSEC	= 0.0;
	double speed_MM_MIN		= 0.0;
	double speed_MM_SEC		= 0.0;
	
	if ( measurementTimeSpan > 0L ) {
		elapsedTimeMSEC = measurementTimeSpan / (1000.0 * 1000.0);
		elapsedTimeSEC  = elapsedTimeMSEC / (1000.0);
		speed_MM_SEC 	= cnc->getTotalDistanceMetric() / elapsedTimeSEC;
		speed_MM_MIN 	= speed_MM_SEC * 60;
	}

	static wxString speedMMMIN(_maxSpeedLabel), speedMMSEC(_maxSpeedLabel), speedSPSEC(_maxSpeedLabel), speedRPM(_maxSpeedLabel);
	
	bool setupSpeedValue = THE_CONTEXT->isProbeMode() == false;
	if ( cnc->isEmulator() == false )
		setupSpeedValue = true;

	if ( setupSpeedValue ) {
		speedMMMIN.assign(CncNumberFormatter::toString(speed_MM_MIN, 1));
		speedMMSEC.assign(CncNumberFormatter::toString(speed_MM_SEC, 1));
	}
	
	// statistic keys
	static const char* SKEY_MIN_BOUND	= "Boundaries - Min";
	static const char* SKEY_MAX_BOUND	= "Boundaries - Max";
	static const char* SKEY_STEP_CNT	= "Step count";
	static const char* SKEY_DISTANCE 	= "Distance";
	static const char* SKEY_TIME 		= "Time consumend";
	static const char* SKEY_SPEED 		= "Feed speed AVG";
	
	// add rows - ones a time
	if ( statisticSummaryListCtrl->getItemCount() == 0 ) {
		statisticSummaryListCtrl->addKey(SKEY_MIN_BOUND, 	"X, Y, Z", 				"mm");
		statisticSummaryListCtrl->addKey(SKEY_MAX_BOUND, 	"X, Y, Z", 				"mm");
		statisticSummaryListCtrl->addKey(SKEY_STEP_CNT, 	"Total, X, Y, Z", 		"steps");
		statisticSummaryListCtrl->addKey(SKEY_DISTANCE, 	"Total, X, Y, Z", 		"mm");
		statisticSummaryListCtrl->addKey(SKEY_TIME, 		"Total, Stepping", 		"ms");
		statisticSummaryListCtrl->addKey(SKEY_SPEED, 		"mm/sec, mm/min", 		"mm/unit");
	}
	
	// update statistic
	statisticSummaryListCtrl->updateValues(SKEY_MIN_BOUND	, _("")
															, CncNumberFormatter::toString(min.getX(), 3)
															, CncNumberFormatter::toString(min.getY(), 3)
															, CncNumberFormatter::toString(min.getZ(), 3));
	
	statisticSummaryListCtrl->updateValues(SKEY_MAX_BOUND	, _("")
															, CncNumberFormatter::toString(max.getX(), 3)
															, CncNumberFormatter::toString(max.getY(), 3)
															, CncNumberFormatter::toString(max.getZ(), 3));
	
	statisticSummaryListCtrl->updateValues(SKEY_STEP_CNT	, CncNumberFormatter::toString(cnc->getTotalDistanceSteps())
															, CncNumberFormatter::toString(cnc->getTotalDistanceStepsX())
															, CncNumberFormatter::toString(cnc->getTotalDistanceStepsY())
															, CncNumberFormatter::toString(cnc->getTotalDistanceStepsZ()));
															
	statisticSummaryListCtrl->updateValues(SKEY_DISTANCE	, CncNumberFormatter::toString((double)(cnc->getTotalDistanceMetric()),  3)
															, CncNumberFormatter::toString((double)(cnc->getTotalDistanceMetricX()), 3)
															, CncNumberFormatter::toString((double)(cnc->getTotalDistanceMetricY()), 3)
															, CncNumberFormatter::toString((double)(cnc->getTotalDistanceMetricZ()), 3));
	
	statisticSummaryListCtrl->updateValues(SKEY_TIME		, _("")
															, _("")
															, CncNumberFormatter::toString((double)(THE_CONTEXT->timestamps.getTotalDurationMillis()), 1)
															, CncNumberFormatter::toString(elapsedTimeMSEC, 1));
	
	statisticSummaryListCtrl->updateValues(SKEY_SPEED		, _("")
															, _("")
															, speedMMSEC
															, speedMMMIN);
															
	statisticSummaryListCtrl->Refresh();
	statisticSummaryListCtrl->Update();
}
