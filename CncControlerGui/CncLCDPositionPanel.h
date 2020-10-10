#ifndef CNCLCDPOSITIONPANEL_H
#define CNCLCDPOSITIONPANEL_H

#include "wxCode/lcdwindow.h"
#include "wxCrafterLCDPanel.h"

class CncLCDPositionPanel : public CncLCDPositionPanelBase {
	
	private:
		wxLCDWindow* lcdW;
		wxLCDWindow* lcdX;
		wxLCDWindow* lcdY;
		wxLCDWindow* lcdZ;
	
	public:
		CncLCDPositionPanel(wxWindow* parent);
		virtual ~CncLCDPositionPanel();
		
		void updateUnit();
		void updateValues();
		
	protected:
		virtual void onPaint(wxPaintEvent& event);
		virtual void onSize(wxSizeEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
};

#endif // CNCLCDPOSITIONPANEL_H
