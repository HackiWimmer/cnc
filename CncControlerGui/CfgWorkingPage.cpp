#include "wxcrafter.h"
#include "MainFrame.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

extern wxPropertyGridManager* 	globlSetupGrid;

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedWorkingCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
		
	MainFrame* app = CncConfig::getGlobalCncConfig()->getTheApp();
	wxASSERT( app != NULL );
		
	wxString name(p->GetName());
	
	if        ( name == CncWork_Ctl_REPLY_THRESHOLD ) {
		app->releaseControllerSetupFromConfig();
		
	} else if ( name == CncWork_Wpt_THICKNESS ) {
		CncConfig::getGlobalCncConfig()->currentZDepth = CncConfig::getGlobalCncConfig()->getWorkpieceThickness();
		CncConfig::getGlobalCncConfig()->initZAxisValues();
		
		app->changeWorkpieceThickness();
		
	} else if ( name == CncWork_Wpt_MAX_THICKNESS_CROSS ) {
		CncConfig::getGlobalCncConfig()->initZAxisValues();
		
		app->changeCrossingThickness();
		
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangingWorkingCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	// Currently nothing todo
}
////////////////////////////////////////////////////////////////////////
void CncConfig::pgSelectedWorkingCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	// Currently nothing todo
}
////////////////////////////////////////////////////////////////////////
void CncConfig::pgButtonWorkingCfgPage(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////
	// Currently nothing todo
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupWorkingCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatName;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	//......................
	curCatName.assign("Working Settings");
	page = globlSetupGrid->AddPage(curCatName, ImageLibConfig().Bitmap(_("BMP_WORK_CFG")));
	root = page->Append( new wxPropertyCategory(curCatName));
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatName);
	fps.propertyChanged 		= &CncConfig::pgChangedWorkingCfgPage;
	fps.propertyChanging		= &CncConfig::pgChangingWorkingCfgPage;
	fps.propertySelected		= &CncConfig::pgSelectedWorkingCfgPage;
	fps.propertyCommandButton	= &CncConfig::pgButtonWorkingCfgPage;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* tool = NULL;
		curCatName.assign("Tool");
		tool = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, tool);
		{
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("Router")); 
			pgParameterMgrArr.Add(_("Pen"));
			prop = tool->AppendChild( new wxEnumProperty("Type", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("ComboBox") );
			registerProperty(CncWork_Tool_TYPE, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 20.0);
			prop = tool->AppendChild( new wxFloatProperty("Diameter [mm]", NEXT_PROP_ID, 0.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncWork_Tool_DIAMETER, prop);
		}
		
		//...................
		wxPGProperty* wpt = NULL;
		curCatName.assign("Workpiece");
		wpt = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, wpt);
		{
			//...............
			validator.SetPrecision(3); validator.SetRange(0, 90.0);
			prop = wpt->AppendChild( new wxFloatProperty("Thickness [mm]", NEXT_PROP_ID, 0.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncWork_Wpt_THICKNESS, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 90.0);
			prop = wpt->AppendChild( new wxFloatProperty("Max Thickness per crossing [mm]", NEXT_PROP_ID, 2.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncWork_Wpt_MAX_THICKNESS_CROSS, prop);
		}
		
		//...................
		wxPGProperty* ctl = NULL;
		curCatName.assign("Controller");
		ctl = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, ctl);
		{
			//...............
			validator.SetPrecision(0); validator.SetRange(0.9, 2000.0);
			prop = ctl->AppendChild( new wxFloatProperty("Reply Threshold [steps]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncWork_Ctl_REPLY_THRESHOLD, prop);
		}
	}
}
