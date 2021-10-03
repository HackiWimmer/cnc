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
#include "CncContext.h"
#include "CncVideoCapturePanel.h"

// ----------------------------------------------------------------------------
// CncVideoCapturePanel Event Table
// ----------------------------------------------------------------------------
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
, errorMessage		()
, cameraTimer		(this, wxID_ANY)
, cameraBitmap		(NULL)
, cameraFrame		(new cv::Mat())
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
	if ( THE_CONFIG->getCameraSupportFlag() == false ) {
		setErrowMessage("Camera support isn't configured.");
		return false;
	}
	
	if ( THE_CONTEXT->hasHardware() == false ) {
		setErrowMessage("The current connected serial port do not support hardware.\nTherefore, no camera available.");
		return false;
	}
		
	if ( !cameraCapture->isOpened() ) {
		// open the defined default camera using default API
		// cap.open(0);
		// OR advance usage: select any API backend
		int deviceID	= THE_CONFIG->getCameraDeviceId();
		int apiID		= cv::CAP_DSHOW;
		
		// open selected camera using selected API
		// Don't use cv::CAP_ANY direct first here because it generates a GStreamer warning
		cameraCapture->open(deviceID, apiID);
		
		if ( !cameraCapture->isOpened() ) 
			cameraCapture->open(deviceID, cv::CAP_ANY); // autodetect default API
		
		// check if we succeeded
		if ( !cameraCapture->isOpened() ) {
			notifyError("ERROR! Unable to open camera");
			return false;
		}
		else {
			//std::cout << "Using the following camera back end: " << cameraCapture->getBackendName() <<std::endl;
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
		
	if ( THE_CONFIG && THE_CONFIG->getCameraSupportFlag() == false )
		return false;
		
	if ( THE_CONTEXT && THE_CONTEXT->hasHardware() == false )
		return false;

	if ( cameraCapture->isOpened() )
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
	//wxPaintDC dc(this);
	//dc.Clear();
	wxAutoBufferedPaintDC paintDC(this);
	paintDC.Clear();
	wxGCDC dc(paintDC);
	
	if ( cameraTimer.IsRunning() == false ) {
		dc.SetTextForeground(*wxRED);
		dc.DrawText(errorMessage, wxPoint(dc.GetSize().x / 4, dc.GetSize().y / 2 - 10 ));
		return;
	}
		
	if ( cameraBitmap == NULL )
		return;
	
	if ( cameraBitmap->IsOk() == false )
		return;
		
	const wxSize size = GetSize();
	const int w  = size.x;
	const int h  = size.y;
	const int bw = cameraBitmap->GetWidth();
	const int bh = cameraBitmap->GetHeight();

	const wxPoint center(w / 2, h / 2);
	//std::cout << wxString::Format("%d, %d, %d, %d, %d, %d\n",w, h, bw, bh, (w - bw) / 2, ( h - bh ) / 2);
	
	dc.DrawBitmap(*cameraBitmap,  (w - bw) / 2, ( h - bh ) / 2, false);
	
	/*
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
	*/
	
	dc.SetBackground(*wxTRANSPARENT_BRUSH);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(*wxRED);
	if ( ddCrossFlag ) {
		dc.DrawLine(0, 0, w, h);
		dc.DrawLine(0, h, w, 0);
	}
	
	if ( hvCrossFlag ) {
		dc.DrawLine(0, center.y, w, center.y);
		dc.DrawLine(center.x, 0, center.x, h);
	}
	
	if ( rectFlag ) {
		const int lr = 7;
		dc.DrawRectangle(center.x - lr + 1, center.y - lr + 1, 2 * lr, 2 * lr);
		dc.DrawCircle(center, 12.0);
		dc.DrawCircle(center, 16.0);
		dc.DrawCircle(center, 22.0);
		dc.DrawCircle(center, 36.0);
	}
}
