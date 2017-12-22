#include <iostream>
#include <wx/intl.h>
#include "CncNumberFormatter.h"
#include "MainFrame.h"
#include "UpdateManagerThread.h"

///////////////////////////////////////////////////////////////////
UpdateManagerThread::UpdateManagerThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler(handler)
, exit(false)
, posSpyContent(UpdateManagerThread::SpyContent::CTL_POSITIONS)
, unit(CncMetric)
, displayFactX(1.0)
, displayFactY(1.0)
, displayFactZ(1.0)
, posSpyQueue()
, posSpyStringQueue()
, setterQueue()
, setterStringQueue()
, lpse()
, posSpyRow(CncPosSpyListCtrl::TOTAL_COL_COUNT)
, lste()
, setterRow(CncSetterListCtrl::TOTAL_COL_COUNT)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
UpdateManagerThread::~UpdateManagerThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	pHandler->updateManagerThread = NULL;
	
	posSpyQueue.reset();
	posSpyStringQueue.reset();
	setterQueue.reset();
	setterStringQueue.reset();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode UpdateManagerThread::Entry() {
///////////////////////////////////////////////////////////////////
	MainFrame::EventId posEvtId = MainFrame::EventId::CTL_POS_UPDATE;
	
/*
 * to sleep with more granullary
	int us = 100; // length of time to sleep, in miliseconds
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = us * 1000L;
	nanosleep(&req, (struct timespec *)NULL);
*/

	unsigned int sleep = 1;
	
	// initialize 
	unit = GBL_CONFIG->getDisplayUnit();
	displayFactX = GBL_CONFIG->getDisplayFactX(unit);
	displayFactY = GBL_CONFIG->getDisplayFactY(unit);
	displayFactZ = GBL_CONFIG->getDisplayFactZ(unit);
	
	wxDateTime tsLast = wxDateTime::UNow();
	
	while ( !TestDestroy() ) {
		this->Sleep(sleep);
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		if ( exit == true )  break;
		
		// --------------------------------------------------------------------
		// format postion spy output
		popAndFormatPosSpyQueue();
		
		// --------------------------------------------------------------------
		// format postion spy output
		// do this at the call of fillSetterList(...) to get a better performanve here
		//popAndFormatSetterQueue();
		
		// --------------------------------------------------------------------
		// process data update
		if ( (wxDateTime::UNow() - tsLast).GetMilliseconds() >= 50 ) {
			UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, posEvtId);
			
			if ( posEvtId == MainFrame::EventId::APP_POS_UPDATE) 	posEvtId = MainFrame::EventId::CTL_POS_UPDATE;
			else													posEvtId = MainFrame::EventId::APP_POS_UPDATE;
			
			wxPostEvent(pHandler, evt);
		}
		
		// --------------------------------------------------------------------
		// process heartbeat
		if ( (wxDateTime::UNow() - tsLast).GetMilliseconds() >= 500 ) {
			UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, MainFrame::EventId::HEARTBEAT);
			wxPostEvent(pHandler, evt);
			
			// debug only
			//clog << posSpyStringQueue.read_available() << ", " << posSpyQueue.read_available() << endl;
			
			tsLast = wxDateTime::UNow();
		}
	}
	
	// post complete event
	UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, MainFrame::EventId::COMPLETED);
	wxPostEvent(pHandler, evt);
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
bool UpdateManagerThread::somethingLeftToDo() {
///////////////////////////////////////////////////////////////////
	return (posSpyStringQueue.read_available() > 0);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postInfo(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_INFO);
	evt.SetString(msg);
	wxPostEvent(pHandler, evt);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postWarning(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_WARNING);
	evt.SetString(msg);
	wxPostEvent(pHandler, evt);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postError(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_ERROR);
	evt.SetString(msg);
	wxPostEvent(pHandler, evt);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::popAndFormatPosSpyQueue() {
///////////////////////////////////////////////////////////////////
	unsigned long count = 0;
	
	while ( posSpyQueue.read_available() ) {
		posSpyQueue.pop(lpse);
		
		switch ( unit ) {
			case CncMetric:		posSpyRow.updateItem(CncPosSpyListCtrl::COL_PID, 	wxString::Format("%d", 		lpse.pos.pid));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_REF, 	wxString::Format("%08ld", 	lpse.pos.id));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_T, 		wxString::Format("%c", 		(char)lpse.pos.speedMode));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_F, 		wxString::Format("%.1lf", 	lpse.pos.speedValue));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_X, 		wxString::Format("%.3lf", 	lpse.pos.pos.getX() * displayFactX));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Y, 		wxString::Format("%.3lf", 	lpse.pos.pos.getY() * displayFactY));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Z, 		wxString::Format("%.3lf", 	lpse.pos.pos.getZ() * displayFactZ));
								break;
								
			case CncSteps:
			default: 			posSpyRow.updateItem(CncPosSpyListCtrl::COL_PID, 	wxString::Format("%d", 		lpse.pos.pid));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_REF, 	wxString::Format("%08ld", 	lpse.pos.id));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_T, 		wxString::Format("%c", 		(char)lpse.pos.speedMode));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_F, 		wxString::Format("%.1lf", 	lpse.pos.speedValue));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_X, 		wxString::Format("%ld", 	lpse.pos.pos.getX()));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Y, 		wxString::Format("%ld", 	lpse.pos.pos.getY()));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Z, 		wxString::Format("%ld", 	lpse.pos.pos.getZ()));
		}
		
		posSpyStringQueue.push(posSpyRow);
		
		if ( count++ > 1024 )
			break;
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::popAndFormatSetterQueue() {
///////////////////////////////////////////////////////////////////
	unsigned long count = 0;
	std::string retVal;
	
	while ( setterQueue.read_available() ) {
		setterQueue.pop(lste);
		unsigned char pid = lste.set.id;

		setterRow.updateItem(CncSetterListCtrl::COL_NUM, 	wxString::Format("%010lu", 		count + 1));
		setterRow.updateItem(CncSetterListCtrl::COL_PID, 	wxString::Format("%u", 			pid));
		
		if ( pid == PID_SEPARATOR ) {
			wxString label("Bookmark: Type(<UNKNOWN>)");
			switch ( lste.set.value ) {
				case SEPARARTOR_SETUP:		label.assign("Bookmark: Type(<SETUP>)"); break;
				case SEPARARTOR_RESET:		label.assign("Bookmark: Type(<RESET>)"); break;
				case SEPARARTOR_RUN:		label.assign("Bookmark: Type(<RUN>)");   break;
			}
			setterRow.updateItem(CncSetterListCtrl::COL_TYPE, 		wxString::Format("%ld", lste.set.value));
			setterRow.updateItem(CncSetterListCtrl::COL_KEY, 		label);
			setterRow.updateItem(CncSetterListCtrl::COL_VAL, 		wxString::Format("%s.%03ld", 	lste.ts.FormatTime(), lste.ts.GetMillisecond()));
			
		} else {
			setterRow.updateItem(CncSetterListCtrl::COL_TYPE, 		"");
			setterRow.updateItem(CncSetterListCtrl::COL_KEY, 		wxString::Format("%s", 			ArduinoPIDs::getPIDLabel((int)pid, retVal)));
			
			if ( pid >= PID_DOUBLE_RANG_START )	
				setterRow.updateItem(CncSetterListCtrl::COL_VAL, 	wxString::Format("%.2lf", 	(double)(lste.set.value/1000)));
			else
				setterRow.updateItem(CncSetterListCtrl::COL_VAL, 	wxString::Format("%ld", 	lste.set.value));
		}
		
		setterStringQueue.push(setterRow);
		
		if ( count++ > 512 )
			break;
	}
}
///////////////////////////////////////////////////////////////////
unsigned int UpdateManagerThread::fillPositionSpy(CncPosSpyListCtrl* lb) {
///////////////////////////////////////////////////////////////////
	if( lb == NULL )
		 return 0;
	
	static const unsigned int MAX_ITEMS = 32000;
	static CncColumContainer posSpyRows[MAX_ITEMS](CncPosSpyListCtrl::TOTAL_COL_COUNT);
	
	unsigned int sizeAvailable = posSpyStringQueue.pop(posSpyRows, MAX_ITEMS);
	
	if ( sizeAvailable > 0 ) {
		if ( lb->appendItems(sizeAvailable, posSpyRows) == false ) {
			postError("UpdateManagerThread::fillPositionSpy(...): Error while append Items!");
		}
	}
	
	return sizeAvailable;
}
///////////////////////////////////////////////////////////////////
unsigned int UpdateManagerThread::fillSetterList(CncSetterListCtrl* lb) {
///////////////////////////////////////////////////////////////////
	if( lb == NULL )
		return 0;
		
	popAndFormatSetterQueue();
		
	static const unsigned int MAX_ITEMS = 4000;
	static CncColumContainer setterRows[MAX_ITEMS](CncSetterListCtrl::TOTAL_COL_COUNT);
	
	unsigned int sizeAvailable = setterStringQueue.pop(setterRows, MAX_ITEMS);
	
	if ( sizeAvailable > 0 ) {
		if ( lb->appendItems(sizeAvailable, setterRows) == false ) {
			postError("UpdateManagerThread::fillSetterList(...): Error while append Items!");
		}
	}
	
	return sizeAvailable;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postEvent(const UpdateManagerThread::Event& evt) {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	
	// the following things will be done immediatly
	switch ( evt.type ) {
											
		case Event::Type::EMPTY_UPD:		// do noting
											break;
											
		case Event::Type::CONFIG_UPD:		// update format factors
											unit = GBL_CONFIG->getDisplayUnit();
											displayFactX = GBL_CONFIG->getDisplayFactX(unit);
											displayFactY = GBL_CONFIG->getDisplayFactY(unit);
											displayFactZ = GBL_CONFIG->getDisplayFactZ(unit);
											
											break;
											
		case Event::Type::SETLST_RESET:		{	// ensure no one else changes the setter list
												wxCriticalSectionLocker lock(pHandler->pThreadCS);
												setterQueue.reset();
												setterStringQueue.reset();
											}
											break;
											
		case Event::Type::POSSPY_RESET:		{	// ensure no one else changes the queues
												wxCriticalSectionLocker lock(pHandler->pThreadCS);
												posSpyQueue.reset();
												posSpyStringQueue.reset();
											}
											break;
											
		case Event::Type::SETTER_ADD:		setterQueue.push(evt);
											break;
											
		case Event::Type::POS_TYP_UPD:		posSpyContent = evt.cnt.posSpyType;
											break;
											
		case Event::Type::APP_POS_UPD:		if ( posSpyContent == UpdateManagerThread::SpyContent::APP_POSITIONS )
												posSpyQueue.push(evt);
											
											break;
											
		case Event::Type::CTL_POS_UPD:		if ( posSpyContent == UpdateManagerThread::SpyContent::CTL_POSITIONS )
												posSpyQueue.push(evt);
											
											break;
											
		default: 							;
	}
}




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

