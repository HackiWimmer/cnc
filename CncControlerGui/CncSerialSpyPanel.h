#ifndef CNC_SERIAL_SPY_PANEL_H
#define CNC_SERIAL_SPY_PANEL_H

#include "wxCrafterSerialSpy.h"

class CncSerialSpyListCtrl;
class CncMessageDialog;
class CncSerialSpyPanel : public CncSerialSpyPanelBase {
	
	public:
		CncSerialSpyPanel(wxWindow* parent);
		virtual ~CncSerialSpyPanel();
		
		CncSerialSpyListCtrl* getSerialSpyCtrl() { return serialSpyListCtrl; } 
		
		void		initDuringConnect();
		void		enableSerialSpy(bool state);
		void		clearSerialSpyBeforNextRun();
		void		clearSerialSpyOnConnect();
		void		updateIntervalSlider();
		
	protected:
		virtual void onLiveDebug(wxCommandEvent& event);
		virtual void onUpdateIntervalSlider(wxScrollEvent& event);
		virtual void onAutoColumnSize(wxCommandEvent& event);
		virtual void onAutoScrolling(wxCommandEvent& event);
		virtual void clearSerialSpy(wxCommandEvent& event);
		virtual void enableSerialSpy(wxCommandEvent& event);
		virtual void markSerialSpy(wxCommandEvent& event);
		virtual void onLiveDecoding(wxCommandEvent& event);
		virtual void openSpyDetailWindow(wxCommandEvent& event);
		virtual void selectSerialSpyMode(wxCommandEvent& event);
		
	private:
		
		CncSerialSpyListCtrl*	serialSpyListCtrl;
		CncMessageDialog*		spyDetailWindow;
		
		void 		selectSerialSpyMode();
		void 		decorateSerialSpy();
};

#endif // CNCSERIALSPYPANEL_H
