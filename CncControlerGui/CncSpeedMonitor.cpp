#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include "CncFileNameService.h"
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
, axisReceivedSpeed()
, axisConfiguredSpeed()
, lastRefresh(CncTimeFunctions::getMilliTimestamp())
, timeIndex(0)
, currentMeasuredFeedSpeed_MM_MIN(0.0)
, currentReceivedFeedSpeed_MM_MIN(0.0)
, currentConfiguredFeedSpeed_MM_MIN(0.0)
////////////////////////////////////////////////////////////////
{
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	m_btToggleConnection->SetBitmapDisabled(bmpOff);
	
	m_refreshTimer->Stop();
	
	// This has to be done to use wxAutoBufferedPaintDC 
	// on EVT_PAINT events correctly
	m_darwingArea->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_leftAxis->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_rightAxis->SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	m_darwingArea->SetBackgroundColour(*wxBLACK);
	m_leftAxis->SetBackgroundColour(*wxBLACK);
	m_rightAxis->SetBackgroundColour(*wxBLACK);
	
	// setup axises
	axisMeasurePoints.pen 		= wxPen(wxColour(0, 190, 0), 		1, wxSOLID);
	axisMeasurePoints.pos 		= wxBOTTOM;
	axisMeasurePoints.fill 		= true;
	axisMeasurePoints.minValue	= 0.0;
	axisMeasurePoints.maxValue	= 3.0;
	axisMeasurePoints.yOffset 	= 5;
	
	axisMeasuredSpeed.pen 		= wxPen(wxColour(255, 80, 80), 		2, wxSOLID);
	axisMeasuredSpeed.pos 		= wxRIGHT;
	axisMeasuredSpeed.fill 		= false;
	axisMeasuredSpeed.yOffset 	= 0;
	
	axisReceivedSpeed.pen 		= wxPen(wxColour(239, 228, 176),	1, wxSOLID);
	axisReceivedSpeed.pos 		= wxRIGHT;
	axisReceivedSpeed.fill 		= false;
	axisReceivedSpeed.yOffset 	= 0;
	
	axisConfiguredSpeed.pen		= wxPen(wxColour(64, 64, 64),		1, wxSOLID);
	axisConfiguredSpeed.pos		= wxLEFT;
	axisConfiguredSpeed.fill 	= true;
	axisMeasuredSpeed.yOffset 	= 0;
	
	setupSizes();
}
////////////////////////////////////////////////////////////////
CncSpeedMonitor::~CncSpeedMonitor() {
////////////////////////////////////////////////////////////////
	m_refreshTimer->Stop();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::activate(bool enable) {
////////////////////////////////////////////////////////////////
	enableConnection(enable);
	
	m_btToggleConnection->Enable(enable);
	
	m_btToggleConfiguredAxis->Enable(enable);
	m_btToggleMeasurePointsAxis->Enable(enable);
	m_btToggleMeasuredSpeedAxis->Enable(enable);
	m_btToggleReceivedSpeedAxis->Enable(enable);
	
	m_btClear->Enable(enable);
	m_btSave->Enable(enable);
	
	m_intervalSlider->Enable(enable);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::setupSizes() {
////////////////////////////////////////////////////////////////
	wxSize size;
	size.SetWidth(MAX_VALUES);
	size.SetHeight(m_scrollWindow->GetClientSize().GetHeight());
	
	m_scrollWindow->SetVirtualSize(size);
	m_darwingArea->SetSize(size);
	m_darwingArea->SetMinSize(size);
	
	m_scrollWindow->Scroll(m_scrollWindow->GetScrollRange(wxHORIZONTAL), 0);
	
	drawingArea.height 			= size.GetHeight() - tMargin - bMargin;
	drawingArea.width  			= size.GetWidth()  - lMargin - rMargin;
	drawingArea.width 			= std::min((int)MAX_VALUES, drawingArea.width);
	
	axisMeasuredSpeed.height 	= drawingArea.height;
	axisReceivedSpeed.height 	= drawingArea.height;
	axisConfiguredSpeed.height 	= drawingArea.height;
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onScrolledSize(wxSizeEvent& event) {
////////////////////////////////////////////////////////////////
	event.StopPropagation();
	setupSizes();
	
	m_darwingArea->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onSize(wxSizeEvent& event) {
////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::reset() {
////////////////////////////////////////////////////////////////
	timeIndex = 0;
	m_scrollWindow->Scroll(m_scrollWindow->GetScrollRange(wxHORIZONTAL), 0);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::init(double maxSpeedValue_MM_MIN) {
////////////////////////////////////////////////////////////////
	const double overSize =  maxSpeedValue_MM_MIN * 0.2;
	
	axisMeasuredSpeed.maxValue 		= maxSpeedValue_MM_MIN + overSize;
	axisReceivedSpeed.maxValue		= maxSpeedValue_MM_MIN + overSize;
	axisConfiguredSpeed.maxValue	= maxSpeedValue_MM_MIN + overSize;
	
	wxASSERT(axisMeasuredSpeed.maxValue   > axisMeasuredSpeed.minValue);
	wxASSERT(axisReceivedSpeed.maxValue   > axisMeasuredSpeed.minValue);
	wxASSERT(axisConfiguredSpeed.maxValue > axisConfiguredSpeed.minValue);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::start(double maxSpeedValue_MM_MIN) {
////////////////////////////////////////////////////////////////
	m_refreshTimer->Stop();
	reset();
		
	init(maxSpeedValue_MM_MIN);
	
	lastRefresh = CncTimeFunctions::getMilliTimestamp();
	lastDataSet = CncTimeFunctions::getMilliTimestamp();
	
	m_refreshTimer->Start(m_intervalSlider->GetValue());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::stop() {
////////////////////////////////////////////////////////////////
	if ( m_refreshTimer->IsRunning() == true )
		m_refreshTimer->Stop();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::setCurrentFeedSpeedValue(const SpeedData& sd) {
////////////////////////////////////////////////////////////////
	currentMeasuredFeedSpeed_MM_MIN		= std::max(0.0, sd.measured_MM_MIN);
	currentReceivedFeedSpeed_MM_MIN		= std::max(0.0, sd.received_MM_MIN);
	currentConfiguredFeedSpeed_MM_MIN	= std::max(0.0, sd.configured_MM_MIN);
	
	// log that min one measure point exists
	axisMeasurePoints.values[timeIndex] = axisMeasurePoints.maxValue;
	
	lastDataSet = CncTimeFunctions::getMilliTimestamp();
	
	if ( m_refreshTimer->IsRunning() == false )
		m_refreshTimer->Start(m_intervalSlider->GetValue());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onRefreshTimer(wxTimerEvent& event) {
////////////////////////////////////////////////////////////////
	// switch this timer off if connection switched off
	if ( m_btToggleConnection->GetValue() == false ) {
		stop();
		return;
	}
	
	// create next data snapshot 
	const unsigned int maxNoDataTimespan = 3000; // ms
	if ( CncTimeFunctions::getMilliTimestamp() - lastDataSet < maxNoDataTimespan ) {
	
		if ( (int)timeIndex > drawingArea.width )
			timeIndex = 0;
			
		axisMeasuredSpeed.values[timeIndex] 	= axisMeasuredSpeed.convert(currentMeasuredFeedSpeed_MM_MIN);
		axisReceivedSpeed.values[timeIndex] 	= axisMeasuredSpeed.convert(currentReceivedFeedSpeed_MM_MIN);
		axisConfiguredSpeed.values[timeIndex] 	= axisConfiguredSpeed.convert(currentConfiguredFeedSpeed_MM_MIN);
		
		timeIndex++;
	}
	else {
		// switch this timer off if longer no data received
		stop();
	}
		
	//  update the view
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
		
		// section A
		for ( unsigned int i = pos; i <= (unsigned int)drawingArea.width; i++, x++ ) {
			if ( i == 0 || x == 0 )
				continue;

			if ( m_btToggleConfiguredAxis->GetValue() )		draw(axisConfiguredSpeed, 	x ,i);
			if ( m_btToggleMeasurePointsAxis->GetValue() ) 	draw(axisMeasurePoints,		x ,i);
			if ( m_btToggleReceivedSpeedAxis->GetValue() )	draw(axisReceivedSpeed, 	x ,i);
			if ( m_btToggleMeasuredSpeedAxis->GetValue() )	draw(axisMeasuredSpeed, 	x ,i);
		}
		
		// section B
		for ( unsigned int i = 0; i < pos; i++, x++ ) {
			if ( i == 0 || x == 0 )
				continue;
				
			if ( m_btToggleConfiguredAxis->GetValue() )		draw(axisConfiguredSpeed, 	x ,i);
			if ( m_btToggleMeasurePointsAxis->GetValue() ) 	draw(axisMeasurePoints,		x ,i);
			if ( m_btToggleReceivedSpeedAxis->GetValue() )	draw(axisReceivedSpeed, 	x ,i);
			if ( m_btToggleMeasuredSpeedAxis->GetValue() )	draw(axisMeasuredSpeed, 	x ,i);
		}
	};
	
	// --------------------------------------------------------
	// drawing
	drawBoundbox();
	drawValues();
	drawMouseLabel();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onPaintLeftAxis(wxPaintEvent& event) {
////////////////////////////////////////////////////////////////
	if ( IsShown() == false )
		return;
	
	wxAutoBufferedPaintDC dc(m_leftAxis);
	dc.Clear();
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	// --------------------------------------------------------
	auto drawCurrentValue = [&](Axis& axis, double value) {
		if ( axis.pos != wxLEFT )
			return;
			
		dc.SetTextForeground(axis.pen.GetColour());
		dc.SetFont(valueFont);
		
		const int x = 1;
		const int y = drawingArea.height - axis.convert(value);
		
		dc.DrawLabel(wxString::Format("%4.0lf", value), wxRect(x, y, 100, 100), wxALIGN_LEFT);
	};

	if ( m_refreshTimer->IsRunning() && m_btToggleConnection->GetValue() == true )
		drawCurrentValue(axisConfiguredSpeed, currentConfiguredFeedSpeed_MM_MIN); 
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onPaintRightAxis(wxPaintEvent& event) {
////////////////////////////////////////////////////////////////
	if ( IsShown() == false )
		return;
	
	wxAutoBufferedPaintDC dc(m_rightAxis);
	dc.Clear();
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	// --------------------------------------------------------
	auto drawCurrentValue = [&](Axis& axis, double value) {
		if ( axis.pos != wxRIGHT )
			return;
			
		dc.SetTextForeground(axis.pen.GetColour());
		dc.SetFont(valueFont);
		
		const int x = 1;
		const int y = drawingArea.height - axis.convert(value);
		
		dc.DrawLabel(wxString::Format("%4.0lf", value), wxRect(x, y, 100, 100), wxALIGN_LEFT);
	};
	
	if ( m_refreshTimer->IsRunning() && m_btToggleConnection->GetValue() == true )
		drawCurrentValue(axisMeasuredSpeed, currentMeasuredFeedSpeed_MM_MIN);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onMouseMotion(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	if ( mouseLabel.x < 0 )
		return;
		
	mouseLabel = {event.GetX(), event.GetY()};
	
	if ( m_refreshTimer->IsRunning() == false )
		m_darwingArea->Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onLeftDown(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	mouseLabel = {event.GetX(), event.GetY()};
	m_darwingArea->Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onLeftUp(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	mouseLabel = {-1, -1};
	m_darwingArea->Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleConfiguredAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	m_darwingArea->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleMeasurePointsAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	m_darwingArea->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleMeasuredSpeedAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	m_darwingArea->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleReceivedSpeedAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	m_darwingArea->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::decorateConnectBtn() {
////////////////////////////////////////////////////////////////
	wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	m_btToggleConnection->GetValue() == true ? m_btToggleConnection->SetBitmap(bmpOn) 
	                                         : m_btToggleConnection->SetBitmap(bmpOff);
	m_btToggleConnection->Refresh();
	m_btToggleConnection->Update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::enableConnection(bool state) {
////////////////////////////////////////////////////////////////
	m_btToggleConnection->SetValue(state);
	decorateConnectBtn();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleConnection() {
////////////////////////////////////////////////////////////////
	enableConnection(!m_btToggleConnection->GetValue());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleConnection(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	decorateConnectBtn();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::changeIntervalSlider(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////
	const int value = m_intervalSlider->GetValue();
	
	m_intervalSlider->SetToolTip(wxString::Format("%d ms", value));
	m_refreshTimer->Start(value);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onSave(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	save();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::save() {
////////////////////////////////////////////////////////////////
	const unsigned int w = m_darwingArea->GetClientSize().GetWidth();
	const unsigned int h = m_darwingArea->GetClientSize().GetHeight();

	wxClientDC cltDC(m_darwingArea);
	wxMemoryDC memDC;
	wxBitmap bitmap(w, h);
	
	memDC.SelectObject(bitmap);
	memDC.Blit( 0, 
				0, 
				w, 
				h,
				&cltDC, 
				0, 
				0 
			);
	
	memDC.SelectObject(wxNullBitmap);
	
	wxString filePath(CncFileNameService::getTempDir());
	wxString fileName(wxString::Format("\\SpeedLog-%s.bmp", wxDateTime::UNow().Format("%d-%m-%y.%H-%M-%S")));
	
	filePath.RemoveLast();
	std::cout << filePath << std::endl;
	
	wxFileDialog saveFileDialog(this, 
	                            "Save speed log as image", 
								filePath, 
								fileName,
								"BMP Files (*.bmp)|*.bmp",  
								wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
								
	if ( saveFileDialog.ShowModal() == wxID_CANCEL ) 
		return;
		
	std::cout << saveFileDialog.GetPath() << std::endl;
	
	bitmap.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_BMP);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onClear(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	clear();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::clear() {
////////////////////////////////////////////////////////////////
	bool timerState = m_refreshTimer->IsRunning();
	if ( timerState == true )
		m_refreshTimer->Stop();
		
	axisMeasurePoints.clear();
	axisMeasuredSpeed.clear();
	axisConfiguredSpeed.clear();
	
	reset();
		
	if ( timerState == true )
		m_refreshTimer->Start(m_intervalSlider->GetValue());

	m_darwingArea->Refresh();
}
