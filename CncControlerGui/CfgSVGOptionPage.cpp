#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
#include "MainFrameProxy.h"
#include "CncConfigCommon.h"
#include "CncConfigProperties.h"
#include "CncConfig.h"

////////////////////////////////////////////////////////////////////////
namespace Cnc {
	namespace Config {
		extern wxPropertyGridManager* globlSetupGrid;
		
		extern bool compare(const wxPGProperty& prop, const wxString& propId);
	};
};

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedSvgCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	if ( Cnc::Config::compare(*p, CncSvg_Parser_REVERSE_Y_AXIS) ) 
		APP_PROXY::prepareMotionMonitorViewType();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupSvgCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root			= NULL;
	wxString curCatLabel;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	curCatLabel.assign("SVG Properties");
	page = Cnc::Config::globlSetupGrid->AddPage(curCatLabel, ImageLibConfig().Bitmap(_("BMP_SVG_CFG")));
	root = page->Append( new wxPropertyCategory(curCatLabel) );
	registerCategory(curCatLabel, root);

	PGFuncPtrStore fps;
	fps.name.assign(curCatLabel);
	fps.propertyChanged 		= &CncConfig::pgChangedSvgCfgPage;
	fps.propertyChanging		= NULL;
	fps.propertySelected		= NULL;
	fps.propertyCommandButton	= NULL;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* parser = NULL;
		curCatLabel.assign("Parsing behaviour");
		parser = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, parser);
		{
			//...............
			prop = parser->AppendChild( new wxBoolProperty("Convert to right hand coord", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T("Template file reload + new run required"));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Parser_REVERSE_Y_AXIS, prop);
		}
		
		//...................
		wxPGProperty* wpt = NULL;
		curCatLabel.assign("Workpiece");
		wpt = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, wpt);
		{
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 90.0);
			prop = wpt->AppendChild( new wxFloatProperty("Max Thickness per crossing [mm]", NEXT_PROP_ID, 2.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			CncConfig::registerProperty(CncSvg_Parser_MAX_THICKNESS_CROSS, prop);
		}

		//...................
		wxPGProperty* zaxis = NULL;
		curCatLabel.assign("Calculate Z Axis Values (Workpiece Thickness)");
		zaxis = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, zaxis);
		{
			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Max Durations", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "#");
			registerProperty(CncRuntime_Z_MAX_DURATIONS, prop);
			
			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Durations", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "#");
			registerProperty(CncRuntime_Z_CALCULATED_DURATIONS, prop);
			
			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Max Duration Thickness", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
			registerProperty(CncRuntime_Z_MAX_DURATION_THICKNESS, prop);
			
			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Workpiece included", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_Z_WORKPIECE_INCLUDED, prop);

			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Workpiece offset", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
			registerProperty(CncRuntime_Z_WORKPIECE_OFFSET, prop);
			
			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Current Z Distance", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
			registerProperty(CncRuntime_Z_CURRENT_Z_DISTANCE, prop);

			//...............
			prop = zaxis->AppendChild( new CncCfgStaticProperty("Calculated Duration Array", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "<List>");
			registerProperty(CncRuntime_Z_DURATION_THICKNESS, prop);
		}
	}
}