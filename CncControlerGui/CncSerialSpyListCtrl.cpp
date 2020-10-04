#include <iostream>
#include <wx/listbase.h>
#include <wx/clipbrd.h>
#include "OSD/CncAsyncKeyboardState.h"
#include "MainFrame.h"
#include "HexDecoder.h"
#include "DataControlModel.h"
#include "CncSerialSpyListCtrl.h"
#include "wxCrafterSerialSpy.h"

// ----------------------------------------------------------------------------
// CncSerialSpyListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_SERIAL_TIMER, 	wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncSerialSpyListCtrl, CncLargeScaledListCtrl)
	EVT_KEY_DOWN			(						CncSerialSpyListCtrl::onKeyDown			)
	EVT_SIZE				(						CncSerialSpyListCtrl::onSize			)
	EVT_TIMER				(wxEVT_SERIAL_TIMER,	CncSerialSpyListCtrl::onTimer			)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY,				CncSerialSpyListCtrl::onSelectListItem	)
	EVT_LIST_ITEM_ACTIVATED	(wxID_ANY,				CncSerialSpyListCtrl::onActivateListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncSerialSpyListCtrl::CncSerialSpyListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, itemAttrDefault			()
, itemAttrInbound			()
, itemAttrOutbound			()
, itemAttrResultOk			()
, itemAttrResultMore		()
, itemAttrResultError		()
, itemAttrResultWarning		()
, itemAttrResultLimit		()
, itemAttrResultHalt		()
, itemAttrResultQuit		()
, itemAttrResultInterrupt	()
, itemAttrResultMarker		()
, itemAttrResultEnable		()
, itemAttrResultDisable		()
, itemAttrResultCommand		()
, entries					()
, serialTimer				(this, wxEVT_SERIAL_TIMER)
, openDetails				(NULL)
, liveDecoding				(false)
, autoScrolling				(false)
, autoColumnSizing			(false)
, tsLast					(CncTimeFunctions::getMilliTimestamp())
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type", 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("#",	 	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Encoded",	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("Decoded",	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	SetBackgroundColour(*wxBLACK);
	SetForegroundColour(*wxWHITE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	wxFont defaultFont(font);
	
	itemAttrDefault.SetBackgroundColour(GetBackgroundColour());
	itemAttrDefault.SetFont(defaultFont);
	itemAttrDefault.SetTextColour(GetForegroundColour());

	itemAttrInbound.SetBackgroundColour(GetBackgroundColour());
	itemAttrInbound.SetFont(defaultFont);
	itemAttrInbound.SetTextColour(wxColour(128, 128, 64));
	
	itemAttrOutbound.SetBackgroundColour(GetBackgroundColour());
	itemAttrOutbound.SetFont(defaultFont);
	itemAttrOutbound.SetTextColour(wxColour(190, 190, 126));
	
	itemAttrResultOk.SetBackgroundColour(wxColour(2, 116, 50));
	itemAttrResultOk.SetFont(defaultFont);
	itemAttrResultOk.SetTextColour(*wxLIGHT_GREY);
	
	itemAttrResultMore.SetBackgroundColour(wxColour(4, 240, 104));
	itemAttrResultMore.SetFont(defaultFont);
	itemAttrResultMore.SetTextColour(*wxBLACK);

	itemAttrResultError.SetBackgroundColour(wxColour(176, 0, 0));
	itemAttrResultError.SetFont(defaultFont);
	itemAttrResultError.SetTextColour(*wxWHITE);
	
	itemAttrResultWarning.SetBackgroundColour(wxColour(255, 162, 117));
	itemAttrResultWarning.SetFont(defaultFont);
	itemAttrResultWarning.SetTextColour(*wxWHITE);

	itemAttrResultLimit 	= itemAttrResultWarning;
	itemAttrResultQuit 		= itemAttrResultWarning;
	itemAttrResultHalt 		= itemAttrResultError;
	itemAttrResultInterrupt = itemAttrResultError;
	
	itemAttrResultMarker.SetBackgroundColour(wxColour(0, 0, 155));
	itemAttrResultMarker.SetFont(defaultFont);
	itemAttrResultMarker.SetTextColour(*wxWHITE);

	itemAttrResultEnable.SetBackgroundColour(wxColour(185, 0, 185));
	itemAttrResultEnable.SetFont(defaultFont);
	itemAttrResultEnable.SetTextColour(*wxWHITE);
	
	itemAttrResultDisable = itemAttrResultEnable;
	
	itemAttrResultCommand.SetBackgroundColour(GetBackgroundColour());
	itemAttrResultCommand.SetFont(defaultFont);
	itemAttrResultCommand.SetTextColour(wxColour(242,242,0));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibSpy().Bitmap("BMP_IN"));
	imageList->Add(ImageLibSpy().Bitmap("BMP_OUT"));
	imageList->Add(ImageLibSpy().Bitmap("BMP_RET_OK"));
	imageList->Add(ImageLibSpy().Bitmap("BMP_RET_ERROR"));
	imageList->Add(ImageLibSpy().Bitmap("BMP_MARKER"));
	SetImageList(imageList, wxIMAGE_LIST_SMALL);

	startRefreshInterval();
}
/////////////////////////////////////////////////////////////
CncSerialSpyListCtrl::~CncSerialSpyListCtrl() {
/////////////////////////////////////////////////////////////
	clearAll();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::enableLiveDecoding(bool state) {
/////////////////////////////////////////////////////////////
	liveDecoding = state;
	
	updateColumnWidth();
	Refresh();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::enableAutoScrolling(bool state) {
/////////////////////////////////////////////////////////////
	autoScrolling = state;
	refreshList();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::enableAutoColumnSizing(bool state) {
/////////////////////////////////////////////////////////////
	autoColumnSizing = state;
	
	updateColumnWidth();
	Refresh();
}
/////////////////////////////////////////////////////////////
int CncSerialSpyListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	typedef CncSerialSpyListCtrl::LineInfo::Type Type;
	
	const int BMP_IN		= 0;
	const int BMP_OUT		= 1;
	const int BMP_RET_OK	= 2;
	const int BMP_RET_ERROR	= 3;
	const int BMP_MARKER	= 4;
	
	if ( column == COL_TYPE && isItemValid(item) == true ) {
		const Entry& le = entries.at(item);
		
		switch ( le.lt ) {
			
			case LT_ResultOk:
			case LT_ResultMore:
				return BMP_RET_OK;
			
			case LT_ResultError:
			case LT_ResultWarning:
			case LT_ResultLimit:
			case LT_ResultHalt:
			case LT_ResultQuit:
			case LT_ResultInterrupt:
				return BMP_RET_ERROR;
			
			case LT_Command:
				return BMP_OUT;
			
			case LT_Marker:
			case LT_Enable:
			case LT_Disable:
				return BMP_MARKER;
			
			default:
				switch ( LineInfo::decodeType(le.line) ) {
					case Type::LIT_OUTBOUND: return BMP_OUT;
					case Type::LIT_INBOUND:  return BMP_IN;
					case Type::LIT_UNKNOWN:  ;
				}
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncSerialSpyListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	typedef CncSerialSpyListCtrl::LineInfo::Type Type;
	
	if ( isItemValid(item) == true ) {
		
		const Entry& le = entries.at(item);
		switch ( le.lt ) {
			case LT_ResultOk:			return (wxListItemAttr*)(&itemAttrResultOk);
			case LT_ResultMore:			return (wxListItemAttr*)(&itemAttrResultMore);
			case LT_ResultError:		return (wxListItemAttr*)(&itemAttrResultError);
			case LT_ResultWarning:		return (wxListItemAttr*)(&itemAttrResultWarning);
			case LT_ResultLimit:		return (wxListItemAttr*)(&itemAttrResultLimit);
			case LT_ResultHalt:			return (wxListItemAttr*)(&itemAttrResultHalt);
			case LT_ResultQuit:			return (wxListItemAttr*)(&itemAttrResultQuit);
			case LT_ResultInterrupt:	return (wxListItemAttr*)(&itemAttrResultInterrupt);
			
			case LT_Marker:				return (wxListItemAttr*)(&itemAttrResultMarker);
			case LT_Enable:				return (wxListItemAttr*)(&itemAttrResultEnable);
			case LT_Disable:			return (wxListItemAttr*)(&itemAttrResultDisable);
			case LT_Command:			return (wxListItemAttr*)(&itemAttrResultCommand);
			
			case LT_Separator:
			case LT_Info:
			case LT_Time:				break;
			// add more on demand
			
			default:
				switch ( LineInfo::decodeType(le.line) ) {
					case Type::LIT_OUTBOUND: return (wxListItemAttr*)(&itemAttrOutbound);
					case Type::LIT_INBOUND:  return (wxListItemAttr*)(&itemAttrInbound);
					case Type::LIT_UNKNOWN:  ;
				}
		}
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&itemAttrDefault);
}
/////////////////////////////////////////////////////////////
wxString CncSerialSpyListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
	
	const Entry& le = entries.at(item);
	wxString ret;
	
	switch ( column ) {
		case COL_NUM:		return wxString::Format("%ld",		(item + 1) % LONG_MAX );
		case COL_TYPE:		return wxString::Format("%d",		(int)le.lt);
		case COL_LINE:		return wxString::Format("%s",		le.line);
		case COL_DECODED:	return wxString::Format("%s",		liveDecoding ? decodeSerialSpyLine(item, ret) : "");
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::flush() {
/////////////////////////////////////////////////////////////
	refreshList();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::refreshList() {
/////////////////////////////////////////////////////////////
	if ( entries.size() <= 0 )
		return;
	
	SetItemCount(entries.size());
	
	if ( autoScrolling == true ) {
		deselectAll();
		EnsureVisible(entries.size() - 1);
		Refresh();
	}
}
/////////////////////////////////////////////////////////////
const wxString CncSerialSpyListCtrl::getLine(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == true ) {
		const Entry& le = entries.at(item);
		return _(le.line);
	}
	return _("");
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::addLine(const wxString& line, const LineType lt) {
/////////////////////////////////////////////////////////////
	entries.push_back(std::move(Entry(line, lt)));
	
	if ( lt >= LT_ResultOk && lt <= LT_ResultInterrupt ) {
		if ( CncTimeFunctions::getMilliTimestamp() - tsLast > refreshInterval / 2 ) {
			refreshList();
			startRefreshInterval();
			tsLast = CncTimeFunctions::getMilliTimestamp();
		}
	}
	else {
		switch ( lt ) {
			case LT_Marker:
			case LT_Enable:
			case LT_Disable:	refreshList();
								break;
								
			default:			;
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	if ( autoColumnSizing == false )
		return;
		
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	// first set default sizes depending on content
	SetColumnWidth(COL_TYPE, 	 26);
	SetColumnWidth(COL_NUM, 	 64);
	SetColumnWidth(COL_LINE, 	120);
	SetColumnWidth(COL_DECODED,	  0);
	
	// try to strech the second (key) column
	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth() 
	         - GetColumnWidth(COL_NUM) 
			 - scrollbarWidth;
	
	size /= liveDecoding ? 2 : 1;
	
	if ( size > GetColumnWidth(COL_LINE) )
		SetColumnWidth(COL_LINE, size);
		
	if ( liveDecoding && size > GetColumnWidth(COL_DECODED) )
		SetColumnWidth(COL_DECODED, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////
bool CncSerialSpyListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return item >= 0 && item < (long)(entries.size());
}
//////////////////////////////////////////////////
bool CncSerialSpyListCtrl::copyToClipboard(bool allRows) {
//////////////////////////////////////////////////
	bool ret = false;
	
	// Write the content of entries to the clipboard
	if ( wxTheClipboard->Open() ) {
		
		if ( entries.size() > 0 ) {
			const size_t firstRow = allRows ? 0                  : GetTopItem();
			const size_t lastRow  = allRows ? GetItemCount() - 1 : firstRow + std::min(GetCountPerPage(), GetItemCount() - 1 );
			
			// check boundaries
			if ( firstRow >= entries.size() || lastRow >= entries.size() )  {
				
				std::cerr << "CncSerialSpyListCtrl::copyToClipboard: Invalid boundaries:"	<< std::endl;
				std::cerr << " Available row count       : " << entries.size()				<< std::endl;
				std::cerr << " Evaluated first / last row: " << firstRow << ", " << lastRow	<< std::endl;
				return false;
			}
			
			auto beg = entries.begin() + firstRow;
			auto end = entries.begin() + lastRow;
			
			wxString content;
			for ( auto it = beg; it != end; ++it )
				content.append(wxString::Format("%s\n", it->line));
			
			// This data objects are held by the clipboard,
			// so do not delete them in the app.
			ret = wxTheClipboard->SetData( new wxTextDataObject(content) );
		}
		else {
			ret = true;
		}
		wxTheClipboard->Close();
	}
	
	return ret;
}
//////////////////////////////////////////////////
void CncSerialSpyListCtrl::onKeyDown(wxKeyEvent& event) {
//////////////////////////////////////////////////
	const bool ctlKey = CncAsyncKeyboardState::isControlPressed();
	const int c       = event.GetUnicodeKey();
	
	if ( c == 'C' && ctlKey == true ) {
		
		if ( isItemValid(getLastSelection()) ) {
			if ( wxTheClipboard->Open() ) {
				wxTheClipboard->SetData( new wxTextDataObject(entries.at(getLastSelection()).line) );
				wxTheClipboard->Close();
			}
		}
		else {
			copyToClipboard(false);
		}
	}
	
	event.Skip(true);
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateColumnWidth();
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////
	const long ic = entries.size();
	if ( ic <= 0 )
		return;
		
	if ( GetItemCount() == ic )
		return;
	
	refreshList();
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( openDetails != NULL ) {
		wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
		wxPostEvent(openDetails, evt);
	}
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
		
	setLastSelection(item);
}
///////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::clearAll() {
///////////////////////////////////////////////////////////////////
	clear();
	entries.clear();
	SetItemCount(entries.size());
	Refresh();
}
///////////////////////////////////////////////////////////////////
const CncSerialSpyListCtrl::LineInfo& CncSerialSpyListCtrl::decodeLineInfo(const wxString& line, CncSerialSpyListCtrl::LineInfo& ret) const {
///////////////////////////////////////////////////////////////////
	ret.context  .clear();
	ret.hexString.clear();
	
	// determine type
	if 		( line.Contains("::<<") ) 	ret.type = CncSerialSpyListCtrl::LineInfo::Type::LIT_INBOUND;
	else if ( line.Contains("::>>") ) 	ret.type = CncSerialSpyListCtrl::LineInfo::Type::LIT_OUTBOUND;
	else								ret.type = CncSerialSpyListCtrl::LineInfo::Type::LIT_UNKNOWN;
	
	if ( ret.type == CncSerialSpyListCtrl::LineInfo::Type::LIT_UNKNOWN )
		return ret;
		
	// general check
	if ( line.Find("0x[") == wxNOT_FOUND )
		return ret;
	
	{
		const int s = line.Find('{');
		const int e = line.Find('}');
		
		if ( s != wxNOT_FOUND && e != wxNOT_FOUND )
			ret.context.assign(line.SubString(s + 1, e - 1).Trim(true).Trim(false));
	}
	
	{
		const int s = line.Find('[');
		const int e = line.Find(']');
	
		if ( s != wxNOT_FOUND && e != wxNOT_FOUND )
			ret.hexString.assign(line.SubString(s + 1, e - 1).Trim(true).Trim(false));
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncSerialSpyListCtrl::decodeSerialSpyLineIntern(long item, SpyHexDecoder::Details& details) const {
///////////////////////////////////////////////////////////////////
	typedef CncSerialSpyListCtrl::LineInfo::Type Type;

	if ( isItemValid(item) == false ) {
		details.more.assign(wxString::Format("Invalid item: %ld", item));
		return false;
	}
		
	const Entry& le = entries.at(item);
	const wxString line(le.line);
	
	CncSerialSpyListCtrl::LineInfo lineInfo;
	decodeLineInfo(line, lineInfo);
	
	// determine prev and next inbound
	details.inbound.prev.clear();
	details.inbound.next.clear();
	if ( lineInfo.type == Type::LIT_INBOUND ) {
		const auto end = entries.begin() + item;
		
		// prev ...
		for ( auto it = end; ; --it ) {
			CncSerialSpyListCtrl::LineInfo li;
			decodeLineInfo(it->line, li);
			details.inbound.prev.Prepend(li.hexString);
			
			// stop at RET_SOH
			if ( li.hexString == wxString::Format("%02X", RET_SOH) )
				break;
				
			// stop at entries.begin()
			if ( it == entries.begin() )
				break;
		}
		
		// next ...
		const auto next = end + 1;
		if (  next != entries.end() ) {
			CncSerialSpyListCtrl::LineInfo li;
			decodeLineInfo(next->line, li);
			details.inbound.next.assign(li.hexString);
		}
	}
	
	// determine detail type
	details.type = (SpyHexDecoder::Details::Type)lineInfo.type;
	if ( details.type == SpyHexDecoder::Details::DT_UNKNOWN )
		return false;
	
	if ( lineInfo.context.IsEmpty() ) {
		details.more.assign("{....} not type value found . . .\n");
		return false;
	}
	
	if ( lineInfo.hexString.IsEmpty() ) {
		details.more.assign("[....] not HEX values found . . .\n");
		return false;
	}
	
	SpyHexDecoder shd(lineInfo.context, lineInfo.hexString);
	shd.decode(details);
	return true;
}
///////////////////////////////////////////////////////////////////
const wxString& CncSerialSpyListCtrl::decodeSerialSpyLine(long item, wxString& ret) const {
///////////////////////////////////////////////////////////////////
	SpyHexDecoder::Details details;
	decodeSerialSpyLineIntern(item, details);
	
	ret.assign(details.more);
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& CncSerialSpyListCtrl::decodeSelectedSpyLine(wxString& ret) const {
///////////////////////////////////////////////////////////////////
	long item = getLastSelection();

	ret.clear();
	if ( isItemValid(item) == false )
		return ret;
		
	SpyHexDecoder::Details details;
	decodeSerialSpyLineIntern(item, details);
	
	ret.Prepend(wxString::Format("%s\n\n", getLine(item)));
	ret.assign(details.more);
	return ret;
}


