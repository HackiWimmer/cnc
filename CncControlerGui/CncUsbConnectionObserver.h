#ifndef CNCUSBCONNECTIONDETECTED_H
#define CNCUSBCONNECTIONDETECTED_H

#include "wxCrafterBaseDialogs.h"

class CncUsbConnectionObserver : public CncUsbConnectionDetectedBase {
	
	public:
		class Deactivator {
			
			private:
				CncUsbConnectionObserver* observer;
				bool prevSensitivity;
				
			public: 
				Deactivator(CncUsbConnectionObserver* o)
				: observer(o)
				, prevSensitivity(false)
				{
					wxASSERT(observer);
					prevSensitivity = observer->getSensitivity();
					observer->setSensitiv(false);
				}
				
				~Deactivator()
				{
					wxASSERT(observer);
					observer->setSensitiv(prevSensitivity);
				}
		};
		
		CncUsbConnectionObserver(wxWindow* parent);
		virtual ~CncUsbConnectionObserver();
		
		void setPortName(const wxString& portName);
		
		bool getSensitivity() const { return sensitivity; }
		void setSensitiv(bool state) { sensitivity = state; }
		
	protected:
		virtual void onCloseDialog(wxCloseEvent& event);
		virtual void onConnect(wxCommandEvent& event);
		virtual void onIgnore(wxCommandEvent& event);
		virtual void onInitDialog(wxInitDialogEvent& event);
		
	private:
		bool sensitivity; 
};

#endif // CNCUSBCONNECTIONDETECTED_H
