#ifndef OPENGL_CONTEXTBASE_H
#define OPENGL_CONTEXTBASE_H

#include <ostream>
#include <vector>
#include <wx/layout.h>
#include <wx/colour.h>

#include "3D/GLViewPort.h"
#include "3D/GLHelperModel.h"
#include "3D/GLHelperCamera.h"
#include "3D/GLGuidePath.h"
#include "3D/GLInclude.h"

#include "CncPosition.h"
#include "CncCommon.h"

/////////////////////////////////////////////////////////////////
struct GLContextOptions
{
	// .........................................................
	// options
	bool autoScale						= true;
	bool blending						= true;
	bool probeMode						= false;
	
	bool showOrigin						= true;
	bool showViewPortBoundaries			= true;
	bool showMillingCutter				= true;
	bool showPosMarker					= true;
	bool showFlyPath					= true;
	bool showTotalBoundBox				= true;
	bool showObjectBoundBox				= true;
	bool showHardwareBox				= true;
	bool showRuler						= true;
	bool showHelpLines					= true;
	bool showGuidePathes				= true;
	
	bool helpLines3D_XYPlane			= true;
	bool helpLines3D_XZPlane			= false;
	bool helpLines3D_YZPlane			= false;
	
	unsigned int rulerOriginOffsetAbs	= 10;
	unsigned int rapidAlpha				= 64;
		
	wxColour hardwareBoxColour			= wxColour(255, 128, 255);
	wxColour totBoundBoxColour			= wxColour(185, 127,  87);
	wxColour objBoundBoxColour			= wxColour(112, 146, 190);
	wxColour rapidColour				= *wxYELLOW;
	wxColour workColour					= *wxWHITE;
	wxColour maxColour					= *wxBLUE;
	wxColour userColour					= *wxLIGHT_GREY;
	
	// .........................................................
	// option interface
	
	bool toggleOption(bool& option) {
		option = !option;
		return option;
	}

	// .........................................................
	// callback interface
	
	struct Callback {
		Callback()          {}
		virtual ~Callback() {}
		
		virtual void notifyChange(GLContextOptions& options) = 0;
	};
	
	typedef std::vector<Callback*> CallbackVector;
	CallbackVector callbackVector;
	
	void registerCallback(Callback* c) {
		callbackVector.push_back(c);
	}
	
	void notifyChange() {
		for ( auto it = callbackVector.begin(); it != callbackVector.end(); ++it ) {
			if ( *it != NULL )
				(*it)->notifyChange(*this);
		}
	}
};

/////////////////////////////////////////////////////////////////
class GLContextBase : public wxGLContext {
	
	public:
		GLContextBase(wxGLCanvas* canvas, const wxString& name);
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
						
		enum FrontCatchingMode { FCM_OFF = 0, FCM_KEEP_IN_FRAME = 1, FCM_ALWAYS_CENTRED = 2 };
		
		typedef std::vector<GLGuidePath> GuidePathes;
		
		// structure to capture the vertex parameter of the a mouse focus. 
		struct MouseVertexInfo {
			
			bool		valid		= false;
			
			long		bufPos		= -1;
			
			int			winX		= 0;
			int			winY		= 0;
			int			winZ		= 0;
			
			GLdouble	vecX 		= 0.0;
			GLdouble	vecY 		= 0.0;
			GLdouble	vecZ 		= 0.0;
			
			void reset() { *this = MouseVertexInfo(); }
			
			int32_t getAsStepsX(float scaleFactor);
			int32_t getAsStepsY(float scaleFactor);
			int32_t getAsStepsZ(float scaleFactor);
			
			double getAsMetricX(float scaleFactor);
			double getAsMetricY(float scaleFactor);
			double getAsMetricZ(float scaleFactor);
		};
		

		// common 
		void clearGuidePathes();
		void addGuidePath(const GLGuidePath& gp);
		
		const char* getContextName() const { return contextName; }
		
		virtual wxString getNormalizedClientIdOfPos(float x, float y, float z);
		virtual long getPositionWithinBuffer(float x, float y, float z);
		
		static void setCurrentCanvas(const wxGLCanvas* canvas) 		{ currentCanvas = canvas; }
		static const wxGLCanvas* getCurrentCanvas()					{ return currentCanvas; }
		static const bool isCurrent(const GLContextBase& c )		{ return c.isCurrent(); }
		
