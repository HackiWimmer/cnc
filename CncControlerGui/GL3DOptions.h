#ifndef GL3DOPTIONS_H
#define GL3DOPTIONS_H
#include "wxcrafter.h"
#include "CncMotionMonitor.h"

class GL3DOptions : public GL3DOptionsBase
{
	public:
		GL3DOptions(CncMotionMonitor* parent);
		virtual ~GL3DOptions();
		
		void cameraCallBack(int angle);
		
	protected:
    virtual void cameraRotationSpeedChanged(wxScrollEvent& event);
		virtual void resetCameraPostion(wxCommandEvent& event);
		virtual void autoCameraRotationAnticlockwise(wxCommandEvent& event);
		virtual void autoCameraRotationClockwise(wxCommandEvent& event);
		virtual void resetModelRotationX(wxCommandEvent& event);
		virtual void resetModelRotationXYZ(wxCommandEvent& event);
		virtual void resetModelRotationY(wxCommandEvent& event);
		virtual void resetModelRotationZ(wxCommandEvent& event);
		virtual void modelRotationXChanged(wxScrollEvent& event);
		virtual void modelRotationYChanged(wxScrollEvent& event);
		virtual void modelRotationZChanged(wxScrollEvent& event);
		virtual void modelRotationXYZChanged(wxScrollEvent& event);
		virtual void cameraRotationChanged(wxScrollEvent& event);
		virtual void propertyChanged(wxPropertyGridEvent& event);
		virtual void propertyChanging(wxPropertyGridEvent& event);
		virtual void onActivate(wxActivateEvent& event);
		virtual void onShowWindow(wxShowEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		
	protected:
		CncMotionMonitor* motionMonitor;
		bool firstTime;
		void restoreFlags();
		
};
#endif // GL3DOPTIONS_H
