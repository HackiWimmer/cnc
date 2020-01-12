#ifndef CNC_SPEED_ACCELERATIOM_GRAPH_H
#define CNC_SPEED_ACCELERATIOM_GRAPH_H


#include <wx/pen.h>
#include <wx/panel.h>

class CncSpeedAccelerationDiagram : public wxPanel {
	
	public: 
		
		struct Impulse {
			int8_t	x	= 0;
			int8_t	y	= 0;
			int8_t	z	= 0;
		};
		
		struct Point {
			Impulse 	i;
			wxRealPoint p;
		};
		
		typedef std::vector<Point> Points;
		
		struct Graph {
			wxString 	name;
			wxPen 		pen;
			Points 		points;
			
			Graph() 
			: name		("Graph name")
			, pen		(*wxBLUE, 1, wxLONG_DASH)
			, points	()
			{}
		};
		
	private:
		
		static const int BORDER = 30;
		typedef std::vector<Graph> GraphList;
		
		GraphList		graphList;
		int32_t			minD;
		int32_t			maxD;
		int32_t			minF;
		int32_t			maxF;
		wxString		xUnit;
		wxString		yUnit;
		bool			showImpulse;
		
		void 	reset();
		
		wxPoint	cnvH2Graph (double h, double v);
		wxPoint	cnvHV2Graph(double h, double v);
		wxPoint	cnvH2Graph (wxRealPoint p)			{ return cnvH2Graph (p.x, p.y); }
		wxPoint	cnvHV2Graph(wxRealPoint p)			{ return cnvHV2Graph(p.x, p.y); }
		
	public:
		CncSpeedAccelerationDiagram(wxWindow* parent);
		virtual ~CncSpeedAccelerationDiagram();
		
		void display();
		void initGraphCount(unsigned int c);
		void initRange(int32_t minF, int32_t maxF, int32_t minD, int32_t maxD);
		void initUnit(const wxString& xUnit, const wxString& yUnit);
		
		CncSpeedAccelerationDiagram::Graph* getGraph(unsigned int graphIndex);
		bool clearGraph(unsigned int graphIndex);
		bool appendPointToGraph(unsigned int graphIndex, const wxRealPoint& p, const Impulse& i);
		
		void setShowImpulseState(bool s) { showImpulse = s; }
		
		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onMouse(wxMouseEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncSpeedAccelerationDiagram);
		wxDECLARE_EVENT_TABLE();
		
};

#endif
