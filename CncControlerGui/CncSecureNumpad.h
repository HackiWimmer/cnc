#ifndef CNCSECURENUMPAD_H
#define CNCSECURENUMPAD_H

#include <wx/uiaction.h>
class CncKeypadBase {
	
	public:
		CncKeypadBase()  {}
		~CncKeypadBase() {}
		
	protected:
		
		// --------------------------------------------------------
		void hitKey(int keyCode, int modifier = wxMOD_NONE) {
			wxUIActionSimulator uai;
			uai.KeyDown(keyCode, modifier);
			uai.KeyUp  (keyCode, modifier);
		}
		
		
		// --------------------------------------------------------
		void hitChar(int keyCode, int modifier = wxMOD_NONE) {
			wxUIActionSimulator uai;
			uai.Char(keyCode, modifier);
		}
};

#include "wxCrafterSecurePanel.h"


class CncSecureNumpad	: public CncSecureNumpadBase 
						, public CncKeypadBase
{
	public:
		enum Type { LONG, DOUBLE };
		
		CncSecureNumpad(wxWindow* parent, Type t, int precision = -1, double min = -1000.0, double max = 1000.0);
		virtual ~CncSecureNumpad();
		
		void	setValue(long v);
		void	setValue(double v);
		void	setValue(const wxString& v);
		
		void	setInfo(const wxString& info);
		
		long	getValueAsLong();
		double	getValueAsDouble();
		
		virtual bool Show(bool show = true);
		
	protected:
	
		virtual void onContextMenuResult(wxContextMenuEvent& event);
		virtual void onTextUpdatedResult(wxCommandEvent& event);
		virtual void onCharResult(wxKeyEvent& event);
		virtual void onLeft(wxCommandEvent& event);
		virtual void onRight(wxCommandEvent& event);
		virtual void onBackspace(wxCommandEvent& event);
		virtual void onClear(wxCommandEvent& event);
		virtual void onNumber(wxCommandEvent& event);
		virtual void onSign(wxCommandEvent& event);
		
	private:
	
		Type		type;
		int			precision;
		int			length;
		double		min;
		double		max;
		wxString	prevContent;
};

#endif // CNCSECURENUMPAD_H
