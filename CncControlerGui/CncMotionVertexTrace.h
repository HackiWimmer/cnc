#ifndef CNCMOTIONVERTEXTRACE_H
#define CNCMOTIONVERTEXTRACE_H

#include "CncMotionMonitorVertexTrace.h"
#include "wxCrafterMotionMonitor.h"

class CncMotionVertexTrace : public CncMotionVertexTraceBase {
	
	public:
		enum ListType{ LT_VERTEX_DATA_TRACE, LT_VERTEX_INDEX_TRACE };

		CncMotionVertexTrace(wxWindow* parent);
		virtual ~CncMotionVertexTrace();
		
		CncVertexTrace::DataListCtrl* getVertexDataList() 	{ return motionVertexDataList;  } 
		CncVertexTrace::IndexListCtrl* getVertexIndexList() { return motionVertexIndexList; } 
		
		void selectClientId(long id, CncMotionVertexTrace::ListType lt);
		
	protected:
		CncVertexTrace::DataListCtrl* motionVertexDataList; 
		CncVertexTrace::IndexListCtrl* motionVertexIndexList; 
		
		void connectMotionVertex(bool state);
};

#endif // CNCMOTIONVERTEXTRACE_H
