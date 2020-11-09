#include <iostream>
#include <wx/window.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/scrolbar.h>
#include <wx/propgrid/propgrid.h>
#include "CncCommon.h"
#include "CncConfigCommon.h"
#include "CncConfigProperties.h"

// -----------------------------------------------------------------------
// CncPGStaticTextCtrlEditor
// -----------------------------------------------------------------------
WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(StaticTextCtrl, CncPGStaticTextCtrlEditor, wxPGTextCtrlEditor)

CncPGStaticTextCtrlEditor::CncPGStaticTextCtrlEditor() 
: wxPGTextCtrlEditor()
//////////////////////////////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////////////////////////////
CncPGStaticTextCtrlEditor::~CncPGStaticTextCtrlEditor() {
//////////////////////////////////////////////////////////////////////////////////////////
	wxPG_EDITOR(StaticTextCtrl) = NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
wxPGWindowList CncPGStaticTextCtrlEditor::CreateControls(wxPropertyGrid* propGrid,
														 wxPGProperty* property,
														 const wxPoint& pos,
														 const wxSize& sz ) const {
//////////////////////////////////////////////////////////////////////////////////////////
	wxPGWindowList wl = wxPGTextCtrlEditor::CreateControls(propGrid, property, pos, sz);
	if ( wl.m_primary )
		wl.m_primary->Enable(false);
	
	if ( wl.m_secondary )
		wl.m_secondary->Enable(false);
	
	return wl;
}

// -----------------------------------------------------------------------
// CncPGStaticTextCtrlEditor
// -----------------------------------------------------------------------
WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(SliderCtrl, CncPGSliderCtrlEditor, wxPGTextCtrlEditor)

CncPGSliderCtrlEditor::CncPGSliderCtrlEditor() 
: wxPGTextCtrlEditor()
//////////////////////////////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////////////////////////////
CncPGSliderCtrlEditor::~CncPGSliderCtrlEditor() {
//////////////////////////////////////////////////////////////////////////////////////////
	wxPG_EDITOR(SliderCtrl) = NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
wxPGWindowList CncPGSliderCtrlEditor::CreateControls(wxPropertyGrid* propGrid,
													 wxPGProperty* property,
													 const wxPoint& pos,
													 const wxSize& sz ) const {
//////////////////////////////////////////////////////////////////////////////////////////
	wxScrollBar* wnd2 = NULL;
	wxSize tcSz(sz);
	
	CncCfgSliderProperty* prop = wxDynamicCast(property, CncCfgSliderProperty);
	if ( prop ) {
		
		wnd2 = new wxScrollBar(propGrid->GetPanel(), wxID_ANY);
		const int labelWidth = wxMax(18, wxRound(sz.x - prop->getMaxLabelWidth()));
		
		tcSz.Set(sz.x - labelWidth, sz.y);
		wnd2->SetSize(pos.x + tcSz.x, pos.y, labelWidth, sz.y);
		
		const int pageSize 	= 10;
		const int range		= prop->getStepCount()+1;
		wnd2->SetScrollbar(prop->translatePropertyValue(), 1, range, pageSize);
	}
	else {
		wxFAIL_MSG("CncPGSliderCtrlEditor editor can be assigned only to a float property");
	}
	
	wxWindow* wnd1 = wxPGTextCtrlEditor::CreateControls(propGrid, property, pos, tcSz).m_primary;
	wnd1->SetValidator(*prop->GetValidator());
	wnd1->Enable(prop->isEditable());
	
	return wxPGWindowList(wnd1, wnd2);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool CncPGSliderCtrlEditor::OnEvent(wxPropertyGrid* propGrid, wxPGProperty* property, wxWindow* wnd, wxEvent& event) const {
//////////////////////////////////////////////////////////////////////////////////////////
	// Control's events are redirected here . . .
	CncCfgSliderProperty* prop = wxDynamicCast(property, CncCfgSliderProperty);
	if ( prop ) {
		wxEventType evtType = event.GetEventType();
		
		if (	evtType == wxEVT_SCROLL_THUMBTRACK
			||	evtType == wxEVT_SCROLL_THUMBRELEASE
			||	evtType == wxEVT_SCROLL_CHANGED
		   ) 
		{
			wxScrollBar* sb = wxDynamicCast(wnd, wxScrollBar);
			const int sbVal = sb ? sb->GetThumbPosition() : 0;
			
			const wxString value(prop->translateSliderValue(sbVal));
			SetControlStringValue(prop, propGrid->GetEditorControl(), value);
			prop->SetValueFromString(value);
			prop->SetModifiedStatus(evtType == wxEVT_SCROLL_CHANGED);
			return true;
		}
	}
	else {
		wxFAIL_MSG("CncPGSliderCtrlEditor editor can be assigned only to a float property");
	}
	
	return wxPGTextCtrlEditor::OnEvent(propGrid, property, wnd, event);
}

// -----------------------------------------------------------------------
// CncCfgStaticProperty
// -----------------------------------------------------------------------
wxColour CncCfgStaticProperty::cellTextColour		= wxColour(*wxBLACK);
wxColour CncCfgStaticProperty::cellBackgroungColour	= wxColour(*wxWHITE);

//////////////////////////////////////////////////////////////////////////////////////////
bool CncCfgStaticProperty::DoSetAttribute(const wxString &name, wxVariant &value) {
//////////////////////////////////////////////////////////////////////////////////////////
	if ( name == Attribute_READONLY ) {
		wxBitmap bmp = ImageLibConfig().Bitmap(_("BMP_LOCKED"));
		SetValueImage(bmp);
		SetTextColour(cellTextColour);
		SetBackgroundColour(cellBackgroungColour);
		Enable(false);
	}
	
	return wxStringProperty::DoSetAttribute(name, value);
}
//////////////////////////////////////////////////////////////////////////////////////////
const wxPGEditor* CncCfgStaticProperty::DoGetEditorClass() const { 
//////////////////////////////////////////////////////////////////////////////////////////
	return wxPropertyGridInterface::GetEditorByName("StaticTextCtrl"); 
}




// -----------------------------------------------------------------------
// CncCfgSliderProperty
// -----------------------------------------------------------------------
wxColour CncCfgSliderProperty::cellTextColour		= wxColour(*wxBLACK);
wxColour CncCfgSliderProperty::cellBackgroungColour	= wxColour(*wxWHITE);

//////////////////////////////////////////////////////////////////////////////////////////
bool CncCfgSliderProperty::DoSetAttribute(const wxString &name, wxVariant &value) {
//////////////////////////////////////////////////////////////////////////////////////////
	if ( name == Attribute_READONLY ) {
		wxBitmap bmp = ImageLibConfig().Bitmap(_("BMP_LOCKED"));
		SetValueImage(bmp);
		SetTextColour(cellTextColour);
		SetBackgroundColour(cellBackgroungColour);
		Enable(false);
	}
	
	return wxFloatProperty::DoSetAttribute(name, value);
}
//////////////////////////////////////////////////////////////////////////////////////////
const wxPGEditor* CncCfgSliderProperty::DoGetEditorClass() const { 
//////////////////////////////////////////////////////////////////////////////////////////
	return wxPropertyGridInterface::GetEditorByName("SliderCtrl"); 
}


//////////////////////////////////////////////////////////////////////////////////////////
unsigned int CncCfgSliderProperty::calcSteps(double min, double max, double size) {
//////////////////////////////////////////////////////////////////////////////////////////
	// check bounds and swap on demand
	if ( min > max ) {
		const double tmp = max;
		max = min;
		min = tmp;
	}
	
	const double distance = max - min;
	
	if ( cnc::dblCompareNull(distance) )
		return 0;
		
	if ( distance < size )
		return 0;
	
	return distance / size;
}
//////////////////////////////////////////////////////////////////////////////////////////
void CncCfgSliderProperty::setRange(double min, double max, unsigned int steps) {
//////////////////////////////////////////////////////////////////////////////////////////
	// check bounds and correct on demand
	sliderStepCount = std::max((unsigned int)1, steps);
	
	// check bounds and swap on demand
	if ( min > max ) {
		const double tmp = max;
		max = min;
		min = tmp;
	}
	
	maxValue = max;
	minValue = min;
	
	const double distance	= maxValue - minValue;
	stepDelta				= distance / sliderStepCount;
	
	wxFloatingPointValidator<double> validator(decimalPaces, NULL, wxNUM_VAL_DEFAULT);
	validator.SetMin(minValue);
	validator.SetMax(maxValue);
	SetValidator(validator);
}
//////////////////////////////////////////////////////////////////////////////////////////
void CncCfgSliderProperty::setDecimalPlaces(unsigned int places, bool showAlwaysSign) {
//////////////////////////////////////////////////////////////////////////////////////////
	decimalPaces = std::min((unsigned int)5, places);
	
	if ( showAlwaysSign )	printFormat	= wxString::Format("%c+.%dlf", '%', decimalPaces);
	else					printFormat	= wxString::Format("%c.%dlf",  '%', decimalPaces);
	
	wxFloatingPointValidator<double> validator(decimalPaces, NULL, wxNUM_VAL_DEFAULT);
	validator.SetMin(minValue);
	validator.SetMax(maxValue);
	SetValidator(validator);
}
//////////////////////////////////////////////////////////////////////////////////////////
int CncCfgSliderProperty::translatePropertyValue() {
//////////////////////////////////////////////////////////////////////////////////////////
	const int ret = ( stepDelta ? (GetValue().GetDouble() - minValue) / stepDelta : 0 );
	//std::cout << CNC_LOG_LOCATION_A(": ") << ret << ", " << GetValue().GetDouble() << std::endl;
	
	return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
wxString CncCfgSliderProperty::translateSliderValue(int x) {
//////////////////////////////////////////////////////////////////////////////////////////
	const double val = minValue + x * stepDelta;
	//std::cout << CNC_LOG_LOCATION_A(": ") << x << ", " << val << std::endl;
	
	return translateValue(val);
}
//////////////////////////////////////////////////////////////////////////////////////////
wxString CncCfgSliderProperty::translateValue(double val) {
//////////////////////////////////////////////////////////////////////////////////////////
	return wxString::Format(printFormat, val);
}
//////////////////////////////////////////////////////////////////////////////////////////
wxString CncCfgSliderProperty::ValueToString(wxVariant& value, int argFlags) const {
//////////////////////////////////////////////////////////////////////////////////////////
	return wxString::Format(printFormat, value.GetDouble());
}
//////////////////////////////////////////////////////////////////////////////////////////
int CncCfgSliderProperty::getMaxLabelWidth() {
//////////////////////////////////////////////////////////////////////////////////////////
	wxClientDC dc(GetGrid());
	dc.SetFont(GetGrid()->GetFont());
	const wxSize s1 = dc.GetTextExtent(translateValue(minValue));
	const wxSize s2 = dc.GetTextExtent(translateValue(maxValue));
	
	return std::max(s1.x, s2.x) + 10;
}

