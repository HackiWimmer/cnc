#include "wxcrafter.h"
#include "MainFrame.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

#include "CncConfigProperty.h"

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
	
	if ( name == CncWork_Ctl_REPLY_THRESHOLD_METRIC ) {
		CncConfig::getGlobalCncConfig()->calculateThresholds();
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
	wxFloatingPointValidator<double> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
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
		wxPGProperty* wpt = NULL;
		curCatName.assign("Workpiece");
		wpt = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, wpt);
		{

			
			
			//...............
			validator.SetPrecision(6); validator.SetRange(0, 90.0);
			prop = wpt->AppendChild( new CncConfigProperty("Test [mm]", NEXT_PROP_ID, "0.0"));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			
			//prop->SetMaxLength(6);
			
			/*
			wxFloatingPointValidator<float>* xxx = ((wxFloatingPointValidator<float>*)(prop->GetValidator()));
			
			clog << xxx.GetPrecision() << endl;
			
			
			xxx->SetRange(0, 90.0);
			xxx->SetPrecision(3);
			prop->SetValidator(*xxx);
			*/
			
			//prop->SetValidator(validator);
			
			prop->SetEditor( wxT("CncTextEditor") );
			CncConfig::registerProperty(CncTEST_TEST, prop);







			//...............
			validator.SetPrecision(3); validator.SetRange(0, 90.0);
			prop = wpt->AppendChild( new wxFloatProperty("Thickness [mm]", NEXT_PROP_ID, 0.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			CncConfig::registerProperty(CncWork_Wpt_THICKNESS, prop);
			
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
		wxPGProperty* ctl = NULL;
		curCatName.assign("Controller");
		ctl = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, ctl);
		{
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("0.001")); 
			pgParameterMgrArr.Add(_("0.005")); 
			pgParameterMgrArr.Add(_("0.010")); 
			pgParameterMgrArr.Add(_("0.020")); 
			pgParameterMgrArr.Add(_("0.030")); 
			pgParameterMgrArr.Add(_("0.040")); 
			pgParameterMgrArr.Add(_("0.050")); 
			pgParameterMgrArr.Add(_("0.060")); 
			pgParameterMgrArr.Add(_("0.070")); 
			pgParameterMgrArr.Add(_("0.080")); 
			pgParameterMgrArr.Add(_("0.090")); 
			pgParameterMgrArr.Add(_("0.100")); 
			pgParameterMgrArr.Add(_("0.200"));
			pgParameterMgrArr.Add(_("0.300"));
			pgParameterMgrArr.Add(_("0.400"));
			pgParameterMgrArr.Add(_("0.500"));
			pgParameterMgrArr.Add(_("0.600"));
			pgParameterMgrArr.Add(_("0.700"));
			pgParameterMgrArr.Add(_("0.800"));
			pgParameterMgrArr.Add(_("0.900"));
			pgParameterMgrArr.Add(_("1.000"));
			pgParameterMgrArr.Add(_("2.000"));
			pgParameterMgrArr.Add(_("5.000"));

			prop = ctl->AppendChild( new wxEnumProperty("Position - Reply Threshold [mm]", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 15));
			prop->Enable(true);
			prop->SetEditor( wxT("ComboBox") );
			CncConfig::registerProperty(CncWork_Ctl_REPLY_THRESHOLD_METRIC, prop);
			
			prop = ctl->AppendChild( new wxStringProperty(" --> Position - Reply Threshold X [steps]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncWork_Ctl_REPLY_THRESHOLD_SETPS_X, prop);
			
			prop = ctl->AppendChild( new wxStringProperty(" --> Position - Reply Threshold Y [steps]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncWork_Ctl_REPLY_THRESHOLD_SETPS_Y, prop);
			
			prop = ctl->AppendChild( new wxStringProperty(" --> Position - Reply Threshold Z [steps]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncWork_Ctl_REPLY_THRESHOLD_SETPS_Z, prop);
			
			//...............
			prop = ctl->AppendChild( new wxBoolProperty("Avoid sending of duplicate Setter values", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_AVOID_DUP_SETTER_VALUES, prop);

		}
	}
}
