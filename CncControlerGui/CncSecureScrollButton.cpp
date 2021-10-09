#include "CncCommon.h"
#include "CncSecureScrollButton.h"

///////////////////////////////////////////////////////////////////////
CncSecureScrollButton::CncSecureScrollButton(wxWindow* parent, wxOrientation orientation, Mode m, int sensitivity, wxSize minSize)
: wxPanel		(parent)
, scrollbar		(NULL)
, caller		(NULL)
, updateTimer	(new wxTimer())
, mode			(m)
, callbackId	(0)
, zeroIndex		(0)
, prevValue		(0)
, released		(false)
, skipped		(false)
///////////////////////////////////////////////////////////////////////
{
	wxFlexGridSizer* flexGridSizer = new wxFlexGridSizer(1, 1, 0, 0);
	flexGridSizer->SetFlexibleDirection( wxBOTH );
	flexGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	flexGridSizer->AddGrowableCol(0);
	flexGridSizer->AddGrowableRow(0);
	this->SetSizer(flexGridSizer);

	scrollbar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), orientation);
	switch (mode) 
	{
		case M_NEGATIVE:	zeroIndex = sensitivity;
							prevValue = zeroIndex;
							scrollbar->SetScrollbar(zeroIndex, 1, sensitivity, 1);
							break;
							
		case M_POSITIVE:	zeroIndex = 0;
							prevValue = zeroIndex;
							scrollbar->SetScrollbar(zeroIndex, 1, sensitivity, 1);
							break;
							
		case M_BOTH:		zeroIndex = sensitivity;
							prevValue = zeroIndex;
							scrollbar->SetScrollbar(zeroIndex, 1, sensitivity * 2 + 1, 1);
							break;
	}
	
	flexGridSizer->Add(scrollbar, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
	scrollbar->SetMinSize(minSize);
	scrollbar->SetBackgroundColour(wxColour(227,227,227));

	SetName(wxT("CncSecureScrollButton"));
	SetSize(wxDLG_UNIT(this, wxSize(-1,-1)));
	if (GetSizer())
		 GetSizer()->Fit(this);
		 
		#warning
	EnableTouchEvents(wxTOUCH_ALL_GESTURES);
		 
	// Connect events
	scrollbar->Connect(wxEVT_SCROLL_CHANGED,			wxScrollEventHandler(CncSecureScrollButton::onChangedValue), NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_THUMBRELEASE,		wxScrollEventHandler(CncSecureScrollButton::onThumbrelease), NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_THUMBTRACK,			wxScrollEventHandler(CncSecureScrollButton::onThumbtrack),   NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_PAGEDOWN,			wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_PAGEUP,				wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_LINEDOWN,			wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_LINEUP,				wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_TOP,				wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Connect(wxEVT_SCROLL_BOTTOM,				wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Connect(wxEVT_LEAVE_WINDOW,				wxMouseEventHandler (CncSecureScrollButton::onLeaveWindow),  NULL, this);
	
	updateTimer->Connect(wxEVT_TIMER,					wxTimerEventHandler (CncSecureScrollButton::onUpdateTimer),  NULL, this);
}
///////////////////////////////////////////////////////////////////////
CncSecureScrollButton::~CncSecureScrollButton() {
///////////////////////////////////////////////////////////////////////
	scrollbar->Disconnect(wxEVT_SCROLL_CHANGED,			wxScrollEventHandler(CncSecureScrollButton::onChangedValue), NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_THUMBRELEASE,	wxScrollEventHandler(CncSecureScrollButton::onThumbrelease), NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_THUMBTRACK,		wxScrollEventHandler(CncSecureScrollButton::onThumbtrack),   NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_PAGEDOWN,		wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_PAGEUP,			wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_LINEDOWN,		wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_LINEUP,			wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_TOP,				wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Disconnect(wxEVT_SCROLL_BOTTOM,			wxScrollEventHandler(CncSecureScrollButton::onSkipValue),    NULL, this);
	scrollbar->Disconnect(wxEVT_LEAVE_WINDOW,			wxMouseEventHandler (CncSecureScrollButton::onLeaveWindow),  NULL, this);
	
	updateTimer->Disconnect(wxEVT_TIMER,				wxTimerEventHandler (CncSecureScrollButton::onUpdateTimer),  NULL, this);
	
	updateTimer->Stop();
	
	wxDELETE(updateTimer);
	wxDELETE(scrollbar);
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::setCallbackInterface(CallbackInterface* inf, int id) { 
///////////////////////////////////////////////////////////////////////
	caller		= inf;
	callbackId	= id;
}
///////////////////////////////////////////////////////////////////////
int CncSecureScrollButton::getCurrentValue() {
///////////////////////////////////////////////////////////////////////
	const int pos = scrollbar->GetThumbPosition();
	switch (mode) 
	{
		case M_NEGATIVE:	return (pos * -1);
		case M_POSITIVE:	return (pos);
		case M_BOTH:		return (pos - zeroIndex) * -1;
	}
	
	return 0;
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::reset() {
///////////////////////////////////////////////////////////////////////
	updateTimer->Stop();
	
	scrollbar->SetThumbPosition(zeroIndex);
	const int currValue = getCurrentValue();
	
	if ( caller ) {
		CncSecureScrollButton::CBI::Data d(callbackId, currValue, prevValue);
		caller->notifyValueChange(d);
	}
	
	prevValue = currValue;
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::publish(bool continuous) {
///////////////////////////////////////////////////////////////////////
	if ( continuous && updateTimer->IsRunning() == false )
		updateTimer->Start(250);
		
	const int currValue = getCurrentValue();
	
	if ( caller ) {
		CncSecureScrollButton::CBI::Data d(callbackId, currValue, prevValue);
		caller->notifyValueChange(d);
	}
	
	prevValue = currValue;
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::onChangedValue(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////////
	reset();
	event.Skip();
	return;
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::onSkipValue(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////////
	reset();
	skipped = true;
	event.Skip();
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::onThumbtrack(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////////
	publish(true);
	event.Skip();
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::onThumbrelease(wxScrollEvent& event) {
///////////////////////////////////////////////////////////////////////
	reset();
	event.Skip();
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::onLeaveWindow(wxMouseEvent& event) { 
///////////////////////////////////////////////////////////////////////
	//reset();
	event.Skip();
}
///////////////////////////////////////////////////////////////////////
void CncSecureScrollButton::onUpdateTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////////
	publish(false);
}