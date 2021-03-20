#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
#include "MainFrameProxy.h"
#include "CncConfigProperties.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

////////////////////////////////////////////////////////////////////////
namespace Cnc {
	namespace Config {
		extern wxPropertyGridManager* globlSetupGrid;
		
		extern bool compare(const wxPGProperty& prop, const wxString& propId);
	};
};

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedGeneralCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	if (    Cnc::Config::compare(*p, CncConfig_STEPS_X)
	     || Cnc::Config::compare(*p, CncConfig_STEPS_Y)
	     || Cnc::Config::compare(*p, CncConfig_STEPS_Z)
	     || Cnc::Config::compare(*p, CncConfig_STEPS_H)
	
	     || Cnc::Config::compare(*p, CncConfig_PITCH_X)
	     || Cnc::Config::compare(*p, CncConfig_PITCH_Y)
	     || Cnc::Config::compare(*p, CncConfig_PITCH_Z)
	     || Cnc::Config::compare(*p, CncConfig_PITCH_H)
	
	     || Cnc::Config::compare(*p, CncConfig_MAX_SPEED_XYZ_MM_MIN) 
	
	     || Cnc::Config::compare(*p, CncConfig_PULSE_WIDTH_HIGH_X)
	     || Cnc::Config::compare(*p, CncConfig_PULSE_WIDTH_HIGH_Y)
	     || Cnc::Config::compare(*p, CncConfig_PULSE_WIDTH_HIGH_Z)
	     || Cnc::Config::compare(*p, CncConfig_PULSE_WIDTH_HIGH_H)
	   )
	{
		THE_CONFIG->calculateSpeedValues();
	}
	else if ( Cnc::Config::compare(*p, CncConfig_MAX_SPEED_XYZ_MM_MIN) ) {
		
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGeneralCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatLabel;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	//......................
	curCatLabel.assign("General Settings");
	page = Cnc::Config::globlSetupGrid->AddPage(curCatLabel, ImageLibConfig().Bitmap(_("BMP_GENERAL_CFG")));
	root = page->Append( new wxPropertyCategory(curCatLabel) );
	registerCategory(curCatLabel, root);

	PGFuncPtrStore fps;
	fps.name.assign(curCatLabel);
	fps.propertyChanged 		= &CncConfig::pgChangedGeneralCfgPage;
	fps.propertyChanging		= NULL;
	fps.propertySelected		= NULL;
	fps.propertyCommandButton	= NULL;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* speed = NULL;
		curCatLabel.assign("Acceleration and Speed");
		speed = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, speed);
		{
			//...................
			wxPGProperty* defaults = NULL;
			curCatLabel.assign("Defaults");
			defaults = speed->AppendChild( new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, defaults);
			{
				const double maxSpeed = 24000.0;
				
				//...................
				prop = defaults->AppendChild(new CncCfgSliderProperty("Max speed X, Y and Z axis", NEXT_PROP_ID, 2000.0, 1000.0, maxSpeed, 2300));
				prop->SetHelpString(_T(""));
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/min");
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(0);
				registerProperty(CncConfig_MAX_SPEED_XYZ_MM_MIN, prop);
				
				//...................
				prop = defaults->AppendChild(new CncCfgSliderProperty("Default Rapid speed X, Y and Z axis", NEXT_PROP_ID, 2000.0, 10.0, maxSpeed, CncCfgSliderProperty::calcSteps(10.0, maxSpeed, 10)));
				prop->SetHelpString(_T(""));
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/min");
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(0);
				registerProperty(CncConfig_DEF_RAPID_SPEED_MM_MIN, prop);
				
				//...................
				prop = defaults->AppendChild(new CncCfgSliderProperty("Default Work speed X, Y and Z axis", NEXT_PROP_ID, 2000.0, 10.0, maxSpeed, CncCfgSliderProperty::calcSteps(10.0, maxSpeed, 10)));
				prop->SetHelpString(_T(""));
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/min");
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(0);
				registerProperty(CncConfig_DEF_WORK_SPEED_MM_MIN, prop);
			}
			
			//...................
			wxPGProperty* accel = NULL;
			curCatLabel.assign("Acceleration Function: F(I) = A * I^2 + B * I + C");
			accel = speed->AppendChild(new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, accel);
			{
				//...................
				prop = accel->AppendChild(new CncCfgSliderProperty("A", NEXT_PROP_ID, 0.0, 0.0, 5.0, 200));
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(3);
				registerProperty(CncConfig_ACCEL_FUNC_PARA_A, prop);
				
				//...................
				prop = accel->AppendChild(new CncCfgSliderProperty("B", NEXT_PROP_ID, 0.5, 0.0, 5.0, 200));
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(3);
				registerProperty(CncConfig_ACCEL_FUNC_PARA_B, prop);
				
				//...................
				prop = accel->AppendChild(new CncCfgSliderProperty("C", NEXT_PROP_ID, 333.0, 0.0, 1000.0, 1000));
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/min");
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(1);
				registerProperty(CncConfig_ACCEL_FUNC_PARA_C, prop);
			}
			
			//...................
			wxPGProperty* deaccel = NULL;
			curCatLabel.assign("Deacceleration Function: F(I) = A * I^2 + B * I + C");
			deaccel = speed->AppendChild(new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, deaccel);
			{
				//...................
				prop = deaccel->AppendChild(new CncCfgSliderProperty("A", NEXT_PROP_ID, 0.0, 0.0, 5.0, 200));
				prop->SetValidator(validator);
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(3);
				registerProperty(CncConfig_DEACCEL_FUNC_PARA_A, prop);
				
				//...................
				prop = deaccel->AppendChild(new CncCfgSliderProperty("B", NEXT_PROP_ID, 0.5, 0.0, 5.0, 200));
				prop->SetValidator(validator);
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(3);
				registerProperty(CncConfig_DEACCEL_FUNC_PARA_B, prop);
				
				//...................
				prop = deaccel->AppendChild(new CncCfgSliderProperty("C", NEXT_PROP_ID, 333.0, 0.0, 1000.0, 1000));
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/min");
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(1);
				registerProperty(CncConfig_DEACCEL_FUNC_PARA_C, prop);
			}
		}
		collapse(speed);
		
		//...................
		wxPGProperty* hardware = NULL;
		curCatLabel.assign("Hardware");
		hardware = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, hardware);
		{
			const wxString hsDimension	("Maximum total dimension");
			const wxString hsStepCount	("Step count for one rotation");
			const wxString hsPitch		("Feed width of one rotation");
			const wxString hsPulseWidth	("Pulse width = logical high duration of one step");
			
			//...................
			wxPGProperty* xAxis = NULL;
			curCatLabel.assign("X-Axis");
			xAxis = hardware->AppendChild( new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, xAxis);
			{
				//...............
				validator.SetPrecision(1); validator.SetRange(1, 1000);
				prop = xAxis->AppendChild( new wxFloatProperty("Max dimension", NEXT_PROP_ID, 200.0));
				prop->SetHelpString(hsDimension);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
				registerProperty(CncConfig_MAX_DIMENSION_X, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(1, 10000);
				prop = xAxis->AppendChild( new wxIntProperty("Steps", NEXT_PROP_ID, 200));
				prop->SetHelpString(hsStepCount);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "1/360'");
				registerProperty(CncConfig_STEPS_X, prop);
				
				//...............
				validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
				prop = xAxis->AppendChild( new wxFloatProperty("Pitch", NEXT_PROP_ID, 1.0));
				prop->SetHelpString(hsPitch);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/360'");
				registerProperty(CncConfig_PITCH_X, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(0, 1000);
				prop = xAxis->AppendChild( new wxIntProperty("High Pulse Width", NEXT_PROP_ID, 100));
				prop->SetHelpString(hsPulseWidth);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "us");
				registerProperty(CncConfig_PULSE_WIDTH_HIGH_X, prop);
			}  
			
			//...................
			wxPGProperty* yAxis = NULL;
			curCatLabel.assign("Y-Axis");
			yAxis = hardware->AppendChild( new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, yAxis);
			{
				//...............
				validator.SetPrecision(1); validator.SetRange(1, 1000);
				prop = yAxis->AppendChild( new wxFloatProperty("Max dimension", NEXT_PROP_ID, 200.0));
				prop->Enable(true);
				prop->SetHelpString(hsDimension);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
				registerProperty(CncConfig_MAX_DIMENSION_Y, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(1, 10000);
				prop = yAxis->AppendChild( new wxIntProperty("Steps", NEXT_PROP_ID, 200));
				prop->SetHelpString(hsStepCount);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "1/360'");
				registerProperty(CncConfig_STEPS_Y, prop);
				
				//...............
				validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
				prop = yAxis->AppendChild( new wxFloatProperty("Pitch", NEXT_PROP_ID, 1.0));
				prop->SetHelpString(hsPitch);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/360'");
				registerProperty(CncConfig_PITCH_Y, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(0, 1000);
				prop = yAxis->AppendChild( new wxIntProperty("High Pulse Width", NEXT_PROP_ID, 100));
				prop->SetHelpString(hsPulseWidth);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "us");
				registerProperty(CncConfig_PULSE_WIDTH_HIGH_Y, prop);
			}
			
			//...................
			wxPGProperty* zAxis = NULL;
			curCatLabel.assign("Z-Axis");
			zAxis = hardware->AppendChild( new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, zAxis);
			{
				//...............
				validator.SetPrecision(1); validator.SetRange(1, 200);
				prop = zAxis->AppendChild( new wxFloatProperty("Max dimension", NEXT_PROP_ID, 100.0));
				prop->Enable(true);
				prop->SetHelpString(hsDimension);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetValidator(validator);
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
				registerProperty(CncConfig_MAX_DIMENSION_Z, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(1, 10000);
				prop = zAxis->AppendChild( new wxIntProperty("Steps", NEXT_PROP_ID, 200));
				prop->SetHelpString(hsStepCount);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "1/360'");
				registerProperty(CncConfig_STEPS_Z, prop);
				
				//...............
				validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
				prop = zAxis->AppendChild( new wxFloatProperty("Pitch", NEXT_PROP_ID, 1.0));
				prop->SetHelpString(hsPitch);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/360'");
				registerProperty(CncConfig_PITCH_Z, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(0, 1000);
				prop = zAxis->AppendChild( new wxIntProperty("High Pulse Width", NEXT_PROP_ID, 100));
				prop->SetHelpString(hsPulseWidth);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "us");
				registerProperty(CncConfig_PULSE_WIDTH_HIGH_Z, prop);
				
			}
			//...................
			wxPGProperty* hAxis = NULL;
			curCatLabel.assign("H-Axis - Podest");
			hAxis = hardware->AppendChild( new wxPropertyCategory(curCatLabel));
			registerCategory(curCatLabel, hAxis);
			{
				//...............
				validator.SetPrecision(1); validator.SetRange(1, 600);
				prop = hAxis->AppendChild( new wxFloatProperty("Max dimension", NEXT_PROP_ID, 100.0));
				prop->Enable(true);
				prop->SetHelpString(hsDimension);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetValidator(validator);
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
				registerProperty(CncConfig_MAX_DIMENSION_H, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(1, 10000);
				prop = hAxis->AppendChild( new wxIntProperty("Steps", NEXT_PROP_ID, 200));
				prop->SetHelpString(hsStepCount);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "1/360'");
				registerProperty(CncConfig_STEPS_H, prop);
				
				//...............
				validator.SetPrecision(3); validator.SetRange(0.1, 50.0);
				prop = hAxis->AppendChild( new wxFloatProperty("Pitch", NEXT_PROP_ID, 1.0));
				prop->SetHelpString(hsPitch);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm/360'");
				registerProperty(CncConfig_PITCH_H, prop);
				
				//...............
				validator.SetPrecision(0); validator.SetRange(0, 1000);
				prop = hAxis->AppendChild( new wxIntProperty("High Pulse Width", NEXT_PROP_ID, 100));
				prop->SetHelpString(hsPulseWidth);
				prop->SetValidator(validator);
				prop->SetEditor( wxT("TextCtrl") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "us");
				registerProperty(CncConfig_PULSE_WIDTH_HIGH_H, prop);
			}
		}
		collapse(hardware);
	}
}