#ifndef CNCSPEEDMONITOR_H
#define CNCSPEEDMONITOR_H

#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include "OSD/CncTimeFunctions.h"
#include "CncAverage.h"
#include "CncCircularBuffer.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "wxCrafterSpeedMonitor.h"

class CncSpeedMonitor : public CncSpeedMonitorBase {

	public:
	
		CncSpeedMonitor(wxWindow* parent);
		virtual ~CncSpeedMonitor();
		
		void save();
		void clear();
		
		void init();
		void start();
		void stop();
		
		void activate(bool state=true);
		void deactivate() { activate(false); }
		
		void toggleConnection();
		void enableConnection(bool state=true);
		void disableConnection() { enableConnection(false); }
		
		void setCurrentFeedSpeedValues(double cfgF_MM_MIN, double rltF_MM_MIN);
		void update();
		
	protected:
		virtual void onChangeDisplayCompression(wxScrollEvent& event);
		virtual void onChangeScrollBarH(wxScrollEvent& event);
		virtual void onChangeScrollBarV(wxScrollEvent& event);
		virtual void onChangeRecordResolution(wxScrollEvent& event);
		virtual void onClear(wxCommandEvent& event);
		virtual void onSave(wxCommandEvent& event);
		virtual void onLeftDown(wxMouseEvent& event);
		virtual void onLeftUp(wxMouseEvent& event);
		virtual void onMouseMotion(wxMouseEvent& event);
		virtual void onPaintLeftAxis(wxPaintEvent& event);
		virtual void onPaintRightAxis(wxPaintEvent& event);
		virtual void onPaint(wxPaintEvent& event);
		virtual void onSize(wxSizeEvent& event);

		virtual void onToggleConfiguredAxis(wxCommandEvent& event);
		virtual void onToggleConnection(wxCommandEvent& event);
		virtual void onToggleMeasurePointsAxis(wxCommandEvent& event);
		virtual void onToggleReceivedSpeedAxis(wxCommandEvent& event);
		
	private:
		
		//--------------------------------------------------------
		struct Diagram {
			
			enum Presentation	{ DRRelative, DRAbsolute };
			enum Orientation	{ DOHorizontal = 0, DOVertical = 1 };
			enum Resolution		{ DS_Sec=1000, DS_TenthSec=100, DS_HundredthSec=10, DS_ThousandthSec=1 };
			enum Compression	{ CPV_1 = 1, CPV_2 = 2, CPV_5 = 5, CPV_10 = 10, CPV_20 = 20, CPV_50 = 50, CPV_100 = 100 };
			
			struct Point {
				CncMilliTimestamp	ts = 0.0;
				CncAverage<double> 	rltF_MM_MIN;
				CncAverage<double> 	cfgF_MM_MIN;
				
				Point() 
				: ts(0)
				, rltF_MM_MIN()
				, cfgF_MM_MIN()
				{}
				
				explicit Point(CncMilliTimestamp t) 
				: ts(t)
				, rltF_MM_MIN()
				, cfgF_MM_MIN()
				{
				}
				
				Point(CncMilliTimestamp t, double r, double c) 
				: ts(t)
				, rltF_MM_MIN()
				, cfgF_MM_MIN()
				{
					rltF_MM_MIN.add(r);
					cfgF_MM_MIN.add(c);
				}
			};
			
			typedef CncCircularBuffer<10 * 1024, Point> Points;
			
			static const int	offsetAxisF 	= 17;
			
			Points 				points;
			Presentation		presentation	= DRAbsolute;
			Orientation			orientation		= DOHorizontal;
			Resolution			resolution		= DS_HundredthSec;
			
			double 				maxF_MM_MIN		= 0.0;
			long				timeOffset		= 0;
			long				timeCompression	= CPV_2;
			bool				showTimePoint	= true;
			bool				showCfgSpeed	= true;
			bool				showRltSpeed	= true;
			
			int					plotTRange		= 0;
			int					plotFRange		= 0;
			
			wxPoint 			mouseLabel		= wxPoint(-1, -1);
			
			wxColor 			grtCol1			= wxColour(255, 255, 255).ChangeLightness( 24);
			wxColor 			grtCol2			= wxColour(255, 255, 255).ChangeLightness(100);
			
