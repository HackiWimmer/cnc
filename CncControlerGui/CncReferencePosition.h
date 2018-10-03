#ifndef CNCREFERENCEPOSITION_H
#define CNCREFERENCEPOSITION_H
#include "wxcrafter.h"
#include "CncControl.h"
#include "MainFrame.h"

class CncReferencePosition : public CncReferencePositionBase
{
	public:
		CncReferencePosition(MainFrame* parent);
		virtual ~CncReferencePosition();
		
		void setMessage(const wxString& msg);
		void setMeasurePlateThickness(const double mpt);

		CncRefPositionMode getReferenceMode();
		double getWorkpieceThickness();
	
	protected:
		virtual void init(wxInitDialogEvent& event);
		virtual void show(wxShowEvent& event);
		virtual void cancel(wxCommandEvent& event);
		virtual void set(wxCommandEvent& event);
		virtual void mode1(wxCommandEvent& event);
		virtual void mode2(wxCommandEvent& event);
		virtual void mode3(wxCommandEvent& event);
		virtual void mode4(wxCommandEvent& event);
		virtual void mode5(wxCommandEvent& event);
		virtual void mode6(wxCommandEvent& event);
		virtual void navigateXNeg(wxMouseEvent& event);
		virtual void navigateXNegYNeg(wxMouseEvent& event);
		virtual void navigateXNegYPos(wxMouseEvent& event);
		virtual void navigateXPos(wxMouseEvent& event);
		virtual void navigateXPosYNeg(wxMouseEvent& event);
		virtual void navigateXPosYPos(wxMouseEvent& event);
		virtual void navigateYNeg(wxMouseEvent& event);
		virtual void navigateYPos(wxMouseEvent& event);
		virtual void navigateZNeg(wxMouseEvent& event);
		virtual void navigateZPos(wxMouseEvent& event);
		virtual void navigateDClick(wxMouseEvent& event);
		virtual void navigateKillFocus(wxFocusEvent& event);
		virtual void navigateLeave(wxMouseEvent& event);
		virtual void navigateLeftUp(wxMouseEvent& event);
		
		short evaluateMode();
		void setMode(short mode);
		
	private:
	
		MainFrame* parentFrame;
		
		bool isWorkpieceThicknessNeeded();
};
#endif // CNCREFERENCEPOSITION_H
