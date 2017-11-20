#include <iostream>
#include <wx/intl.h>
#include <wx/numformatter.h>
#include "MainFrame.h"
#include "UpdateManagerThread.h"

///////////////////////////////////////////////////////////////////
UpdateManagerThread::UpdateManagerThread(MainFrame *handler)
: wxThread(wxTHREAD_DETACHED)
, pHandler(handler)
, exit(false)
, appPosSpyList()
, ctlPosSpyList()
, setterList()
///////////////////////////////////////////////////////////////////
{
	appPosSpyList.reserve(1000 * 1000);
	ctlPosSpyList.reserve(1000 * 1000);
	setterList.reserve(1000 * 1000);
}
///////////////////////////////////////////////////////////////////
UpdateManagerThread::~UpdateManagerThread() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	pHandler->updateManagerThread = NULL;
	
	appPosSpyList.clear();
	ctlPosSpyList.clear();
	setterList.clear();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::stop() {
///////////////////////////////////////////////////////////////////
	exit = true;
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode UpdateManagerThread::Entry() {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event LastPosSpyEntry;
	UpdateManagerThread::EventId posEvtId = UpdateManagerThread::EventId::CTL_POS_UPDATE;
	
	unsigned int sleep = 1;
	
	wxDateTime tsLast = wxDateTime::UNow();
	while ( !TestDestroy() ) {
		this->Sleep(sleep);
		
		// recheck this here after the sleep
		if ( TestDestroy() ) break;
		if ( exit == true )  break;
		
		// process data update
		if ( (wxDateTime::UNow() - tsLast).GetMilliseconds() >= 50 ) {
			
			UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, posEvtId);
			LastPosSpyEntry lpse;
			{
				// ensure no one else updates posSpyList
				wxCriticalSectionLocker lock(pHandler->pThreadCS);
				switch ( posEvtId ) {
					
					case UpdateManagerThread::EventId::APP_POS_UPDATE:	if ( appPosSpyList.size() > 0 )	lpse = appPosSpyList.back();
																		else 							lpse.pos.reset();
																		
																		posEvtId = UpdateManagerThread::EventId::CTL_POS_UPDATE;
																		break;
																		
					case UpdateManagerThread::EventId::CTL_POS_UPDATE:	if ( ctlPosSpyList.size() > 0 )	lpse = ctlPosSpyList.back();
																		else 							lpse.pos.reset();
																		
																		posEvtId = UpdateManagerThread::EventId::APP_POS_UPDATE;
																		break;
																		
					default:											;
				}
			}
			
			evt.setCurrentPosition(lpse.pos.pos);
			
			evt.setSpeedMode((char)lpse.pos.speedMode);
			evt.setSpeedValue(lpse.pos.speedValue);
			
			evt.setReferenceId(lpse.pos.id);
			
			wxPostEvent(pHandler, evt);
		}
		
		// process heartbeat
		if ( (wxDateTime::UNow() - tsLast).GetMilliseconds() >= 500 ) {
			UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, UpdateManagerThread::EventId::HEARTBEAT);
			wxPostEvent(pHandler, evt);
			
			tsLast = wxDateTime::UNow();
		}
	}
	
	UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, UpdateManagerThread::EventId::COMPLETED);
	wxPostEvent(pHandler, evt);
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::fillPositionSpy(wxListBox* lb, UpdateManagerThread::SpyContent sc, CncConfig& config) {
///////////////////////////////////////////////////////////////////
	if( lb == NULL )
		return;
		
	CncUnit unit = config.getDisplayUnit();
	double displayFactX = GBL_CONFIG->getDisplayFactX(unit);
	double displayFactY = GBL_CONFIG->getDisplayFactY(unit);
	double displayFactZ = GBL_CONFIG->getDisplayFactZ(unit);
	
	{
		// ensure no one else updates posSpyList
		wxCriticalSectionLocker lock(pHandler->pThreadCS);
		
		//clog << posSpyList->size() << ", " << lb->GetCount()  << endl;
		
		const int insertPos 	= 0;
		unsigned long prevCount = lb->GetCount();
		wxString row;
		
		PosSpyList* list = ( sc == UpdateManagerThread::SpyContent::APP_POSITIONS ? &appPosSpyList : &ctlPosSpyList );
		for ( auto it = list->begin() + lb->GetCount(); it != list->end(); ++it ) {
			
			switch ( unit ) {
				case CncSteps:	row.Printf("%08ld %c%05.1lf % 10ld  % 10ld  % 10ld", 
											it->pos.id,
											(char)it->pos.speedMode,
											it->pos.speedValue,
											it->pos.pos.getX(), 
											it->pos.pos.getY(), 
											it->pos.pos.getZ());
								break;
								
				case CncMetric:	row.Printf("%08ld %c%05.1lf % 10.3lf  % 10.3lf  % 10.3lf", 
											it->pos.id,
											(char)it->pos.speedMode,
											it->pos.speedValue,
											it->pos.pos.getX() * displayFactX, 
											it->pos.pos.getY() * displayFactY, 
											it->pos.pos.getZ() * displayFactZ);
								break;
			}
			
			//lb->Insert(row, insertPos);
			lb->Append(row);
			
			if ( lb->GetCount() - prevCount >= 5000 )
				break;
		}
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::fillSetterList(wxListCtrl* lb) {
///////////////////////////////////////////////////////////////////
	if( lb == NULL )
		return;
		
	{
		// ensure no one else updates posSpyList
		wxCriticalSectionLocker lock(pHandler->pThreadCS);
		
		// decorate the columns
		if ( lb->GetColumnCount() == 0 ) {
			lb->AppendColumn("#:", 		wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
			lb->AppendColumn("Key:",	wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
			lb->AppendColumn("Value:", 	wxLIST_FORMAT_RIGHT, wxLIST_AUTOSIZE);
			
			lb->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, true);
		}
		
		// fill/append the list
		const int insertPos = 0;
		unsigned long count = lb->GetItemCount();
		std::string retVal;
		
		for ( auto it = setterList.begin() + lb->GetItemCount(); it != setterList.end(); ++it ) {
			lb->InsertItem(insertPos, "",  0);

			if ( it->set.id == PID_SEPARATOR ) {
				wxString label("Bookmark: Type(<DEFAULT>)");
				wxColour bgColour(*wxBLACK);
				wxColour fgColour(*wxWHITE);
				
				switch ( it->set.value ) {
					case SEPARARTOR_SETUP:		label.assign("Bookmark: Type(<SETUP>)");
												bgColour.Set(0, 64, 64);
												break;
												
					case SEPARARTOR_RESET:		label.assign("Bookmark: Type(<RESET>)");
												bgColour.Set(0, 0, 64);
												break;
												
					case SEPARARTOR_RUN:		label.assign("Bookmark: Type(<RUN>)");
												bgColour.Set(128, 64, 64);
												break;
					default:					;
				}
				
				lb->SetItemBackgroundColour(insertPos, bgColour);
				lb->SetItemTextColour(insertPos, fgColour);
				
				lb->SetItem(insertPos, UMT_SETLST_NUM, wxString::Format("%010lu", ++count));
				lb->SetItem(insertPos, UMT_SETLST_KEY, wxString::Format("%s:", label));
				lb->SetItem(insertPos, UMT_SETLST_VAL, wxString::Format("%s.%03ld", it->ts.FormatTime(), it->ts.GetMillisecond()));
				
			} else {
				lb->SetItem(insertPos, UMT_SETLST_NUM, wxString::Format("%010lu", ++count));
				lb->SetItem(insertPos, UMT_SETLST_KEY, ArduinoPIDs::getPIDLabel((int)it->set.id, retVal));

				switch ( it->set.id ) {
					case PID_PITCH_X:
					case PID_PITCH_Y:
					case PID_PITCH_Z:		lb->SetItem(insertPos, UMT_SETLST_VAL, wxNumberFormatter::ToString((double)(it->set.value/1000.0), 2));
											break;

					default:				lb->SetItem(insertPos, UMT_SETLST_VAL, wxNumberFormatter::ToString((long)(it->set.value)));
				}
			}
			
			if ( count%100 ) {
				UpdateManagerEvent evt(wxEVT_UPDATE_MANAGER_THREAD, UpdateManagerThread::EventId::DISPATCH_ALL);
				//wxPostEvent(pHandler, evt);
			}
		}
		
		// first set default sizes depending on content
		lb->SetColumnWidth(UMT_SETLST_NUM, wxLIST_AUTOSIZE);
		lb->SetColumnWidth(UMT_SETLST_KEY, wxLIST_AUTOSIZE);
		lb->SetColumnWidth(UMT_SETLST_VAL, wxLIST_AUTOSIZE);
		
		// try to strech the second (key) column
		const int scrollbarWidth = 26;
		int size = lb->GetSize().GetWidth() - lb->GetColumnWidth(UMT_SETLST_NUM) - lb->GetColumnWidth(UMT_SETLST_VAL) - scrollbarWidth;
		if ( size > lb->GetColumnWidth(UMT_SETLST_KEY) )
			lb->SetColumnWidth(UMT_SETLST_KEY, size);
			
		lb->EnsureVisible(0);
	}
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postEvent(const UpdateManagerThread::Event& evt) {
///////////////////////////////////////////////////////////////////
	typedef UpdateManagerThread::Event Event;
	
	// the following things will be done immediatly
	switch ( evt.type ) {
											
		case Event::Type::EMPTY_UPD:		// do noting
											break;
											
		case Event::Type::COMMAND_UPD:		// curently do noting
											break;
											
		case Event::Type::CONFIG_UPD:		// curently do noting
											break;
											
		case Event::Type::SETLST_RESET:		{
												wxCriticalSectionLocker lock(pHandler->pThreadCS);
												setterList.clear();
											}
											break;
											
		case Event::Type::SETTER_ADD:		setterList.push_back(evt);
											break;
											
		case Event::Type::POSSPY_RESET:		{
												wxCriticalSectionLocker lock(pHandler->pThreadCS);
												appPosSpyList.clear();
												ctlPosSpyList.clear();
											}
											break;
											
		case Event::Type::APP_POS_UPD:		appPosSpyList.push_back(evt);
											break;
											
		case Event::Type::CTL_POS_UPD:		ctlPosSpyList.push_back(evt);
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

