#ifndef CNC_OPENGL_DRAW_PANE_H
#define CNC_OPENGL_DRAW_PANE_H

#include <wx/timer.h>
#include <wx/glcanvas.h>
#include <wx/button.h>
#include "CncDrawPaneContext.h"

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

// control ids
enum {
	SpinTimer = wxID_HIGHEST + 1
};

////////////////////////////////////////////////////////////		
struct CncOpenGLData {
////////////////////////////////////////////////////////////		
	
	private:
		static const int MAX_QUAT = 4;
	    bool initialized;           // have OpenGL been initialized?
	    
    public:
    	// OpenGL view data	
	    float beginx, beginy;		// position of mouse
	    float quat[MAX_QUAT];       // orientation of object
	    float zoom;                 // field of view in degrees
	    
	    ///////////////////////////////////////////////////////
	    CncOpenGLData() 
	    ///////////////////////////////////////////////////////
	    : initialized(false)
    	, beginx(0.0f)
    	, beginy(0.0f)
    	, zoom(45.0f)
    	{
    		for ( int i=0; i<MAX_QUAT; i++ ) {
    			quat[i] = 0.0f;
    		}
	    }
	    
	    ///////////////////////////////////////////////////////
	    ~CncOpenGLData() {
	    ///////////////////////////////////////////////////////
		}
		
		///////////////////////////////////////////////////////
		void initialize() {
		///////////////////////////////////////////////////////
			initialized = true;
		}
		
		///////////////////////////////////////////////////////
		bool isInitialized() {
		///////////////////////////////////////////////////////	
			return initialized;
		}
};

////////////////////////////////////////////////////////////		
class CncOpenGLDrawPane : public wxGLCanvas {
////////////////////////////////////////////////////////////		

	public:
		
		enum DrawPaneViewType { DPVT_Front, DPVT_Rear, DPVT_Top, DPVT_Bottom, DPVT_Left, DPVT_Right, DPVT_3D };
		
		// constructor
		CncOpenGLDrawPane(wxWindow *parent, int *attribList = NULL);
		~CncOpenGLDrawPane();
		
		// current data vector
		DrawPaneData& getDataVector();
		DrawPaneData& clearDataVector();
		
		// repaining
		void displayDataVector();

		// setter
		void determineDisplayAngles(float ax, float ay, float az);
		void setSpinTimerInterval(const unsigned int i) {
			spinTimerInterval = i;
		}
		
		// test
		void runOpenGLTest();
		
		// views
		void view(DrawPaneViewType view);
		void viewFront() 	{ view(DPVT_Front);  }
		void viewRear()		{ view(DPVT_Rear);   }
		void viewTop()		{ view(DPVT_Top);    }
		void viewBottom()	{ view(DPVT_Bottom); }
		void viewLeft()		{ view(DPVT_Left);   }
		void viewRight()	{ view(DPVT_Right);  }
		void view3D()		{ view(DPVT_3D);  	 }
		
		// control
		void clear3D();
		void animate3D();
		
		void setPlayButton(wxButton* pb);
		
		// global helper functions
		static void CheckGLError();

		// global context
		static CncOpenGLDrawPaneContext& initGlobalContext(wxGLCanvas *canvas);

	private:
		
		// globale context
		static CncOpenGLDrawPaneContext* globalContext;
		static const unsigned int DEFAULT_SPIN_TIMER_INTERVAL = 150;

		unsigned int spinTimerInterval;
		float fovy;
		wxTimer spinTimer;
		DisplayAngels displayAngels;
		wxButton* playButton;
		DrawPaneData data;
		CncOpenGLData globalData;
		
		void resetProjectionMode();
		void initializeOpenGL();
		void rotate();
		void spinPaneFromKeyboard(float xSpin, float ySpin);

		void OnPaint(wxPaintEvent& event);
		void OnMouse(wxMouseEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnEraseBackground(wxEraseEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnSpinTimer(wxTimerEvent& event);
		
		void stopSpinTimer();
		void startSpinTimer();
		void switchSpinTimer();
		
		void decoratePlayButton(bool state);

		wxDECLARE_NO_COPY_CLASS(CncOpenGLDrawPane);
		wxDECLARE_EVENT_TABLE();
};

#endif