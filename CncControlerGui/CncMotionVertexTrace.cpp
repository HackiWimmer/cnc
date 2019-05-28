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
	motionVertexDataList = new CncVertexTrace::DataListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_motionVertexDataListPlaceholder, motionVertexDataList);
	
	motionVertexIndexList = new CncVertexTrace::IndexListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_motionVertexIndexListPlaceholder, motionVertexIndexList);
}
/////////////////////////////////////////////////////////////////////
CncMotionVertexTrace::~CncMotionVertexTrace() {
/////////////////////////////////////////////////////////////////////
	delete motionVertexDataList;
	delete motionVertexIndexList;
}
/////////////////////////////////////////////////////////////////////
void CncMotionVertexTrace::clearMotionVertex(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	motionVertexDataList->clear();
	motionVertexIndexList->clear();
}
/////////////////////////////////////////////////////////////////////
void CncMotionVertexTrace::connectMotionVertex(bool state) {
/////////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	state == true ? m_btConnectMotionVertex->SetBitmap(bmpOn) 			: m_btConnectMotionVertex->SetBitmap(bmpOff);
	state == true ? m_btConnectMotionVertex->SetToolTip("Disable List")	: m_btConnectMotionVertex->SetToolTip("Enable List");
	
	m_btConnectMotionVertex->SetValue(state);
	
	m_btConnectMotionVertex->Refresh();
	m_btConnectMotionVertex->Update();
	
	if ( state == false ) {
		motionVertexDataList->clear();
		motionVertexIndexList->clear();
	}
}
/////////////////////////////////////////////////////////////////////
void CncMotionVertexTrace::connectMotionVertex(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////
	const bool state = m_btConnectMotionVertex->GetValue();
	connectMotionVertex(state);
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