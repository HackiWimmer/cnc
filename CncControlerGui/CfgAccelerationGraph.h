#ifndef CfgAccelerationGraph_H
#define CfgAccelerationGraph_H

#include <wx/panel.h>
#include "CncSpeedController.h"

class CfgAccelerationGraph : public wxPanel {
	
	CncSpeedController* speedController;
	
	struct Point{
		uint32_t  step;
		int32_t   value;
	};
	
	typedef std::vector<Point> Graph;
	
	Graph graph;
	int32_t	minValue;
	int32_t	maxValue;
	uint32_t stepsToMove;
	uint32_t startMarker;
	uint32_t stopMarker;
	
	void reset();
	
	public:
		CfgAccelerationGraph(wxWindow* parent);
		virtual ~CfgAccelerationGraph();
		
		void init(CncSpeedController* sc);
		bool calculate(char axis, int32_t stepsToMove);
		void display();
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CfgAccelerationGraph);
		wxDECLARE_EVENT_TABLE();
};

#endif