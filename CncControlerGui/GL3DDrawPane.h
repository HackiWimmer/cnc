#ifndef GL3DDRAWPANE_H
#define GL3DDRAWPANE_H

#include "CncMotionMonitor.h"
#include "wxCrafterMotionMonitor.h"

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
		
		void enable(bool state);
		
		void setMotionMonitor(CncMotionMonitor* m);
		
		void showFromTop();
		void showFromBottom();
		void showFromFront();
		void showFromRear();
		void showFromLeft();
		void showFromRight();
		void showFromPerspective1();
		void showFromPerspective2();
		void showFromPerspective3();
		void showFromPerspective4();
		
	protected:
		virtual void onClearMonitor(wxCommandEvent& event);
		virtual void onRefreshMonitor(wxCommandEvent& event);
		virtual void onToggleGuidePathes(wxCommandEvent& event);
		virtual void onToggleBoundBox(wxCommandEvent& event);
		virtual void onToggleFlyPathes(wxCommandEvent& event);
		virtual void onToggleHelpLines(wxCommandEvent& event);
		virtual void onToggleOrigin(wxCommandEvent& event);
		virtual void onTogglePosMarker(wxCommandEvent& event);
		virtual void onToggleRuler(wxCommandEvent& event);
		virtual void onToggleOptionPane(wxCommandEvent& event);
		virtual void onShowFromTop(wxCommandEvent& event)			{ showFromTop();          }
		virtual void onShowFromBottom(wxCommandEvent& event)		{ showFromBottom();       }
		virtual void onShowFromFront(wxCommandEvent& event)			{ showFromFront();        }
		virtual void onShowFromRear(wxCommandEvent& event)			{ showFromRear();         }
		virtual void onShowFromLeft(wxCommandEvent& event)			{ showFromLeft();         }
		virtual void onShowFromRight(wxCommandEvent& event)			{ showFromRight();        }
		virtual void onShowFromPerspective1(wxCommandEvent& event)	{ showFromPerspective1(); }
		virtual void onShowFromPerspective2(wxCommandEvent& event)	{ showFromPerspective2(); }
		virtual void onShowFromPerspective3(wxCommandEvent& event)	{ showFromPerspective3(); }
		virtual void onShowFromPerspective4(wxCommandEvent& event)	{ showFromPerspective4(); }
		
		inline void onPaintRotatePane3D(const char axis, wxPanel* panel, int angle);

		void onPaintRotatePaneX3D(wxPaintEvent& event);
		void onPaintRotatePaneY3D(wxPaintEvent& event);
		void onPaintRotatePaneZ3D(wxPaintEvent& event);
		void onPaintScalePane3D(wxPaintEvent& event);
		
		CncMotionMonitor* motionMonitor;
		
	private:
		void activate3DPerspectiveButton(wxButton* bt);
};
#endif // GL3DDRAWPANE_H
