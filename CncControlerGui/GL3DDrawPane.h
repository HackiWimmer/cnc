#ifndef GL3DDRAWPANE_H
#define GL3DDRAWPANE_H

#include "CncMotionMonitor.h"
#include "wxcrafter.h"

class GL3DDrawPane : public GL3DDrawPaneBase 
                          , GLContextOptions::Callback
						  , CncMotionMonitor::Callback
{
	public:
		GL3DDrawPane(wxWindow* parent);
		virtual ~GL3DDrawPane();
		
		virtual void notifyChange(GLContextOptions& options);
		virtual void notifyChange(CncMotionMonitor& mm);
		virtual void notifyCameraAngleChange(int angle);
		
		void setMotionMonitor(CncMotionMonitor* m);
		
	protected:
		CncMotionMonitor* motionMonitor;
		
		inline void onPaintRotatePane3D(const char axis, wxPanel* panel, int angle);

		void onPaintRotatePaneX3D(wxPaintEvent& event);
		void onPaintRotatePaneY3D(wxPaintEvent& event);
		void onPaintRotatePaneZ3D(wxPaintEvent& event);
		void onPaintScalePane3D(wxPaintEvent& event);
};
#endif // GL3DDRAWPANE_H