		const bool isCurrent() const  	 							{ return currentCanvas == associatedCanvas; }
		const wxGLCanvas* getAssociatedCanvas() const				{ return associatedCanvas; }
				
		static void globalInit();
		
		void enable(bool state = true) 	{ enabled = state; } 
		void disable() 					{ enable(false); }
		bool isEnabled() 				{ return enabled; }
		bool isProbeMode() 				{ return options.probeMode; }
		void decorateProbeMode(bool state);
		
		bool init();
		void display();
		
		void reshapeAbsolute(int px, int py);
		void reshapeRelative(int dx, int dy);
		void reshape();
		void reshapeViewMode();
		void reshapeBestOrigion(const CncDoubleBoundaries& box);
		void reshapePosToCenter(const CncLongBoundaries& box);
		void reshapePosToCenter(const CncDoubleBoundaries& box);
		void reshapePosToCenter(int px, int py);
		void reshapePosToCenterIfOutOfFocus(int px, int py);
		void reshapeCompleteVisible(const CncDoubleBoundaries& box);
		void reshapeCompleteVisible();
		
		int getLastReshapeX()			const;
		int getLastReshapeY()			const;
		int getCurrentWindowWidth()		const;
		int getCurrentWindowHeight()	const; 
		
		void setFrontCatchingMode(FrontCatchingMode mode);
		const char* getFrontCatchingModeAsStr(FrontCatchingMode mode);
		
		void setAutoScaling(bool as);
		void normalizeScaling();
		void normalizeRotation();
		void normalizeCamera();
		
		GLContextOptions& getOptions() 				{ return options; }
		
		// smoothing
		void enableSmoothing(bool enable=true);
		void disableSmoothing() 					{ enableSmoothing(false); }
		bool isSmoothingEnabled();
		
		// viewPort
		void centreViewport();
		void resetViewport();
		
		virtual float getMaxScaleFactor();
		virtual float getCurrentScaleFactor() const;

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
		
		// Mouse vertex
		bool convertWinCoordsToVertex(int winX, int winY, GLdouble& vX, GLdouble& vY, GLdouble& vZ);
		bool logWinCoordsToVertex(int winX, int winY);
		void delWinCoordsToVertex();
		
		bool isVertexVisible(const CncDoublePosition& p);
		bool isVertexVisible(GLdouble px, GLdouble py, GLdouble pz);
		bool isBoxVisible(const CncDoubleBoundaries& box);

		bool makeCompleteVisible(const CncDoubleBoundaries& box);
		bool keepVisible(GLdouble px, GLdouble py, GLdouble pz);
		
		const MouseVertexInfo& getCurrentMouseVertexInfo() { return currentMouseVertexInfo; }
		
		float getMouseVertexX()				{ return currentMouseVertexInfo.vecX; }
		float getMouseVertexY()				{ return currentMouseVertexInfo.vecY; }
		float getMouseVertexZ()				{ return currentMouseVertexInfo.vecZ; }
		
		int32_t getMouseVertexAsStepsX()	{ return currentMouseVertexInfo.getAsStepsX(getCurrentScaleFactor()); }
		int32_t getMouseVertexAsStepsY()	{ return currentMouseVertexInfo.getAsStepsY(getCurrentScaleFactor()); }
		int32_t getMouseVertexAsStepsZ()	{ return currentMouseVertexInfo.getAsStepsZ(getCurrentScaleFactor()); }
		
		double getMouseVertexAsMetricX()	{ return currentMouseVertexInfo.getAsMetricX(getCurrentScaleFactor()); }
		double getMouseVertexAsMetricY()	{ return currentMouseVertexInfo.getAsMetricY(getCurrentScaleFactor()); }
		double getMouseVertexAsMetricZ()	{ return currentMouseVertexInfo.getAsMetricZ(getCurrentScaleFactor()); }
		
		void setSpindlePowerState(CncSpindlePowerState state);
		
		virtual bool getBounderies(CncDoubleBoundaries& ret) const;
		virtual std::ostream& traceInformation(std::ostream& o) const;
		
	protected:
		
		////////////////////////////////////////////////////
		class CoordOrginInfo
		{
			public:
				struct Colours 
				{
					wxColour x, y, z;
					
