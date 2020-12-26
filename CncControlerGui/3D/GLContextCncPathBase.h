#ifndef OPENGL_CONTEXT_PATH_BASE_H
#define OPENGL_CONTEXT_PATH_BASE_H

#include <cfloat>
#include <vector>
#include "3D/GLCncPathData.h"
#include "3D/GLLineCluster.h"
#include "3D/GLContextBase.h"

class GLContextCncPathBase;

///////////////////////////////////////////////////
class CncGLContextObserver {
	
	private:
		static CncGLContextObserver* theObserver;
		
	protected:
		CncGLContextObserver();
		CncGLContextObserver(const CncGLContextObserver&);
		
	public:
		
		// Notification interface
		class Callback {
			public:
				Callback() {}
				~Callback() {}
				
				virtual void nofifyForCurrent(const wxString& newCtxName) = 0;
				virtual void nofifyForRegistered(const wxString& newCtxName) = 0;
				virtual void nofifyMessage(const char type, const wxString& curCtxName, const wxString& functName, const wxString& msg) = 0;
		};
		
		// additional context information
		struct CtxInfo {
		};
		
		// detor
		~CncGLContextObserver();
		
		// instance interface
		static CncGLContextObserver* getInstance() {
			if ( CncGLContextObserver::theObserver == NULL )
				CncGLContextObserver::theObserver = new CncGLContextObserver();
				
			return CncGLContextObserver::theObserver;
		}
		
	private:
		typedef std::map<GLContextCncPathBase*, CncGLContextObserver::CtxInfo> ContextMap;
		typedef std::vector<GLContextCncPathBase*> ContextIndex;
		
		GLContextCncPathBase* 		currentContext;
		ContextMap 					contextMap;
		ContextIndex 				contextIdx;
		Callback* 					callback;
		
		const wxString& getContextItemText(GLContextCncPathBase* ctx, long row, long column) const;
		
	public:
		
		const wxString& getCurrentContextName() const;
		const GLContextCncPathBase* getCurrentContext() const			{ return currentContext; }
		const unsigned int getContextCount() const						{ return contextIdx.size(); }
		const unsigned int getContextValueCount() const					{ return 32; }
		void setCallbackInterface(CncGLContextObserver::Callback* c)	{ callback = c; }
		
		bool prepareContextSwitch(const GLContextCncPathBase* ctx);
		void switchContext(const GLContextCncPathBase* ctx);
		
		const wxString& getCurrentContextItemText(long row, long column) const;
		const wxString& getRegisteredContextItemText(long row, long column) const;
		const wxString& getRegisteredContextItemText(GLContextCncPathBase* ctx) const;
		
		void appendMessage(const char type, const wxString& functName, const wxString& msg) const;
};
#define GL_CTX_OBS CncGLContextObserver::getInstance()

/////////////////////////////////////////////////////////////////
class GLContextCncPathBase : public GLContextBase {
	
	public:
	
		enum DrawType { DT_POINTS, DT_LINE_STRIP };
		
		/////////////////////////////////////////////////////////
		GLContextCncPathBase(wxGLCanvas* canvas, const wxString& contextName);
		virtual ~GLContextCncPathBase();
		
		virtual bool SetCurrent(const wxGLCanvas &win) const; 
		
		void clearPathData();
		void appendPathData(const GLOpenGLPathBuffer::CncVertex& vertex);
		void reconstruct(const GLOpenGLPathBuffer::ReconstructOptions& opt);
		
		void setCurrentClientId(long cid);
		void highlightClientId(long firstClientId, long lastClientId = -1L);
		void highlightClientIds(cnc::LongValues ids);
		
		void highlightGudePathes(long cid);
		void dimUpGudePathes();
		void dimDownGudePathes();
		
		const GLI::GLCncPath& getPathData() 				{ return cncPath; }
		GLContextCncPathBase::DrawType getDrawType() 		{ return drawType; }
		void setDrawType(GLContextCncPathBase::DrawType t) 	{ drawType = t; }
		GLOpenGLPathBufferStore* getOpenGLBufferStore() 	{ return cncPath.getOpenGLBufferStore(); }
		
		// ruler
		/////////////////////////////////////////////////////////
		GLI::GLAxisRuler& getRulerX() 			{ return ruler.xAxis; }
		GLI::GLAxisRuler& getRulerY() 			{ return ruler.yAxis; }
		GLI::GLAxisRuler& getRulerZ() 			{ return ruler.zAxis; }
		
		GLI::GLHelpLineXYPlane& getXYPlane()	{ return ruler.helpLinesXY; }
		GLI::GLHelpLineXZPlane& getXZPlane()	{ return ruler.helpLinesXZ; }
		GLI::GLHelpLineYZPlane& getYZPlane()	{ return ruler.helpLinesYZ; }
		
		// origin
		/////////////////////////////////////////////////////////
		int getOriginX() { return ( viewPort != NULL ? viewPort->getX() : 0 ); }
		int getOriginY() { return ( viewPort != NULL ? viewPort->getY() : 0 ); }
		
		long getPathItemCount()			{ return cncPath.size(); }
		
		void setVirtualEnd(long val) 	{ cncPath.setVirtualEnd(val); }
		void setVirtualEndToFirst() 	{ cncPath.setVirtualEndToFirst(); }
		void setVirtualEndToLast() 		{ cncPath.setVirtualEndToLast(); }
		
		long previewNextVertexId()		{ return cncPath.previewNextVertexId(); }
		long previewPreviousVertexId()	{ return cncPath.previewPreviousVertexId(); }
		
		void incVirtualEnd() 			{ cncPath.incVirtualEnd(); } 
		void decVirtualEnd() 			{ cncPath.decVirtualEnd(); }
		void incVirtualEndById() 		{ cncPath.incVirtualEndById(); }
		void decVirtualEndById() 		{ cncPath.decVirtualEndById(); }
		
		void spoolVertiesForCurrentId() { cncPath.spoolVertiesForCurrentId(); }
		
		const long getVirtualEnd() 		{ return cncPath.getVirtualEnd(); }
		const long getVirtualEndAsId()	{ return cncPath.getVirtualEndAsId(); }
		
		void activateNotifications() 	{ cncPath.activateNotifications(); }
		void deactivateNotifications() 	{ cncPath.deactivateNotifications(); }
		
		void registerCallback(GLI::GLCncPath::Callback* cb) 			{ cncPath.registerCallback(cb);   }
		
		void activateOpenGlContext(bool state = true);
		void deactivateOpenGlContext() { activateOpenGlContext(false); }

	protected:
		
		GLI::GLCncPath		cncPath;
		GLI::GLXYZRuler		ruler;
		DrawType 			drawType;
		
		long 				currentClientId;
		
		wxColour			rulerColourX;
		wxColour			rulerColourY;
		wxColour			rulerColourZ;
		
		virtual void initBufferStore();
		
		virtual float getAutoScaleFactor() { return ( options.autoScale ? cncPath.getAutoScaleFact() : 1.0); }
		
		virtual void determineModel();
		virtual void markCurrentPosition();
		
		void drawGuidePathes();
		void drawHardwareBox();
		void drawBoundBox();
		void drawRuler();
		
		friend class CncMotionMonitor;
		friend class CncGLContextObserver;
};

#endif