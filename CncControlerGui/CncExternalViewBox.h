#ifndef CNCEXTERNALVIEWBOX_H
#define CNCEXTERNALVIEWBOX_H
#include "wxCrafterBaseDialogs.h"

//////////////////////////////////////////////////////////////////
class CncExternalViewBox : public CncExternalViewBoxBase {
	
	public:
		
		enum Default		{ VIEW1 = 0, VIEW2 = 1, VIEW3 = 2, VIEW4 = 3 };
		enum Preview		{ FILE = 0, TEMPLATE = 1 };
	
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
		virtual void onAttachPage1(wxCommandEvent& event);
		virtual void onAttachPage2(wxCommandEvent& event);
		virtual void onAttachPage3(wxCommandEvent& event);
		virtual void onAttachPage4(wxCommandEvent& event);
		virtual void onViewBookChanged(wxNotebookEvent& event);
		virtual void onMotion(wxMouseEvent& event);
		virtual void onMinMax(wxCommandEvent& event);
		virtual void onStartMove(wxMouseEvent& event);
		virtual void onStopMove(wxMouseEvent& event);
		virtual void onCloseFromButton(wxCommandEvent& event);
		virtual void onShow(wxShowEvent& event);
		
		void swapControls(unsigned int idx);
		
	private:

		static const unsigned int MAX_VIEWS = 4;
		bool		guiSensitivity;
		wxWindow* 	sourceCtrl	[MAX_VIEWS];
		wxWindow* 	targetCtrl	[MAX_VIEWS];
		wxString	title		[MAX_VIEWS];
		SwapState 	swapState	[MAX_VIEWS];
		
		wxPoint 	moveDelta;
};


#endif // CNCEXTERNALVIEWBOX_H
