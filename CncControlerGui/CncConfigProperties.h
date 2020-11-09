#ifndef CNC_CONFIG_PROPERTY
#define CNC_CONFIG_PROPERTY

#include <iostream>
#include <wx/textctrl.h>
#include <wx/numformatter.h>
#include <wx/propgrid/props.h>
#include <wx/propgrid/editors.h>
#include <wx/propgrid/propgridiface.h>
#include "wxCrafterImages.h"

// -----------------------------------------------------------------------
// CncPGStaticTextCtrlEditor
// -----------------------------------------------------------------------
class CncPGStaticTextCtrlEditor : public wxPGTextCtrlEditor {
		wxDECLARE_DYNAMIC_CLASS(CncPGStaticTextCtrlEditor);
		
	public:
		CncPGStaticTextCtrlEditor();
		virtual ~CncPGStaticTextCtrlEditor();
		
		wxString GetName() const wxOVERRIDE;
		
		virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
											  wxPGProperty* property,
											  const wxPoint& pos,
											  const wxSize& size) const wxOVERRIDE;
};

// -----------------------------------------------------------------------
// CncPGSliderCtrlEditor
// -----------------------------------------------------------------------
class CncPGSliderCtrlEditor : public wxPGTextCtrlEditor {
		wxDECLARE_DYNAMIC_CLASS(CncPGSliderCtrlEditor);
		
	public:
		CncPGSliderCtrlEditor();
		virtual ~CncPGSliderCtrlEditor();
		
		wxString GetName() const wxOVERRIDE;
		
		virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
											  wxPGProperty* property,
											  const wxPoint& pos,
											  const wxSize& size) const wxOVERRIDE;
											  
		virtual bool OnEvent(wxPropertyGrid* propgrid, 
							 wxPGProperty* property, 
							 wxWindow* wnd, 
							 wxEvent& event) const wxOVERRIDE;
};

// -----------------------------------------------------------------------
// CncCfgStaticProperty
// -----------------------------------------------------------------------
class CncCfgStaticProperty : public wxStringProperty {
	
	public:
		CncCfgStaticProperty(const wxString &label=wxPG_LABEL, const wxString &name=wxPG_LABEL, const wxString &value=wxEmptyString)
		: wxStringProperty (label, name, value)
		{}
		
		~CncCfgStaticProperty()
		{}
		
		virtual bool DoSetAttribute(const wxString &name, wxVariant &value);
		virtual const wxPGEditor* DoGetEditorClass() const;
		
		static wxColour cellTextColour;
		static wxColour cellBackgroungColour;
};

// -----------------------------------------------------------------------
// CncCfgSliderProperty
// -----------------------------------------------------------------------
class CncCfgSliderProperty : public wxFloatProperty {
	
	private:
		bool			editable;
		wxString		printFormat;
		unsigned int	decimalPaces;
		double 			stepDelta;
		double 			minValue;
		double 			maxValue;
		unsigned int	sliderStepCount;
		
	protected:
		wxString		translateSliderValue(int x);
		wxString		translateValue(double val);
		int 			translatePropertyValue();
		int				getMaxLabelWidth();
		
	public:
		CncCfgSliderProperty(const wxString &label=wxPG_LABEL, const wxString &name=wxPG_LABEL, 
							 double value=0.0, double min=0.0, double max=0.0, unsigned int steps=0)
		: wxFloatProperty 		(label, name, value)
		, editable				(true)
		, printFormat			("%.1lf")
		, decimalPaces			(1)
		, stepDelta				(0.0)
		, minValue				(min)
		, maxValue				(max)
		, sliderStepCount		(steps)
		{
			setRange(minValue, maxValue, sliderStepCount);
		}
		
		~CncCfgSliderProperty()
		{}
		
		virtual bool				DoSetAttribute(const wxString &name, wxVariant &value);
		virtual wxString 			ValueToString(wxVariant & value, int argFlags = 0) const;
		virtual const wxPGEditor*	DoGetEditorClass() const;
		
		void						setDecimalPlaces(unsigned int places, bool showAlwaysSign=false);
		void						setRange(double min, double max, unsigned int steps);
		void						setEditable(bool enable) { editable = enable; }
		
		bool						isEditable()		const { return editable; }
		unsigned int				getStepCount()		const { return sliderStepCount; }
		
		double						getMinValue()		const { return minValue; }
		double						getMaxValue()		const { return maxValue; }
		
		static unsigned int			calcSteps(double min, double max, double size);
		
		static wxColour cellTextColour;
		static wxColour cellBackgroungColour;
		
		friend CncPGSliderCtrlEditor;
};

#endif
