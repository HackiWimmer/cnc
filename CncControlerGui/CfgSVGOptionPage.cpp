#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
#include "MainFrameProxy.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

extern wxPropertyGridManager* 	globlSetupGrid;

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedSvgCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	const wxString name(p->GetName());
	if ( name == CncSvg_Parser_REVERSE_Y_AXIS ) 
		APP_PROXY::prepareMotionMonitorViewType();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupSvgCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatName;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	curCatName.assign("SVG Properties");
	page = globlSetupGrid->AddPage(curCatName, ImageLibConfig().Bitmap(_("BMP_SVG_CFG")));
	root = page->Append( new wxPropertyCategory(curCatName) );
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatName);
	fps.propertyChanged 		= &CncConfig::pgChangedSvgCfgPage;
	fps.propertyChanging		= NULL;
	fps.propertySelected		= NULL;
	fps.propertyCommandButton	= NULL;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* wpt = NULL;
		curCatName.assign("Workpiece");
		wpt = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, wpt);
		{
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 90.0);
			prop = wpt->AppendChild( new wxFloatProperty("Max Thickness per crossing [mm]", NEXT_PROP_ID, 2.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			CncConfig::registerProperty(CncWork_Wpt_MAX_THICKNESS_CROSS, prop);
		}
				//...................
		wxPGProperty* parser = NULL;
		curCatName.assign("Parsing behaviour");
		parser = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, parser);
		{
			//...............
			prop = parser->AppendChild( new wxBoolProperty("Convert to right hand coord", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T("Template file reload + new run required"));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Parser_REVERSE_Y_AXIS, prop);
		}
	}
}