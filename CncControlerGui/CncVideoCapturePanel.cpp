#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include "OSD/CncCameraHelper.h"
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncVideoCapturePanel.h"

// ----------------------------------------------------------------------------
// CncNavigatorPanel Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_CNC_NAVIGATOR_PANEL_TIMER,	wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncVideoCapturePanel, wxPanel)

	EVT_PAINT					(CncVideoCapturePanel::onPaint)
	EVT_TIMER					(wxID_ANY, CncVideoCapturePanel::onTimer)
	
/*	EVT_SIZE					(CncVideoCapturePanel::onSize)
	EVT_MOUSE_EVENTS			(CncVideoCapturePanel::onMouse)
	EVT_KEY_DOWN				(CncVideoCapturePanel::onKeyDown)
	EVT_ENTER_WINDOW			(CncVideoCapturePanel::onEnter)
	EVT_LEAVE_WINDOW			(CncVideoCapturePanel::onLeave)
	EVT_SET_FOCUS				(CncVideoCapturePanel::onSetFocus)
	EVT_KILL_FOCUS				(CncVideoCapturePanel::onKillFocus)
	EVT_ERASE_BACKGROUND		(CncVideoCapturePanel::onEraseBackground)
*/
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////
CncVideoCapturePanel::CncVideoCapturePanel(wxWindow* parent) 
///////////////////////////////////////////////////////////////////
: wxPanel			(parent)
, hvCrossFlag		(true)
, ddCrossFlag		(true)
, rectFlag			(true)
, cameraTimer		(this, wxID_ANY)
, cameraBitmap		(NULL)
, cameraFrame		(new cv::Mat(1024, 628, CV_64FC4))
, cameraCapture		(new cv::VideoCapture())
, caller			(NULL)
{
	stop();
	
	// This has to be done to use wxAutoBufferedPaintDC 
	// on EVT_PAINT events correctly
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}
///////////////////////////////////////////////////////////////////
CncVideoCapturePanel::~CncVideoCapturePanel() {
///////////////////////////////////////////////////////////////////
	stop();
	wxDELETE (cameraBitmap);
	wxDELETE (cameraCapture);
	wxDELETE (cameraFrame);
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::showHVCross(bool b) {
///////////////////////////////////////////////////////////////////
	hvCrossFlag = b;
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::showDDCross(bool b) {
///////////////////////////////////////////////////////////////////
	ddCrossFlag = b;
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::showRect(bool b) {
///////////////////////////////////////////////////////////////////
	rectFlag = b;
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::notifyProgess(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	if ( caller )	caller->notifyProgess(msg);
	else			std::cout << msg << std::endl;
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::notifyError(const wxString& msg) {
///////////////////////////////////////////////////////////////////
	if ( caller )	caller->notifyError(msg);
	else			std::cerr << msg << std::endl;
}
///////////////////////////////////////////////////////////////////
bool CncVideoCapturePanel::start() {
///////////////////////////////////////////////////////////////////
	if ( THE_CONFIG->getCameraSupportFlag() == false )
		return false;
		
	if ( THE_CONFIG->getCameraSupportFlag() == true ) {
		// open the default camera using default API
		// cap.open(0);
		// OR advance usage: select any API backend
		int deviceID	= THE_CONFIG->getCameraDeviceId();
		int apiID		= cv::CAP_ANY; // autodetect default API
		
		// open selected camera using selected API
		cameraCapture->open(deviceID, apiID);
		
		// check if we succeeded
		if ( !cameraCapture->isOpened() ) {
			notifyError("ERROR! Unable to open camera");
			return false;
		}
	}
	
	const bool ret = cameraTimer.Start(TIMERVALUE, true);
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncVideoCapturePanel::stop() {
///////////////////////////////////////////////////////////////////
	if ( cameraTimer.IsRunning() )
		cameraTimer.Stop();
		
	if ( THE_CONFIG->getCameraSupportFlag() == true )
		cameraCapture->release();
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::onTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( !cameraCapture->isOpened() )
		return;
		
	// wait for a new frame from camera and store it into 'frame'
	if ( cameraCapture->read(*cameraFrame) == false ) {
		notifyError(CNC_LOG_FUNCT_A(": read failed"));
		return;
	}
	
	if ( cameraFrame->empty() ) {
		notifyError(CNC_LOG_FUNCT_A(": cameraFrame->empty() == true"));
		return;
	}
	
	const cv::Size s	= cameraFrame->size();
	const int w			= s.width;
	const int h			= s.height;
	
	if ( cameraBitmap == NULL || cameraBitmap->GetWidth() != w || cameraBitmap->GetHeight() != h )
	{
		wxDELETE(cameraBitmap);
		cameraBitmap = new wxBitmap(w, h, 24);
	}
	
	if ( cameraBitmap == NULL ) {
		notifyError(CNC_LOG_FUNCT_A(": cameraBitmap == NULL"));
		cameraTimer.Stop();
		return;
	}

	if ( OSD::convertMatBitmapTowxBitmap(*cameraFrame, *cameraBitmap) == false ) {
		notifyError(CNC_LOG_FUNCT_A(": convertMatBitmapTowxBitmap(*cameraFrame, *cameraBitmap) == false"));
		return;
	}

	Refresh();
	cameraTimer.Start(TIMERVALUE, true);
}
///////////////////////////////////////////////////////////////////
void CncVideoCapturePanel::onPaint(wxPaintEvent& event) {
///////////////////////////////////////////////////////////////////
	//wxAutoBufferedPaintDC dc(this);
	wxPaintDC dc(this);
	dc.Clear();
	
	if ( cameraTimer.IsRunning() == false )
		return;
		
	if ( cameraBitmap == NULL )
		return;
		
	if ( cameraBitmap->IsOk() == false )
		return;
	
	int bw = cameraBitmap->GetWidth();
	int bh = cameraBitmap->GetHeight();
	
	dc.DrawBitmap(*cameraBitmap, 0, 0, false);
	dc.SetBackground(*wxWHITE_BRUSH);
	
	if ( dc.GetSize().x > bw ) {
		dc.SetClippingRegion(bw, 0, dc.GetSize().x - bw, dc.GetSize().y);
		dc.Clear();
		dc.DestroyClippingRegion();
	}
	
	if ( dc.GetSize().y > bh ) {
		dc.SetClippingRegion(0, bh, dc.GetSize().x, dc.GetSize().y - bh );
		dc.Clear();
	}
	
	//create graphics context from it
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);
	if ( gc != NULL ) {
		// make a path that contains a circle and some lines
		const wxPen pen(*wxRED, 1, wxSOLID);
		gc->SetPen(pen);
		
		wxGraphicsPath path = gc->CreatePath();
		
		const wxRealPoint center(dc.GetSize().x / 2.0, dc.GetSize().y / 2.0);
		
		path.AddCircle( center.x, center.y, 30.0 );
		
		if ( ddCrossFlag ) {
			path.MoveToPoint(0.0, 0.0);
			path.AddLineToPoint(dc.GetSize().x, dc.GetSize().y);
			
			path.MoveToPoint(dc.GetSize().x, 0.0);
			path.AddLineToPoint(0.0, dc.GetSize().y);
		}
		
		if ( hvCrossFlag ) {
			path.MoveToPoint(0.0, center.y);
			path.AddLineToPoint(dc.GetSize().x, center.y);
			
			path.MoveToPoint(center.x, 0.0);
			path.AddLineToPoint(center.x, dc.GetSize().y);
		}
		
		path.CloseSubpath();
		
		if ( rectFlag ) {
			path.AddRectangle(center.x - 7.5, center.y - 7.5, 15.0, 15.0);
		}
		
		gc->StrokePath(path);
		
		delete gc;
	}
}



