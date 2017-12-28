#ifndef OPENGL_CONTEXTBASE_H
#define OPENGL_CONTEXTBASE_H

#include <ostream>
#include <wx/colour.h>

#include "3D/GLViewPort.h"
#include "3D/GLCncPathData.h"
#include "3D/GLHelperModel.h"
#include "3D/GLHelperCamera.h"

#ifdef _USE_WX_DUMMY_CANVAS 
	#include "wxDummyCanvas.h"
#else
	#include <wx/glcanvas.h>
#endif

/////////////////////////////////////////////////////////////////
class GLContextBase : public wxGLContext {
	
	public:
		GLContextBase(wxGLCanvas* canvas);
		virtual ~GLContextBase();
		
		enum ModelType { MT_LEFT_HAND, MT_RIGHT_HAND };
		enum ViewType  { VT_2D = 100, VT_3D = 200 };
		enum ViewMode  {	V2D_TOP 		= VT_2D, 
							V2D_BOTTOM, 
							V2D_LEFT, 
							V2D_RIGHT, 
							V2D_FRONT, 
							V2D_REAR,
							
							V3D_ISO1 		= VT_3D, 
							V3D_ISO2, 
							V3D_ISO3, 
							V3D_ISO4,
							
							V2D_CAM_ROT_XY_ZTOP
						};
		
		// common 
		virtual const char* getContextName() = 0;
		virtual void keyboardHandler(unsigned char c);
		
		static void globalInit();
		static void traceOpenGLVersionInfo(std::ostream& s);
		static void traceeOpenGLExtentionInfo(std::ostream& s);
		
		void enable(bool state = true) { enabled = state; } 
		void disable() { enable(false); }
		bool isEnabled() { return enabled; }
		
		void init();
		void display();
		void reshape(int w, int h);
		void reshape(int w, int h, int x, int y);
		void reshapeViewMode(int w, int h);
		void reshapeViewMode();
		
		int getLastReshapeX() { return lastReshapeX; }
		int getLastReshapeY() { return lastReshapeY; }
		int getLastReshapeW() { return lastReshapeW; }
		int getLastReshapeH() { return lastReshapeH; }
		
		void setAutoScaling(bool as);
		void normalizeScaling();
		void normalizeRotation();
		void normalizeCamera();
		
		// smoothing
		void enableSmoothing(bool enable=true);
		void disableSmoothing() { enableSmoothing(false); }
		bool isSmoothingEnabled();
		
		// postion marker
		void enablePositionMarker(bool enable=true) { posMarker = enable; }
		bool isPositionMarkerEnabled() { return posMarker; }
		
		// viewPort
		void centerViewport();
		
		virtual float getMaxScaleFactor();
		virtual float getCurrentScaleFactor();

		// view mode
		void setViewMode(GLContextBase::ViewMode newMode, bool force=false);
		GLContextBase::ViewMode getViewMode() { return viewMode; }
		const char* getViewModeAsString();
		
		bool isViewMode2D() { return isViewMode2D(viewMode); }
		bool isViewMode3D() { return isViewMode3D(viewMode); }
		bool isViewMode2D(GLContextBase::ViewMode newMode) { return (newMode >= V2D_TOP && newMode < V3D_ISO1); }
		bool isViewMode3D(GLContextBase::ViewMode newMode) { return (newMode >= V3D_ISO1); }
		
		GLContextBase::ViewType getViewType() { return getViewType(viewMode); }
		GLContextBase::ViewType getViewType(GLContextBase::ViewMode newMode) {
			if ( isViewMode2D(newMode) )
				return ViewType::VT_2D;
				
			return ViewType::VT_3D;
		}
		
		void setModelType(const ModelType mt) { modelType = mt; }
		const ModelType getModelType() { return modelType; }

		GLViewPort::PreDefPos convertViewMode(GLContextBase::ViewMode newMode);
		
		GLI::ModelScale& 		getModelScale() 	{ return modelScale; }
		GLI::ModelRotate&		getModelRotation() 	{ return modelRotate; }
		GLI::CameraPosition& 	getCameraPosition() { return cameraPos; }
		
		void setZoomFactor(float z) {
			if ( z <= 0.0f )
				return;
				
			zoom = z;
		}
		
		// error
		void checkGLError();
		
	protected:
		////////////////////////////////////////////////////
		class CoordOrginInfo {
			public:
				struct Colours {
					wxColour x, y, z;
					
					Colours() 
					: x(255, 0, 0), y(27, 139, 61), z(0, 0, 255)
					{}
				};
			
				Colours colours;
				float length;

				CoordOrginInfo() 
				: colours(), length(0.25f)
				{}
		};
		
		bool				enabled;
		bool 				initialized;
		bool				drawViewPortBounderies;
		bool				posMarker;
		bool				autoScale;
		bool				blending;
		
		float				zoom;
		
		ViewMode			viewMode;
		CoordOrginInfo 		coordOriginInfo;
		ModelType			modelType;
		GLViewPort*			viewPort;
		GLI::ModelScale 	modelScale;
		GLI::ModelRotate	modelRotate;
		GLI::CameraPosition cameraPos;
		
		
		virtual float getAutoScaleFactor() { return 1.0; }
		
		virtual void drawCoordinateOrigin();
		virtual void drawPosMarker(float x, float y, float z);
		
		virtual void determineViewPort(int w, int h, int x=0, int y=0);
		
		// protected context interface
		virtual void initContext() = 0;
		virtual GLViewPort* createViewPort() = 0;
		virtual void markCurrentPosition() = 0;
		
		virtual void determineProjection(int w, int h);
		virtual void determineCameraPosition();
		virtual void determineModel();
		
		void renderBitmapString(float x, float y, float z, void* font, const char* string);
		 
	private:
		
		int lastReshapeX;
		int lastReshapeY;
		int lastReshapeW;
		int lastReshapeH;
		
		void determineViewPortBounderies();
		void drawSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);

};

#endif