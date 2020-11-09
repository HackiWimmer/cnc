#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
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
void CncConfig::pgChangedGCodeCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGCodeCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatLabel;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	curCatLabel.assign("GCode Properties");
	page = Cnc::Config::globlSetupGrid->AddPage(curCatLabel, ImageLibConfig().Bitmap(_("BMP_GCODE_CFG")));
	root = page->Append( new wxPropertyCategory(curCatLabel));
	registerCategory(curCatLabel, root);

	PGFuncPtrStore fps;
	fps.name.assign(curCatLabel);
	fps.propertyChanged 		= &CncConfig::pgChangedGCodeCfgPage;
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
			prop = parser->AppendChild( new CncCfgStaticProperty("Placeholder", NEXT_PROP_ID, ""));
			prop->SetHelpString("Only a place holder");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncGCode_Parser_PLACEHOLDER, prop);
		}
	}
}
