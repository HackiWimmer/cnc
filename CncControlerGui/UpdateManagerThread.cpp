#include <iostream>
#include "MainFrame.h"
#include "UpdateManagerThread.h"

///////////////////////////////////////////////////////////////////
UpdateManagerThread::UpdateManagerThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler(handler)
, queueReset(false)
, exit(false)
, displayFactX(1)
, displayFactY(1)
, displayFactZ(1)
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
void UpdateManagerThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode UpdateManagerThread::Entry() {
///////////////////////////////////////////////////////////////////
	unsigned int sleep = 1;
	
	displayFactX = GBL_CONFIG->getDisplayFactX(GBL_CONFIG->getDisplayUnit());
	displayFactY = GBL_CONFIG->getDisplayFactY(GBL_CONFIG->getDisplayUnit());
	displayFactZ = GBL_CONFIG->getDisplayFactZ(GBL_CONFIG->getDisplayUnit());
	
	while ( !TestDestroy() ) {
		this->Sleep(sleep);
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		if ( exit == true )  break;
		
		if ( eventQueue.size() > 0 ) {
			
			// it's very important to freeze the gui controlls considered below 
			// during it's content becomes change!
			// Because, the onPaint() event isn't thread safe and the 
			// eventloop.Disptch() call will crash sometimes.
			freezeControls(true);
			
				while ( eventQueue.size() > 0 ) {
					this->Sleep(sleep);
					
					// recheck this here after the sleep
					if ( TestDestroy() ) break;
					if ( exit == true )  break;
					
					pop();
					idle();
					postHeartbeat();
				}
				
			freezeControls(false);
		}
		
		idle();
		
		// always post a heartbeat
		postHeartbeat();
	}
	
	wxCommandEvent evt(wxEVT_UPDATE_MANAGER_THREAD_COMPLETED);
	pHandler->AddPendingEvent(evt);
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::idle() {
///////////////////////////////////////////////////////////////////
	checkQueueReset();
	immediateUpdate();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::freezeControl(wxWindow* ctl, bool onlyHidden) {
///////////////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;
		
	if ( onlyHidden == true && ctl->IsShownOnScreen() == true )
		return;
	
	ctl->Enable(false);
	
	if ( ctl->IsFrozen() == false )
		ctl->Freeze();
	
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::thawControl(wxWindow* ctl) {
///////////////////////////////////////////////////////////////////
	if ( ctl == NULL )
		return;

	if ( ctl->IsFrozen() == true )
		ctl->Thaw();

	ctl->Enable(true);
	ctl->Refresh();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::freezeControls(bool state) {
///////////////////////////////////////////////////////////////////
	if ( state == true ) {
		freezeControl(pHandler->GetPositionSpy(), false);
		freezeControl(pHandler->getProcessedSetterControl(), false);
	} else {
		thawControl(pHandler->GetPositionSpy());
		thawControl(pHandler->getProcessedSetterControl());
	}
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
	clearPositionSpy();
		
	queueReset = false;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postEvent(const UpdateManagerThread::Event& evt) {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	
	// the following things will be done immediatly
	switch ( evt.type ) {
		case Event::Type::EMPTY_UPD:	// do noting
										break;
										
		case Event::Type::CONFIG_UPD:	eventQueue.push(evt); 
										break;
										
		case Event::Type::POSSPY_RESET:	clearPositionSpy();
										break;
										
		case Event::Type::QUEUE_RESET:	queueReset = true;
										break;
										
		case Event::Type::Z_VIEW_RESET:	resetZView();
										break;
										
		case Event::Type::Z_VIEW_UPD:	updateZView();
										break;
										
		case Event::Type::SETTER_ADD:	eventQueue.push(evt); 
										break;
										
		case Event::Type::SPEED_UPD:	lastSpeedEvent				= evt;
										lastSpeedEvent.processed	= false;
										break;
										
		case Event::Type::COMMAND_UPD:	lastCmdEvent 				= evt;
										lastCmdEvent.processed		= false;
										break;
										
		case Event::Type::APP_POS_UPD:	lastAppPosEvent 			= evt;
										lastAppPosEvent.processed	= false;
										break;
										
		case Event::Type::CTL_POS_UPD:	lastCtlPosEvent				= evt;
										lastCtlPosEvent.processed	= false;
										eventQueue.push(evt); 
										break;
										
		default: 						;
	}
	
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::pop() {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	
	const unsigned int portionCount = 32;
	unsigned int counter = 0;
	
	// it's very important to freeze the gui controlls considered below 
	// during it's content becomes change!
	// Because, the onPaint() event isn't thread safe and the 
	// eventloop.Disptch() call will crash sometimes.
	freezeControls(true);
	
	unsigned int prevQueueSize = eventQueue.size();
	
	// do a portion of work
	do {
		
		if ( eventQueue.size() == 0 )
			break;
			
		if ( counter%portionCount == 0 ) {
			if ( prevQueueSize - counter != eventQueue.size() )
				break;
				
			// stay in loop if no new queue entries are arrived
		}
		
		Event evt = eventQueue.pop();
		switch ( evt.type ) {
			case Event::Type::CTL_POS_UPD:	updatePositionSpy(evt); 
											break;
											
			case Event::Type::SETTER_ADD:	updateSetterList(evt);
											break;
											
			case Event::Type::CONFIG_UPD:	configUpdate();
											break;
											
			default: 						; // Waste the event;
		}
		
		counter++;
 
	} while ( true );
	
	freezeControls(false);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::immediateUpdate() {
///////////////////////////////////////////////////////////////////
	updateCmdInfo();
	updateAppPosition();
	updateCtlPosition();
	updateSpeedView();
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
	if ( CncConfig::getGlobalCncConfig() == NULL )
		return;
		
	if ( lastAppPosEvent.processed == true )
		return;
		
	wxString formatString(" %4.3f");
	// this presupposes that displayFactX = 1.0 always also valid for y and z ( 1.0 means steps)
	if ( cnc::dblCompare(displayFactX, 1.0) )
		formatString.assign(" %8.0f");
	
	// application position
	pHandler->getAppPosControlX()->ChangeValue(wxString::Format(formatString, lastAppPosEvent.pos.curr.getX() * displayFactX));
	pHandler->getAppPosControlY()->ChangeValue(wxString::Format(formatString, lastAppPosEvent.pos.curr.getY() * displayFactY));
	pHandler->getAppPosControlZ()->ChangeValue(wxString::Format(formatString, lastAppPosEvent.pos.curr.getZ() * displayFactZ));
	
	// Z view 
	updateZView();
		
	lastAppPosEvent.processed = true;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateCtlPosition() {
///////////////////////////////////////////////////////////////////
	if ( CncConfig::getGlobalCncConfig() == NULL )
		return;
		
	if ( lastCtlPosEvent.processed == true )
		return;
		
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
	if ( CncConfig::getGlobalCncConfig() == NULL )
		return;
		
	wxString formatString(" %4.3f");
	// this presupposes that displayFactX = 1.0 always also valid for y and z ( 1.0 means steps)
	if ( cnc::dblCompare(displayFactX, 1.0) )
		formatString.assign(" %8.0f");
		
	wxDataViewListCtrl* ctl = pHandler->GetPositionSpy();
	if ( ctl ) {
		DcmRow row;
		DataControlModel::addPositionSpyRow(row, evt.pos.id, wxString::Format(" %c", (char)evt.pos.speedMode),
											wxString::Format(formatString, evt.pos.curr.getX() * displayFactX),
											wxString::Format(formatString, evt.pos.curr.getY() * displayFactY),
											wxString::Format(formatString, evt.pos.curr.getZ() * displayFactZ));
		if ( row. size() == 6 ) {
			ctl->InsertItem(0, row);
			ctl->EnsureVisible(ctl->RowToItem(0));
			
			if ( pHandler ) {
				pHandler->GetBtClearPositionSpy()->SetToolTip(wxString::Format("Item count: %d", ctl->GetItemCount()));
			}
		} else {
			std::cerr << "UpdateManagerThread::updatePositionSpy: Invalid row size: " << row.size() << std::endl;
			for ( auto it=row.begin(); it != row.end(); ++it )
				std::cerr << ' ' << it->GetString() << std::endl;
		}
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::clearPositionSpy() {
///////////////////////////////////////////////////////////////////
	// it's very important to freeze the gui controlls considered below 
	// during it's content becomes change!
	// Because, the onPaint() event isn't thread safe and the 
	// eventloop.Disptch() call will crash sometimes.
	freezeControls(true);
	
	if ( pHandler->GetPositionSpy() ) {
		pHandler->GetPositionSpy()->DeleteAllItems();
	}
	
	freezeControls(false);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::resetZView() {
///////////////////////////////////////////////////////////////////
	if ( pHandler->getZView() ) 
		pHandler->getZView()->resetAll();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateZView() {
///////////////////////////////////////////////////////////////////
	if ( CncConfig::getGlobalCncConfig() == NULL )
		return;
 
	if ( pHandler->getZView() ) {
		double displayFactZ = CncConfig::getGlobalCncConfig()->getDisplayFactZ(CncConfig::getGlobalCncConfig()->getDisplayUnit());
		pHandler->getZView()->updateView(lastAppPosEvent.pos.curr.getZ() * displayFactZ);
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateSetterList(UpdateManagerThread::Event evt) {
///////////////////////////////////////////////////////////////////
	wxDataViewListCtrl* ps = pHandler->getProcessedSetterControl();
	if ( ps == NULL )
		return;
		
	unsigned int size = ps->GetItemCount();
		
	if ( size > maxSetterEntries ) {
		ps->DeleteItem(size - 1);
		
		wxVariant c0;
		ps->GetValue(c0, 0, 0);
		size = c0.GetInteger() +1;
	}
		
	std::string retVal;
	DcmItemList rows;
	DataControlModel::addNumKeyValueRow(rows, size, ArduinoPIDs::getPIDLabel((int)evt.set.id, retVal), evt.set.value);
	
	if ( rows[0].size() == 3) {
		ps->InsertItem(0, rows[0]);
		ps->EnsureVisible(ps->RowToItem(0));
	} else {
		std::cerr << "UpdateManagerThread::updateSetterList: Invalid rows size: " << rows.size() << std::endl;
		wxVector<wxVariant> row = rows[0];
		for ( auto it=row.begin(); it != row.end(); ++it )
			std::cerr << ' ' << it->GetString() << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateSpeedView() {
///////////////////////////////////////////////////////////////////
	if ( lastSpeedEvent.processed == true )
		return;

	CncSpeedView* sc = pHandler->getSpeedView();
	if ( sc != NULL )
		sc->setCurrentSpeedXYZ(lastSpeedEvent.spd.xSpeed, lastSpeedEvent.spd.ySpeed, lastSpeedEvent.spd.zSpeed);
	
	lastSpeedEvent.processed = true;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::configUpdate() {
///////////////////////////////////////////////////////////////////
	// currently nothing more todo
	CncConfig::getGlobalCncConfig()->discardModifications();
	
	displayFactX = GBL_CONFIG->getDisplayFactX(CncConfig::getGlobalCncConfig()->getDisplayUnit());
	displayFactY = GBL_CONFIG->getDisplayFactY(CncConfig::getGlobalCncConfig()->getDisplayUnit());
	displayFactZ = GBL_CONFIG->getDisplayFactZ(CncConfig::getGlobalCncConfig()->getDisplayUnit());

	
	
	#warning - move the code below to collectSummary
	/*
	if ( CncConfig::getGlobalCncConfig() == NULL )
		return;
		
	// in this case is nothing to do
	// and a continious update will be avoided
	if ( CncConfig::getGlobalCncConfig()->isModified() == false )
		return;
		
	// discard here because the control update follows below
	CncConfig::getGlobalCncConfig()->discardModifications();

	// config controls
	wxDataViewListCtrl* scc = pHandler->getStaticCncConfigControl();
	wxDataViewListCtrl* dcc = pHandler->getDynamicCncConfigControl();
	wxVector<wxVector<wxVariant>> list;
	
	if ( scc != NULL ) {
		scc->DeleteAllItems();
		
		DataControlModel::addKeyValueRow(list, "Steps (x)", 				(int)CncConfig::getGlobalCncConfig()->getStepsX());
		DataControlModel::addKeyValueRow(list, "Steps (y)", 				(int)CncConfig::getGlobalCncConfig()->getStepsY());
		DataControlModel::addKeyValueRow(list, "Steps (z)", 				(int)CncConfig::getGlobalCncConfig()->getStepsZ());
		DataControlModel::addKeyValueRow(list, "Puls width offset (x)", 	(int)CncConfig::getGlobalCncConfig()->getPulsWidthOffsetX());
		DataControlModel::addKeyValueRow(list, "Puls width offset (y)", 	(int)CncConfig::getGlobalCncConfig()->getPulsWidthOffsetY());
		DataControlModel::addKeyValueRow(list, "Puls width offset (z)", 	(int)CncConfig::getGlobalCncConfig()->getPulsWidthOffsetZ());
		DataControlModel::addKeyValueRow(list, "Pitch (x)", 				CncConfig::getGlobalCncConfig()->getPitchX());
		DataControlModel::addKeyValueRow(list, "Pitch (y)", 				CncConfig::getGlobalCncConfig()->getPitchY());
		DataControlModel::addKeyValueRow(list, "Pitch (z)",					CncConfig::getGlobalCncConfig()->getPitchZ());
		DataControlModel::addKeyValueRow(list, "Multiplier (x)", 			(int)CncConfig::getGlobalCncConfig()->getMultiplierX());
		DataControlModel::addKeyValueRow(list, "Multiplier (y)", 			(int)CncConfig::getGlobalCncConfig()->getMultiplierY());
		DataControlModel::addKeyValueRow(list, "Multiplier (z)", 			(int)CncConfig::getGlobalCncConfig()->getMultiplierZ());
		DataControlModel::addKeyValueRow(list, "Max speed XY", 				(int)CncConfig::getGlobalCncConfig()->getMaxSpeedXY());
		DataControlModel::addKeyValueRow(list, "Rapid speed XY", 			(int)CncConfig::getGlobalCncConfig()->getRapidSpeedXY());
		DataControlModel::addKeyValueRow(list, "Work speed XY", 			(int)CncConfig::getGlobalCncConfig()->getWorkSpeedXY());
		DataControlModel::addKeyValueRow(list, "Max speed Z", 				(int)CncConfig::getGlobalCncConfig()->getMaxSpeedZ());
		DataControlModel::addKeyValueRow(list, "Rapid speed Z", 			(int)CncConfig::getGlobalCncConfig()->getRapidSpeedZ());
		DataControlModel::addKeyValueRow(list, "Work speed Z", 				(int)CncConfig::getGlobalCncConfig()->getWorkSpeedZ());
		
		// 
		for (auto it = list.begin(); it != list.end(); ++it) 
			scc->AppendItem(*it);
	}
	
	list.clear();
	if ( dcc != NULL ) {
		dcc->DeleteAllItems();
		
		DataControlModel::addKeyValueRow(list, "Tool diameter", 			CncConfig::getGlobalCncConfig()->getToolDiameter());
		DataControlModel::addKeyValueRow(list, "Curve lib resolution", 		wxString::Format("%.3lf", CncConfig::getCurveLibResolution()));
		DataControlModel::addKeyValueRow(list, "Max Dimension (X)", 		CncConfig::getGlobalCncConfig()->getMaxDimensionX());
		DataControlModel::addKeyValueRow(list, "Max Dimension (Y)", 		CncConfig::getGlobalCncConfig()->getMaxDimensionY());
		DataControlModel::addKeyValueRow(list, "Max Dimension (Z)", 		CncConfig::getGlobalCncConfig()->getMaxDimensionZ());
		DataControlModel::addKeyValueRow(list, "Step Sign (x)", 			CncConfig::getGlobalCncConfig()->getStepSignX());
		DataControlModel::addKeyValueRow(list, "Step Sign (y)", 			CncConfig::getGlobalCncConfig()->getStepSignY());
		DataControlModel::addKeyValueRow(list, "Reply Threshold", 			(int)CncConfig::getGlobalCncConfig()->getReplyThreshold());
		DataControlModel::addKeyValueRow(list, "Z axis values:", 			"");
		DataControlModel::addKeyValueRow(list, "  Max durations", 			(int)CncConfig::getGlobalCncConfig()->getMaxDurations());
		DataControlModel::addKeyValueRow(list, "  Workpiece offset", 		CncConfig::getGlobalCncConfig()->getWorkpieceOffset());
		DataControlModel::addKeyValueRow(list, "  Max duration thickness",	CncConfig::getGlobalCncConfig()->getMaxDurationThickness());
		DataControlModel::addKeyValueRow(list, "  Calculated durations", 	(int)CncConfig::getGlobalCncConfig()->getDurationCount());
		DataControlModel::addKeyValueRow(list, "  Current Z distance", 		CncConfig::getGlobalCncConfig()->getCurZDistance());
		DataControlModel::addKeyValueRow(list, "  Wpt is included", 		CncConfig::getGlobalCncConfig()->getReferenceIncludesWpt());
		
		for (unsigned int i=0; i<CncConfig::getGlobalCncConfig()->getMaxDurations(); i++) {
			if ( CncConfig::getGlobalCncConfig()->getDurationThickness(i) != 0.0 ) {
				wxString key("  Duration step[");
				key << i;
				key << "]";
				DataControlModel::addKeyValueRow(list, key, CncConfig::getGlobalCncConfig()->getDurationThickness(i));
			}
		}
		
		// append
		for (auto it = list.begin(); it != list.end(); ++it) 
			dcc->AppendItem(*it);
	}
	*/
}
