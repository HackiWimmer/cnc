#include <iostream>
#include "CncMonitorSplitterWindow.h"

// ----------------------------------------------------------------------------
// CncMonitorSplitterWindow Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncMonitorVSplitterWindow, wxSplitterWindow)
	EVT_SIZE(CncMonitorVSplitterWindow::onSize)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncMonitorVSplitterWindow::CncMonitorVSplitterWindow(wxWindow *parent)
: wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				   wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN)
				
, lastRightWindowWidth(defaultRightWindowWidth)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncMonitorVSplitterWindow::~CncMonitorVSplitterWindow() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncMonitorVSplitterWindow::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	if ( isRightWindowShown() ) {
		const int splitterWidth = GetSize().GetWidth();
		
		int sashPosition = 0;
		if ( splitterWidth > lastRightWindowWidth )
			sashPosition = splitterWidth - lastRightWindowWidth;
		
		SetSashPosition(sashPosition);
	}
}
///////////////////////////////////////////////////////////////////
bool CncMonitorVSplitterWindow::SplitHorizontally(wxWindow *window1, wxWindow *window2, int sashPosition) {
///////////////////////////////////////////////////////////////////
	wxASSERT(NULL);
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncMonitorVSplitterWindow::SplitVertically(wxWindow *window1, wxWindow *window2, int sashPosition) {
///////////////////////////////////////////////////////////////////
	wxASSERT(window1);
	wxASSERT(window2);
	
	const int splitterWidth = GetSize().GetWidth();
	
	sashPosition 			= 0;
	lastRightWindowWidth 	= splitterWidth;
	
	if ( splitterWidth > defaultRightWindowWidth ) {
		sashPosition 			= splitterWidth - defaultRightWindowWidth;
		lastRightWindowWidth 	= defaultRightWindowWidth;
	}
	
	return wxSplitterWindow::SplitVertically(window1, window2, sashPosition);
}
///////////////////////////////////////////////////////////////////
bool CncMonitorVSplitterWindow::OnSashPositionChange(int newSashPosition) {
///////////////////////////////////////////////////////////////////
	const int splitterWidth = GetSize().GetWidth();
	
	lastRightWindowWidth = splitterWidth - newSashPosition;
	return wxSplitterWindow::OnSashPositionChange(newSashPosition);
}
///////////////////////////////////////////////////////////////////
void CncMonitorVSplitterWindow::toggleRightWindow() {
///////////////////////////////////////////////////////////////////
	showRightWindow(!isRightWindowShown());
}
///////////////////////////////////////////////////////////////////
void CncMonitorVSplitterWindow::showRightWindow(bool show) {
///////////////////////////////////////////////////////////////////
	const int splitterWidth	= GetSize().GetWidth();
	
	if ( show == false ) {
		SetMinimumPaneSize(0);
		SetSashGravity(1.0);
		SetSashPosition(splitterWidth * 10);
		
	} else {
		SetMinimumPaneSize(1);
		
		if ( lastRightWindowWidth < defaultRightWindowWidth / 4 )
			lastRightWindowWidth = defaultRightWindowWidth;
			
		SetSashGravity((splitterWidth - lastRightWindowWidth) / splitterWidth);
		SetSashPosition(splitterWidth - lastRightWindowWidth);
		
	}
}

