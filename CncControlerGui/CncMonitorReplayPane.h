#ifndef CNCMONITORREPLAYPANE_H
#define CNCMONITORREPLAYPANE_H

#include "3D/VerticeData.h"
#include "3D/GLContextCncPathBase.h"

class CncMotionMonitor;

class CncMonitorReplayPane : public CncMonitorReplayPaneBase
                           , public GLI::GLCncPath::Callback   
{
	public:
		explicit CncMonitorReplayPane(wxWindow* parent);
		virtual ~CncMonitorReplayPane();
		
		void enable(bool state);
		
		void setMotionMonitor(CncMotionMonitor* mm);
		bool isProcessing() { return processing; }
		
		void updateControls();
		
	protected:
		
		virtual void selectReplayUnit(wxCommandEvent& event);
		
		virtual void replayLeftDClickNext(wxMouseEvent& event)	{ doAbort(); }
		virtual void replayLeftDClickPrev(wxMouseEvent& event)	{ doAbort(); }
		virtual void replayLeaveNext(wxMouseEvent& event) 		{ doAbort(); }
		virtual void replayLeavePrev(wxMouseEvent& event) 		{ doAbort(); }
		virtual void replayLeftUpNext(wxMouseEvent& event) 		{ doAbort(); }
		virtual void replayLeftUpPrev(wxMouseEvent& event) 		{ doAbort(); }
		
		virtual void replayLeftDownNext(wxMouseEvent& event);
		virtual void replayLeftDownPrev(wxMouseEvent& event);
		virtual void replayEnd(wxCommandEvent& event);
		virtual void replayPause(wxCommandEvent& event);
		virtual void replayPlayAll(wxCommandEvent& event);
		virtual void replayPlayCurrentId(wxCommandEvent& event);
		virtual void replayStart(wxCommandEvent& event);
		virtual void replayStop(wxCommandEvent& event);
		
	protected:
	
		enum UnitSelection { Unit_Id = 0, Unit_Vertex = 1 };
		
		virtual void notifyCncPathChanged();
		
		bool				abort;
		bool				processing;
		CncMotionMonitor* 	motionMonitor;
		
		bool increment(bool fine=false);
		bool decrement(bool fine=false);
		void display();
		void doAbort() 					{ abort = true; }
		
		void setProcessing(bool state) 	{ processing = state; }
		
		void replayPlay(bool stopByIdChange = false);
		
		// ----------------------------------------------------
		class Processing {
			static bool running;
			
			public:
				CncMonitorReplayPane* pane;
				
				explicit Processing(CncMonitorReplayPane* p) : pane(p) 	{ running = true; pane->setProcessing(true); }
				~Processing() 											{ running = false; pane->setProcessing(false); }
				
				static bool isRunning() { return running; }
		};
		
		// ----------------------------------------------------
		class Decorate {
			protected:
				CncMonitorReplayPane* pane;
				
				explicit Decorate(CncMonitorReplayPane* p);
				virtual ~Decorate();
		};
		
		// ----------------------------------------------------
		class DecoratePlay : public Decorate {
			public: 
				explicit DecoratePlay(CncMonitorReplayPane* pane) 
				: Decorate(pane)
				{
					pane->GetReplayStart()->Enable(false);
					pane->GetReplayPrev()->Enable(false);
					pane->GetReplayNext()->Enable(false);
					pane->GetReplayEnd()->Enable(false);
					pane->GetReplayPlayAll()->Enable(false);
					pane->GetReplayPlayCurrentId()->Enable(false);
					
					pane->GetReplayPause()->Enable(true);
					pane->GetReplayStop()->Enable(true);
				}
				
				virtual ~DecoratePlay() {};
		};
		
		// ----------------------------------------------------
		class DecoratePrev : public Decorate {
			public: 
				explicit DecoratePrev(CncMonitorReplayPane* pane) 
				: Decorate(pane)
				{
					pane->GetReplayStart()->Enable(false);
					pane->GetReplayNext()->Enable(false);
					pane->GetReplayEnd()->Enable(false);
					pane->GetReplayPlayAll()->Enable(false);
					pane->GetReplayPlayCurrentId()->Enable(false);
					pane->GetReplayPause()->Enable(false);
					
					pane->GetReplayPrev()->Enable(true);
					pane->GetReplayStop()->Enable(true);
				}
				
				virtual ~DecoratePrev() {};
		};
		
		// ----------------------------------------------------
		class DecorateNext : public Decorate {
			public: 
				explicit DecorateNext(CncMonitorReplayPane* pane) 
				: Decorate(pane)
				{
					pane->GetReplayStart()->Enable(false);
					pane->GetReplayPrev()->Enable(false);
					pane->GetReplayEnd()->Enable(false);
					pane->GetReplayPlayAll()->Enable(false);
					pane->GetReplayPlayCurrentId()->Enable(false);

					pane->GetReplayPause()->Enable(false);
					
					pane->GetReplayNext()->Enable(true);
					pane->GetReplayStop()->Enable(true);
				}
				
				virtual ~DecorateNext() {};
		};
};

#endif // CNCMONITORREPLAYPANE_H
