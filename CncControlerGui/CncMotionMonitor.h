#ifndef CNC_MOTION_MONITOR_H
#define CNC_MOTION_MONITOR_H

#include <wx/panel.h>
#include <wx/timer.h>

#include "3D/CncGLCanvas.h"
#include "3D/VerticeData.h"
#include "3D/GLContextCncPathBase.h"
#include "3D/GLContextBase.h"
#include "CncPathListManager.h"
#include "CncPosition.h"
#include "CncCommon.h"

wxDECLARE_EVENT(wxEVT_MOTION_MONITOR_TIMER, wxTimerEvent);

/////////////////////////////////////////////////////////////
class CncMotionMonitor : public CncGlCanvas 
                       , public GLI::GLCncPath::Callback 
{
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
		
		void registerMonitorCallback(GLI::GLCncPath::Callback* cb) {
			wxASSERT(monitor != NULL);
			monitor->registerCallback(cb);
		}
		
		// constructor
		CncMotionMonitor(wxWindow *parent, int *attribList = NULL);
		virtual ~CncMotionMonitor();
		
		std::ostream& traceInformation(std::ostream& o) const;
		void traceInformation();
		
		virtual bool Show(bool show);
		virtual void update(bool force=false);
		
		void synchronizeClientId();

		// is used from global key down hook, that's the reason why it is public
		void onKeyDown(wxKeyEvent& event);
		void onKeyDown(int keyCode);
		
		// interface
		void enable(bool state);
		void decorateProbeMode(bool state);
		void clear();
		void appendVertex(const GLI::VerticeLongData& vd);
		void appendVertex(long clientId, CncSpeedMode sm, const CncLongPosition& pos);
		void appendGuidPath(const CncPathListManager& plm);
		
		void centreViewport();
		void resetRotation();
		
		void setModelType(const GLContextBase::ModelType mt);
		
		void setAngleX(int a) { monitor->getModelRotation().setAngleX(a); Refresh(); }
		void setAngleY(int a) { monitor->getModelRotation().setAngleY(a); Refresh(); }
		void setAngleZ(int a) { monitor->getModelRotation().setAngleZ(a); Refresh(); }
		
		int getAngleX() { return monitor->getModelRotation().angleX(); }
		int getAngleY() { return monitor->getModelRotation().angleY(); }
		int getAngleZ() { return monitor->getModelRotation().angleZ(); }
		
		int getCameraEyeAngle() { return monitor->getCameraPosition().getCurXYPlaneEyeAngle(); }
		
		unsigned int calculateScaleDisplay(unsigned int height);
		
		void initVertexListCtr();
		
		void normalizeMonitor();
		void reconstruct();
		
		void pushProcessMode();
		void popProcessMode();
		
		void pushInteractiveProcessMode();
		void popInteractiveProcessMode();

		void pushReplayMode();
		void popReplayMode();
		
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
		void setCurrentClientId(long id);
		void resetCurrentClientId() 		{ setCurrentClientId(-1L); }
		void highlightClientId(long firstClientId, long lastClientId = -1L);
		void dimAllVectiesDown();
		void normalizeAllSelectionEffects();

		void updateMonitorAndOptions();
			
		long getPathItemCount()									{ return monitor->getPathItemCount(); }
		
		void setVirtualEnd(long val)							{ monitor->setVirtualEnd(val); }
		void setVirtualEndToFirst()								{ monitor->setVirtualEndToFirst(); }
		void setVirtualEndToLast()								{ monitor->setVirtualEndToLast(); }
		
		long previewNextVertexId()								{ return monitor->previewNextVertexId(); }
		long previewPreviousVertexId()							{ return monitor->previewPreviousVertexId(); }
		long previewNextVertexNormalizedId()					{ return monitor->previewNextVertexNormalizedId(); }
		long previewPreviousVertexNormalizedId()				{ return monitor->previewPreviousVertexNormalizedId(); }

		void incVirtualEnd() 									{ monitor->incVirtualEnd(); } 
		void decVirtualEnd() 									{ monitor->decVirtualEnd(); }
		void incVirtualEndById() 								{ monitor->incVirtualEndById(); }
		void decVirtualEndById() 								{ monitor->decVirtualEndById(); }
		
		const long getVirtualEnd()								{ return monitor->getVirtualEnd(); }
		const long getVirtualEndAsId()							{ return monitor->getVirtualEndAsId(); }
		const long getVirtualEndAsNormalizedId()				{ return monitor->getVirtualEndAsNormalizedId(); }
		
		void clearHighlightEffects()							{ monitor->clearHighlightEffects(); }
		
		void setSpindlePowerState(CncSpindlePowerState state)	{ monitor->setSpindlePowerState(state); Refresh(); }
		
		bool makeHardwareSpaceVisible();
		bool makeWorkingSpaceVisible();
		bool makeCompleteVisibleMetric(const CncDoubleBoundaries& box);
		
	protected:
		GLContextCncPathBase* 		monitor;
		wxTimer 					cameraRotationTimer;
		int 						cameraRotationStepWidth;
		int 						cameraRotationSpeed;
		float 						zoom;
		double						lastGestureZoomFactor;
		long 						currentClientID;
		
		virtual void notifyCncPathChanged();
		
		void createRuler(const GLContextBase::ModelType mt);

		void onPaint(wxPaintEvent& event);
		void onMouse(wxMouseEvent& event);
		void onSize(wxSizeEvent& event);
		void onLeave(wxMouseEvent& event);
		void onEraseBackground(wxEraseEvent& event);
		void onCameraRotationTimer(wxTimerEvent& event);
		
		void onPan(wxPanGestureEvent& event);
		void onZoom(wxZoomGestureEvent& event);
		void onRotate(wxRotateGestureEvent& event);
		void onTwoFingerTap(wxTwoFingerTapEvent& event);
		void onLongPress(wxLongPressEvent& event);
		void onPressAndTap(wxPressAndTapEvent& event);
		
		void performMouseCoordAndToolTip();
		
		bool makeCompleteVisible(const CncDoubleBoundaries& box);
		
	private:
		
		bool processMode;
		
		inline void appendVertex(long id, CncSpeedMode sm, float x, float y, float z);
		inline void onPaint();
		
		wxDECLARE_NO_COPY_CLASS(CncMotionMonitor);
		wxDECLARE_EVENT_TABLE();
};

#endif