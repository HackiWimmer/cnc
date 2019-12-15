#include <iostream>
#include <wx/intl.h>
#include "CncNumberFormatter.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "GlobalFunctions.h"
#include "UpdateManagerThread.h"

////////////////////////////////////////////////////////////////////
extern GlobalConstStringDatabase globalStrings;

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
	for ( unsigned int i=0; i<MAX_POS_SPY_ITEMS; i++)
		posSpyRows[i].initColumnCount(CncPosSpyListCtrl::TOTAL_COL_COUNT);
		
	for ( unsigned int i=0; i<MAX_SETTER_ITEMS; i++)
		setterRows[i].initColumnCount(CncSetterListCtrl::TOTAL_COL_COUNT);
}
///////////////////////////////////////////////////////////////////
UpdateManagerThread::~UpdateManagerThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pUpdateManagerThreadCS);
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
	APPEND_THREAD_IDTO_STACK_TRACE_FILE;
	
	const unsigned int milliseconds = 1;
	MainFrame::EventId posEvtId = MainFrame::EventId::CTL_POS_UPDATE;
	
	// initialize 
	unit 		 = THE_CONFIG->getDisplayUnit();
	displayFactX = THE_CONFIG->getDisplayFactX(unit);
	displayFactY = THE_CONFIG->getDisplayFactY(unit);
	displayFactZ = THE_CONFIG->getDisplayFactZ(unit);
	
	wxDateTime tsLastData      = wxDateTime::UNow();
	wxDateTime tsLastHeartbeat = wxDateTime::UNow();
	
	while ( !TestDestroy() ) {
		this->Sleep(milliseconds);
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		if ( exit == true )  break;
		
		// --------------------------------------------------------------------
		// format postion spy output
		popAndFormatPosSpyQueue();
		
		// --------------------------------------------------------------------
		// process data update
		if ( (wxDateTime::UNow() - tsLastData).GetMilliseconds() >= 100 ) {
			const UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, posEvtId);
			
			if ( posEvtId == MainFrame::EventId::APP_POS_UPDATE) 	posEvtId = MainFrame::EventId::CTL_POS_UPDATE;
			else													posEvtId = MainFrame::EventId::APP_POS_UPDATE;
			
			wxPostEvent(pHandler, evt);
			tsLastData = wxDateTime::UNow();
		}
		
		// --------------------------------------------------------------------
		// process heartbeat
		if ( (wxDateTime::UNow() - tsLastHeartbeat).GetMilliseconds() >= 500 ) {
			const UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, MainFrame::EventId::HEARTBEAT);
			
			wxPostEvent(pHandler, evt);
			tsLastHeartbeat = wxDateTime::UNow();
		}
	}
	
	// post complete event
	wxQueueEvent(pHandler, new UpdateManagerEvent(wxEVT_UPDATE_MANAGER_THREAD, MainFrame::EventId::COMPLETED));
	
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
	static wxString speedInfo("");
	const  wxString rFormat(globalStrings.posSpyRefFormat);
	unsigned long count = 0;
	
	while ( posSpyQueue.read_available() ) {
		posSpyQueue.pop(lpse);
		
		if ( lpse.pos.currentSpeedValue < 0.0 )	speedInfo.assign(wxString::Format("%.1lf / %s",    lpse.pos.configuredSpeedValue, _maxSpeedLabel));
		else									speedInfo.assign(wxString::Format("%.1lf / %.1lf", lpse.pos.configuredSpeedValue, lpse.pos.currentSpeedValue));
		
		switch ( unit ) {
			case CncMetric:		posSpyRow.updateItem(CncPosSpyListCtrl::COL_PID, 	wxString::Format("%d", 			lpse.pos.pid));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_REF, 	wxString::Format(rFormat, 		lpse.pos.id));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_T, 		wxString::Format("%c", 			lpse.pos.speedMode));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_F, 		speedInfo);
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_X, 		wxString::Format("%.3lf", 		lpse.pos.pos.getX() * displayFactX));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Y, 		wxString::Format("%.3lf", 		lpse.pos.pos.getY() * displayFactY));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Z, 		wxString::Format("%.3lf", 		lpse.pos.pos.getZ() * displayFactZ));
								break;
								
			case CncSteps:
			default: 			posSpyRow.updateItem(CncPosSpyListCtrl::COL_PID, 	wxString::Format("%d", 			lpse.pos.pid));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_REF, 	wxString::Format(rFormat, 		lpse.pos.id));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_T, 		wxString::Format("%c", 			lpse.pos.speedMode));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_F, 		speedInfo);
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_X, 		wxString::Format("%ld", 		(long)(lpse.pos.pos.getX())));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Y, 		wxString::Format("%ld", 		(long)(lpse.pos.pos.getY())));
								posSpyRow.updateItem(CncPosSpyListCtrl::COL_Z, 		wxString::Format("%ld", 		(long)(lpse.pos.pos.getZ())));
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
		
		setterRow.updateItem(CncSetterListCtrl::COL_NUM, 	wxString::Format("%06lu", 		count%1000000 + 1));
		setterRow.updateItem(CncSetterListCtrl::COL_PID, 	wxString::Format("%u", 			pid));
		
		if ( pid == PID_SEPARATOR ) {
			wxString label("Bookmark: Type(<UNKNOWN>)");
			wxString portName;
			pHandler->getCurrentPortName(portName);
			
			int32_t value = lste.set.values.front();
			switch ( value ) {
				case SEPARARTOR_SETUP:		label.assign(wxString::Format("Bookmark: Type(<SETUP>[%s])", portName)); break;
				case SEPARARTOR_RESET:		label.assign(wxString::Format("Bookmark: Type(<RESET>[%s])", portName)); break;
				case SEPARARTOR_RUN:		label.assign(wxString::Format("Bookmark: Type(<RUN>[%s])",   portName)); break;
			}
			setterRow.updateItem(CncSetterListCtrl::COL_TYPE, 		wxString::Format("%ld", 		(long)value));
			setterRow.updateItem(CncSetterListCtrl::COL_KEY, 		label);
			setterRow.updateItem(CncSetterListCtrl::COL_VAL, 		wxString::Format("%s.%03ld", 	lste.time.FormatTime(), (long)lste.time.GetMillisecond()));
			
		} else {
			setterRow.updateItem(CncSetterListCtrl::COL_TYPE, 		"");
			setterRow.updateItem(CncSetterListCtrl::COL_KEY, 		wxString::Format("%s", 			ArduinoPIDs::getPIDLabel((int)pid, retVal)));
			setterRow.updateItem(CncSetterListCtrl::COL_UNIT, 		wxString::Format("%s", 			ArduinoPIDs::getPIDUnit((int)pid, retVal)));

			std::stringstream ss;
			cnc::traceSetterValueList(ss, lste.set.values, pid < PID_DOUBLE_RANG_START ? 1 : DBL_FACT);
			setterRow.updateItem(CncSetterListCtrl::COL_VAL, wxString::Format("%s", ss.str()));
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
	
	unsigned int sizeAvailable = posSpyStringQueue.pop(posSpyRows, MAX_POS_SPY_ITEMS);
	
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
	
	unsigned int sizeAvailable = setterStringQueue.pop(setterRows, MAX_SETTER_ITEMS);
	
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
											unit = THE_CONFIG->getDisplayUnit();
											displayFactX = THE_CONFIG->getDisplayFactX(unit);
											displayFactY = THE_CONFIG->getDisplayFactY(unit);
											displayFactZ = THE_CONFIG->getDisplayFactZ(unit);
											
											break;
											
		case Event::Type::SETLST_RESET:		{	// ensure no one else changes the setter list
												wxCriticalSectionLocker lock(pHandler->pUpdateManagerThreadCS);
												setterQueue.reset();
												setterStringQueue.reset();
											}
											break;
											
		case Event::Type::POSSPY_RESET:		{	// ensure no one else changes the queues
												wxCriticalSectionLocker lock(pHandler->pUpdateManagerThreadCS);
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
