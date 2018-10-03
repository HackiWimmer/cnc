#ifndef CNC_CONFIG_PROPERTY
#define CNC_CONFIG_PROPERTY

#include <iostream>
#include <wx/textctrl.h>
#include <wx/numformatter.h>
#include <wx/propgrid/editors.h>


//////////////////////////////////////////////////////////////////////////////////////////
class CncTextCtrlEditor : public wxPGTextCtrlEditor {
	
	public:
		
		static wxPGEditor* cncTextCtrlEditor;
		
		
		/////////////////////////////////////////////////////////////////////////////////
		CncTextCtrlEditor() 
		: wxPGTextCtrlEditor() 
		{
		}
		
		/////////////////////////////////////////////////////////////////////////////////
		~CncTextCtrlEditor() {
		
		}
		
		/////////////////////////////////////////////////////////////////////////////////
		static void init();
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual wxPGWindowList CreateControls(wxPropertyGrid* propGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz ) const;
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual void DrawValue(wxDC& dc, wxPGProperty* property, const wxRect& rect) const;
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual void OnFocus( wxPGProperty* property, wxWindow* wnd ) const;
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual bool OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* wnd_primary, wxEvent& event ) const;
		static bool OnTextCtrlEvent( wxPropertyGrid* propGrid, wxPGProperty* WXUNUSED(property), wxWindow* ctrl, wxEvent& event);

};

//////////////////////////////////////////////////////////////////////////////////////////
class CncConfigProperty : public wxFloatProperty {
	
	public:
		/////////////////////////////////////////////////////////////////////////////////
		CncConfigProperty() 
		: wxFloatProperty()
		{
			m_value = "";
		}
		
		/////////////////////////////////////////////////////////////////////////////////
		CncConfigProperty( 	const wxString& label = wxPG_LABEL,
							const wxString& name  = wxPG_LABEL,
							const wxString& value = wxEmptyString )
		: wxFloatProperty(label, name)
		{
			// m_value is wxVariant
			m_value = value;
		}
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual ~CncConfigProperty() { 
		}
		
		/////////////////////////////////////////////////////////////////////////////////
		const wxPGEditor* DoGetEditorClass() const;
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual wxString ValueToString( wxVariant& value, int argFlags ) const {
			// TODO: Convert given property value to a string
			
			return wxNumberFormatter::ToString(value.GetDouble(), 3, argFlags);
			
			
			
			//return wxFloatProperty::ValueToString(value, argFlags);
		}
		
		/////////////////////////////////////////////////////////////////////////////////
		virtual bool StringToValue( wxVariant& variant, const wxString& text, int argFlags ) {
			// TODO: Adapt string to property value.
			return wxFloatProperty::StringToValue(variant, "text", argFlags);
		}
		
	protected:
};

#endif
