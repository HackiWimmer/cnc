#include <iostream>
#include <wx/imaglist.h>
#include "wxcrafter.h"
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "CncConfig.h"
#include "CncMotionMonitorVertexTrace.h"

extern GlobalConstStringDatabase globalStrings;

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
	AppendColumn("Client ID", 		wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("Type",			wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("X-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Y-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Z-Target",		wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("R",				wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("G",				wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("B",				wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("A",				wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::NORMAL);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(191, 205, 219));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibMotionVertex().Bitmap("BMP_DATA"));	// 0
	
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
	if ( column == 0 )
		return 0;
		
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
	
	wxString ret;
	switch ( column ) {
		case  0:	ret.assign("D");
					break;
		case  1:	ret.assign(wxString::Format(globalStrings.vertexTraceFormat, v.getClientId()));
					break;
		case  2:	ret.assign(wxString::Format("%c", v.getType()));
					break;
		case  3:	ret.assign(wxString::Format("%lf", v.getX()));
					break;
		case  4:	ret.assign(wxString::Format("%lf", v.getY()));
					break;
		case  5:	ret.assign(wxString::Format("%lf", v.getZ()));
					break;
		case  6:	ret.assign(wxString::Format("%d", (int)v.getR()));
					break;
		case  7:	ret.assign(wxString::Format("%d", (int)v.getG()));
					break;
		case  8:	ret.assign(wxString::Format("%d", (int)v.getB()));
					break;
		case  9:	ret.assign(wxString::Format("%d", (int)v.getA()));
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
	
	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_VERTEX_DATA_TRACE);
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
	AppendColumn("Client ID", 		wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
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
		SetItemCount(vertexBufferStore->getVertexCount());
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
		
	GLOpenGLPathBuffer::CncVertex v;
	vertexBufferStore->getVertex(v, (unsigned long)item);
	
	wxString ret;
	switch ( column ) {
		case  0:	ret.assign("I");
					break;
					
		case  1:	ret.assign(wxString::Format(globalStrings.vertexTraceFormat, v.getClientId()));
					break;
					
		case  2:	ret.assign("TODO");
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
	
	SelectEventBlocker blocker(this);
	THE_APP->tryToSelectClientId(ln, MainFrame::TemplateSelSource::TSS_VERTEX_INDEX_TRACE);
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
	
	long ret = vertexBufferStore->findFirstEntryForClientId(id);
	if ( ret >= 0 )
		selectItem(ret);
	
	return ( ret >= 0 );
}
/////////////////////////////////////////////////////////////
bool CncVertexTrace::IndexListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	if ( vertexBufferStore == NULL )
		return false;
		
	 return ( item >= 0 && item < (long)(vertexBufferStore->getVertexCount() - 1) );
}
