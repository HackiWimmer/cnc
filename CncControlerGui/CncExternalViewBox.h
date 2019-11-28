#ifndef CNCEXTERNALVIEWBOX_H
#define CNCEXTERNALVIEWBOX_H
#include "wxCrafterBaseDialogs.h"

//////////////////////////////////////////////////////////////////
class CncExternalViewBox : public CncExternalViewBoxBase {
	
	public:
		CncExternalViewBox(wxWindow* parent, wxWindow* source, long style = wxRESIZE_BORDER);
		virtual ~CncExternalViewBox();
		
		bool getGuiSensitivity() const		{ return guiSensitivity; }
		void setGuiSensitivity(bool state) 	{ guiSensitivity = state; }
		
		void swapControls();
		
	protected:
		
		bool guiSensitivity;
		
		virtual void onMotion(wxMouseEvent& event);
		virtual void onMinMax(wxCommandEvent& event);
		virtual void onStartMove(wxMouseEvent& event);
		virtual void onStopMove(wxMouseEvent& event);
		virtual void onCloseFromButton(wxCommandEvent& event);
		virtual void onShow(wxShowEvent& event);
		
	private:
		wxWindow* sourceCtrl;
		wxPoint moveDelta;
};

//////////////////////////////////////////////////////////////////
class CncExternalViewBoxSecure : public CncExternalViewBox {
	
	public:
		CncExternalViewBoxSecure(wxWindow* parent, wxWindow* source) 
		: CncExternalViewBox(parent, source, 0) {
		}
		
		virtual ~CncExternalViewBoxSecure() {
		}
};

#endif // CNCEXTERNALVIEWBOX_H
