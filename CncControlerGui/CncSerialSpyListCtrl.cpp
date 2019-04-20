#include <iostream>
#include <wx/listbase.h>
#include "MainFrame.h"
#include "HexDecoder.h"
#include "DataControlModel.h"
#include "CncSerialSpyListCtrl.h"

// ----------------------------------------------------------------------------
// CncSerialSpyListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_SERIAL_TIMER, 	wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncSerialSpyListCtrl, CncLargeScaledListCtrl)
	EVT_SIZE(							CncSerialSpyListCtrl::onSize)
	EVT_TIMER(wxEVT_SERIAL_TIMER, 		CncSerialSpyListCtrl::onTimer)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, 	CncSerialSpyListCtrl::onSelectListItem)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, 	CncSerialSpyListCtrl::onActivateListItem)
	EVT_LIST_ITEM_FOCUSED(wxID_ANY, 	CncSerialSpyListCtrl::onFocusListItem)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncSerialSpyListCtrl::CncSerialSpyListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, itemAttrDefault()
, itemAttrResultOk()
, itemAttrResultMore()
, itemAttrResultError()
, itemAttrResultWarning()
, itemAttrResultLimit()
, itemAttrResultHalt()
, itemAttrResultQuit()
, itemAttrResultInterrupt()
, itemAttrResultMarker()
, itemAttrResultEnable()
, itemAttrResultDisable()
, itemAttrResultCommand()
, serialTimer(this, wxEVT_SERIAL_TIMER)
, lastItemCount(0)
, tsLast(CncTimeFunctions::getMilliTimestamp())
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("Type", 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("#",	 	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Value",	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	SetBackgroundColour(*wxBLACK);
	SetForegroundColour(*wxWHITE);
	
	decodeSerialSpyLine(-1, "", true);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
	SetFont(font);
	
	wxFont defaultFont(font);
	
	itemAttrDefault.SetBackgroundColour(GetBackgroundColour());
	itemAttrDefault.SetFont(defaultFont);
	itemAttrDefault.SetTextColour(GetForegroundColour());

	itemAttrResultOk.SetBackgroundColour(wxColour(2, 116, 50));
	itemAttrResultOk.SetFont(defaultFont);
	itemAttrResultOk.SetTextColour(*wxWHITE);
	
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

	/*
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibSetterList().Bitmap("BMP_SETTER"));		// 0
	imageList->Add(ImageLibSetterList().Bitmap("BMP_SEPARATOR"));	// 1
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	*/
	
	startRefreshInterval();
}
/////////////////////////////////////////////////////////////
CncSerialSpyListCtrl::~CncSerialSpyListCtrl() {
/////////////////////////////////////////////////////////////
	// currently nothing to do
}
/////////////////////////////////////////////////////////////
int CncSerialSpyListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	// currently nothing to do
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncSerialSpyListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	wxString t(getItemText(item, COL_TYPE));
	long type;
	t.ToLong(&type);
	
	switch ( type ) {
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
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&itemAttrDefault);
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::flush() {
/////////////////////////////////////////////////////////////
	refreshList();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::refreshList() {
/////////////////////////////////////////////////////////////
	if ( getItemCount() <= 0 )
		return;
	
	deselectAll();
	EnsureVisible(0);
	Refresh();
}
/////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::addLine(const wxString& line, const LineType lt) {
/////////////////////////////////////////////////////////////
	CncColumContainer cc(TOTAL_COL_COUNT);
	cc.updateItem(COL_TYPE, 	wxString::Format("%d",  (int)lt));
	cc.updateItem(COL_NUM, 		wxString::Format("%ld", (getItemCount() + 1) % LONG_MAX ));
	cc.updateItem(COL_LINE, 	line);
	
	appendItem(cc);
	
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
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	// first set default sizes depending on content
	SetColumnWidth(COL_TYPE, 	  1);
	SetColumnWidth(COL_NUM, 	 64);
	SetColumnWidth(COL_LINE, 	120);
	
	// try to strech the second (key) column
	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth() 
	         - GetColumnWidth(COL_NUM) 
			 - scrollbarWidth;
			 
	if ( size > GetColumnWidth(COL_LINE) )
		SetColumnWidth(COL_LINE, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateColumnWidth();
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////
	const long ic = getItemCount();
	if ( ic <= 0 )
		return;
		
	if ( lastItemCount == ic )
		return;
		
	lastItemCount = ic;
	refreshList();
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onActivateListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED);
	wxPostEvent(THE_APP->GetBtOpenSpyDetailWindow(), evt);
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onFocusListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	//std::cout << "CncSerialSpyListCtrl::onFocusListItem" << std::endl;
}
/////////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////////////
	long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	setLastSelection(item);
	
	long ln;
	getRow(item).getItem(COL_NUM).ToLong(&ln);
	decodeSerialSpyLine(ln, getRow(item).getItem(COL_LINE));
}
///////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::clearDetails() {
///////////////////////////////////////////////////////////////////
	THE_APP->GetSpyInboundDetails()->DeleteAllItems();
	THE_APP->GetSpyOutboundDetails()->DeleteAllItems();
	THE_APP->GetSpyUnknownDetails()->DeleteAllItems();
	
	THE_APP->GetSypDetailsBook()->SetSelection(SpyHexDecoder::Details::DT_UNKNOWN);
	
	decodeSerialSpyLine(-1, "", true);
}
///////////////////////////////////////////////////////////////////
void CncSerialSpyListCtrl::decodeSerialSpyLine(long linenumber, const wxString& line, bool displayInfo) {
///////////////////////////////////////////////////////////////////
	if ( IsFrozen() )
		return;
	
	struct Details : public SpyHexDecoder::Details {
		private:
			DcmRow row;
			long linenumber;
			
		public:
			
			///////////////////////////////////////////////////////////
			Details(long ln) 
			: SpyHexDecoder::Details()
			, linenumber(ln)
			{
				THE_APP->GetSpyInboundDetails()->DeleteAllItems();
				THE_APP->GetSpyOutboundDetails()->DeleteAllItems();
				THE_APP->GetSpyUnknownDetails()->DeleteAllItems();
				
				THE_APP->GetSypDetailsBook()->SetSelection(DT_UNKNOWN);
				more.assign("No detail information available . . .");
			}
			
			///////////////////////////////////////////////////////////
			void display() {
				
				auto displayLinenumber = [&](wxTextCtrl* ctrl) {
					if ( ctrl == NULL )
						return;
						
					if ( linenumber > 0 )	ctrl->ChangeValue(wxString::Format("%ld", linenumber));
					else					ctrl->ChangeValue("");
					
				};
				
				switch ( type ) {
					case DT_INBOUND: 	DataControlModel::addSpyInboundRow(row, 
																		   context, 
																		   cmd,
																		   pid, 
																		   index, 
																		   more);
										displayLinenumber(THE_APP->GetLnInboundDetails());
										THE_APP->GetSpyInboundDetails()->AppendItem(row);
										THE_APP->GetSypDetailsBook()->SetSelection(DT_INBOUND);
										break;
										
					case DT_OUTBOUND: 	DataControlModel::addSpyOutboundRow(row, 
																			context,
																			cmd,
																			portion,
																			more);
										displayLinenumber(THE_APP->GetLnOutboundDetails());
										THE_APP->GetSpyOutboundDetails()->AppendItem(row);
										THE_APP->GetSypDetailsBook()->SetSelection(DT_OUTBOUND);
										break;
										
					default:			DataControlModel::addSpyDateilInfoRow(row, more);
										displayLinenumber(THE_APP->GetLnUnkonwnDetails());
										THE_APP->GetSpyUnknownDetails()->AppendItem(row);
										THE_APP->GetSypDetailsBook()->SetSelection(DT_UNKNOWN);
				}
				
				THE_APP->updateSpyDetailWindow();
			}
		
	} details(linenumber);
	
	// determine detail type
	if 		( line.Contains("::<<") ) 	details.type = SpyHexDecoder::Details::DT_INBOUND;
	else if ( line.Contains("::>>") ) 	details.type = SpyHexDecoder::Details::DT_OUTBOUND;
	else								details.type = SpyHexDecoder::Details::DT_UNKNOWN;
	
	if ( details.type == SpyHexDecoder::Details::DT_UNKNOWN ) {
		details.display();
		return;
	}
		
	// general check
	if ( line.Find("0x[") == wxNOT_FOUND ) {
		if ( displayInfo == true ) {
			details.more.assign("Nothing to decode . . .\n");
			details.display();
		}
			
		return;
	}
	
	int s = line.Find('{');
	int e = line.Find('}');
	
	if ( s != wxNOT_FOUND && e == wxNOT_FOUND ) {
		details.more.assign("{....} not type value found . . .\n");
		details.display();
		return;
	}
	
	wxString contextInfo(line.SubString(s + 1, e - 1).Trim(true).Trim(false));
	
	s = line.Find('[');
	e = line.Find(']');
	
	if ( s != wxNOT_FOUND && e == wxNOT_FOUND ) {
		details.more.assign("[....] not HEX values found . . .\n");
		details.display();
		return;
	}
	
	wxString hexString = line.SubString(s + 1, e - 1).Trim(true).Trim(false);
	
	SpyHexDecoder shd(contextInfo, hexString);
	shd.decode(details);
	details.display();
}
