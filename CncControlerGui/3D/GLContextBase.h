#ifndef OPENGL_CONTEXTBASE_H
#define OPENGL_CONTEXTBASE_H

#include <wx/colour.h>

#include "3D/GLViewPort.h"
#include "3D/GLCncPathData.h"
#include "3D/GLModelHelper.h"
#include "CncConfig.h"

#ifdef _USE_WX_DUMMY_CANVAS 
	#include "wxDummyCanvas.h"
#else
	#include <wx/glcanvas.h>
#endif

#ifdef _USE_WX_DUMMY_CANVAS 
	namespace OGL_DEBUG {
		extern void debugInfo(const char* context, const char* function, const char* message);
		extern void debugError(const char* context, const char* function, const char* message);
	}
#else
	namespace OGL_DEBUG {
		void debugInfo(const char* context, const char* function, const char* message);
		void debugError(const char* context, const char* function, const char* message);
	}
#endif

/////////////////////////////////////////////////////////////////
class OpenGLContextBase : public wxGLContext {
	
	public:
		OpenGLContextBase(wxGLCanvas* canvas);
		virtual ~OpenGLContextBase();
		
		enum ViewType { VT_2D = 100, VT_3D = 200 };
		enum ViewMode {	V2D_TOP 		= VT_2D, 
						V2D_BOTTOM, 
						V2D_LEFT, 
						V2D_RIGHT, 
						V2D_FRONT, 
						V2D_REAR,
						
						V3D_ISO1 		= VT_3D, 
						V3D_ISO2, 
						V3D_ISO3, 
						V3D_ISO4, 
					};
		
		// public context interface 
		virtual const char* getContextName() = 0;
		virtual void keyboardHandler(unsigned char c);
		
		void init();
		void display();
		void reshape(int w, int h, int x=0, int y=0);
		void reshapeViewMode(int w, int h);
		
		//smoothing
		void enableSmoothing(bool enable=true);
		void disableSmoothing() { enableSmoothing(false); }
		bool isSmoothingEnabled();
		
		// viewPort
		void centerViewport();
		
		// view mode
		void setViewMode(OpenGLContextBase::ViewMode m);
		OpenGLContextBase::ViewMode getViewMode() { return viewMode; }
		
		bool isViewMode2D() { return isViewMode2D(viewMode); }
		bool isViewMode3D() { return isViewMode3D(viewMode); }
		bool isViewMode2D(OpenGLContextBase::ViewMode m) { return (m >= V2D_TOP && m < V3D_ISO1); }
		bool isViewMode3D(OpenGLContextBase::ViewMode m) { return (m >= V3D_ISO1); }
		
		OpenGLContextBase::ViewType getViewType() { return getViewType(viewMode); }
		OpenGLContextBase::ViewType getViewType(OpenGLContextBase::ViewMode m) {
			if ( isViewMode2D(m) )
				return ViewType::VT_2D;
				
			return ViewType::VT_3D;
		}

		GLViewPort::PreDefPos convertViewMode(OpenGLContextBase::ViewMode m);
		
		// model scale
		void decScale() { modelScale.decScale(); display(); }
		void incScale() { modelScale.incScale(); display(); }
		
		// model rotate
		void resetAngles() { modelRotate.reset(); }
		void restoreDefaultAngles() { modelRotate.restoreDefaults(); }
		
		void decAngle()  { modelRotate.decAngle();  display(); }
		void decAngleX() { modelRotate.decAngleX(); display(); }
		void decAngleY() { modelRotate.decAngleY(); display(); }
		void decAngleZ() { modelRotate.decAngleZ(); display(); }
		
		void incAngle()  { modelRotate.incAngle();  display(); }
		void incAngleX() { modelRotate.incAngleX(); display(); }
		void incAngleY() { modelRotate.incAngleY(); display(); }
		void incAngleZ() { modelRotate.incAngleZ(); display(); }
		
		// error
		void checkGLError();
		
	protected:
		////////////////////////////////////////////////////
		class CoordOrginInfo {
			public:
				struct Colours {
					wxColour x, y, z;
					
					Colours() 
					: x(255, 0, 0), y(0, 255, 0), z(0, 0, 255)
					{}
				};
			
				Colours colours;
				float length;

				CoordOrginInfo() 
				: colours(), length(0.25f)
				{}
		};

		bool 				initialized;
		bool				drawViewPortBounderies;
		ViewMode			viewMode;
		CoordOrginInfo 		coordOriginInfo;
		GLViewPort*			viewPort;
		GLI::ModelScale 	modelScale;
		GLI::ModelRotate	modelRotate;
		
		virtual void determineCoordinateOrigin();
		virtual void determineViewPort(int w, int h, int x=0, int y=0);
		
		// protected context interface
		virtual void initContext() = 0;
		virtual GLViewPort* createViewPort() = 0;
		
		virtual void determineProjection(int w, int h);
		virtual void determineCameraPosition();
		virtual void determineModel();
		
		void renderBitmapString(float x, float y, float z, void* font, const char* string);
		 
	private:
		void determineViewPortBounderies();
};

#endif