					Colours() 
					: x(255, 64, 64), y(89, 89, 255), z(27, 139, 61)
					{}
				};
			
				Colours colours;
				float length;

				CoordOrginInfo() 
				: colours(), length(0.25f)
				{}
		};
		
		////////////////////////////////////////////////////
		class FeedbackBuffer
		{
			private:
				
				GLenum			type;
				GLContextBase*	context;
				GLint 			size;
				GLint			count;
				GLfloat*		buffer;
				GLint			prevMode;
				
				int getEntrySize()
				{
					const int k = 4; // rgb mode
					
					switch ( type )
					{
						case GL_2D:					return 2;
						case GL_3D:					return 3;
						case GL_3D_COLOR:			return 3 + k;
						case GL_3D_COLOR_TEXTURE:	return 7 + k;
						case GL_4D_COLOR_TEXTURE:	return 8 + k;
					}
					
					return -1;
				}
				
			public:
				
				FeedbackBuffer(GLenum t, GLContextBase* ctx);
				~FeedbackBuffer();
				
				GLenum getType()	{ return type; }
				GLint getSize()		{ return size; }
				
				void  activate();
				GLint deactivate();
				
				bool checkIfAllFeedbackPointsAreVisible();
				void traceFeedbackBuffer(std::ostream& out);
				void traceFeedbackBufferRaw(std::ostream& out);
		};
		
		wxGLCanvas*				associatedCanvas;
		wxString 				contextName;
		
		bool					enabled;
		bool 					initialized;
		
		FrontCatchingMode		frontCatchingMode;
		
		GuidePathes				guidePathes;
		
		MouseVertexInfo			currentMouseVertexInfo;
		GLContextOptions 		options;
		
		float					zoom;
		
		ViewMode				viewMode;
		CoordOrginInfo 			coordOriginInfo;
		ModelType				modelType;
		GLViewPort*				viewPort;
		GLI::ModelScale 		modelScale;
		GLI::ModelRotate		modelRotate;
		GLI::CameraPosition		cameraPos;
		
		CncSpindlePowerState	spindlePowerState;
		
		virtual void drawMousePosition();
		virtual void drawMovePosition(float x, float y, float z);
		virtual void drawCrossHair(float x, float y, float z);
		virtual void drawCoordinateOrigin();
		
		void determineViewPort(int w, int h);
		void determineViewPort(int w, int h, int x, int y=0);
		
		// protected context interface
		virtual void initContext() = 0;
		virtual void initBufferStore() = 0;
		virtual GLViewPort* createViewPort() = 0;
		virtual void markCurrentPosition() = 0;
		
		virtual void determineProjection(int w, int h);
		virtual void determineCameraPosition();
		virtual void determineModel();
		
		void drawTeapot(void);
		void renderBitmapString(float x, float y, float z, void* font, const char* string);
		
		GLuint LoadBMP(const wxImage& img);
		void drawBox(GLfloat size, GLenum type);
		 
		void drawSolidCone(GLdouble radius, GLdouble height, GLint slices, GLint stacks, bool bottomUp=true);
		void drawSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
		
		void drawSilhouetteCone(GLdouble radius, GLdouble height, GLint slices, GLint stacks, bool bottomUp=true);
		
		void drawFeedbackVertices();
		
		void drawAdditionalThings();
		
		void traceOpenGLMatrix(std::ostream &ostr, int id);
		
		CncLongPosition evaluateWindowCoordinates(double px, double py, double pz) const ;
		CncLongPosition evaluateWindowCoordinates(const CncDoublePosition& p) const;
		
		int getVertextMostlyInDirection(wxEdge direction, const CncDoublePositionVector& v);
		int getCornerIdxMostlyInDirection(wxEdge direction, const CncDoubleBoundaries& b);
		
		wxPoint determineBestOrigin(const CncDoubleBoundaries& b);
		
		void traceBoundariesInfos(std::ostream& o, const CncDoubleBoundaries& box) const;
		
	private:
		
		static const wxGLCanvas* currentCanvas;
		
		typedef std::vector<CncDoublePosition> FeedbackVectices;
		
		GLuint 				theTexture;
		FeedbackVectices	feedbackVertices;
		
		void determineViewPortBoundaries();
		void drawMillingCutter(CncDimensions d, float x, float y, float z);
};

#endif
