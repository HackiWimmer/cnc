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
		virtual void initDilaog(wxInitDialogEvent& event);
		virtual void connect(wxCommandEvent& event);
		virtual void ignore(wxCommandEvent& event);
		
	private:
		bool sensitivity; 
};

#endif // CNCUSBCONNECTIONDETECTED_H
