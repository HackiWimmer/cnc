#include <iostream>
#include <wx/window.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/propgrid/propgrid.h>
#include "CncConfigProperty.h"

// initialize the global pointer
wxPGEditor* CncTextCtrlEditor::cncTextCtrlEditor = NULL;

//////////////////////////////////////////////////////////////////////////////////////////
void CncTextCtrlEditor::init() {
//////////////////////////////////////////////////////////////////////////////////////////
	CncTextCtrlEditor::cncTextCtrlEditor = wxPropertyGrid::RegisterEditorClass(new CncTextCtrlEditor(), "CncTextEditor");
}
/////////////////////////////////////////////////////////////////////////////////
wxPGWindowList CncTextCtrlEditor::CreateControls(wxPropertyGrid* propGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz ) const {
	wxWindow* wnd = wxPGTextCtrlEditor::CreateControls(propGrid, property, pos, sz).m_primary;
	if ( wnd != NULL ) {
		wnd->SetWindowStyleFlag(wnd->GetWindowStyleFlag() | wxTE_RIGHT);
		wnd->SetBackgroundColour(*wxGREEN);
	}
	return wnd;
}
//////////////////////////////////////////////////////////////////////////////////////////
void CncTextCtrlEditor::DrawValue(wxDC& dc, wxPGProperty* property, const wxRect& rect ) const {
//////////////////////////////////////////////////////////////////////////////////////////
//?????
	if ( !property->IsValueUnspecified() ) {
		wxString drawStr = property->GetDisplayedString();
		dc.DrawText( drawStr, rect.x + wxPG_XBEFORETEXT + 50 , rect.y );
	}
}
/*
//////////////////////////////////////////////////////////////////////////////////////////
void wxPGTextCtrlEditor_OnFocus( wxPGProperty* property, wxTextCtrl* tc ) {
//////////////////////////////////////////////////////////////////////////////////////////
	// Make sure there is correct text (instead of unspecified value
	// indicator or hint text)
	int flags = property->HasFlag(wxPG_PROP_READONLY) ? 0 : wxPG_EDITABLE_VALUE;
	
	wxString correctText = property->GetValueAsString(flags);

	if ( tc->GetValue() != correctText )
	{
		property->GetGrid()->SetupTextCtrlValue(correctText);
		tc->SetValue(correctText);
	}

	tc->SelectAll();
}
*/
//////////////////////////////////////////////////////////////////////////////////////////
void CncTextCtrlEditor::OnFocus( wxPGProperty* property, wxWindow* wnd ) const {
//////////////////////////////////////////////////////////////////////////////////////////
	wxTextCtrl* tc = wxStaticCast(wnd, wxTextCtrl);
	//wxPGTextCtrlEditor_OnFocus(property, tc);
	wxFloatingPointValidator<double> validator(3, NULL, wxNUM_VAL_ZERO_AS_BLANK);
	validator.SetMin(0.0);
	validator.SetMax(42.0);
		
	tc->SetValidator(validator);
	
}



//////////////////////////////////////////////////////////////////////////////////////////
bool CncTextCtrlEditor::OnEvent( wxPropertyGrid* propGrid, wxPGProperty* property, wxWindow* ctrl, wxEvent& event ) const {
//////////////////////////////////////////////////////////////////////////////////////////
    return CncTextCtrlEditor::OnTextCtrlEvent(propGrid, property, ctrl, event);
}



// Provided so that, for example, ComboBox editor can use the same code
// (multiple inheritance would get way too messy).
bool CncTextCtrlEditor::OnTextCtrlEvent( wxPropertyGrid* propGrid,
                                          wxPGProperty* WXUNUSED(property),
                                          wxWindow* ctrl,
                                          wxEvent& event ) 
{
    if ( !ctrl )
        return false;

    if ( event.GetEventType() == wxEVT_TEXT_ENTER )
    {
        if ( propGrid->IsEditorsValueModified() )
        {
            return true;
        }
    }

    else if ( event.GetEventType() == wxEVT_TEXT )
    {
		/*
		wxTextCtrl* tc = wxDynamicCast(ctrl, wxTextCtrl);
		wxFloatingPointValidator<double> validator(3, NULL, wxNUM_VAL_DEFAULT);
		validator.SetMin(0.0);
		validator.SetMax(42.0);
		
		validator.SetWindow(tc);
		
		std::clog << tc->GetValue() << std::endl;
		*/
		//event.Skip(false);
		
	/*
        // Pass this event (with PG id) outside wxPropertyGrid
        // with so that, if necessary, program can tell when user
        // is editing a textctrl.
        wxEvent *evt = event.Clone();
        evt->SetId(propGrid->GetId());
        propGrid->GetEventHandler()->QueueEvent(evt);

        propGrid->EditorsValueWasModified();
		 * 
	*/
    }

    return false;
}



//////////////////////////////////////////////////////////////////////////////////////////
const wxPGEditor* CncConfigProperty::DoGetEditorClass() const {
//////////////////////////////////////////////////////////////////////////////////////////
	// Determines editor used by property.
	// You can replace 'TextCtrl' below with any of these
	// builtin-in property editor identifiers: Choice, ComboBox,
	// TextCtrlAndButton, ChoiceAndButton, CheckBox, SpinCtrl,
	// DatePickerCtrl.
	
	if ( CncTextCtrlEditor::cncTextCtrlEditor != NULL )
		return CncTextCtrlEditor::cncTextCtrlEditor;
	
	return wxPGEditor_TextCtrl;
}
