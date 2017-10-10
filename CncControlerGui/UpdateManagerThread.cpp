#include <iostream>
#include "MainFrame.h"
#include "UpdateManagerThread.h"

///////////////////////////////////////////////////////////////////
UpdateManagerThread::UpdateManagerThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler(handler)
, cncConfig()
, setterList()
, setterCounter(0)
, enabled(false)
, queueReset(false)
, exit(false)
, eventQueue()
///////////////////////////////////////////////////////////////////
{}
///////////////////////////////////////////////////////////////////
UpdateManagerThread::~UpdateManagerThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	pHandler->updateManagerThread = NULL;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::enableDisplay(bool state) {
///////////////////////////////////////////////////////////////////
	enabled = state;
	
	// reset the current config pointer, 
	// it will be restored by the nex post...() call
	cncConfig = NULL;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode UpdateManagerThread::Entry() {
///////////////////////////////////////////////////////////////////
	unsigned int sleep = 1;
	
	while ( !TestDestroy() ) {
		this->Sleep(sleep);
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		
		// break on exit
		if ( exit == true ) break;
			
		// check if something should be done
		if ( enabled == true ) {
			while ( eventQueue.size() > 0 ) {
				this->Sleep(sleep);
				
				// recheck this here after the sleep
				if ( TestDestroy() ) break;
				
				// break on exit
				if ( exit == true ) break;
				
				pop();
				
				checkQueueReset();
				immediateUpdate();
				postHeartbeat();
			}
			
			checkQueueReset();
			immediateUpdate();
		}
		
		postHeartbeat();
	}
	
	wxCommandEvent evt(wxEVT_UPDATE_MANAGER_THREAD_COMPLETED);
	pHandler->AddPendingEvent(evt);
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postHeartbeat() {
///////////////////////////////////////////////////////////////////
	static long counter = 0;
	if ( ++counter%400 == 0 ) {
		
		wxCommandEvent evt(wxEVT_UPDATE_MANAGER_THREAD_UPDATE);
		pHandler->AddPendingEvent(evt);
		
		counter = 0;
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::checkQueueReset() {
///////////////////////////////////////////////////////////////////
	if ( queueReset == false )
		return;
		
	eventQueue.clear();
	
	wxDataViewListCtrl* ctl = pHandler->GetPositionSpy();
	if ( ctl ) 
		ctl->DeleteAllItems();
		
	queueReset = false;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postEvent(const UpdateManagerThread::Event& evt) {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	switch ( evt.type ) {
		case Event::Type::EMPTY_UPD:	// do noting
										break;
										
		case Event::Type::QUEUE_RESET:	queueReset = true;
										break;
										
		case Event::Type::COMMAND_UPD:	lastCmdEvent 				= evt;
										lastCmdEvent.processed		= false;
										break;
										
		case Event::Type::APP_POS_UPD:	lastAppPosEvent 			= evt;
										lastAppPosEvent.processed	= false;
										eventQueue.push(evt); 
										break;
										
		case Event::Type::CTL_POS_UPD:	lastCtlPosEvent				= evt;
										lastCtlPosEvent.processed	= false;
										break;
										
		default: 						;
	}
	
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::immediateUpdate() {
///////////////////////////////////////////////////////////////////
	updateCmdInfo();
	updateAppPosition();
	updateCtlPosition();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::pop() {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	
	Event evt = eventQueue.pop();
	switch ( evt.type ) {
		case Event::Type::APP_POS_UPD:	updatePositionSpy(evt); break;
		default: ;
		
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateCmdInfo() {
///////////////////////////////////////////////////////////////////
	if ( lastCmdEvent.processed == true )
		return;

	pHandler->getCmdCounterControl()->ChangeValue(wxString::Format(wxT("%i"),  lastCmdEvent.cmd.counter));
	pHandler->getCmdDurationControl()->ChangeValue(wxString::Format(wxT("%i"), lastCmdEvent.cmd.duration));
	
	lastCmdEvent.processed = true;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateAppPosition() {
///////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;
		
	if ( lastAppPosEvent.processed == true )
		return;
		
	double displayFactX = cncConfig->getDisplayFactX(cncConfig->getUnit());
	double displayFactY = cncConfig->getDisplayFactY(cncConfig->getUnit());
	double displayFactZ = cncConfig->getDisplayFactZ(cncConfig->getUnit());
	
	wxString formatString(" %4.3f");
	// this presupposes that displayFactX = 1.0 always also valid for y and z ( 1.0 means steps)
	if ( cnc::dblCompare(displayFactX, 1.0) )
		formatString.assign(" %8.0f");
	
	// application position
	pHandler->getAppPosControlX()->ChangeValue(wxString::Format(formatString, lastAppPosEvent.pos.curr.getX() * displayFactX));
	pHandler->getAppPosControlY()->ChangeValue(wxString::Format(formatString, lastAppPosEvent.pos.curr.getY() * displayFactY));
	pHandler->getAppPosControlZ()->ChangeValue(wxString::Format(formatString, lastAppPosEvent.pos.curr.getZ() * displayFactZ));
	
	// Z view 
	if ( pHandler->getZView() ) 
		pHandler->getZView()->updateView(lastAppPosEvent.pos.curr.getZ() * displayFactZ, *cncConfig);
		
	lastAppPosEvent.processed = true;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateCtlPosition() {
///////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;
		
	if ( lastCtlPosEvent.processed == true )
		return;
		
	double displayFactX = cncConfig->getDisplayFactX(cncConfig->getUnit());
	double displayFactY = cncConfig->getDisplayFactY(cncConfig->getUnit());
	double displayFactZ = cncConfig->getDisplayFactZ(cncConfig->getUnit());
	
	wxString formatString(" %4.3f");
	// this presupposes that displayFactX = 1.0 always also valid for y and z ( 1.0 means steps)
	if ( cnc::dblCompare(displayFactX, 1.0) )
		formatString.assign(" %8.0f");
	
	// application position
	pHandler->getCtlPosControlX()->ChangeValue(wxString::Format(formatString, lastCtlPosEvent.pos.curr.getX() * displayFactX));
	pHandler->getCtlPosControlY()->ChangeValue(wxString::Format(formatString, lastCtlPosEvent.pos.curr.getY() * displayFactY));
	pHandler->getCtlPosControlZ()->ChangeValue(wxString::Format(formatString, lastCtlPosEvent.pos.curr.getZ() * displayFactZ));
	
	lastCtlPosEvent.processed = true;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updatePositionSpy(UpdateManagerThread::Event evt) {
///////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;
		
	double displayFactX = cncConfig->getDisplayFactX(cncConfig->getUnit());
	double displayFactY = cncConfig->getDisplayFactY(cncConfig->getUnit());
	double displayFactZ = cncConfig->getDisplayFactZ(cncConfig->getUnit());
	
	wxString formatString(" %4.3f");
	// this presupposes that displayFactX = 1.0 always also valid for y and z ( 1.0 means steps)
	if ( cnc::dblCompare(displayFactX, 1.0) )
		formatString.assign(" %8.0f");
		
	wxDataViewListCtrl* ctl = pHandler->GetPositionSpy();
	if ( ctl ) {
		DcmRow row;
		DataControlModel::addPositionSpyRow(row, evt.pos.id, evt.pos.speedMode,
											wxString::Format(formatString, evt.pos.curr.getX() * displayFactX),
											wxString::Format(formatString, evt.pos.curr.getY() * displayFactY),
											wxString::Format(formatString, evt.pos.curr.getZ() * displayFactZ));
											
		ctl->InsertItem(0, row);
		ctl->EnsureVisible(ctl->RowToItem(0));
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postClearPositionSpy() {
///////////////////////////////////////////////////////////////////
	if ( pHandler->GetPositionSpy() ) {
		pHandler->GetPositionSpy()->DeleteAllItems();
		pHandler->GetPositionSpy()->Refresh();
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postConfigUpdate(CncConfig* config) {
///////////////////////////////////////////////////////////////////
	if ( config == NULL )
		return;
		
	cncConfig = config;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postResetZView() {
///////////////////////////////////////////////////////////////////
	if ( pHandler->getZView() ) 
		pHandler->getZView()->resetAll();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postUpdateZView() {
///////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;
 
	if ( pHandler->getZView() ) {
		double displayFactZ = cncConfig->getDisplayFactZ(cncConfig->getUnit());
		pHandler->getZView()->updateView(lastAppPosEvent.pos.curr.getZ() * displayFactZ, * cncConfig);
	}

}















///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postSetterValue(unsigned char id,  int32_t value) {
///////////////////////////////////////////////////////////////////
	setterList.push_back(std::make_pair(id, value));
	setterCounter = setterCounter%UINT_MAX + 1;
	
	if ( setterList.size() > maxSetterEntries )
		setterList.erase(setterList.begin());
}





///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateConfigurationControls() {
///////////////////////////////////////////////////////////////////
	return;
	
	
	if ( cncConfig == NULL )
		return;
		
	// in this case is nothing to do
	// and a continious update will be avoided
	if ( cncConfig->isModified() == false )
		return;
		
	// discard here because the control update follows below
	cncConfig->discardModifications();
	
	// speed control
	CncSpeedView* sc = pHandler->getSpeedView();
	if ( sc != NULL ) {
		sc->setCurrentSpeedX(cncConfig->getSpeedX());
		sc->setCurrentSpeedY(cncConfig->getSpeedY());
		sc->setCurrentSpeedZ(cncConfig->getSpeedZ());
	}
	
	// config controls
	wxDataViewListCtrl* scc = pHandler->getStaticCncConfigControl();
	wxDataViewListCtrl* dcc = pHandler->getDynamicCncConfigControl();
	
	// static config
	wxVector<wxVector<wxVariant>> tmpRows;
	if ( scc != NULL ) {
		// get the config
		cncConfig->getStaticValues(tmpRows);
		wxVector<wxVector<wxVariant>> rows;
		// make a deep copy to be thread safe
		{
			wxCriticalSectionLocker enter(pHandler->pThreadCS);
			rows = tmpRows;
		}
		
		scc->Enable(false);
		scc->Freeze();
		scc->DeleteAllItems();
		
		for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
			wxVector<wxVariant> row = *it;
			scc->AppendItem(row);
		}
		
		scc->Thaw();
		scc->Enable(true);
	}

	// dynamic config
	tmpRows.clear();
	if ( dcc != NULL ) {
		// get the config
		cncConfig->getDynamicValues(tmpRows);
		wxVector<wxVector<wxVariant>> rows;
		// make a deep copy to be thread safe
		{
			wxCriticalSectionLocker enter(pHandler->pThreadCS);
			rows = tmpRows;
		}
		
		dcc->Enable(false);
		dcc->Freeze();
		dcc->DeleteAllItems();
		
		for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it) {
			wxVector<wxVariant> row = *it;
			dcc->AppendItem(row);
		}
		
		dcc->Thaw();
		dcc->Enable(true);
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateSetterControls() {
///////////////////////////////////////////////////////////////////
	return;
	
	wxDataViewListCtrl* ps = pHandler->getProcessedSetterControl();
	
	// make a deep copy to be thread safe
	SetterList sl;
	unsigned int sc = 0;
	{
		wxCriticalSectionLocker enter(pHandler->pThreadCS);
		sl = setterList;
		sc = setterCounter;
	}
	
	// in this case nothing is do to
	// and a continious update will be avoided
	if ( sl.size() == (unsigned int)ps->GetItemCount() )
		return;
		
	DcmItemList rows;
	std::string retVal;
	unsigned int cnt = sc - sl.size();
	for ( auto it = sl.begin(); it != sl.end(); ++it ) {
		unsigned char id 	= std::get<0>(*it);
		int32_t value 		= std::get<1>(*it);
		DataControlModel::addNumKeyValueRow(rows, (++cnt), ArduinoPIDs::getPIDLabel((int)id, retVal), value);
	}
	
	if ( ps != NULL ) {
		ps->Enable(false);
		ps->Freeze();
		ps->DeleteAllItems();
		
		for (wxVector<wxVector<wxVariant>>::iterator it = rows.begin(); it != rows.end(); ++it)
			ps->AppendItem(*it);
		
		int itemCount = ps->GetItemCount();
		ps->EnsureVisible(ps->RowToItem(itemCount - 1));
		ps->Thaw();
		ps->Enable(true);
	}
}