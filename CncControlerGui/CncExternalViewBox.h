#ifndef CNCEXTERNALVIEWBOX_H
#define CNCEXTERNALVIEWBOX_H
#include "wxCrafterBaseDialogs.h"

//////////////////////////////////////////////////////////////////
class CncExternalViewBox : public CncExternalViewBoxBase {
	
	public:
		
		enum Preview { FILE = 0, TEMPLATE = 1 };
	
	
	public:
		enum SwapState { SS_DEFAULT, SS_SWAPED };

		CncExternalViewBox(wxWindow* parent, long style = wxRESIZE_BORDER);
		virtual ~CncExternalViewBox();
		
		bool getGuiSensitivity() const			{ return guiSensitivity; }
		void setGuiSensitivity(bool state) 		{ guiSensitivity = state; }
		
		void swapControls();
		
		const SwapState getSwapState(unsigned int idx) const;
		bool setupView(unsigned int idx, wxWindow* source, const wxString& title);
		bool selectView(unsigned int idx);
		
		void setStatusTextLeft(const wxString& text) 	{ m_statusTextLeft-> ChangeValue(text); }
		void setStatusTextMid(const wxString& text)		{ m_statusTextMid->  ChangeValue(text); }
		void setStatusTextRight(const wxString& text)	{ m_statusTextRight->ChangeValue(text); }
		
	protected:
		
		
		bool guiSensitivity;
		
		virtual void onViewBookChnaged(wxNotebookEvent& event);
		virtual void onMotion(wxMouseEvent& event);
		virtual void onMinMax(wxCommandEvent& event);
		virtual void onStartMove(wxMouseEvent& event);
		virtual void onStopMove(wxMouseEvent& event);
		virtual void onCloseFromButton(wxCommandEvent& event);
		virtual void onShow(wxShowEvent& event);
		
		void swapControls(unsigned int idx);
		
	private:

		static const unsigned int MAX_VIEWS = 4;
		wxWindow* 	sourceCtrl	[MAX_VIEWS];
		wxWindow* 	targetCtrl	[MAX_VIEWS];
		wxString	title		[MAX_VIEWS];
		SwapState 	swapState	[MAX_VIEWS];
		
		wxPoint 	moveDelta;
};


#endif // CNCEXTERNALVIEWBOX_H
