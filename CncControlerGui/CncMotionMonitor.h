#ifndef CNC_MOTION_MONITOR_H
#define CNC_MOTION_MONITOR_H

#include <wx/panel.h>
#include <wx/timer.h>

#include "CncGLCanvas.h"
#include "CncPosition.h"
#include "CncCommon.h"
#include "3D/VerticeData.h"
#include "3D/GLContextPathBase.h"

wxDECLARE_EVENT(wxEVT_MOTION_MONITOR_TIMER, wxTimerEvent);

class GL3DOptions;
class CncVectiesListCtrl;

/////////////////////////////////////////////////////////////
class CncMotionMonitor : public CncGlCanvas {

	public:
		
		// .........................................................
		// callback interface
		
		struct Callback {
			Callback()          {}
			virtual ~Callback() {}
			
			virtual void notifyChange(CncMotionMonitor& mm) = 0;
			virtual void notifyCameraAngleChange(int angle) = 0;
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
		
		void notifyCameraAngleChange(int angle) {
			for ( auto it = callbackVector.begin(); it != callbackVector.end(); ++it ) {
				if ( *it != NULL )
					(*it)->notifyCameraAngleChange(angle);
			}
		}
		
		// constructor
		CncMotionMonitor(wxWindow *parent, int *attribList = NULL);
		virtual ~CncMotionMonitor();
		
		// is used from global key down hook, that's the reason why it is public
		void onKeyDown(wxKeyEvent& event);
		void onKeyDown(int keyCode);
		
		// interface
		virtual void display();
		
		void enable(bool state);
		void decorateProbeMode(bool state);
		void clear();
		void appendVertice(const GLI::VerticeLongData& vd);
		void centerViewport();
		void resetRotation();
		
		void setModelType(const GLContextBase::ModelType mt);
		
		void setAngleX(int a) { monitor->getModelRotation().setAngleX(a); display(); }
		void setAngleY(int a) { monitor->getModelRotation().setAngleY(a); display(); }
		void setAngleZ(int a) { monitor->getModelRotation().setAngleZ(a); display(); }
		
		int getAngleX() { return monitor->getModelRotation().angleX(); }
		int getAngleY() { return monitor->getModelRotation().angleY(); }
		int getAngleZ() { return monitor->getModelRotation().angleZ(); }
		
		int getCameraEyeAngle() { return monitor->getCameraPosition().getCurXYPlaneEyeAngle(); }
		
		unsigned int calculateScaleDisplay(unsigned int height);
		
		long fillVectiesListCtr(long curCount, CncVectiesListCtrl* listCtrl);
		
		void normalizeMonitor();
		void pushProcessMode();
		void popProcessMode();
		void reconstruct();
		
		void tracePathData(std::ostream& s);
		
		// camera handling
		enum CameraMode{ CM_OFF, CM_CLOCKWISE, CM_COUNTER_CLOCKWISE};
		void rotateCamera(int angle);
		int getCameraRotationSpeed() { return cameraRotationSpeed; }
		void setCameraRotationSpeed(int speed);
		void cameraRotationTimerHandler(CncMotionMonitor::CameraMode cm);
		
		// zoom handling
		float getZoom() 		{ return zoom; }
		void setZoom(float z) 	{ zoom = z; monitor->setZoomFactor(zoom); }
		
		// smoothing
		bool isSmoothingEnabled() 				{ return monitor->isSmoothingEnabled(); }
		void enableSmoothing(bool state=true) 	{ monitor->enableSmoothing(state); }
		
		// draw type
		GLContextCncPathBase::DrawType getDrawType() 		{ return monitor->getDrawType(); }
		void setDrawType(GLContextCncPathBase::DrawType t) 	{ monitor->setDrawType(t); }
		
		GLContextOptions& getContextOptions() 				{ return monitor->getOptions(); }
		
		GLContextBase::ViewMode getViewMode() 				{ return monitor->getViewMode(); }
		
		// client id
		long getCurrentClientId() 			{ return currentClientID; }
		void setCurrentClientId(long id) 	{ currentClientID = id; monitor->setCurrentClientId(id); }
		void resetCurrentClientId() 		{ setCurrentClientId(-1L); }
		
		void updateMonitor();
		
		long getPathItemCount()			{ return monitor->getPathItemCount(); }
		
		void setVirtualEnd(long val) 	{ monitor->setVirtualEnd(val); }
		void resetVirtualEnd() 			{ monitor->resetVirtualEnd();  }
		long getVirtualEnd() 			{ return monitor->getVirtualEnd(); }
		
	protected:
	
		GLContextCncPathBase* 		monitor;
		GLContextCncPathBase* 		testCube;
		
		wxTimer cameraRotationTimer;
		int cameraRotationStepWidth;
		int cameraRotationSpeed;
		
		bool isShown;
		
		float zoom;
		
		bool currentClientID;
		
		void createRuler();
		
		void onPaint(wxPaintEvent& event);
		void onMouse(wxMouseEvent& event);
		void onSize(wxSizeEvent& event);
		void onLeave(wxMouseEvent& event);
		void onEraseBackground(wxEraseEvent& event);
		void onCameraRotationTimer(wxTimerEvent& event);
		
	private:
		
		inline void appendVertice(long id, float x, float y, float z, GLI::GLCncPathVertices::CncMode cm);
		inline void onPaint();
		
		wxDECLARE_NO_COPY_CLASS(CncMotionMonitor);
		wxDECLARE_EVENT_TABLE();
};

#endif