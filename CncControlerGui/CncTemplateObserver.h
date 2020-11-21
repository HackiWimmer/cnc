#ifndef CNCTEMPLATEOBSERVER_H
#define CNCTEMPLATEOBSERVER_H
#include "wxCrafterTemplateObserver.h"

class CncTemplateObserver : public CncTemplateObserverBase {
	
	public:
		CncTemplateObserver(wxWindow* parent);
		virtual ~CncTemplateObserver();
		
	protected:
		virtual void openTemplateExtern(wxCommandEvent& event);
		virtual void clearTrace(wxCommandEvent& event);
		virtual void changeObservationMode(wxCommandEvent& event);
		virtual void observe(wxTimerEvent& event);

		void startObservation();
		void stopObservation();
		
		void suspendObservation();
		void reconstructObservation();
		
		void enableControls(bool state);
		
		
		void changeObservation(bool state);
		void logTimestamp();
		void logFileName();
		void logTemplateModificationTimeStamp();
		
		bool isCurrentTemplateChanged();
		void performActions();
		
		void logInformation(const wxString& msg, const wxTextAttr ta);
		void logInformation(const wxString& msg);

		void logWarning(const wxString& msg);
		void logError(const wxString& msg);
		
		void activateSourecEditor(bool state);
		
		void enableObservationTimer(bool state);
		
	private:
		
		struct Styles {
			wxTextAttr taInfo			= wxTextAttr(wxColour(232, 232, 232));
			wxTextAttr taWarning		= wxTextAttr(wxColour(0, 157, 157));
			wxTextAttr taError			= wxTextAttr(wxColour(255, 16, 16));
			
			wxTextAttr taStart			= wxTextAttr(wxColour(232, 232, 232));
			wxTextAttr taEnd			= wxTextAttr(wxColour(232, 232, 232));
			
			wxTextAttr taSuspend		= wxTextAttr(wxColour(232, 232, 232));
			wxTextAttr taReconstruct	= wxTextAttr(wxColour(232, 232, 232));
			
			wxTextAttr taActivate		= wxTextAttr(wxColour(128, 128, 255));
			wxTextAttr taDeactivate		= wxTextAttr(wxColour(128, 128, 255));
			
			wxTextAttr taDetected		= wxTextAttr(wxColour(255, 255, 117));
			wxTextAttr taAction			= wxTextAttr(wxColour(0, 155, 0));
			
		};
		
		bool 		observationActive;
		Styles 		styles;
		wxDateTime 	lastTemplateModification;
		
	public:
	
		class Deactivator {
			
			private:
				CncTemplateObserver* observer;
				
			public:
				Deactivator(CncTemplateObserver* obs) 
				: observer(obs)
				{
					if ( observer == NULL )
						return;
						
					if ( observer->observationActive == false )
						return;
						
					observer->suspendObservation();
					observer->enableControls(false);
				}
				
				~Deactivator() {
					if ( observer == NULL )
						return;
						
					if ( observer->observationActive == false )
						return;
						
					observer->reconstructObservation();
					observer->enableControls(true);
				}
		};
};

#endif // CNCTEMPLATEOBSERVER_H
