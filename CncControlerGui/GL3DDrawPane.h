
#ifndef GL3DDRAWPANE_H
#define GL3DDRAWPANE_H

#include "CncMotionMonitor.h"
#include "wxCrafterMotionMonitor.h"

class GL3DDrawPane : public GL3DDrawPaneBase 
                   , public GLContextOptions::Callback
                   , public CncMotionMonitor::Callback
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
		
		void toggleOptionPane();
		void toggleHardwareBox();
		void toggleTotalBoundBox();
		void toggleObjectBoundBox();
		void toggleFlyPathes();
		void toggleGuidePathes();
		void toggleHelpLines();
		void toggleOrigin();
		void togglePosMarker();
		void toggleRuler();
		void toggleMillingCutter();
		
		bool stateHardwareBox()								 const { return motionMonitor->getContextOptions().showHardwareBox;   }
		bool stateTotalBoundBox()							 const { return motionMonitor->getContextOptions().showTotalBoundBox; }
		bool stateObjectBoundBox()							 const { return motionMonitor->getContextOptions().showObjectBoundBox;}
		bool stateFlyPathes()								 const { return motionMonitor->getContextOptions().showFlyPath;       }
		bool stateGuidePathes()								 const { return motionMonitor->getContextOptions().showGuidePathes;   }
		bool stateHelpLines()								 const { return motionMonitor->getContextOptions().showHelpLines;     }
		bool stateOrigin()									 const { return motionMonitor->getContextOptions().showOrigin;        }
		bool stateRuler()									 const { return motionMonitor->getContextOptions().showRuler;         }
		bool statePosMarker()								 const { return motionMonitor->getContextOptions().showPosMarker;     }
		bool stateMillingCutter()							 const { return motionMonitor->getContextOptions().showMillingCutter; }
		
		void resetView();
		void clearMonitor();
		void refreshMonitor();
		
	protected:
		virtual void onResetView(wxCommandEvent& event)				{ resetView();            }
		virtual void onShowMillingCutter(wxCommandEvent& event)		{ toggleMillingCutter();  }
		virtual void onToggleHardwareBox(wxCommandEvent& event)		{ toggleHardwareBox();    }
		virtual void onClearMonitor(wxCommandEvent& event)			{ clearMonitor();         }
		virtual void onRefreshMonitor(wxCommandEvent& event)		{ refreshMonitor();       }
		virtual void onToggleGuidePathes(wxCommandEvent& event)		{ toggleGuidePathes();    }
		virtual void onToggleTotalBoundBox(wxCommandEvent& event)	{ toggleTotalBoundBox();  }
		virtual void onToggleObjectBoundBox(wxCommandEvent& event)	{ toggleObjectBoundBox(); }
		virtual void onToggleFlyPathes(wxCommandEvent& event)		{ toggleFlyPathes();      }
		virtual void onToggleHelpLines(wxCommandEvent& event)		{ toggleHelpLines();      }
		virtual void onToggleOrigin(wxCommandEvent& event)			{ toggleOrigin();         }
		virtual void onTogglePosMarker(wxCommandEvent& event)		{ togglePosMarker();      }
		virtual void onToggleRuler(wxCommandEvent& event)			{ toggleRuler();          }
		virtual void onToggleOptionPane(wxCommandEvent& event)		{ toggleOptionPane();     }
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
