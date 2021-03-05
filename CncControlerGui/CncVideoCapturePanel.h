#ifndef CNC_VIDEO_CAPTURE_PANEL_H
#define CNC_VIDEO_CAPTURE_PANEL_H

#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/panel.h>

namespace cv { 
	class Mat; 
	class VideoCapture;
}

class CncVideoCapturePanel : public wxPanel {
	
	public:
		// ----------------------------------------------------------
		class CallbackInterface {

			public:
				CallbackInterface() {}
				virtual ~CallbackInterface() {}

				virtual void notifyProgess(const wxString& msg) {}
				virtual void notifyError(const wxString& msg) {}
		};
		
		CncVideoCapturePanel(wxWindow* parent);
		virtual ~CncVideoCapturePanel();
		
		bool start();
		bool stop();
		
		void showHVCross(bool b);
		void showDDCross(bool b);
		void showRect(bool b);
		
		void setCaller(CallbackInterface* c) { caller = c; }
		
	private:
		
		static const int	FPS 			= 25;
		static const int	TIMERVALUE		= ( 1000 / FPS );
		
		bool				hvCrossFlag;
		bool				ddCrossFlag;
		bool				rectFlag;
		wxTimer				cameraTimer;
		wxBitmap*			cameraBitmap;
		cv::Mat*			cameraFrame;
		cv::VideoCapture*	cameraCapture;
		CallbackInterface*	caller;
		
		void				notifyProgess(const wxString& msg);
		void				notifyError(const wxString& msg);
		
		void				onPaint(wxPaintEvent& event);
		void				onTimer(wxTimerEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncVideoCapturePanel);
		wxDECLARE_EVENT_TABLE();
};

#endif