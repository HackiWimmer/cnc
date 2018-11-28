#ifndef CNC_MONITOR_SPLITTER_WINDOW_H
#define CNC_MONITOR_SPLITTER_WINDOW_H

#include <wx/splitter.h>

class CncMonitorVSplitterWindow : public wxSplitterWindow {
	
	public:
		CncMonitorVSplitterWindow(wxWindow *parent);
		virtual ~CncMonitorVSplitterWindow();
		
		virtual bool SplitHorizontally(wxWindow *window1, wxWindow *window2, int sashPosition = 0);
		virtual bool SplitVertically(wxWindow *window1, wxWindow *window2, int sashPosition = 0);
		
		void toggleRightWindow();
		void showRightWindow(bool show);
		void hideRightWindow() { showRightWindow(false); }
		
	protected:
	
		const int defaultRightWindowWidth = 200;
		int lastRightWindowWidth;
		
		virtual bool OnSashPositionChange (int newSashPosition);
		void onSize(wxSizeEvent& event);
		
		bool isRightWindowShown() { return GetSashGravity() < 1.0; }

		wxDECLARE_EVENT_TABLE();
};

#endif