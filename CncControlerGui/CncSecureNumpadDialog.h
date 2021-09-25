#ifndef CNCSECURENUMPADDIALOG_H
#define CNCSECURENUMPADDIALOG_H

#include "CncSecureNumpad.h"
#include "wxCrafterSecurePanel.h"

class CncSecureNumpadDialog : public CncSecureNumpadDialogBase 
{
	public:
		CncSecureNumpadDialog(wxWindow* parent, CncSecureNumpad::Type t, int precision = -1, double min = -1000.0, double max = 1000.0);
		virtual ~CncSecureNumpadDialog();
		
		void	setValue(long v)				{ numpad->setValue(v);               }
		void	setValue(double v)				{ numpad->setValue(v);               }
		void	setValue(const wxString& v)		{ numpad->setValue(v);               }
		
		void	setInfo(const wxString& info)	{ numpad->setInfo(info);             }
		
		long	getValueAsLong()				{ return numpad->getValueAsLong();   }
		double	getValueAsDouble()				{ return numpad->getValueAsDouble(); }
		
	protected:
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onCancel(wxCommandEvent& event);
		virtual void onOk(wxCommandEvent& event);
		
	private:
		CncSecureNumpad* numpad;
};

#endif // CNCSECURENUMPADDIALOG_H
