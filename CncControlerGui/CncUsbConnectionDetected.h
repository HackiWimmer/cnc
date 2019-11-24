#ifndef CNCUSBCONNECTIONDETECTED_H
#define CNCUSBCONNECTIONDETECTED_H

#include "wxCrafterBaseDialogs.h"

class CncUsbConnectionDetected : public CncUsbConnectionDetectedBase {
	
	public:
		CncUsbConnectionDetected(wxWindow* parent);
		virtual ~CncUsbConnectionDetected();
		
		void setPortName(const wxString& portName);
		
	protected:
    virtual void initDilaog(wxInitDialogEvent& event);
		virtual void connect(wxCommandEvent& event);
		virtual void ignore(wxCommandEvent& event);
};

#endif // CNCUSBCONNECTIONDETECTED_H
