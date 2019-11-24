#ifndef GL3DOPTIONPANE_H
#define GL3DOPTIONPANE_H

#include "CncMotionMonitor.h"
#include "3D/GLContextBase.h"
#include "wxCrafterMotionMonitor.h"

class GL3DOptionPane : public GL3DOptionPaneBase 
                            , GLContextOptions::Callback
							, CncMotionMonitor::Callback
{
	
	public:
		GL3DOptionPane(wxWindow* parent);
		virtual ~GL3DOptionPane();
		
		virtual void notifyChange(GLContextOptions& options);
		virtual void notifyChange(CncMotionMonitor& mm);
		virtual void notifyCameraAngleChange(int angle);
		
		void setMotionMonitor(CncMotionMonitor* m);
		
	protected:
    virtual void resetModelPostion(wxCommandEvent& event);
		CncMotionMonitor* motionMonitor;
		
		virtual void autoCameraRotationAnticlockwise(wxCommandEvent& event);
		virtual void autoCameraRotationClockwise(wxCommandEvent& event);
		virtual void cameraRotationChanged(wxScrollEvent& event);
		virtual void cameraRotationSpeedChanged(wxScrollEvent& event);
		virtual void modelRotationXChanged(wxScrollEvent& event);
		virtual void modelRotationXYZChanged(wxScrollEvent& event);
		virtual void modelRotationYChanged(wxScrollEvent& event);
		virtual void modelRotationZChanged(wxScrollEvent& event);
		virtual void resetCameraPostion(wxCommandEvent& event);
		virtual void resetModelRotationX(wxCommandEvent& event);
		virtual void resetModelRotationXYZ(wxCommandEvent& event);
		virtual void resetModelRotationY(wxCommandEvent& event);
		virtual void resetModelRotationZ(wxCommandEvent& event);
		virtual void propertyChanged(wxPropertyGridEvent& event);
		virtual void propertyChanging(wxPropertyGridEvent& event);
		
};
#endif // GL3DOPTIONPANE_H
