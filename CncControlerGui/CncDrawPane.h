#ifndef CNC_OPENGL_DRAW_PANE_H
#define CNC_OPENGL_DRAW_PANE_H

#include <sstream>
#include <wx/timer.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
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
		
		///////////////////////////////////////////////////
		struct ViewPort {
			float border;
			float x;
			float y;
			int w;
			int h;
			
			///////////////////////////////////////////////
			ViewPort() {
				border 	= 0.0f;
				x = y 	= 0.0f;
				w = h 	= 1;
			}
			
			///////////////////////////////////////////////
			friend std::ostream &operator<< (std::ostream &ostr, const ViewPort &a) {
				ostr << a.x << ',' << a.y << ',' << a.w << ',' << a.h << ':' << a.border;
				return ostr;
			}
		};
		
		///////////////////////////////////////////////////
		struct Translate {
			float x;
			float y;
			float z;
			
			///////////////////////////////////////////////
			Translate() {
				init();
			}
			
			///////////////////////////////////////////////
			void init() {
				x = y = 0.0f;
				z = -2.0f;
			}
			
			///////////////////////////////////////////////
			friend std::ostream &operator<< (std::ostream &ostr, const Translate &a) {
				ostr << a.x << ',' << a.y << ',' << a.z;
				return ostr;
			}
		};
		
		///////////////////////////////////////////////////
		struct Scale {
			float x;
			float y;
			float z;
			
			///////////////////////////////////////////////
			Scale() {
				init();
			}
			
			///////////////////////////////////////////////
			void init() {
				x = y = z = 1.0f;
			}
			
			///////////////////////////////////////////////
			friend std::ostream &operator<< (std::ostream &ostr, const Scale &a) {
				ostr << a.x << ',' << a.y << ',' << a.z;
				return ostr;
			}
		};
		
		///////////////////////////////////////////////
		static void destroyGlobalContext() {
			if ( globalContext != NULL )
				delete globalContext;
				globalContext = NULL;
		}

	private:
		
		// globale context
		static CncOpenGLDrawPaneContext* globalContext;
		static const unsigned int DEFAULT_SPIN_TIMER_INTERVAL = 50;

		CncOpenGLDrawPaneContext::WorkpieceInfo workpieceInfo;
		unsigned int spinTimerInterval;
		DrawPaneViewType currentViewType;
		ViewPort viewPort;
		Translate translate;
		Scale scale;
		DisplayAngels displayAngels;
		
		wxTimer spinTimer;
		
		wxButton* playButton;
		wxTextCtrl* traceCtrl;
		wxSpinCtrl* spinAngleX;
		wxSpinCtrl* spinAngleY;
		wxSpinCtrl* spinAngleZ;
		
		DrawPaneOrigin currentOrigin;

		DrawPaneData data;
		CncOpenGLData globalData;
		
		void resetProjectionMode();
		void initializeOpenGL();
		void rotate(bool updateCtrl = true);
		void spinPaneFromKeyboard(float xSpin, float ySpin);

		void OnPaint(wxPaintEvent& event);
		void OnMouse(wxMouseEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnEraseBackground(wxEraseEvent& event);
		void OnSpinTimer(wxTimerEvent& event);
		
		void stopSpinTimer();
		void startSpinTimer();
		void switchSpinTimer();
		
		void decoratePlayButton(bool state);
		
		void trace(const wxString& msg);
		void trace(const std::stringstream& ss) { return trace(ss.str().c_str()); }
		
		void rotateByKey(wxKeyEvent& event);
		void translateByKey(wxKeyEvent& event);
		void moveViewPortByKey(wxKeyEvent& event);
		
		void scaleByMouse(wxMouseEvent& event);
		void translateByMouse(wxMouseEvent& event);
		void moveViewPortByMouse(wxMouseEvent& event);

	public:
		
		// constructor
		CncOpenGLDrawPane(wxWindow *parent, int *attribList = NULL);
		~CncOpenGLDrawPane();
		
		// current data vector
		DrawPaneData& getDataVector();
		DrawPaneData& clearDataVector();
		void setDataVector(const DrawPaneData& data);
		void appendDataVector(const DoublePointPair3D& data);
		
		// repainting
		void displayDataVector();

		// setter
		void setWorkpieceInfo(const CncOpenGLDrawPaneContext::WorkpieceInfo& wi);
		void initDisplayAngles(float ax, float ay, float az);
		void setDisplayAngles(float ax, float ay, float az, bool updateCtrl = false);
		void setSpinTimerInterval(const unsigned int i) { spinTimerInterval = i; }
		
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
		
		void view3D()		{ view(DPVT_3D_ISO1);}
		void view3DIso1()	{ view(DPVT_3D_ISO1);}
		void view3DIso2()	{ view(DPVT_3D_ISO2);}
		void view3DIso3()	{ view(DPVT_3D_ISO3);}
		void view3DIso4()	{ view(DPVT_3D_ISO4);}
		
		// predefined origns
		void evaluateViewPort();
		void setOriginTL() 		{ currentOrigin = DPO_TOP_LEFT; 	return evaluateViewPort(); }
		void setOriginTR() 		{ currentOrigin = DPO_TOP_RIGHT; 	return evaluateViewPort(); }
		void setOriginBL() 		{ currentOrigin = DPO_BOTTOM_LEFT; 	return evaluateViewPort(); }
		void setOriginBR() 		{ currentOrigin = DPO_BOTTOM_RIGHT; return evaluateViewPort(); }
		void setOriginCenter() 	{ currentOrigin = DPO_CENTER; 		return evaluateViewPort(); }
		
		// control
		void clear3D();
		void animate3D();
		
		void setPlayButton(wxButton* pb);
		void setTraceCtrl(wxTextCtrl* tr);
		void setSpinCtrls(wxSpinCtrl* sx, wxSpinCtrl* sy, wxSpinCtrl* sz);
		
		// global helper functions
		static void CheckGLError();

		// global context
		static CncOpenGLDrawPaneContext& initGlobalContext(wxGLCanvas *canvas);
		
		// is used from global kex down hook
		void OnKeyDown(wxKeyEvent& event);

		wxDECLARE_NO_COPY_CLASS(CncOpenGLDrawPane);
		wxDECLARE_EVENT_TABLE();
};

#endif