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
		
	wxString name(p->GetName());
	
	if (    name == CncConfig_STEPS_X
	     || name == CncConfig_STEPS_Y
	     || name == CncConfig_STEPS_Z

	     || name == CncConfig_PITCH_X
	     || name == CncConfig_PITCH_Y
	     || name == CncConfig_PITCH_Z

	     || name == CncConfig_DEF_RAPID_SPEED_PERCENT
	     || name == CncConfig_DEF_WORK_SPEED_PERCENT

	     || name == CncConfig_PULS_WIDTH_LOW_X
	     || name == CncConfig_PULS_WIDTH_LOW_Y
	     || name == CncConfig_PULS_WIDTH_LOW_Z 
		 
	     || name == CncConfig_PULS_WIDTH_HIGH_X
	     || name == CncConfig_PULS_WIDTH_HIGH_Y
	     || name == CncConfig_PULS_WIDTH_HIGH_Z 
	   ) 
	{
		GBL_CONFIG->calculateSpeedValues();
		if ( THE_APP ) {
			THE_APP->initSpeedConfigPlayground();
		}
	}
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
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Max speed X axis [mm/minute]", NEXT_PROP_ID, 100.0));
			prop->Enable(false);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_SPEED_X_MM_MIN, prop);

			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Max speed Y axis [mm/minute]", NEXT_PROP_ID, 100.0));
			prop->Enable(false);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_SPEED_Y_MM_MIN, prop);

			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Max speed Z axis [mm/minute]", NEXT_PROP_ID, 100.0));
			prop->Enable(false);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_SPEED_Z_MM_MIN, prop);

			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Consolidated Max speed X, Y and Z axis [mm/minute]", NEXT_PROP_ID, 100.0));
			prop->Enable(false);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_SPEED_XYZ_MM_MIN, prop);
			
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			pgParameterMgrArr.Add(_("RAPID")); 
			pgParameterMgrArr.Add(_("WORK"));
			prop = speed->AppendChild( new wxEnumProperty("Default rapid speed mode XYZ axis", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("ComboBox") );
			registerProperty(CncConfig_DEF_SPEED_MODE_XYZ, prop);

			//...................
			validator.SetPrecision(2); validator.SetRange(0.0, 10.0);
			prop = speed->AppendChild( new wxFloatProperty("Default rapid speed X, Y and Z axis [% from max]", NEXT_PROP_ID, 0.8));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_DEF_RAPID_SPEED_PERCENT, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Default rapid speed X, Y and Z axis [mm/minute]", NEXT_PROP_ID, 100.0));
			prop->Enable(false);
			prop->SetValidator(validator);
			registerProperty(CncConfig_DEF_RAPID_SPEED_MM_MIN, prop);

			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 10.0);
			prop = speed->AppendChild( new wxFloatProperty("Default work speed X, Y and Z axis [% from max]", NEXT_PROP_ID, 0.8));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_DEF_WORK_SPEED_PERCENT, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Default work speed X, Y and Z axis [mm/minute]", NEXT_PROP_ID, 100.0));
			prop->Enable(false);
			prop->SetValidator(validator);
			registerProperty(CncConfig_DEF_WORK_SPEED_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Acceleration start speed X axis [mm/minute]", NEXT_PROP_ID, 300.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_ACCEL_START_SPEED_X_MM_MIN, prop);

			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Acceleration stop speed X axis [mm/minute]", NEXT_PROP_ID, 300.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_ACCEL_STOP_SPEED_X_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Acceleration start speed Y axis [mm/minute]", NEXT_PROP_ID, 300.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_ACCEL_START_SPEED_Y_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Acceleration stop speed Y axis [mm/minute]", NEXT_PROP_ID, 300.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_ACCEL_STOP_SPEED_Y_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Acceleration start speed Y axis [mm/minute]", NEXT_PROP_ID, 300.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_ACCEL_START_SPEED_Z_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Acceleration stop speed Z axis [mm/minute]", NEXT_PROP_ID, 300.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_ACCEL_STOP_SPEED_Z_MM_MIN, prop);

			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Max work speed X axis [mm/minute]", NEXT_PROP_ID, 1500.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_WORK_SPEED_X_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Max work speed X axis [mm/minute]", NEXT_PROP_ID, 1500.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_WORK_SPEED_Y_MM_MIN, prop);
			
			//...................
			validator.SetPrecision(1); validator.SetRange(0.0, 5000.0);
			prop = speed->AppendChild( new wxFloatProperty("Max work speed X axis [mm/minute]", NEXT_PROP_ID, 1500.0));
			prop->Enable(true);
			prop->SetValidator(validator);
			registerProperty(CncConfig_MAX_WORK_SPEED_Z_MM_MIN, prop);
		
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
			prop = axis->AppendChild( new wxIntProperty("Steps X Axis [/360]", NEXT_PROP_ID, 200));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_X, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 10000);
			prop = axis->AppendChild( new wxIntProperty("Steps Y Axis [/360]", NEXT_PROP_ID, 200));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_Y, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 10000);
			prop = axis->AppendChild( new wxIntProperty("Steps Z Axis [/360]", NEXT_PROP_ID, 200));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_Z, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(1, 10000);
			prop = axis->AppendChild( new wxIntProperty("Steps XYZ AVG [/360]", NEXT_PROP_ID, 200));
			prop->Enable(false);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_STEPS_XYZ, prop);
		
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
			prop = axis->AppendChild( new wxFloatProperty("Pitch X Axis [mm/360]", NEXT_PROP_ID, 1.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PITCH_X, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
			prop = axis->AppendChild( new wxFloatProperty("Pitch Y Axis [mm/360]", NEXT_PROP_ID, 1.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PITCH_Y, prop);
			
			//...............
			validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
			prop = axis->AppendChild( new wxFloatProperty("Pitch Z Axis [mm/360]", NEXT_PROP_ID, 1.0));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PITCH_Z, prop);
		
			//...............
			validator.SetPrecision(0); validator.SetRange(0, 1000);
			prop = axis->AppendChild( new wxIntProperty("Low Puls Width X Axis [us]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_LOW_X, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(0, 1000);
			prop = axis->AppendChild( new wxIntProperty("High Puls Width X Axis [us]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_HIGH_X, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(0, 1000);
			prop = axis->AppendChild( new wxIntProperty("Low Puls Width Y Axis [us]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_LOW_Y, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(0, 1000);
			prop = axis->AppendChild( new wxIntProperty("High Puls Width Y Axis [us]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_HIGH_Y, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(0, 1000);
			prop = axis->AppendChild( new wxIntProperty("Low Puls Width Z Axis [us]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_LOW_Z, prop);
			
			//...............
			validator.SetPrecision(0); validator.SetRange(0, 1000);
			prop = axis->AppendChild( new wxIntProperty("High Puls Width Z Axis [us]", NEXT_PROP_ID, 100));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetValidator(validator);
			prop->SetEditor( wxT("TextCtrl") );
			registerProperty(CncConfig_PULS_WIDTH_HIGH_Z, prop);
			
		}
	}
}