			wxFont 				valFont1		= wxFont(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"));
			
			wxPen 				grtPen			= wxPen(grtCol1, 										1, wxSOLID);
			wxPen 				cfgPen			= wxPen(wxColor(153, 217, 234).ChangeLightness(100),	1, wxSOLID);
			wxPen				rltPen			= wxPen(wxColor(  0, 255,   0).ChangeLightness(100),	1, wxSOLID);
			wxPen				pntPen			= wxPen(wxColor(255, 128,   0).ChangeLightness(100),	1, wxSOLID);
			
			wxBrush				cfgBrush		= wxBrush(cfgPen.GetColour().ChangeLightness(20));
			wxBrush				rltBrush		= wxBrush(rltPen.GetColour().ChangeLightness(20));
			
			CncAverage<double>	defaultValue;
			
			//----------------------------------------------------
			size_t count() const { return points.getSize(); }
			
			//----------------------------------------------------
			double getVirtualMaxF() const {
				return ( presentation == DRAbsolute ? THE_CONFIG->getMaxSpeedXYZ_MM_MIN() 
													: maxF_MM_MIN);
			}
			
			//----------------------------------------------------
			void clear() {
				maxF_MM_MIN = 0.0;
				points.clear();
			}
			
			//----------------------------------------------------
			const CncAverage<double>& getLastCfgF() {
				if ( count() == 0 )
					return defaultValue;
					
				return points.rbegin()->cfgF_MM_MIN;
			}
			
			//----------------------------------------------------
			const CncAverage<double>& getLastRltF() {
				if ( count() == 0 )
					return defaultValue;
					
				return points.rbegin()->rltF_MM_MIN;
			}
			
			//----------------------------------------------------
			void append(double r, double c) { 
				const CncMilliTimestamp ts	= ( (long)(CncTimeFunctions::getMilliTimestamp() / resolution) * resolution );
				maxF_MM_MIN 				= std::max(maxF_MM_MIN, std::max(r, c));
				
				// the first entry
				if ( count() == 0 ) {
					points.add(std::move(Point(ts, r, c)));
					return;
				}
				
				// or a next new entry . . . every new unit = millisecond / resolution * resolution
				auto last = points.rbegin();
				if ( abs((long)(ts - last->ts)) > 0 ) {
					points.add(std::move(Point(ts, r, c)));
					return;
				}
				
				// or accumulate
				// but this is primary sufficient for the measured real-time values
				// last->cfgF_MM_MIN.add(c);
				
				last->rltF_MM_MIN.add(r);
			}
			
			//----------------------------------------------------
			void appendAgain(unsigned int millisFeedForward = 0 ) { 
				if ( count() == 0 )
					return;
					
				auto last = points.rbegin();
				
				if ( millisFeedForward == 0 )
					append(last->rltF_MM_MIN.getAvg(), last->cfgF_MM_MIN.getAvg());
				else
					points.add(std::move(Point(last->ts + millisFeedForward, last->rltF_MM_MIN.getAvg(), last->cfgF_MM_MIN.getAvg())));
			}
			
			//----------------------------------------------------
			inline void plotBtLf(wxGCDC& dc, const wxRect& rect);
			inline void plotToRt(wxGCDC& dc, const wxRect& rect);
			inline void plotMain(wxGCDC& dc, const wxRect& rect);
			
			inline int  getFAsPx(double value);
		}; //struct Diagram 
		
		Diagram 			diagram;
		
		void restoreTimeOffset();
		void determineTimeOffset();
		void reset();
		void decorateConnectBtn();
};

class CncSpeedMonitorRunner {

	private:
		CncSpeedMonitor* monitor;

	public:
		CncSpeedMonitorRunner(CncSpeedMonitor* sm, bool clear = true)
		: monitor(sm)
		{
			if ( monitor && THE_CONTEXT->canSpeedMonitoring() ) {
				monitor->clear();
				monitor->start();
			}
		}

		~CncSpeedMonitorRunner() {
			if ( monitor )
				monitor->stop();
		}
};

#endif // CNCSPEEDMONITOR_H

