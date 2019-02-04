#include "CncMotionMonitor.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncMonitorReplayPane.h"

///////////////////////////////////////////////////////////////////
CncMonitorReplayPane::Decorate::Decorate(CncMonitorReplayPane* p) 
: pane(p)
///////////////////////////////////////////////////////////////////
{
	THE_APP->enableControls(false);
	
	THE_APP->GetBtnEmergenyStop()->Enable(false);
	THE_APP->GetRcPause()->Enable(false);
	THE_APP->GetRcStop()->Enable(false);
}
///////////////////////////////////////////////////////////////////
CncMonitorReplayPane::Decorate::~Decorate() {
///////////////////////////////////////////////////////////////////
	pane->GetReplayStart()->Enable(true);
	pane->GetReplayPrev()->Enable(true);
	pane->GetReplayNext()->Enable(true);
	pane->GetReplayEnd()->Enable(true);
	pane->GetReplayPlay()->Enable(true);
	pane->GetReplayPause()->Enable(true);
	pane->GetReplayStop()->Enable(true);
	
	THE_APP->enableControls(true);
}


///////////////////////////////////////////////////////////////////
CncMonitorReplayPane::CncMonitorReplayPane(wxWindow* parent)
: CncMonitorReplayPaneBase(parent)
, abort(false)
, processing(false)
, motionMonitor(NULL)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncMonitorReplayPane::~CncMonitorReplayPane() {
///////////////////////////////////////////////////////////////////
	abort = true;
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::updateControls() {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
	
	GetCurrentVertexId()->ChangeValue(wxString::Format("%ld", motionMonitor->getVirtualEndAsId()));
	GetCurrentVertex()->ChangeValue(wxString::Format("%ld", motionMonitor->getVirtualEnd()));
	GetTotalVerties()->ChangeValue(wxString::Format("%ld", motionMonitor->getPathItemCount()));
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::notifyCncPathChanged() {
///////////////////////////////////////////////////////////////////
	if ( THE_APP->isProcessing() == true )
		return;
	
	updateControls();
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::setMotionMonitor(CncMotionMonitor* mm) {
///////////////////////////////////////////////////////////////////
	motionMonitor = mm;
	wxASSERT(motionMonitor != NULL);
	
	motionMonitor->registerMonitorCallback(this);
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::display() {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	THE_APP->tryToSelectClientId(motionMonitor->getVirtualEndAsId());
	
	motionMonitor->setCurrentClientId(motionMonitor->getVirtualEndAsId());
	motionMonitor->display();
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayPlay(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
	
	Processing p(this);
	DecoratePlay dp(this);
	
	abort = false;

	long start = motionMonitor->getVirtualEnd();
	if ( start >= motionMonitor->getPathItemCount() - 1 )
		start = 0;
	
	motionMonitor->setVirtualEnd(start);
	while ( motionMonitor->getVirtualEnd() < motionMonitor->getPathItemCount() - 1 ) {
		
		const bool fine = GetCbStepUnit()->GetSelection() != 0;
		if ( fine )	motionMonitor->incVirtualEnd();
		else		motionMonitor->incVirtualEndById();
		
		display();
			
		THE_APP->dispatchAll();
		
		if ( abort == true )
			break;
	}
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayPause(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	abort = true; 
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayStop(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	abort = true;
	 
	if ( motionMonitor == NULL )
		return;
	
	motionMonitor->setVirtualEndToLast();
	display();
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayStart(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
		
	motionMonitor->setVirtualEndToFirst();
	display();
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayLeftDownPrev(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	Processing p(this);
	DecoratePrev dp(this);
	
	abort = false;
	int count = 0;
	while( decrement(GetCbStepUnit()->GetSelection() != 0) ) {
		
		if      ( count < 1 ) { count++;	wxThread::This()->Sleep(350); }
		else if ( count < 2 ) { count++;	wxThread::This()->Sleep(150); }
		else if ( count < 3 ) { count++;	wxThread::This()->Sleep( 50); }
		
		THE_APP->dispatchAll();
	}
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayLeftDownNext(wxMouseEvent& event) {
///////////////////////////////////////////////////////////////////
	Processing p(this);
	DecorateNext dn(this);
	
	abort = false;
	int count = 0;
	while( increment(GetCbStepUnit()->GetSelection() != 0) ) {
		
		if      ( count < 1 ) { count++;	wxThread::This()->Sleep(350); }
		else if ( count < 2 ) { count++;	wxThread::This()->Sleep(150); }
		else if ( count < 3 ) { count++;	wxThread::This()->Sleep( 50); }
		
		THE_APP->dispatchAll();
	}
}
///////////////////////////////////////////////////////////////////
void CncMonitorReplayPane::replayEnd(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return;
	
	if ( motionMonitor->getPathItemCount() <= 0 )
		return;
		
	motionMonitor->setVirtualEndToLast();
	display();
}

///////////////////////////////////////////////////////////////////
bool CncMonitorReplayPane::increment(bool fine) {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return false;
	
	if ( abort == true )
		return false;
	
	const long idx = motionMonitor->getVirtualEnd();
	if ( idx <= 0 )
		return false;
		
	if ( idx >= motionMonitor->getPathItemCount() )
		motionMonitor->setVirtualEnd(1);
		
	// increment
	if ( fine == true ) 	motionMonitor->incVirtualEnd();
	else 					motionMonitor->incVirtualEndById();
	
	display();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncMonitorReplayPane::decrement(bool fine) {
///////////////////////////////////////////////////////////////////
	if ( motionMonitor == NULL )
		return false;
	
	if ( abort == true )
		return false;
		
	const long idx = motionMonitor->getVirtualEnd();
	
	if ( idx <= 1 )
		return false;
	
	// decrement
	if ( fine == true )		motionMonitor->decVirtualEnd();
	else					motionMonitor->decVirtualEndById();
	
	display();
	
	return true;
}

