#include "wxcrafter.h"
#include "MainFrame.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

extern wxPropertyGridManager* 	globlSetupGrid;

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedSvgCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	MainFrame* app = CncConfig::getGlobalCncConfig()->getTheApp();
	wxASSERT( app != NULL );
		
	wxString name(p->GetName());
	
	if        ( name == CncSvg_Emu_COPY_FACTOR ) {
		app->releaseControllerSetupFromConfig();
		
	} else if ( name == CncSvg_Emu_RSLT_WITH_ORIG_PATH ) {
		app->releaseControllerSetupFromConfig();
		
	} else if ( name == CncSvg_Emu_RSLT_ONLY_WITH_FIRST_CROSS ) {
		app->releaseControllerSetupFromConfig();
		
	}
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
			prop = parser->AppendChild( new wxBoolProperty("Reverse Y axis", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Parser_REVERSE_Y_AXIS, prop);
		}
		
		//...................
		wxPGProperty* emu = NULL;
		curCatName.assign("Outbound Emulator");
		emu = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, emu);
		{
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("0.25")); 
			pgParameterMgrArr.Add(_("0.50"));
			pgParameterMgrArr.Add(_("0.75"));
			pgParameterMgrArr.Add(_("1.00"));
			pgParameterMgrArr.Add(_("1.25"));
			pgParameterMgrArr.Add(_("1.50"));
			pgParameterMgrArr.Add(_("1.75"));
			pgParameterMgrArr.Add(_("2.00"));
			pgParameterMgrArr.Add(_("4.00"));
			prop = emu->AppendChild( new wxEnumProperty("Copy factor", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 3));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("ComboBox") );
			registerProperty(CncSvg_Emu_COPY_FACTOR, prop);
			
			//...............
			prop = emu->AppendChild( new wxBoolProperty("Result with original path", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Emu_RSLT_WITH_ORIG_PATH, prop);
			
			//...............
			prop = emu->AppendChild( new wxBoolProperty("Result only with first crossing", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			registerProperty(CncSvg_Emu_RSLT_ONLY_WITH_FIRST_CROSS, prop);
		}
	}
}