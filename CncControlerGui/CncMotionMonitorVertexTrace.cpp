#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterImages.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncUserEvents.h"
#include "CncMotionMonitorVertexTrace.h"

extern GlobalConstStringDatabase globalStrings;

#include <wx/frame.h>
extern wxFrame* THE_FRAME;

// ----------------------------------------------------------------------------
// CncVertexTrace::DataListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncVertexTrace::DataListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncVertexTrace::DataListCtrl::onSize)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncVertexTrace::DataListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncVertexTrace::DataListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncVertexTrace::DataListCtrl::DataListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, dataItemAttr()
, vertexBufferStore(NULL)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("*",		 		wxLIST_FORMAT_LEFT, 	22);
	AppendColumn("#", 				wxLIST_FORMAT_RIGHT, 	38);
	AppendColumn("Client ID", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Type",			wxLIST_FORMAT_LEFT, 	50);
	AppendColumn("X-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Red",				wxLIST_FORMAT_RIGHT, 	46);
	AppendColumn("Green",			wxLIST_FORMAT_RIGHT, 	46);
	AppendColumn("Blue",			wxLIST_FORMAT_RIGHT, 	46);
	AppendColumn("Alpha",			wxLIST_FORMAT_RIGHT, 	46);
	AppendColumn("Colour result",	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibMotionVertex().Bitmap("BMP_DATA"));	// 0
	imageList->Add(ImageLibMotionVertex().Bitmap("BMP_DATA"));	// 1
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	dataItemAttr.SetBackgroundColour(GetBackgroundColour());
	dataItemAttr.SetFont(font);
	dataItemAttr.SetTextColour(GetTextColour());
}
/////////////////////////////////////////////////////////////
CncVertexTrace::DataListCtrl::~DataListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::DataListCtrl::setVertexBufferStore(GLOpenGLPathBufferStore* vbs) { 
/////////////////////////////////////////////////////////////
	vertexBufferStore = vbs; 
	clear();
	
	if ( vertexBufferStore != NULL )
		SetItemCount(vertexBufferStore->getVertexCount());
}
/////////////////////////////////////////////////////////////
int CncVertexTrace::DataListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return -1;

	switch ( column ) {
		case COL_TYPE:		return 0;
		
		case COL_COLOUR:	// create the colour image on the fly . . .
							GLOpenGLPathBuffer::CncVertex v;
							vertexBufferStore->getVertex(v, (unsigned long)item);
							
							wxImage img(16, 16);
							img.SetRGB({0, 0, img.GetWidth(), img.GetHeight()}, v.getR(), v.getG(), v.getB());
							
							GetImageList(wxIMAGE_LIST_SMALL)->Replace(1, wxBitmap(img));
							return 1;
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncVertexTrace::DataListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(&dataItemAttr);
}
/////////////////////////////////////////////////////////////
wxString CncVertexTrace::DataListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return _("");
		
	if ( item < 0 )
		return _("");
		
	if ( item > (long)vertexBufferStore->getVertexCount() - 1 )
		return _("");

	GLOpenGLPathBuffer::CncVertex v;
	vertexBufferStore->getVertex(v, (unsigned long)item);
	
	const long bufferId = vertexBufferStore->getBufferId((unsigned long)item);
	
	wxString ret;
	switch ( column ) {
		case  COL_TYPE:		ret.assign("D");
							break;
		case  COL_BUF_ID:	ret.assign(wxString::Format("%ld", bufferId));
							break;
		case  COL_CLD_ID:	ret.assign(wxString::Format(globalStrings.vertexTraceFormat, v.getClientId()));
							break;
		case  COL_T:		ret.assign(wxString::Format("%c", v.getType()));
							break;
		case  COL_X:		ret.assign(wxString::Format("%lf", v.getX()));
							break;
		case  COL_Y:		ret.assign(wxString::Format("%lf", v.getY()));
							break;
		case  COL_Z:		ret.assign(wxString::Format("%lf", v.getZ()));
							break;
		case  COL_R:		ret.assign(wxString::Format("%d", (int)v.getR()));
							break;
		case  COL_G:		ret.assign(wxString::Format("%d", (int)v.getG()));
							break;
		case  COL_B:		ret.assign(wxString::Format("%d", (int)v.getB()));
							break;
		case  COL_A:		ret.assign(wxString::Format("%d", (int)v.getA()));
							break;
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::DataListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();

	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncVertexTrace::DataListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	if ( GetColumnCount() <= 0 )
		return;

	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();

	int colWidthSum = 0;
	for ( int i = 0; i < GetColumnCount(); i++ ) {
		if ( i == COL_STRECH )
			continue;

		colWidthSum += GetColumnWidth(i);
	}

	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth()
	         - colWidthSum
			 - scrollbarWidth;

	SetColumnWidth(COL_STRECH, size);

	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::DataListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	long ln;
	GetItemText(item, COL_SEARCH).ToLong(&ln);
	
	typedef IndividualCommandEvent::EvtMainFrame ID;
	typedef IndividualCommandEvent::ValueName VN;

	IndividualCommandEvent evt(ID::TryToSelectClientIds);
	evt.setValue(VN::VAL1, ln);
	evt.setValue(VN::VAL2, ln);
	evt.setValue(VN::VAL3, (int)(ClientIdSelSource::ID::TSS_VERTEX_DATA_TRACE));

	SelectEventBlocker blocker(this);
	wxPostEvent(THE_FRAME, evt);
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::DataListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
bool CncVertexTrace::DataListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return false;
	
	long ret = vertexBufferStore->findFirstEntryForClientId(id);
	if ( ret >= 0 )
		selectItem(ret);
	
	return ( ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncVertexTrace::DataListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return false;
		
	 return ( item >= 0 && item < (long)(vertexBufferStore->getVertexCount() - 1) );
}



// ----------------------------------------------------------------------------
// CncVertexTrace::IndexListCtrl Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncVertexTrace::IndexListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncVertexTrace::IndexListCtrl::onSize)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncVertexTrace::IndexListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncVertexTrace::IndexListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncVertexTrace::IndexListCtrl::IndexListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, indexItemAttr()
, vertexBufferStore(NULL)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("*",		 		wxLIST_FORMAT_LEFT, 	22);
	AppendColumn("#", 				wxLIST_FORMAT_RIGHT, 	38);
	AppendColumn("Client ID", 		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Index List",		wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibMotionVertex().Bitmap("BMP_INDEX"));	// 0
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	indexItemAttr.SetBackgroundColour(GetBackgroundColour());
	indexItemAttr.SetFont(font);
	indexItemAttr.SetTextColour(GetTextColour());
}
/////////////////////////////////////////////////////////////
CncVertexTrace::IndexListCtrl::~IndexListCtrl() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::IndexListCtrl::setVertexBufferStore(GLOpenGLPathBufferStore* vbs) { 
/////////////////////////////////////////////////////////////
	vertexBufferStore = vbs; 
	clear();
	
	if ( vertexBufferStore != NULL )
		SetItemCount(vertexBufferStore->getClientIdCount());
}
/////////////////////////////////////////////////////////////
int CncVertexTrace::IndexListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( column == 0 )
		return 0;
		
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncVertexTrace::IndexListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	return (wxListItemAttr*)(&indexItemAttr);
}
/////////////////////////////////////////////////////////////
wxString CncVertexTrace::IndexListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return _("");
		
	if ( item < 0 )
		return _("");
	
	unsigned int bufferId 	= 0;
	long clientId 			=  CLIENT_ID.INVALID;
	
	wxString ret;
	switch ( column ) {
		case  COL_TYPE:		ret.assign("I");
							break;
							
		case COL_BUF_ID:	bufferId = vertexBufferStore->getBufferIdForIdx((unsigned long)item);
							ret.assign(wxString::Format("%u", bufferId));
							break;
							
		case  COL_CLD_ID:	clientId = vertexBufferStore->getClientIdForIdx((unsigned long)item);
							ret.assign(wxString::Format(globalStrings.vertexTraceFormat, clientId));
							break;
							
		case  COL_INFO:		//clientId = vertexBufferStore->getClientIdForIdx((unsigned long)item);
							//vertexBufferStore->getIndexForClientIdAsString(clientId, ret, true);
							vertexBufferStore->getIndexForIdxAsString((unsigned long)item, ret, true);
							
							break;
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::IndexListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////
	updateColumnWidth();

	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncVertexTrace::IndexListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	if ( GetColumnCount() <= 0 )
		return;

	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();

	int colWidthSum = 0;
	for ( int i = 0; i < GetColumnCount(); i++ ) {
		if ( i == COL_STRECH )
			continue;

		colWidthSum += GetColumnWidth(i);
	}

	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth()
	         - colWidthSum
			 - scrollbarWidth;

	SetColumnWidth(COL_STRECH, size);

	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::IndexListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	long ln;
	GetItemText(item, COL_SEARCH).ToLong(&ln);
	
	typedef IndividualCommandEvent::EvtMainFrame ID;
	typedef IndividualCommandEvent::ValueName VN;

	IndividualCommandEvent evt(ID::TryToSelectClientIds);
	evt.setValue(VN::VAL1, ln);
	evt.setValue(VN::VAL2, ln);
	evt.setValue(VN::VAL3, (int)(ClientIdSelSource::ID::TSS_VERTEX_INDEX_TRACE));

	SelectEventBlocker blocker(this);
	wxPostEvent(THE_FRAME, evt);
}
/////////////////////////////////////////////////////////////
void CncVertexTrace::IndexListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	// currently nothing todo
}
/////////////////////////////////////////////////////////////
bool CncVertexTrace::IndexListCtrl::searchReferenceById(const long id) {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return false;
	
	long ret = vertexBufferStore->findFirstIndexForClientId(id);
	if ( ret >= 0 )
		selectItem(ret);
	
	return ( ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncVertexTrace::IndexListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return false;
		
	 return ( item >= 0 && item < (long)(vertexBufferStore->getClientIdCount() - 1) );
}
