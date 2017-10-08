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
, exit(false)
, appPos(0, 0, 0)
, ctlPos(0, 0, 0)
, cmdCounter(0)
, cmdDuration(0)
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
	unsigned int cnt 	= 0;
	unsigned int sleep 	= 50;
	
	while ( !TestDestroy() ) {
		// sleep a moment
		this->Sleep(sleep);
		
		// recheck this here after the sleep
		if ( TestDestroy() ) 
			break;
		
		// break on exit
		if ( exit == true )
			break;
			
		// check if something should be done
		if ( enabled == false )
			continue;
			
		// update the controls
		display();
		
		//if ( ++cnt%100 == 0 ) {
			
			//wxCommandEvent evt1(wxEVT_XXX);
			//pHandler->AddPendingEvent(evt1);
			
			wxThreadEvent evt2(wxID_ANY);
			pHandler->AddPendingEvent(evt2);
			
			cnt = 0;
		//}
		
	}
	
	#warning here is something to do
	
	//std::clog << "TestDestroy()" << std::endl;
	//this->Sleep(500);
	
	// signal the event handler that this thread is going to be destroyed
	// NOTE: here we assume that using the m_pHandler pointer is safe,
	//       (in this case this is assured by the MyFrame destructor)
	//wxQueueEvent(pHandler, new wxThreadEvent(wxEVT_COMMAND_MYTHREAD_COMPLETED));
	
	// how to throw a custom event
	//wxThreadEvent evt(UPDATE_MANAGER_THREAD_COMPLETED, wxID_ANY);
	//evt.SetInt(iError); 
	//pHandler->AddPendingEvent(evt);
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postAppPos(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	appPos.set(pos);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postCtlPos(const CncLongPosition& pos) {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	ctlPos.set(pos);
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postCmdValues(long counter, long duration) {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	cmdCounter 	= counter;
	cmdDuration	= duration;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postConfigUpdate(CncConfig* config) {
///////////////////////////////////////////////////////////////////
	if ( config == NULL )
		return;
		
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	cncConfig = config;
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postSetterValue(unsigned char id,  int32_t value) {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	setterList.push_back(std::make_pair(id, value));
	
	setterCounter = setterCounter%UINT_MAX + 1;
	
	if ( setterList.size() > maxSetterEntries )
		setterList.erase(setterList.begin());
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postResetZView() {
///////////////////////////////////////////////////////////////////
	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	if ( pHandler->getZView() ) 
		pHandler->getZView()->resetAll();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::postUpdateZView() {
///////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;

	wxCriticalSectionLocker enter(pHandler->pThreadCS);
	if ( pHandler->getZView() ) {
		double displayFactZ = cncConfig->getDisplayFactZ(cncConfig->getUnit());
		pHandler->getZView()->updateView(appPos.getZ() * displayFactZ, *cncConfig);
	}
}



///////////////////////////////////////////////////////////////////
void UpdateManagerThread::display() {
///////////////////////////////////////////////////////////////////
	// important: be thread safe in all prodedures below!
	updatePositionControls();
	//todo
	//updateConfigurationControls();
	//updateSetterControls();
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updatePositionControls() {
///////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;
		
	double displayFactX = cncConfig->getDisplayFactX(cncConfig->getUnit());
	double displayFactY = cncConfig->getDisplayFactY(cncConfig->getUnit());
	double displayFactZ = cncConfig->getDisplayFactZ(cncConfig->getUnit());
	
	// application position
	if ( cnc::dblCompare(displayFactX, 1.0) )	pHandler->getAppPosControlX()->ChangeValue(wxString::Format(wxT("%8.0f"), appPos.getX() * displayFactX));
	else 										pHandler->getAppPosControlX()->ChangeValue(wxString::Format(wxT("%4.3f"), appPos.getX() * displayFactX));
	
	if ( cnc::dblCompare(displayFactY, 1.0) )	pHandler->getAppPosControlY()->ChangeValue(wxString::Format(wxT("%8.0f"), appPos.getY() * displayFactY));
	else										pHandler->getAppPosControlY()->ChangeValue(wxString::Format(wxT("%4.3f"), appPos.getY() * displayFactY));
	
	if ( cnc::dblCompare(displayFactZ, 1.0) )	pHandler->getAppPosControlZ()->ChangeValue(wxString::Format(wxT("%8.0f"), appPos.getZ() * displayFactZ));
	else										pHandler->getAppPosControlZ()->ChangeValue(wxString::Format(wxT("%4.3f"), appPos.getZ() * displayFactZ));
	
	// controller position
	if ( cnc::dblCompare(displayFactX, 1.0) )	pHandler->getCtlPosControlX()->ChangeValue(wxString::Format(wxT("%8.0f"), ctlPos.getX() * displayFactX));
	else 										pHandler->getCtlPosControlX()->ChangeValue(wxString::Format(wxT("%4.3f"), ctlPos.getX() * displayFactX));
	
	if ( cnc::dblCompare(displayFactY, 1.0) )	pHandler->getCtlPosControlY()->ChangeValue(wxString::Format(wxT("%8.0f"), ctlPos.getY() * displayFactY));
	else										pHandler->getCtlPosControlY()->ChangeValue(wxString::Format(wxT("%4.3f"), ctlPos.getY() * displayFactY));
	
	if ( cnc::dblCompare(displayFactZ, 1.0) )	pHandler->getCtlPosControlZ()->ChangeValue(wxString::Format(wxT("%8.0f"), ctlPos.getZ() * displayFactZ));
	else										pHandler->getCtlPosControlZ()->ChangeValue(wxString::Format(wxT("%4.3f"), ctlPos.getZ() * displayFactZ));
	
	// Z view 
	if ( pHandler->getZView() ) 
		pHandler->getZView()->updateView(appPos.getZ() * displayFactZ, *cncConfig);
	 
	// command values 
	pHandler->getCmdCounterControl()->ChangeValue(wxString::Format(wxT("%i"), cmdCounter));
	pHandler->getCmdDurationControl()->ChangeValue(wxString::Format(wxT("%i"), cmdDuration));
}
///////////////////////////////////////////////////////////////////
void UpdateManagerThread::updateConfigurationControls() {
///////////////////////////////////////////////////////////////////
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
	wxDataViewListCtrl* ps = pHandler->getProcessedSetterControl();
	
	// make a deep copy to be thread safe
	SetterList sl;
	{
		wxCriticalSectionLocker enter(pHandler->pThreadCS);
		sl = setterList;
	}
	
	// in this case nothing is do to
	// and a continious update will be avoided
	if ( sl.size() == (unsigned int)ps->GetItemCount() )
		return;
		
	DcmItemList rows;
	std::string retVal;
	unsigned int cnt = setterCounter - sl.size();
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