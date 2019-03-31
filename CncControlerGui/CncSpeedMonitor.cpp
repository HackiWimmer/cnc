#include <wx/dcbuffer.h>
#include "CncSpeedMonitor.h"

////////////////////////////////////////////////////////////////
CncSpeedMonitor::CncSpeedMonitor(wxWindow* parent)
: CncSpeedMonitorBase(parent)
, drawingArea(lMargin, tMargin, 0, 0)
, valueFont(7, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"))
, labelFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Segoe UI"))
, mouseLabel(-1, -1)
, axisMeasurePoints()
, axisMeasuredSpeed()
, axisConfiguredSpeed()
, lastRefresh(CncTimeFunctions::getMilliTimestamp())
, timeIndex(0)
, currentMeasuredFeedSpeed_MM_MIN(0.0)
, currentConfiguredFeedSpeed_MM_MIN(0.0)
////////////////////////////////////////////////////////////////
{
	// This has to be done to use wxAutoBufferedPaintDC 
	// on EVT_PAINT events correctly
	m_darwingArea->SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	m_refreshTimer->Stop();
	
	// setup axises
	axisMeasurePoints.pen 		= wxPen(wxColour(0, 190, 0), 	1, wxSOLID);
	axisMeasurePoints.pos 		= wxBOTTOM;
	axisMeasurePoints.fill 		= true;
	axisMeasurePoints.minValue	= 0.0;
	axisMeasurePoints.maxValue	= 3.0;
	axisMeasurePoints.yOffset 	= 5;
	
	axisMeasuredSpeed.pen 		= wxPen(wxColour(255, 80, 80), 	2, wxSOLID);
	axisMeasuredSpeed.pos 		= wxRIGHT;
	axisMeasuredSpeed.fill 		= false;
	axisMeasuredSpeed.yOffset 	= 0;
	
	axisConfiguredSpeed.pen		= wxPen(wxColour(64, 64, 64),	1, wxSOLID);
	axisConfiguredSpeed.pos		= wxLEFT;
	axisConfiguredSpeed.fill 	= true;
	axisMeasuredSpeed.yOffset 	= 0;
}
////////////////////////////////////////////////////////////////
CncSpeedMonitor::~CncSpeedMonitor() {
////////////////////////////////////////////////////////////////
	m_refreshTimer->Stop();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::reset() {
////////////////////////////////////////////////////////////////
	timeIndex = 0;
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::start(double maxSpeedValue_MM_MIN) {
////////////////////////////////////////////////////////////////
	m_refreshTimer->Stop();
	reset();
	
	axisMeasuredSpeed.maxValue 		= maxSpeedValue_MM_MIN + maxSpeedValue_MM_MIN * 0.1;
	axisConfiguredSpeed.maxValue	= maxSpeedValue_MM_MIN + maxSpeedValue_MM_MIN * 0.1;
	
	wxASSERT(axisMeasuredSpeed.maxValue   > axisMeasuredSpeed.minValue);
	wxASSERT(axisConfiguredSpeed.maxValue > axisConfiguredSpeed.minValue);
	
	lastRefresh = CncTimeFunctions::getMilliTimestamp();
	m_refreshTimer->Start(m_intervalSlider->GetValue());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::stop() {
////////////////////////////////////////////////////////////////
	m_refreshTimer->Stop();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::setCurrentFeedSpeedValue(double measured_MM_MIN, double configured_MM_MIN) {
////////////////////////////////////////////////////////////////
	currentMeasuredFeedSpeed_MM_MIN		= std::max(0.0, measured_MM_MIN);
	currentConfiguredFeedSpeed_MM_MIN	= std::max(0.0, configured_MM_MIN);
	
	// log that min one measure point exists
	axisMeasurePoints.values[timeIndex] = axisMeasurePoints.maxValue;
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onTimer(wxTimerEvent& event) {
////////////////////////////////////////////////////////////////
	if ( (int)timeIndex > drawingArea.width )
		timeIndex = 0;
		
	axisMeasuredSpeed.values[timeIndex] 	= axisMeasuredSpeed.convert(currentMeasuredFeedSpeed_MM_MIN);
	axisConfiguredSpeed.values[timeIndex] 	= axisConfiguredSpeed.convert(currentConfiguredFeedSpeed_MM_MIN);
	
	timeIndex++;
	
	if ( CncTimeFunctions::getMilliTimestamp() - lastRefresh > refreshInterval ) {
		lastRefresh = CncTimeFunctions::getMilliTimestamp();
		Refresh();
	}
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onPaint(wxPaintEvent& event) {
////////////////////////////////////////////////////////////////
	if ( IsShown() == false )
		return;
	
	wxAutoBufferedPaintDC dc(m_darwingArea);
	dc.Clear();
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	// --------------------------------------------------------
	auto drawBoundbox = [&]() {
		dc.SetPen(wxPen(*wxWHITE, 1, wxDOT));
		dc.DrawRectangle(drawingArea);
		
		const unsigned int dy = drawingArea.GetHeight() / 3; 
		dc.DrawLine(lMargin, tMargin + dy * 1, lMargin + drawingArea.GetWidth(), tMargin + dy * 1);
		dc.DrawLine(lMargin, tMargin + dy * 2, lMargin + drawingArea.GetWidth(), tMargin + dy * 2);
		dc.DrawLine(lMargin, tMargin + dy * 3, lMargin + drawingArea.GetWidth(), tMargin + dy * 3);
	};
	
	// --------------------------------------------------------
	auto drawMouseLabel = [&]() {
		if      ( mouseLabel.x < 0 )									return;
		else if ( mouseLabel.x < (int)lMargin ) 						return;
		else if ( mouseLabel.y < (int)tMargin ) 						return;
		else if ( mouseLabel.x > (int)lMargin + drawingArea.width) 		return;
		else if ( mouseLabel.y > (int)tMargin + drawingArea.height) 	return;
		
		dc.SetTextForeground(*wxRED);
		dc.SetFont(labelFont);
		
		const double value = axisMeasuredSpeed.convertToValue(tMargin + drawingArea.height - mouseLabel.y);
		dc.DrawLabel(wxString::Format("%4.0lf", value), wxRect(mouseLabel.x, mouseLabel.y - 10, 10, 10), wxALIGN_NOT);
	};
	
	// --------------------------------------------------------
	auto drawValues = [&]() {
		
		auto draw = [&](Axis& axis, unsigned int x, unsigned int i) {
			dc.SetPen(axis.pen);
			if ( axis.fill == false ) {
				dc.DrawLine(lMargin + x - 1, axis.yOffset + tMargin + drawingArea.height - axis.values[i - 1],
							lMargin + x,     axis.yOffset + tMargin + drawingArea.height - axis.values[i    ]);
			}
			else {
				dc.DrawRectangle(lMargin + x, axis.yOffset + tMargin + drawingArea.height, 1, -axis.values[i]);
			}
		};
		
		const unsigned int  pos = timeIndex;
		unsigned int 		x = 0;
		
		for ( unsigned int i = pos; i <= (unsigned int)drawingArea.width; i++, x++ ) {
			if ( i == 0 || x == 0 )
				continue;

			if ( m_btToggleConfiguredAxis->GetValue() )		draw(axisConfiguredSpeed, 	x ,i);
			if ( m_btToggleMeasurePointsAxis->GetValue() ) 	draw(axisMeasurePoints,		x ,i);
			
			draw(axisMeasuredSpeed, x ,i);
		}
		
		for ( unsigned int i = 0; i < pos; i++, x++ ) {
			if ( i == 0 || x == 0 )
				continue;
				
			if ( m_btToggleConfiguredAxis->GetValue() )		draw(axisConfiguredSpeed, 	x ,i);
			if ( m_btToggleMeasurePointsAxis->GetValue() ) 	draw(axisMeasurePoints,		x ,i);
			
			draw(axisMeasuredSpeed, x ,i);
		}
	};
	
	// --------------------------------------------------------
	auto drawCurrentValue = [&](Axis& axis, double value) {
		dc.SetTextForeground(axis.pen.GetColour());
		dc.SetFont(valueFont);
		
		const int x = (axis.pos == wxLEFT ? 1 : lMargin + drawingArea.width + 1);
		const int y = drawingArea.height - axis.convert(value);
		
		dc.DrawLabel(wxString::Format("%4.0lf", value), wxRect(x, y, 100, 100), wxALIGN_NOT);
	};
	
	// --------------------------------------------------------
	// drawing
	drawBoundbox();
	
	if ( m_refreshTimer->IsRunning() && m_btToggleConnection->GetValue() == true ) {
		drawValues();
		drawCurrentValue(axisConfiguredSpeed, currentConfiguredFeedSpeed_MM_MIN); 
		drawCurrentValue(axisMeasuredSpeed, currentMeasuredFeedSpeed_MM_MIN);
	}
	
	drawMouseLabel();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onSize(wxSizeEvent& event) {
////////////////////////////////////////////////////////////////
	const wxSize size = m_darwingArea->GetClientSize();

	drawingArea.height 			= size.GetHeight() - tMargin - bMargin;
	drawingArea.width  			= size.GetWidth()  - lMargin - rMargin;
	drawingArea.width 			= std::min((int)MAX_VALUES, drawingArea.width);
	
	axisMeasuredSpeed.height 	= drawingArea.height;
	axisConfiguredSpeed.height 	= drawingArea.height;
	
	reset();
	Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onMouseMotion(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	if ( mouseLabel.x < 0 )
		return;
		
	mouseLabel = {event.GetX(), event.GetY()};
	Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onLeftDown(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	mouseLabel = {event.GetX(), event.GetY()};
	Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onLeftUp(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	mouseLabel = {-1, -1};
	Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleConfiguredAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleMeasurePointsAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleConnection(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	m_btToggleConnection->GetValue() == true ? m_btToggleConnection->SetBitmap(bmpOn) 
	                                         : m_btToggleConnection->SetBitmap(bmpOff);
	m_btToggleConnection->Refresh();
	m_btToggleConnection->Update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::changeIntervalSlider(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////
	const int value = m_intervalSlider->GetValue();
	
	m_intervalSlider->SetToolTip(wxString::Format("%d ms", value));
	m_refreshTimer->Start(value);
}
