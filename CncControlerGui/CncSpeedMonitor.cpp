#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include "wxCrafterImages.h"
#include "CncCommon.h"
#include "CncContext.h"
#include "CncFileNameService.h"
#include "CncSpeedSlider.h"
#include "CncSpeedMonitor.h"

////////////////////////////////////////////////////////////////
CncSpeedMonitor::CncSpeedMonitor(wxWindow* parent)
: CncSpeedMonitorBase		(parent)
, diagram					()
////////////////////////////////////////////////////////////////
{
	wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	m_btToggleConnectionH->SetBitmapDisabled(bmpOff.ConvertToDisabled());
	m_btToggleConnectionV->SetBitmapDisabled(bmpOff.ConvertToDisabled());
	
	// This has to be done to use wxAutoBufferedPaintDC 
	// on EVT_PAINT events correctly
	m_leftAxisH		->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_darwingAreaH	->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_rightAxisH	->SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	m_topAxisV		->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_darwingAreaV	->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_bottomAxisV	->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_leftAxisH		->SetBackgroundColour(*wxBLACK);
	m_darwingAreaH	->SetBackgroundColour(*wxBLACK);
	m_rightAxisH	->SetBackgroundColour(*wxBLACK);
	
	m_topAxisV		->SetBackgroundColour(*wxBLACK);
	m_darwingAreaV	->SetBackgroundColour(*wxBLACK);
	m_bottomAxisV	->SetBackgroundColour(*wxBLACK);
	
	init();
}
////////////////////////////////////////////////////////////////
CncSpeedMonitor::~CncSpeedMonitor() {
////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::activate(bool enable) {
////////////////////////////////////////////////////////////////
	enableConnection(enable);
	
	m_btToggleConnectionV->Enable(enable);
	m_btToggleConnectionH->Enable(enable);
	
	m_btToggleConfiguredAxisH->Enable(enable);
	m_btToggleConfiguredAxisV->Enable(enable);
	m_btToggleMeasurePointsAxisH->Enable(enable);
	m_btToggleMeasurePointsAxisV->Enable(enable);
	m_btToggleReceivedSpeedAxisH->Enable(enable);
	m_btToggleReceivedSpeedAxisV->Enable(enable);
	
	m_btClearH->Enable(enable);
	m_btClearV->Enable(enable);
	m_btSaveH->Enable(enable);
	m_btSaveV->Enable(enable);
	
	m_sliderRecordResolutionH->Enable(enable);
	m_sliderRecordResolutionV->Enable(enable);
	
	m_scrollBarH->Enable(enable);
	m_scrollBarV->Enable(enable);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::reset() {
////////////////////////////////////////////////////////////////
	clear();
	
	const wxRect rect = GetClientRect();
	m_scrollBarH->SetScrollbar(diagram.points.getSize(), rect.GetWidth(),  diagram.points.getSize(), rect.GetWidth(),  true);
	m_scrollBarV->SetScrollbar(diagram.points.getSize(), rect.GetHeight(), diagram.points.getSize(), rect.GetHeight(), true);
	
	determineTimeOffset();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::start() {
////////////////////////////////////////////////////////////////
	reset();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::stop() {
////////////////////////////////////////////////////////////////
}






////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onSave(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	save();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::save() {
////////////////////////////////////////////////////////////////
	const unsigned int w = m_darwingAreaH->GetClientSize().GetWidth();
	const unsigned int h = m_darwingAreaH->GetClientSize().GetHeight();

	wxClientDC cltDC(m_darwingAreaH);
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
	
	wxString filePath(CncFileNameService::getTempDirSession());
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
void CncSpeedMonitor::onMouseMotion(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	if ( diagram.mouseLabel.x < 0 )
		return;
	
	diagram.mouseLabel = {event.GetX(), event.GetY()};
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	da->Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onLeftDown(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	diagram.mouseLabel = {event.GetX(), event.GetY()};
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	da->Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onLeftUp(wxMouseEvent& event) {
////////////////////////////////////////////////////////////////
	diagram.mouseLabel = {-1, -1};
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	da->Refresh();
	
	event.Skip();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onPaint(wxPaintEvent& event) {
////////////////////////////////////////////////////////////////
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	if ( da->IsShownOnScreen() == false )
		return;
	
	wxAutoBufferedPaintDC dc(da);
	dc.Clear();
	diagram.plotMain(dc, da->GetRect());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onPaintLeftAxis(wxPaintEvent& event) {
////////////////////////////////////////////////////////////////
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_leftAxisH : m_topAxisV );
	if ( da->IsShownOnScreen() == false )
		return;
	
	wxAutoBufferedPaintDC dc(da);
	dc.Clear();
	diagram.plotBtLf(dc, da->GetRect());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onPaintRightAxis(wxPaintEvent& event) {
////////////////////////////////////////////////////////////////
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_rightAxisH : m_bottomAxisV );
	if ( da->IsShownOnScreen() == false )
		return;
	
	wxAutoBufferedPaintDC dc(da);
	dc.Clear();
	diagram.plotToRt(dc, da->GetRect());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onSize(wxSizeEvent& event) {
////////////////////////////////////////////////////////////////
	event.Skip();
	const wxRect rect = m_darwingAreaH->IsShownOnScreen() ? m_darwingAreaH->GetRect() : m_darwingAreaV->GetRect();
	
	// select orientation
	if ( rect.GetWidth() >= rect.GetHeight() )	diagram.orientation = Diagram::DOHorizontal;
	else										diagram.orientation	= Diagram::DOVertical;
	m_drawingAreaBook->SetSelection(diagram.orientation);
	
	// scrollbar handling
	m_scrollBarH->SetScrollbar(diagram.points.getSize(), rect.GetWidth(),  diagram.points.getSize(), rect.GetWidth(),  true);
	m_scrollBarV->SetScrollbar(diagram.points.getSize(), rect.GetHeight(), diagram.points.getSize(), rect.GetHeight(), true);
	
	// main plot range
	const wxWindow* da  = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	const wxRect daRect = da->GetRect();
	diagram.plotTRange  = ( diagram.orientation == Diagram::DOHorizontal ? daRect.GetWidth()  : daRect.GetHeight() );
	diagram.plotFRange  = ( diagram.orientation == Diagram::DOHorizontal ? daRect.GetHeight() : daRect.GetWidth()  );
	
	restoreTimeOffset();
	update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onToggleMeasurePointsAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	wxBitmapToggleButton* masterButton = horztl ? m_btToggleMeasurePointsAxisH : m_btToggleMeasurePointsAxisV;
	wxBitmapToggleButton* slaveButton  = horztl ? m_btToggleMeasurePointsAxisV : m_btToggleMeasurePointsAxisH;
	slaveButton->SetValue(masterButton->GetValue());
	diagram.showTimePoint = masterButton->GetValue();
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	da->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onToggleConfiguredAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	wxBitmapToggleButton* masterButton = horztl ? m_btToggleConfiguredAxisH : m_btToggleConfiguredAxisV;
	wxBitmapToggleButton* slaveButton  = horztl ? m_btToggleConfiguredAxisV : m_btToggleConfiguredAxisH;
	slaveButton->SetValue(masterButton->GetValue());
	diagram.showCfgSpeed = masterButton->GetValue();
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	da->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onToggleReceivedSpeedAxis(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	wxBitmapToggleButton* masterButton = horztl ? m_btToggleReceivedSpeedAxisH : m_btToggleReceivedSpeedAxisV;
	wxBitmapToggleButton* slaveButton  = horztl ? m_btToggleReceivedSpeedAxisV : m_btToggleReceivedSpeedAxisH;
	slaveButton->SetValue(masterButton->GetValue());
	diagram.showRltSpeed = masterButton->GetValue();
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	da->Refresh();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::decorateConnectBtn() {
////////////////////////////////////////////////////////////////
	const wxBitmap bmpOn  = ImageLib16().Bitmap("BMP_CONNECTED");
	const wxBitmap bmpOff = ImageLib16().Bitmap("BMP_DISCONNECTED");
	
	auto updateCntBtn = [&](wxBitmapToggleButton* b) {
		b->GetValue() == true ? b->SetBitmap(bmpOn) 
	                          : b->SetBitmap(bmpOff);
		b->Refresh();
		b->Update();
	};
	
	updateCntBtn(m_btToggleConnectionH);
	updateCntBtn(m_btToggleConnectionV);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::enableConnection(bool state) {
////////////////////////////////////////////////////////////////
	decorateConnectBtn();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::toggleConnection() {
////////////////////////////////////////////////////////////////
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	wxBitmapToggleButton* masterButton = horztl ? m_btToggleConnectionH : m_btToggleConnectionV;
	wxBitmapToggleButton* slaveButton  = horztl ? m_btToggleConnectionV : m_btToggleConnectionH;
	slaveButton->SetValue(masterButton->GetValue());
	
	enableConnection(!masterButton->GetValue());
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onToggleConnection(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	decorateConnectBtn();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::init() {
////////////////////////////////////////////////////////////////
	typedef Diagram::Resolution  DRes;
	typedef Diagram::Compression DCom;
	
	switch ( diagram.resolution ) {
		case DRes::DS_Sec:				m_sliderRecordResolutionH->SetValue(1);	m_sliderRecordResolutionV->SetValue(1); break;
		case DRes::DS_TenthSec:			m_sliderRecordResolutionH->SetValue(2);	m_sliderRecordResolutionV->SetValue(2); break;
		case DRes::DS_HundredthSec:		m_sliderRecordResolutionH->SetValue(3);	m_sliderRecordResolutionV->SetValue(3); break;
		case DRes::DS_ThousandthSec:	m_sliderRecordResolutionH->SetValue(4);	m_sliderRecordResolutionV->SetValue(4); break;
	}
	m_sliderRecordResolutionH->SetToolTip(wxString::Format("%d ms", (int)(diagram.resolution)));
	m_sliderRecordResolutionV->SetToolTip(wxString::Format("%d ms", (int)(diagram.resolution)));
	
	switch ( diagram.timeCompression ) {
		case DCom::CPV_1:				m_sliderTimeCompressionH->SetValue(1); m_sliderTimeCompressionH->SetValue(1); break;
		case DCom::CPV_2:				m_sliderTimeCompressionH->SetValue(2); m_sliderTimeCompressionH->SetValue(2); break;
		case DCom::CPV_5:				m_sliderTimeCompressionH->SetValue(3); m_sliderTimeCompressionH->SetValue(3); break;
		case DCom::CPV_10:				m_sliderTimeCompressionH->SetValue(4); m_sliderTimeCompressionH->SetValue(4); break;
		case DCom::CPV_20:				m_sliderTimeCompressionH->SetValue(5); m_sliderTimeCompressionH->SetValue(5); break;
		case DCom::CPV_50:				m_sliderTimeCompressionH->SetValue(6); m_sliderTimeCompressionH->SetValue(6); break;
		case DCom::CPV_100:				m_sliderTimeCompressionH->SetValue(7); m_sliderTimeCompressionH->SetValue(7); break;
	}
	m_sliderTimeCompressionH->SetToolTip(wxString::Format("Factor %ld", diagram.timeCompression));
	m_sliderTimeCompressionV->SetToolTip(wxString::Format("Factor %ld", diagram.timeCompression));
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onChangeRecordResolution(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////
	typedef Diagram::Resolution DRes;
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	
	wxSlider* masterSlider = horztl ? m_sliderRecordResolutionH : m_sliderRecordResolutionV;
	wxSlider* slaveSlider  = horztl ? m_sliderRecordResolutionV : m_sliderRecordResolutionH;
	const int value = masterSlider->GetValue();
	switch ( value ) {
		case 1:	diagram.resolution = DRes::DS_Sec;				break;
		case 2:	diagram.resolution = DRes::DS_TenthSec; 		break;
		case 3:	diagram.resolution = DRes::DS_HundredthSec;		break;
		case 4:	diagram.resolution = DRes::DS_ThousandthSec;	break;
	}
	slaveSlider->SetValue(value);
	
	m_sliderRecordResolutionH->SetToolTip(wxString::Format("%d ms", (int)(diagram.resolution)));
	m_sliderRecordResolutionV->SetToolTip(wxString::Format("%d ms", (int)(diagram.resolution)));
	clear();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onChangeDisplayCompression(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////
	typedef Diagram::Compression DCom;
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	
	wxSlider* masterSlider = horztl ? m_sliderTimeCompressionH : m_sliderTimeCompressionV;
	wxSlider* slaveSlider  = horztl ? m_sliderTimeCompressionV : m_sliderTimeCompressionH;
	
	const int value = masterSlider->GetValue();
	switch ( value ) {
		case 1:	diagram.timeCompression = DCom::CPV_1;		break;
		case 2:	diagram.timeCompression = DCom::CPV_2; 		break;
		case 3:	diagram.timeCompression = DCom::CPV_5;		break;
		case 4:	diagram.timeCompression = DCom::CPV_10;		break;
		case 5:	diagram.timeCompression = DCom::CPV_20;		break;
		case 6:	diagram.timeCompression = DCom::CPV_50;		break;
		case 7:	diagram.timeCompression = DCom::CPV_100;	break;
	}
	slaveSlider->SetValue(value);
	
	m_sliderTimeCompressionH->SetToolTip(wxString::Format("Factor %ld", diagram.timeCompression));
	m_sliderTimeCompressionV->SetToolTip(wxString::Format("Factor %ld", diagram.timeCompression));
	
	update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::restoreTimeOffset() {
////////////////////////////////////////////////////////////////
	const wxRect rect = GetClientRect();
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	
	if ( horztl )	m_scrollBarH->SetThumbPosition(m_scrollBarH->GetRange() + rect.GetWidth()  - diagram.timeOffset);
	else			m_scrollBarV->SetThumbPosition(m_scrollBarV->GetRange() + rect.GetHeight() - diagram.timeOffset);
	
	update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::determineTimeOffset() {
////////////////////////////////////////////////////////////////
	const wxRect rect = GetClientRect();
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	
	if ( horztl )	diagram.timeOffset = m_scrollBarH->GetRange() - (m_scrollBarH->GetThumbPosition() + rect.GetWidth());
	else			diagram.timeOffset = m_scrollBarV->GetRange() - (m_scrollBarV->GetThumbPosition() + rect.GetHeight());
	
	diagram.timeOffset /= diagram.timeCompression;
	
	update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onChangeScrollBarH(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////
	determineTimeOffset();
	m_scrollBarH->SetToolTip(wxString::Format("-%ld ms", diagram.timeOffset));
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onChangeScrollBarV(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////
	determineTimeOffset();
	m_scrollBarV->SetToolTip(wxString::Format("-%ld ms", diagram.timeOffset));
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::onClear(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////
	clear();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::clear() {
////////////////////////////////////////////////////////////////
	diagram.clear();
	
	setCurrentFeedSpeedValues(0.0, 0.0);
	update();
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::setCurrentFeedSpeedValues(double cfgF_MM_MIN, double rltF_MM_MIN) {
////////////////////////////////////////////////////////////////
	
	if ( rltF_MM_MIN < 0.0 )
		std::cout << "rltF_MM_MIN = " << rltF_MM_MIN << std::endl;

	if ( cnc::dblCompare(rltF_MM_MIN, 0.0) )
		;//std::cout << "rltF_MM_MIN = " << rltF_MM_MIN << std::endl;
	
	
	
	
	diagram.append(rltF_MM_MIN, cfgF_MM_MIN);
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::update() {
////////////////////////////////////////////////////////////////
	const bool horztl = diagram.orientation == Diagram::DOHorizontal;
	wxBitmapToggleButton* masterButton = horztl ? m_btToggleConnectionH : m_btToggleConnectionV;

	if ( masterButton->GetValue() == false )
		return;
	
	//diagram.appendAgain(10);
	
	wxWindow* da = ( diagram.orientation == Diagram::DOHorizontal ? m_darwingAreaH : m_darwingAreaV );
	wxWindow* dl = ( diagram.orientation == Diagram::DOHorizontal ? m_leftAxisH    : m_topAxisV     );
	wxWindow* dr = ( diagram.orientation == Diagram::DOHorizontal ? m_rightAxisH   : m_bottomAxisV  );
	
	da->Refresh();
	dl->Refresh();
	dr->Refresh();
}
////////////////////////////////////////////////////////////////
int CncSpeedMonitor::Diagram::getFAsPx(double value) {
////////////////////////////////////////////////////////////////
	const bool horztl = (orientation == DOHorizontal);
	
	if ( horztl )
		return plotFRange - offsetAxisF - ( value / getVirtualMaxF() * plotFRange);
	
	return offsetAxisF + value / getVirtualMaxF() * plotFRange;
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::Diagram::plotBtLf(wxAutoBufferedPaintDC& dc, const wxRect& rect) {
////////////////////////////////////////////////////////////////
	if ( showCfgSpeed == false ) 
		return;
		
	if ( points.getCount() == 0 )
		return;
		
	dc.SetPen			(cfgPen);
	dc.SetBrush			(cfgBrush);
	dc.SetTextForeground(cfgPen.GetColour());
	dc.SetFont			(valFont1);
	
	auto last = points.rbegin(); 
	last      = last + timeOffset;
	
	const double v		= last->cfgF_MM_MIN.getAvg();
	const int 	 px		= getFAsPx(v) - ( orientation == DOVertical ? offsetAxisF : 0 );
	
	if ( orientation == DOHorizontal ) {
		dc.DrawRectangle(1, px, rect.GetWidth() - 2 , rect.GetHeight() - px - 1);
		dc.DrawLabel(wxString::Format("%.0lf", v), wxRect(1, px - 12, rect.GetWidth(), 40), wxALIGN_RIGHT | wxALIGN_TOP);
	}
	else {
		dc.DrawRectangle(1, 1, px, rect.GetHeight() - 1);
		dc.DrawLabel(wxString::Format("%.0lf", v), wxRect(px + 2, 4, 40, rect.GetHeight()), wxALIGN_LEFT | wxALIGN_TOP);
	}
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::Diagram::plotToRt(wxAutoBufferedPaintDC& dc, const wxRect& rect) {
////////////////////////////////////////////////////////////////
	if ( showRltSpeed == false ) 
		return;
		
	if ( points.getCount() == 0 )
		return;
		
	dc.SetPen			(rltPen);
	dc.SetBrush			(rltBrush);
	dc.SetTextForeground(rltPen.GetColour());
	dc.SetFont			(valFont1);
	
	auto last = points.rbegin(); 
	last      = last + timeOffset;
	
	const double v		= last->rltF_MM_MIN.getAvg();
	const int 	 px		= getFAsPx(v) - ( orientation == DOVertical ? offsetAxisF : 0 );
	
	if ( orientation == DOHorizontal ) {
		dc.DrawRectangle(1, px, rect.GetWidth() - 2 , rect.GetHeight() - px - 1);
		dc.DrawLabel(wxString::Format("%.0lf", v), wxRect(1, px - 12, rect.GetWidth(), 40), wxALIGN_RIGHT | wxALIGN_TOP);
	}
	else {
		dc.DrawRectangle(1, 1, px , rect.GetHeight() - 1);
		dc.DrawLabel(wxString::Format("%.0lf", v), wxRect(px + 2, 4, 40, rect.GetHeight()), wxALIGN_LEFT | wxALIGN_TOP);
	}
}
////////////////////////////////////////////////////////////////
void CncSpeedMonitor::Diagram::plotMain(wxAutoBufferedPaintDC& dc, const wxRect& rect) {
////////////////////////////////////////////////////////////////
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	
	// plot grid
	dc.SetPen				(grtPen);
	dc.SetTextForeground	(grtCol2);
	dc.SetFont				(valFont1);
	
	if ( orientation == DOHorizontal ) {
		// horizontal lines
		{
			const int dfMinor = plotFRange/10;
			const int dfStart = plotFRange - offsetAxisF;
			
			grtPen.SetColour(grtCol1);
			dc.SetPen(grtPen);
			
			for (int f = dfStart; f > 0; f -= dfMinor )
				dc.DrawLine(0, f, plotTRange, f);
		}
		// vertical lines --> moving dimension
		{
			const int	dtMinor	= 10;
			const int	dtMajor	= dtMinor * 10;
			const int	dtStart = (plotTRange - 1) + (timeOffset % (dtMajor));
			int			vlCnter	= (timeOffset / dtMajor) * dtMajor;
			
			for (int t = dtStart; t > 0; t -= dtMinor ) {
				
				if ( vlCnter % (dtMajor) == 0 ) {
					// time scale lable
					grtPen.SetColour(grtCol2);
					const int vlCounterValue = vlCnter * timeCompression;
					dc.DrawLabel(wxString::Format("%c%d [ms]", (vlCounterValue ? '-' : ' '), vlCounterValue), wxRect(t - 50, 0, 50, 10), wxALIGN_RIGHT);
				}
				else {
					grtPen.SetColour(grtCol1);
				}
				
				dc.SetPen(grtPen);
				dc.DrawLine(t, 10, t, plotFRange - offsetAxisF);
				
				vlCnter += dtMinor;
			}
		}
	}
	else {
		// vertical lines
		{
			const int	dfMinor = plotFRange/10;
			const int	dfStart = offsetAxisF;
			
			grtPen.SetColour(grtCol1);
			dc.SetPen(grtPen);
			
			for (int f = dfStart; f < plotTRange; f += dfMinor )
				dc.DrawLine(f, 0, f, plotTRange);
		}
		// horizontal lines --> moving dimension
		{
			const int	dtMinor = 10;
			const int	dtMajor	= dtMinor * 10;
			const int	dtStart = (plotTRange - 1) + (timeOffset % (dtMajor));
			int			vlCnter	= (timeOffset / dtMajor) * dtMajor;
			
			for (int t = dtStart; t > 0; t -= dtMinor ) {
				
				if ( vlCnter % (dtMajor) == 0 ) {
					// time scale lable
					grtPen.SetColour(grtCol2);
					const int vlCounterValue = vlCnter * timeCompression;
					dc.DrawLabel(wxString::Format("%c%d [ms]", (vlCounterValue ? '-' : ' '), vlCounterValue), wxRect(plotFRange - 50 , t, 50, 10), wxALIGN_RIGHT);
				}
				else {
					grtPen.SetColour(grtCol1);
				}
				
				dc.SetPen(grtPen);
				dc.DrawLine(17, t, plotFRange, t);
				
				vlCnter += dtMinor;
			}
		}
	}
	
	// plot graphs - minimu 2 points required
	if ( points.getCount() > 1 ) {
		
		//----------------------------------------------------------------
		auto getTsAsPx = [&](CncMilliTimestamp tsFirst, auto it) {
			return plotTRange - abs((long)(it->ts - tsFirst)) / timeCompression;
		};
		
		//----------------------------------------------------------------
		auto plotPointHorizontal = [&](CncMilliTimestamp tsFirst, auto itPrev, auto itCurr) {
			
			const int ts1 = getTsAsPx(tsFirst, itPrev);
			const int ts2 = getTsAsPx(tsFirst, itCurr);
			
			if ( showCfgSpeed == true ) {
				const int y1 = getFAsPx(itPrev->cfgF_MM_MIN.getAvg());
				const int y2 = getFAsPx(itCurr->cfgF_MM_MIN.getAvg());
					
				dc.SetPen(cfgPen);
				if ( y1 == y2 ) { dc.DrawLine(ts1, y1, ts2, y2); }
				else			{ dc.DrawLine(ts1, y1, ts2, y1); dc.DrawLine(ts2, y1, ts2, y2); }
			}
			
			if ( showRltSpeed == true ) {
				const int y1 =  getFAsPx(itPrev->rltF_MM_MIN.getAvg());
				const int y2 =  getFAsPx(itCurr->rltF_MM_MIN.getAvg());
				
				dc.SetPen(rltPen);
				dc.DrawLine(ts1, y1, ts2, y2);
			}
			
			if ( showTimePoint == true ) {
				dc.SetPen(pntPen);
				dc.DrawLine(ts2, plotFRange - offsetAxisF, ts2, plotFRange);
			}
		};
		
		//----------------------------------------------------------------
		auto plotPointVertical = [&](CncMilliTimestamp tsFirst, auto itPrev, auto itCurr) {
			
			const int ts1 = getTsAsPx(tsFirst, itPrev);
			const int ts2 = getTsAsPx(tsFirst, itCurr);
			
			if ( showCfgSpeed == true ) {
				const int x1 = getFAsPx(itPrev->cfgF_MM_MIN.getAvg());
				const int x2 = getFAsPx(itCurr->cfgF_MM_MIN.getAvg());
				
				dc.SetPen(cfgPen);
				if ( x1 == x2 )	{ dc.DrawLine(x1, ts1, x2, ts2); } 
				else 			{ dc.DrawLine(x1, ts1, x1, ts2); dc.DrawLine(x1, ts2, x2, ts2); }
			}
			
			if ( showRltSpeed == true ) {
				const int x1 = getFAsPx(itPrev->rltF_MM_MIN.getAvg());
				const int x2 = getFAsPx(itCurr->rltF_MM_MIN.getAvg());
				
				dc.SetPen(rltPen);
				dc.DrawLine(x1, ts1, x2, ts2);
			}
			
			if ( showTimePoint == true ) {
				dc.SetPen(pntPen);
				dc.DrawLine(0, ts2, offsetAxisF, ts2);
			}
		};
		
		// over all relevant points
		auto rend = points.rend();
		auto prev = points.rbegin(); 
		prev = prev + timeOffset;
		
		CncMilliTimestamp tsFirst = prev->ts;
		
		for ( auto it = prev + 1; it.hasMore(rend); ++it ) {
			
			if ( orientation == DOHorizontal )	plotPointHorizontal(tsFirst, prev, it);
			else								plotPointVertical  (tsFirst, prev, it);
			
			// if the last ts was negative (out of the ploting area) stop the ploting
			// a check after the plot has 1 negative step but a closed grah
			const int tsPx = getTsAsPx(tsFirst, it);
			if ( tsPx < 0 )
				break;
				
			prev = it;
		}
	}
	
	// mouse lable
	if ( mouseLabel.x < 0 )
		return;
	
	dc.SetTextForeground	(grtCol2);
	dc.SetFont				(valFont1);
	
	if ( orientation == DOHorizontal ) {
		const int t = (timeOffset + (plotTRange - mouseLabel.x)) * timeCompression;  
		const int f = ( (plotFRange - offsetAxisF - mouseLabel.y) * getVirtualMaxF() ) / ( plotFRange - offsetAxisF);
		
		if ( f > 0 ) {
			const bool		b = ( plotTRange - mouseLabel.x ) > 100;
			const int 		a = ( b ? wxALIGN_LEFT | wxALIGN_TOP                            : wxALIGN_RIGHT | wxALIGN_TOP );
			const wxRect	r = ( b ? wxRect(mouseLabel.x + 5, mouseLabel.y - 10, 100, 100) : wxRect(mouseLabel.x - 100, mouseLabel.y - 10, 100, 100) );
			
			dc.DrawLabel(wxString::Format("%d ms, ~%d mm/min", t, f), r, a);
		}
	}
	else {
		const int t = (timeOffset + (plotTRange - mouseLabel.y)) * timeCompression;  
		const int f = ( ( mouseLabel.x - offsetAxisF ) * getVirtualMaxF() ) / ( plotFRange - offsetAxisF);
		
		if ( f > 0 ) {
			const bool		b = ( plotFRange - mouseLabel.x ) > 100;
			const int 		a = ( b ? wxALIGN_LEFT | wxALIGN_TOP                            : wxALIGN_RIGHT | wxALIGN_TOP );
			const wxRect	r = ( b ? wxRect(mouseLabel.x + 5, mouseLabel.y - 10, 100, 100) : wxRect(mouseLabel.x - 100, mouseLabel.y - 10, 100, 100) );
			
			dc.DrawLabel(wxString::Format("%d ms, ~%d mm/min", t, f), r, a);
		}
	}
}

