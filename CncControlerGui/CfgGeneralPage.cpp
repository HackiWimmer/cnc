#include "wxcrafter.h"
#include "MainFrame.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

extern wxPropertyGridManager* 	globlSetupGrid;

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedGeneralCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
		
	//std::clog << "CncConfig::pgChangedGeneralCfgPage" << std::endl;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGeneralCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatName;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	//......................
	curCatName.assign("General Settings");
	page = globlSetupGrid->AddPage(curCatName, ImageLibConfig().Bitmap(_("BMP_GENERAL_CFG")));
	root = page->Append( new wxPropertyCategory(curCatName));
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatName);
	fps.propertyChanged 		= &CncConfig::pgChangedGeneralCfgPage;
	fps.propertyChanging		= NULL;
	fps.propertySelected		= NULL;
	fps.propertyCommandButton	= NULL;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* speed = NULL;
		curCatName.assign("Speed");
		speed = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, speed);
		{
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("RAPID")); 
			pgParameterMgrArr.Add(_("WORK"));
			prop = speed->AppendChild( new wxEnumProperty("Default rapid speed mode XY axis", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("ComboBox") );
			registerProperty(CncConfig_DEF_SPEED_MODE_XY, prop);
			
			//...................
			prop = speed->AppendChild( new wxIntProperty("Max speed XY [rpm]", NEXT_PROP_ID, 100));
			prop->Enable(false);
			prop->SetHelpString(_T("Max speed [rpm] of the stepper motors for the x and y axis."));
			registerProperty(CncConfig_MAX_SPEED_XY, prop);
			
			//...............
			validator.SetPrecision(1); validator.SetRange(0.09, 1);
			prop = speed->AppendChild( new wxFloatProperty("Default rapid speed XY Axis [% from max]", NEXT_PROP_ID, 0.9));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_DEF_RAPID_SPEED_XY, prop);
			
			//...............
			validator.SetPrecision(1); validator.SetRange(0.09, 1);
			prop = speed->AppendChild( new wxFloatProperty("Default work speed XY Axis [% from max]", NEXT_PROP_ID, 0.9));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_DEF_WORK_SPEED_XY, prop);
			
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("RAPID")); 
			pgParameterMgrArr.Add(_("WORK"));
			prop = speed->AppendChild( new wxEnumProperty("Default rapid speed mode Z axis", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("ComboBox") );
			registerProperty(CncConfig_DEF_SPEED_MODE_Z, prop);
			
			//...................
			prop = speed->AppendChild( new wxIntProperty("Max speed Z [rpm]", NEXT_PROP_ID, 100));
			prop->Enable(false);
			prop->SetHelpString(_T("Max speed [rpm] of the stepper motor for the z axis."));
			registerProperty(CncConfig_MAX_SPEED_Z, prop);
			
			//...............
			validator.SetPrecision(1); validator.SetRange(0.09, 1);
			prop = speed->AppendChild( new wxFloatProperty("Default rapid speed Z Axis [% from max]", NEXT_PROP_ID, 0.9));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_DEF_RAPID_SPEED_Z, prop);
			
			//...............
			validator.SetPrecision(1); validator.SetRange(0.09, 1);
			prop = speed->AppendChild( new wxFloatProperty("Default work speed Z Axis [% from max]", NEXT_PROP_ID, 0.9));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_DEF_WORK_SPEED_Z, prop);
		}
		//...................
		wxPGProperty* dimensions = NULL;
		curCatName.assign("Dimension");
		dimensions = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, dimensions);
		{
			//...............
			validator.SetPrecision(1); validator.SetRange(1, 400);
			prop = dimensions->AppendChild( new wxFloatProperty("Max dimension X Axis [mm]", NEXT_PROP_ID, 200.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_MAX_DIMENSION_X, prop);
			
			//...............
			validator.SetPrecision(1); validator.SetRange(1, 400);
			prop = dimensions->AppendChild( new wxFloatProperty("Max dimension Y Axis [mm]", NEXT_PROP_ID, 200.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_MAX_DIMENSION_Y, prop);
			
			//...............
			validator.SetPrecision(1); validator.SetRange(1, 100);
			prop = dimensions->AppendChild( new wxFloatProperty("Max dimension Z Axis [mm]", NEXT_PROP_ID, 100.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_DIMENSION_Z, prop);
		}
		//...................
		wxPGProperty* axis = NULL;
		curCatName.assign("Axis");
		axis = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, axis);
		{
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 10000);
			prop = axis->AppendChild( new wxIntProperty("Steps X Axis [/rotation]", NEXT_PROP_ID, 200));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_X, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 10000);
			prop = axis->AppendChild( new wxIntProperty("Steps Y Axis [/rotation]", NEXT_PROP_ID, 200));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_Y, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 10000);
			prop = axis->AppendChild( new wxIntProperty("Steps Z Axis [/rotation]", NEXT_PROP_ID, 200));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_Z, prop);
		
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
			prop = axis->AppendChild( new wxFloatProperty("Pitch X Axis [mm/rotation]", NEXT_PROP_ID, 1.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PITCH_X, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
			prop = axis->AppendChild( new wxFloatProperty("Pitch Y Axis [mm/rotation]", NEXT_PROP_ID, 1.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PITCH_Y, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
			prop = axis->AppendChild( new wxFloatProperty("Pitch Z Axis [mm/rotation]", NEXT_PROP_ID, 1.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PITCH_Z, prop);
		
			//...............
			validator.SetPrecision(0); validator.SetRange(50, 5000);
			prop = axis->AppendChild( new wxIntProperty("PulsWidthOffset X Axis [ms]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_OFFSET_Y, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(50, 5000);
			prop = axis->AppendChild( new wxIntProperty("PulsWidthOffset Y Axis [ms]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_OFFSET_X, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(50, 5000);
			prop = axis->AppendChild( new wxIntProperty("PulsWidthOffset Z Axis [ms]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_OFFSET_Z, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 32);
			prop = axis->AppendChild( new wxIntProperty("Step multiplier X Axis [ms]", NEXT_PROP_ID, 1));
			prop->Enable(false);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_MULTIPLIER_X, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 32);
			prop = axis->AppendChild( new wxIntProperty("Step multiplier Y Axis [ms]", NEXT_PROP_ID, 1));
			prop->Enable(false);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_MULTIPLIER_Y, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 32);
			prop = axis->AppendChild( new wxIntProperty("Step multiplier Z Axis [ms]", NEXT_PROP_ID, 1));
			prop->Enable(false);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_MULTIPLIER_Z, prop);
		}
	}
}