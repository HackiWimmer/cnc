#include <iostream>
#include <wx/imaglist.h>
#include "wxCrafterImages.h"

#include "CncNumberFormatter.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "CncArduino.h"
#include "CncSetterListCtrl.h"

// ----------------------------------------------------------------------------
// CncSetterListCtrl Event Table
// ----------------------------------------------------------------------------
wxDEFINE_EVENT(wxEVT_SETTER_DISPLAY_TIMER,  wxTimerEvent);

wxBEGIN_EVENT_TABLE(CncSetterListCtrl, CncLargeScaledListCtrl)
	EVT_PAINT(								CncSetterListCtrl::onPaint)
	EVT_SIZE(								CncSetterListCtrl::onSize)
	EVT_TIMER(wxEVT_SETTER_DISPLAY_TIMER,	CncSetterListCtrl::onDisplayTimer)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////
CncSetterListCtrl::CncSetterListCtrl(wxWindow *parent, long style)
: CncLargeScaledListCtrl(parent, style)
, setterEntries()
, defaultItemAttr()
, separatorRunItemAttr()
, separatorResetItemAttr()
, separatorSetupItemAttr()
, displayTimer					(this, wxEVT_SETTER_DISPLAY_TIMER)
, displayTimerInterval			(333)
/////////////////////////////////////////////////////////////
{
	// add colums
	AppendColumn("",	 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("#",	 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("PID", 	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Key", 	wxLIST_FORMAT_LEFT, 	wxLIST_AUTOSIZE);
	AppendColumn("Value",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	AppendColumn("Unit",	wxLIST_FORMAT_RIGHT, 	wxLIST_AUTOSIZE);
	
	// determine styles
	setListType(CncLargeScaledListCtrl::ListType::REVERSE);
	
	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Segoe UI"));
	SetFont(font);
	
	SetBackgroundColour(wxColour(  0,  0,  0));
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibSetterList().Bitmap("BMP_SETTER_VALUE"));		// 0
	imageList->Add(ImageLibSetterList().Bitmap("BMP_SETTER_SEPARATOR"));	// 1
	
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
	
	defaultItemAttr.SetBackgroundColour(GetBackgroundColour());
	defaultItemAttr.SetFont(font);
	defaultItemAttr.SetTextColour(*wxLIGHT_GREY);
	
	wxFont separatorFont(font);
	separatorFont.MakeBold();

	separatorRunItemAttr.SetBackgroundColour(wxColour(128, 64, 64));
	separatorRunItemAttr.SetFont(separatorFont);
	separatorRunItemAttr.SetTextColour(*wxWHITE);

	separatorResetItemAttr.SetBackgroundColour(wxColour(0, 0, 64));
	separatorResetItemAttr.SetFont(separatorFont);
	separatorResetItemAttr.SetTextColour(*wxWHITE);

	separatorSetupItemAttr.SetBackgroundColour(wxColour(0, 64, 64));
	separatorSetupItemAttr.SetFont(separatorFont);
	separatorSetupItemAttr.SetTextColour(*wxWHITE);
}
/////////////////////////////////////////////////////////////
CncSetterListCtrl::~CncSetterListCtrl() {
/////////////////////////////////////////////////////////////
	setterEntries.clear();
}
/////////////////////////////////////////////////////////////
void CncSetterListCtrl::addSetter(unsigned char pid, const cnc::SetterValueList& v) {
/////////////////////////////////////////////////////////////
	setterEntries.push_back(std::move(SetterEntry(pid, v)));
	SetItemCount(setterEntries.size());
	
	if ( IsShownOnScreen() == true )
		if ( displayTimer.IsRunning() == false )
			displayTimer.Start(displayTimerInterval, wxTIMER_ONE_SHOT);
}
/////////////////////////////////////////////////////////////
void CncSetterListCtrl::clearAll() {
/////////////////////////////////////////////////////////////
	setterEntries.clear();
	clear();
}
/////////////////////////////////////////////////////////////
long CncSetterListCtrl::translateItem(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return -1;
		
	return setterEntries.size() - 1 - item;
}
/////////////////////////////////////////////////////////////
bool CncSetterListCtrl::isItemValid(long item) const {
/////////////////////////////////////////////////////////////
	return item >= 0 && item < (long)(setterEntries.size());
}
/////////////////////////////////////////////////////////////
wxString CncSetterListCtrl::OnGetItemText(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == false )
		return _("");
		
	const long tItem 		= translateItem(item);
	const SetterEntry& se 	= setterEntries.at(tItem);
	const unsigned char pid = se.pid;
	const int32_t value 	= se.values.front();
	
	if ( pid == PID_SEPARATOR ) {
		
		switch ( column ) {
			case COL_TYPE:	return wxString::Format("%ld",		(long)value);
			case COL_NUM:	return wxString::Format("%06lu",	tItem%1000000 + 1);
			case COL_PID:	return wxString::Format("%u",		pid);
			case COL_UNIT:	return _("");
			case COL_VAL:	return wxString::Format("%s.%03ld", 	se.time.FormatTime(), (long)se.time.GetMillisecond());
			case COL_KEY:	{
								wxString label("Bookmark: Type(<UNKNOWN>)");
								wxString portName;
								THE_APP->getCurrentPortName(portName);
								
								switch ( value ) {
									case SEPARARTOR_SETUP:		return wxString::Format("Bookmark: Type(<SETUP>[%s])", portName);
									case SEPARARTOR_RESET:		return wxString::Format("Bookmark: Type(<RESET>[%s])", portName);
									case SEPARARTOR_RUN:		return wxString::Format("Bookmark: Type(<RUN>[%s])",   portName);
								}
								
								return _("");
							}
		}
		
	} else {
		static std::string retVal;
		
		std::stringstream ss;
		cnc::traceSetterValueList(ss, se.values, pid < PID_FLOAT_RANG_START ? 1 : FLT_FACT);
		
		switch ( column ) {
			case COL_TYPE:	return _("");
			case COL_NUM:	return wxString::Format("%06lu", 	tItem%1000000 + 1);
			case COL_PID: 	return wxString::Format("%u",		pid);
			case COL_KEY:	return wxString::Format("%s",		ArduinoPIDs::getPIDLabel((int)pid, retVal));
			case COL_VAL:	return wxString::Format("%s",		ss.str());
			case COL_UNIT:	return wxString::Format("%s",		ArduinoPIDs::getPIDUnit((int)pid, retVal));
		}
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////
int CncSetterListCtrl::OnGetItemColumnImage(long item, long column) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == true ) {
		
		const SetterEntry& se = setterEntries.at(item = translateItem(item));
		const unsigned char pid = se.pid;
		
		switch ( column ) {
			case COL_TYPE: 	if ( pid == PID_SEPARATOR )	return 1;
							else						return 0;
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
wxListItemAttr* CncSetterListCtrl::OnGetItemAttr(long item) const {
/////////////////////////////////////////////////////////////
	if ( isItemValid(item) == true ) {
		
		const SetterEntry& se = setterEntries.at(translateItem(item));
		const unsigned char pid = se.pid;
		
		if ( pid == PID_SEPARATOR ) {
			wxString val(GetItemText(item, COL_TYPE));
			long id; val.ToLong(&id);
			
			switch ( id ) {
				case SEPARARTOR_SETUP:	return (wxListItemAttr*)(&separatorSetupItemAttr);
				case SEPARARTOR_RESET:	return (wxListItemAttr*)(&separatorResetItemAttr);
				case SEPARARTOR_RUN:	return (wxListItemAttr*)(&separatorRunItemAttr);
			}
		}
	}
	
	// this indicates to use the default style
	return (wxListItemAttr*)(&defaultItemAttr);
}
/////////////////////////////////////////////////////////////////////
void CncSetterListCtrl::updateColumnWidth() {
/////////////////////////////////////////////////////////////////////
	// avoid flicker
	if ( IsFrozen() == false )
		Freeze();
		
	// first set default sizes depending on content
	SetColumnWidth(COL_TYPE, 	 22);
	SetColumnWidth(COL_NUM, 	 64);
	SetColumnWidth(COL_PID, 	 40);
	SetColumnWidth(COL_KEY, 	wxLIST_AUTOSIZE);
	SetColumnWidth(COL_VAL, 	120);
	SetColumnWidth(COL_UNIT, 	 58);
	
	// try to strech the second (key) column
	const int scrollbarWidth = 26;
	int size = GetSize().GetWidth() 
	         - GetColumnWidth(COL_NUM) 
			 - GetColumnWidth(COL_VAL) 
			 - GetColumnWidth(COL_UNIT)
			 - GetColumnWidth(COL_TYPE) 
			 - GetColumnWidth(COL_PID) 
			 - scrollbarWidth;
			 
	if ( size > GetColumnWidth(COL_KEY) )
		SetColumnWidth(COL_KEY, size);
		
	if ( IsFrozen() == true )
		Thaw();
}
/////////////////////////////////////////////////////////////////////
void CncSetterListCtrl::onSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	updateColumnWidth();
	event.Skip(true);
}
//////////////////////////////////////////////////
void CncSetterListCtrl::onDisplayTimer(wxTimerEvent& event) {
//////////////////////////////////////////////////
	Refresh();
}
//////////////////////////////////////////////////
void CncSetterListCtrl::onPaint(wxPaintEvent& event) {
//////////////////////////////////////////////////
	const long items = GetItemCount();
	
	THE_APP->GetSetterListCount()->ChangeValue(CncNumberFormatter::toString(items));
	THE_APP->GetSetterListCount()->SetToolTip(THE_APP->GetSetterListCount()->GetLabel());
	
	event.Skip();
}
