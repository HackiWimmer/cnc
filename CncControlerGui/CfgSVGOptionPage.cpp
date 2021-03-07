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
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("2D")); 
			pgParameterMgrArr.Add(_("3D"));
			prop = parser->AppendChild( new wxEnumProperty("Default display view", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
			prop->Enable(true);
			prop->SetEditor( wxT("ComboBox") );
			registerProperty(CncSvg_Parser_DEFAULT_VIEW_TYPE, prop);
			
			//...............
			prop = parser->AppendChild( new wxBoolProperty("Convert to right hand coord", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T("new run required"));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Parser_REVERSE_Y_AXIS, prop);
			
			//...............
			prop = parser->AppendChild( new wxBoolProperty("Consider Viewbox", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T("new run required"));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Parser_CONSIDER_VIEWBOX, prop);
			
			//...............
			prop = parser->AppendChild( new wxBoolProperty("Using colour scheme as default", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T("Colour scheme is used to define path handling, like inner- or outer- tool-path-correction, etc.."));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Parser_USE_COLOUR_SCHEME, prop);
		}
		
		//...................
		wxPGProperty* wpt = NULL;
		curCatLabel.assign("Workpiece");
		wpt = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, wpt);
		{
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 90.0);
			prop = wpt->AppendChild( new wxFloatProperty("Default thickness per crossing [mm]", NEXT_PROP_ID, 2.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
			registerProperty(CncSvg_Parser_MAX_THICKNESS_CROSS, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 5.0);
			prop = wpt->AppendChild( new wxFloatProperty("Default surface (workpiece) offset", NEXT_PROP_ID, 1.5));
			prop->Enable(true);
			prop->SetHelpString("");
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
			registerProperty(CncSvg_Parser_SUREFACE_Z_OFFSET, prop);
		}
	}
}