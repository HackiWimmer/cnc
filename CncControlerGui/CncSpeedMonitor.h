#ifndef CNCSPEEDMONITOR_H
#define CNCSPEEDMONITOR_H

#include "OSD/CncTimeFunctions.h"
#include "wxcrafter.h"

class CncSpeedMonitor : public CncSpeedMonitorBase {

	public:
	
		struct SpeedData {
			double configured_MM_MIN	= 0.0;
			double measured_MM_MIN		= 0.0;
			double received_MM_MIN		= 0.0; 
		};
		
		CncSpeedMonitor(wxWindow* parent);
		virtual ~CncSpeedMonitor();
		
		void save();
		void clear();
		
		void init(double maxSpeedValue_MM_MIN);
		void start(double maxSpeedValue_MM_MIN);
		void stop();
		
		void activate(bool state=true);
		void deactivate() { activate(false); }
		
		void toggleConnection();
		void enableConnection(bool state=true);
		void disableConnection() { enableConnection(false); }
			
		void setCurrentFeedSpeedValue(const SpeedData& sd);
		
	protected:
		virtual void onClear(wxCommandEvent& event);
		virtual void onSave(wxCommandEvent& event);
		virtual void onRefreshTimer(wxTimerEvent& event);
		virtual void onScrolledSize(wxSizeEvent& event);
		virtual void changeIntervalSlider(wxScrollEvent& event);
		virtual void toggleConfiguredAxis(wxCommandEvent& event);
		virtual void toggleConnection(wxCommandEvent& event);
		virtual void toggleMeasurePointsAxis(wxCommandEvent& event);
		virtual void toggleMeasuredSpeedAxis(wxCommandEvent& event);
		virtual void toggleReceivedSpeedAxis(wxCommandEvent& event);
		virtual void onLeftDown(wxMouseEvent& event);
		virtual void onLeftUp(wxMouseEvent& event);
		virtual void onMouseMotion(wxMouseEvent& event);
		virtual void onPaintLeftAxis(wxPaintEvent& event);
		virtual void onPaintRightAxis(wxPaintEvent& event);
		virtual void onPaint(wxPaintEvent& event);
		virtual void onSize(wxSizeEvent& event);
		
	private:
		
		static const unsigned int refreshInterval	=  100;
		
		static const unsigned int lMargin 			=    8;
		static const unsigned int rMargin 			=    8;
		static const unsigned int tMargin 			=    8;
		static const unsigned int bMargin 			=    8;
		
		static const unsigned int MAX_VALUES 		= 4 * 1024;
		
		struct Axis {
			bool fill	= false;
			wxPen pen 	= wxPen(*wxWHITE, 1, wxSOLID);
			int pos 	= wxRIGHT;
			int yOffset	= 0;
			
			unsigned int height = 0;
						
			double minValue 	= 0.0;
			double maxValue 	= 0.0;
			
			unsigned int values[MAX_VALUES];
			
			// --------------------------------------------------
			Axis() {
				clear();
			}
			
			// --------------------------------------------------
			void clear() {
				for ( unsigned int i = 0; i < MAX_VALUES; i++ )
					values[i] = 0;
			}
			
			// --------------------------------------------------
			unsigned int convert(double value) {
				if ( maxValue <= 0.0 )
					return 0;
				
				value -= minValue;
				
				if ( value <= minValue )
					return 0;
					
				if ( value >= maxValue )
					return height;
				
				return value * height / maxValue;
			}
			
			// --------------------------------------------------
			double convertToValue(unsigned int value) {
				if ( height == 0 )
					return 0;
					
				return value * maxValue / height;
			}
		};
		
		wxRect drawingArea;
		wxFont valueFont;
		wxFont labelFont;
		wxPoint mouseLabel;
		
		Axis axisMeasurePoints;
		Axis axisMeasuredSpeed;
		Axis axisReceivedSpeed;
		Axis axisConfiguredSpeed;
		
		CncMilliTimestamp lastRefresh;
		CncMilliTimestamp lastDataSet;
		unsigned int timeIndex;
		double currentMeasuredFeedSpeed_MM_MIN;
		double currentReceivedFeedSpeed_MM_MIN;
		double currentConfiguredFeedSpeed_MM_MIN;
		
		void reset();
		void setupSizes();
		
		void decorateConnectBtn();
	
};

class CncSpeedMonitorRunner {

	private:
		CncSpeedMonitor* monitor;

	public:
		CncSpeedMonitorRunner(CncSpeedMonitor* sm, double feedSpeed)
		: monitor(sm)
		{
			if ( monitor )
				monitor->start(feedSpeed);
		}

		~CncSpeedMonitorRunner() {
			if ( monitor )
				monitor->stop();
		}
};

#endif // CNCSPEEDMONITOR_H
