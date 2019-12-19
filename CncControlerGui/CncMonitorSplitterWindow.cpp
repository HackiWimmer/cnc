#include <iostream>
#include "CncCommon.h"
#include "CncStatisticsPane.h"
#include "CncMonitorSplitterWindow.h"

// ----------------------------------------------------------------------------
// CncMonitorVSplitterWindow Event Table
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
	//const int splitterWidth	= IsShown() ? GetSize().GetWidth() : 2000;
	const int splitterWidth	= 2000;
	
	CNC_PRINT_LOCATION
	std::cout << "show = " << show <<  "; splitterWidth = " << splitterWidth << std::endl;

	if ( show == false ) {
		// Use a min pane size > 0, otherwise the pane may be disappears for ever
		SetMinimumPaneSize(1);
		SetSashGravity(1.0);
		SetSashPosition(splitterWidth * 10);
		
		//SetSashInvisible(true);

	} else {
		SetMinimumPaneSize(20);
		
		if ( lastRightWindowWidth < defaultRightWindowWidth / 4 )
			lastRightWindowWidth = defaultRightWindowWidth;
			
		SetSashGravity((splitterWidth - lastRightWindowWidth) / splitterWidth);
		SetSashPosition(splitterWidth - lastRightWindowWidth);
		
		//SetSashInvisible(false);
	}
}


// ----------------------------------------------------------------------------
// CncMonitorHSplitterWindow Event Table
// ----------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(CncMonitorHSplitterWindow, wxSplitterWindow)
	EVT_SIZE(CncMonitorHSplitterWindow::onSize)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncMonitorHSplitterWindow::CncMonitorHSplitterWindow(wxWindow *parent)
: wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				   wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN)
				
, lastBottomWindowHeight(defaultBottomWindowHeight)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncMonitorHSplitterWindow::~CncMonitorHSplitterWindow() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncMonitorHSplitterWindow::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
	
	if ( isBottomWindowShown() ) {
		const int splitterHeight = GetSize().GetHeight();
		
		int sashPosition = 0;
		if ( splitterHeight > lastBottomWindowHeight )
			sashPosition = splitterHeight - lastBottomWindowHeight;
		
		SetSashPosition(sashPosition);
	}
}
///////////////////////////////////////////////////////////////////
bool CncMonitorHSplitterWindow::SplitHorizontally(wxWindow *window1, wxWindow *window2, int sashPosition) {
///////////////////////////////////////////////////////////////////
	wxASSERT(window1);
	wxASSERT(window2);
	
	const int splitterHeight = GetSize().GetHeight();
	
	sashPosition 			= 0;
	lastBottomWindowHeight 	= splitterHeight;
	
	if ( splitterHeight > defaultBottomWindowHeight ) {
		sashPosition 			= splitterHeight - defaultBottomWindowHeight;
		lastBottomWindowHeight 	= defaultBottomWindowHeight;
	}
	
	return wxSplitterWindow::SplitHorizontally(window1, window2, sashPosition);
}
///////////////////////////////////////////////////////////////////
bool CncMonitorHSplitterWindow::SplitVertically(wxWindow *window1, wxWindow *window2, int sashPosition) {
///////////////////////////////////////////////////////////////////
	wxASSERT(NULL);
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncMonitorHSplitterWindow::OnSashPositionChange(int newSashPosition) {
///////////////////////////////////////////////////////////////////
	const int splitterHeight = GetSize().GetHeight();
	lastBottomWindowHeight = splitterHeight - newSashPosition;
	
	return wxSplitterWindow::OnSashPositionChange(newSashPosition);
}
///////////////////////////////////////////////////////////////////
int CncMonitorHSplitterWindow::getCurrentButtomContext() {
///////////////////////////////////////////////////////////////////
	CncStatisticsPane* sp = static_cast<CncStatisticsPane*>(GetWindow2());
	if ( sp == NULL )
		return -1;
		
	return sp->GetContextBook()->GetSelection();
}
///////////////////////////////////////////////////////////////////
void CncMonitorHSplitterWindow::selectBottomContext(int context) {
///////////////////////////////////////////////////////////////////
	CncStatisticsPane* sp = static_cast<CncStatisticsPane*>(GetWindow2());
	if ( sp == NULL )
		return;
	
	int pc = sp->GetContextBook()->GetPageCount();
	if ( context < 0 || context > pc - 1 )
		return;
		
	sp->GetContextBook()->SetSelection(context);
}
///////////////////////////////////////////////////////////////////
void CncMonitorHSplitterWindow::toggleBottomWindow() {
///////////////////////////////////////////////////////////////////
	showBottomWindow(!isBottomWindowShown());
}
///////////////////////////////////////////////////////////////////
void CncMonitorHSplitterWindow::showBottomWindow(bool show) {
///////////////////////////////////////////////////////////////////
	const int splitterHeight	= GetSize().GetHeight();
	
	if ( show == false ) {
		// Use a min pane size > 0, otherwise the pane may be disappears for ever
		SetMinimumPaneSize(1);
		SetSashGravity(1.0);
		SetSashPosition(splitterHeight * 10);
		
	} else {
	
		switch ( getCurrentButtomContext() ) {
			case MontiorBottomContextSelection::VAL::REPLAY_PANEL:		lastBottomWindowHeight = 29; break;
			case MontiorBottomContextSelection::VAL::STATISTIC_PANEL:	lastBottomWindowHeight = defaultBottomWindowHeight; break;
			default:													lastBottomWindowHeight = defaultBottomWindowHeight;
		}
		
		SetMinimumPaneSize(20);
		
		SetSashGravity((splitterHeight - lastBottomWindowHeight) / splitterHeight);
		SetSashPosition(splitterHeight - lastBottomWindowHeight);
	}
}



