#include <wx/clipbrd.h>

#include "GlobalFunctions.h"
#include "CncNumberFormatter.h"
#include "CncControl.h"
#include "CncMotionMonitor.h"
#include "MainFrame.h"
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
, vectiesListCtrl(NULL)
///////////////////////////////////////////////////////////////////
{
	// statistic summary
	statisticSummaryListCtrl = new CncStatisticSummaryListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_statisticSummaryListCtrl, statisticSummaryListCtrl);
	
	// vecties list
	vectiesListCtrl = new CncVectiesListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_vectiesListCtrl, vectiesListCtrl);
	
		// replay control
	replayControl = new CncMonitorReplayPane(this);
	GblFunc::replaceControl(m_replayPlaceholder, replayControl);
}
///////////////////////////////////////////////////////////////////
CncStatisticsPane::~CncStatisticsPane() {
///////////////////////////////////////////////////////////////////
	delete statisticSummaryListCtrl;
	delete vectiesListCtrl;
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
	replayControl->setMotionMonitor(motionMonitor);
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::updateReplayPane() {
///////////////////////////////////////////////////////////////////
	if ( replayControl == NULL )
		return;
		
	replayControl->updateControls();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::clearMotionMonitorVecties(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	vectiesListCtrl->clear();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::copyMotionMonitorVecties(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( vectiesListCtrl->getItemCount() == 0 )
		return;

	// Write some text to the clipboard
	if ( wxTheClipboard->Open() ) {
		THE_APP->startAnimationControl();

		wxString content;
		content.reserve(1024 * 1024);
		
		for ( long i=0; i<vectiesListCtrl->getItemCount(); i++ )
			vectiesListCtrl->getRow(i).trace(content);
		
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		wxTheClipboard->SetData( new wxTextDataObject(content) );
		wxTheClipboard->Close();
		
		THE_APP->stopAnimationControl();
	}
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::traceMotionMonitorVecties(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	std::clog << "Motion Monitor Data - ";
	
	std::stringstream ss;
	motionMonitor->tracePathData(ss);
	
	THE_APP->GetLogger()->Freeze();
	THE_APP->GetLogger()->AppendText(ss.str().c_str());
	THE_APP->GetLogger()->Thaw();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::clear() {
///////////////////////////////////////////////////////////////////
	vectiesListCtrl->clear();
	statisticSummaryListCtrl->resetValues();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::updateVectiesList() {
///////////////////////////////////////////////////////////////////
	long ic = vectiesListCtrl->getItemCount();
	
	if ( vectiesListCtrl->IsFrozen() == false )
		vectiesListCtrl->Freeze();
		
	motionMonitor->fillVectiesListCtr(ic, vectiesListCtrl);
	vectiesListCtrl->SetToolTip(wxString::Format("Item count: %ld", vectiesListCtrl->getItemCount()));

	if ( vectiesListCtrl->IsFrozen() == true )
		vectiesListCtrl->Thaw();
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
		
	if ( vectiesListCtrl->IsShownOnScreen() )
		updateVectiesList();
		
	if ( cnc == NULL )
		return;
		
	CncDoublePosition min(cnc->getMinPositionsMetric());
	CncDoublePosition max(cnc->getMaxPositionsMetric());
	CncNanoTimespan measurementTimeSpan = cnc->getMeasurementNanoTimeSpanTotal();
	
	double elapsedTimeMSEC	= 0.0;
	double elapsedTimeSEC	= 0.0;
	double speed_MM_MIN		= 0.0;
	double speed_MM_SEC		= 0.0;
	long speed_SP_SEC		= 0;
	long speed_RPM			= 0;
	
	if ( measurementTimeSpan > 0L ) {
		elapsedTimeMSEC = measurementTimeSpan / (1000.0 * 1000.0);
		elapsedTimeSEC  = elapsedTimeMSEC / (1000.0);
		speed_MM_SEC 	= cnc->getTotalDistance() / elapsedTimeSEC;
		speed_MM_MIN 	= speed_MM_SEC * 60;
		
		speed_SP_SEC    = cnc->getStepCounter() / elapsedTimeSEC;
		speed_RPM		= (speed_SP_SEC / GBL_CONFIG->getStepsXYZ() ) * 60;
	}

	static wxString speedMMMIN(_maxSpeedLabel), speedMMSEC(_maxSpeedLabel), speedSPSEC(_maxSpeedLabel), speedRPM(_maxSpeedLabel);
	
	bool setupSpeedValue = GBL_CONTEXT->isProbeMode() == false;
	if ( cnc->isEmulator() == false )
		setupSpeedValue = true;

	if (  setupSpeedValue ) {
		speedMMMIN.assign( CncNumberFormatter::toString(speed_MM_MIN, 1));
		speedMMSEC.assign( CncNumberFormatter::toString(speed_MM_SEC, 1));
		speedSPSEC.assign( CncNumberFormatter::toString(speed_SP_SEC));
		speedRPM.assign(   CncNumberFormatter::toString(speed_RPM));
	}
	
	// statistic keys
	static const char* SKEY_MIN_BOUND	= "Boundaries - Min";
	static const char* SKEY_MAX_BOUND	= "Boundaries - Max";
	static const char* SKEY_STEP_CNT	= "Step count";
	static const char* SKEY_POS_CNT		= "Position count";
	static const char* SKEY_DISTANCE 	= "Distance";
	static const char* SKEY_TIME 		= "Time consumend";
	static const char* SKEY_SPEED 		= "Feed speed AVG";
	static const char* SKEY_SPEED_EXT	= "Performance AVG";
	
	// add rows - ones a time
	if ( statisticSummaryListCtrl->getItemCount() == 0 ) {
		statisticSummaryListCtrl->addKey(SKEY_MIN_BOUND, 	"X, Y, Z", 				"mm");
		statisticSummaryListCtrl->addKey(SKEY_MAX_BOUND, 	"X, Y, Z", 				"mm");
		statisticSummaryListCtrl->addKey(SKEY_STEP_CNT, 	"Total, X, Y, Z", 		"steps");
		statisticSummaryListCtrl->addKey(SKEY_POS_CNT, 		"Total", 				"steps");
		statisticSummaryListCtrl->addKey(SKEY_DISTANCE, 	"Total, X, Y, Z", 		"mm");
		statisticSummaryListCtrl->addKey(SKEY_TIME, 		"Total, Stepping", 		"ms");
		statisticSummaryListCtrl->addKey(SKEY_SPEED_EXT, 	"steps/sec, rpm", 		"unit");
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
	
	statisticSummaryListCtrl->updateValues(SKEY_STEP_CNT	, CncNumberFormatter::toString(cnc->getStepCounter())
															, CncNumberFormatter::toString(cnc->getStepCounterX())
															, CncNumberFormatter::toString(cnc->getStepCounterY())
															, CncNumberFormatter::toString(cnc->getStepCounterZ()));
	
	statisticSummaryListCtrl->updateValues(SKEY_POS_CNT		, _("")
															, _("")
															, _("")
															, CncNumberFormatter::toString(cnc->getPositionCounter()));
	
	statisticSummaryListCtrl->updateValues(SKEY_DISTANCE	, CncNumberFormatter::toString((double)(cnc->getTotalDistance()),  3)
															, CncNumberFormatter::toString((double)(cnc->getTotalDistanceX()), 3)
															, CncNumberFormatter::toString((double)(cnc->getTotalDistanceY()), 3)
															, CncNumberFormatter::toString((double)(cnc->getTotalDistanceZ()), 3));
	
	statisticSummaryListCtrl->updateValues(SKEY_TIME		, _("")
															, _("")
															, CncNumberFormatter::toString((double)(THE_APP->getProcessLastDuration()), 1)
															, CncNumberFormatter::toString(elapsedTimeMSEC, 1));
	
	statisticSummaryListCtrl->updateValues(SKEY_SPEED_EXT	, _("")
															, _("")
															, speedSPSEC
															, speedRPM);
	
	statisticSummaryListCtrl->updateValues(SKEY_SPEED		, _("")
															, _("")
															, speedMMSEC
															, speedMMMIN);
															
	statisticSummaryListCtrl->Refresh();
	statisticSummaryListCtrl->Update();
}
///////////////////////////////////////////////////////////////////
void CncStatisticsPane::statisticBookChanged(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////	
	unsigned int sel = event.GetSelection();
	
	if ( (wxWindow*)event.GetEventObject() == m_statisticBook ) {
		switch ( sel ) {
			case StatisticSelection::VAL::SUMMARY_PANEL:
									break;
									
			case StatisticSelection::VAL::VECTIES_PANAL:
									updateVectiesList();
									break;
		}
	} 
}
