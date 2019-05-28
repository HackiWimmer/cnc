#ifndef CNCMOTIONVERTEXTRACE_H
#define CNCMOTIONVERTEXTRACE_H

#include "CncMotionMonitorVertexTrace.h"
#include "wxcrafter.h"

class CncMotionVertexTrace : public CncMotionVertexTraceBase {
	
	public:
		enum ListType{ LT_VERTEX_DATA_TRACE, LT_VERTEX_INDEX_TRACE };

		CncMotionVertexTrace(wxWindow* parent);
		virtual ~CncMotionVertexTrace();
		
		bool connect(bool state = true)						{ m_btConnectMotionVertex->SetValue(state); connectMotionVertex(state); return isConnected(); }
		bool disconnect()									{ return connect(false); }
		
		bool isConnected() 									{ return m_btConnectMotionVertex->GetValue(); }
		CncVertexTrace::DataListCtrl* getVertexDataList() 	{ return motionVertexDataList;  } 
		CncVertexTrace::IndexListCtrl* getVertexIndexList() { return motionVertexIndexList; } 
		
		void selectClientId(long id, CncMotionVertexTrace::ListType lt);
		
	protected:
		CncVertexTrace::DataListCtrl* motionVertexDataList; 
		CncVertexTrace::IndexListCtrl* motionVertexIndexList; 
		
		virtual void clearMotionVertex(wxCommandEvent& event);
		virtual void connectMotionVertex(wxCommandEvent& event);
		
		void connectMotionVertex(bool state);
};

#endif // CNCMOTIONVERTEXTRACE_H
