#ifndef CNCSECURESLIDEPADDIALOG_H
#define CNCSECURESLIDEPADDIALOG_H

#include "CncSecureSlidepad.h"
#include "wxCrafterSecurePanel.h"

class CncSecureSlidepadDialog : public CncSecureSlidepadDialogBase
{
	public:
		CncSecureSlidepadDialog(wxWindow* parent);
		virtual ~CncSecureSlidepadDialog();
		
		bool	setValues(const CncSecureSlidepad::SliderValues& list, int index)	{ return slidepad->setValues(list, index); }
		void	setInfo(const wxString& info)										{ slidepad->setInfo(info);                 }
		int		getValue() const 													{ return slidepad->getValue();             }
		
	protected:
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onCancel(wxCommandEvent& event);
		virtual void onOk(wxCommandEvent& event);
		
	private:
		CncSecureSlidepad* slidepad;
};

#endif // CNCSECURESLIDEPADDIALOG_H
