#include "GlobalFunctions.h"
#include "CncMotionMonitorVertexTrace.h"
#include "CncMotionVertexTrace.h"

/////////////////////////////////////////////////////////////////////
CncMotionVertexTrace::CncMotionVertexTrace(wxWindow* parent)
: CncMotionVertexTraceBase(parent)
, motionVertexDataList(NULL)
, motionVertexIndexList(NULL)
/////////////////////////////////////////////////////////////////////
{
	motionVertexDataList = new CncVertexTrace::DataListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_motionVertexDataListPlaceholder, motionVertexDataList);
	
	motionVertexIndexList = new CncVertexTrace::IndexListCtrl(this, wxLC_HRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_motionVertexIndexListPlaceholder, motionVertexIndexList);
}
/////////////////////////////////////////////////////////////////////
CncMotionVertexTrace::~CncMotionVertexTrace() {
/////////////////////////////////////////////////////////////////////
	delete motionVertexDataList;
	delete motionVertexIndexList;
}
/////////////////////////////////////////////////////////////////////
void CncMotionVertexTrace::selectClientId(long id, CncMotionVertexTrace::ListType lt) {
/////////////////////////////////////////////////////////////////////
	switch ( lt ) {
		case LT_VERTEX_DATA_TRACE:	motionVertexDataList->searchReferenceById(id);
									break;
		case LT_VERTEX_INDEX_TRACE:	motionVertexIndexList->searchReferenceById(id);
									break;
	}
}