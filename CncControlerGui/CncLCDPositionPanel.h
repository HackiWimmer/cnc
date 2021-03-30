#ifndef CNCLCDPOSITIONPANEL_H
#define CNCLCDPOSITIONPANEL_H

#include "wxCode/lcdwindow.h"
#include "wxCrafterLCDPanel.h"

class CncLCDPositionPanel : public CncLCDPositionPanelBase {
	
	private:
		
		enum PosType { PT_PHYSICAL = 0, PT_LOGICAL = 1 };
		
		PosType			posType;
		wxLCDWindow*	lcdF;
		wxLCDWindow*	lcdS;
		wxLCDWindow*	lcdX;
		wxLCDWindow*	lcdY;
		wxLCDWindow*	lcdZ;
		
		wxColour		defBckCol;
		wxColor			defGryCol;
		
		void evaluatePositionType();
		void decoratePosType(PosType pt);
	
	public:
		CncLCDPositionPanel(wxWindow* parent);
		virtual ~CncLCDPositionPanel();
		
		void updateUnit();
		void updateValues();
		
	protected:
		virtual void onChangePositionType(wxCommandEvent& event);
		virtual void onPaint(wxPaintEvent& event);
		virtual void onSize(wxSizeEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
		virtual void onUpdateTimer(wxTimerEvent& event);
};

#endif // CNCLCDPOSITIONPANEL_H
