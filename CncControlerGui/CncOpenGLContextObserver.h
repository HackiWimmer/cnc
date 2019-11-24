#ifndef CNCOPENGLCONTEXTOBSERVER_H
#define CNCOPENGLCONTEXTOBSERVER_H

#include "3D/GLContextCncPathBase.h"
#include "CncOpenGLContextObserverListCtrl.h"
#include "wxCrafterContextObserver.h"

class CncOpenGLContextObserver : public CncOpenGLContextObserverBase 
                               , public CncGLContextObserver::Callback
{
	public:
		CncOpenGLContextObserver(wxWindow* parent);
		virtual ~CncOpenGLContextObserver();
		
		virtual void nofifyForCurrent(const wxString& newCtxName);
		virtual void nofifyForRegistered(const wxString& newCtxName);
		virtual void nofifyMessage(const char type, const wxString& curCtxName, const wxString& functName, const wxString& msg);
		
	protected:
		
		bool 		heartbeatFlag;
		int 		infoCounter;
		
		CncOpenGLCurrentContextObserverListCtrl* 	currentCtxList;
		CncOpenGLRegisteredContextObserverListCtrl*	registeredCtxList;
		CncOpenGLContextObserverHistoryListCtrl*	historyList;
		
		virtual void onContinuousTimer(wxTimerEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
		
		void addHistoryItem(const wxString& ctxName, const wxString& funct, const wxString& item, const char type = 'I', const char mode = 'N');
		void activateInfoBell() { infoCounter = 5; }
		
	private:
		typedef std::vector<CncColumContainer> 	RowContainer;
		RowContainer historyStage;
};

#endif // CNCOPENGLCONTEXTOBSERVER_H
