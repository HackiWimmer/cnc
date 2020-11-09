#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
#include "GlobalStrings.h"
#include "CncConfigCommon.h"
#include "CncControl.h"
#include "CncConfig.h"
#include "MainFrameProxy.h"
#include "CncPreprocessor.h"
#include "CncConfigProperties.h"

////////////////////////////////////////////////////////////////////////
extern GlobalConstStringDatabase globalStrings;

namespace Cnc {
	namespace Config {
		extern wxPropertyGridManager*	globlSetupGrid;
		
		extern bool compare(const wxPGProperty& prop, const wxString& propId);
	};
};

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedWorkingCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
		
	if ( Cnc::Config::compare(*p, CncWork_Ctl_REPLY_THRESHOLD_METRIC) ) {
		
		CncConfig::getGlobalCncConfig()->calculateThresholds();
		APP_PROXY::releaseControllerSetupFromConfig();
		
	} 
	else if ( Cnc::Config::compare(*p, CncSvg_Parser_MAX_THICKNESS_CROSS) ) {
		
		CncConfig::getGlobalCncConfig()->initZAxisValues();
		APP_PROXY::changeCrossingThickness();
		
	}
	else if ( Cnc::Config::compare(*p, CncWork_Ctl_PRE_PROSSOR_ANALYSE) ) {
		
		wxPGProperty* p = getProperty(CncWork_Ctl_PRE_PROSSOR_ANALYSE); 
		wxASSERT(p); 
		const bool analyse = p->GetValue().GetBool();
		
		p = getProperty(CncWork_Ctl_PRE_PROSSOR_SKIP_EMPTY); 
		wxASSERT(p); 
		p->Enable(analyse);
		
		p = getProperty(CncWork_Ctl_PRE_PROSSOR_COMBINE_MOVES); 
		wxASSERT(p); 
		p->Enable(analyse);
		
	}
	else if ( Cnc::Config::compare(*p, CncWork_Ctl_PRE_PROSSOR_USE_OPERATING_TRACE) ) {
		
		APP_PROXY::getCncPreProcessor()->enableOperatingTrace(THE_CONFIG->getPreProcessorUseOperatingTrace());
		
	}
	else if ( Cnc::Config::compare(*p, CncWork_Ctl_PRE_PROSSOR_CNT_PATH_LIST_ENTRIES) ) {
		
		APP_PROXY::getCncPreProcessor()->enablePathListEntries(THE_CONFIG->getPreProcessorCntPathListEntries());
		
	}
	else if ( Cnc::Config::compare(*p, CncWork_Ctl_PRE_PROSSOR_CNT_SEQUENCE_MOVES) ) {
		
		APP_PROXY::getCncPreProcessor()->enableMoveSequences(THE_CONFIG->getPreProcessorCntMoveSequneces());
		
	}
	else if ( Cnc::Config::compare(*p, CncWork_Ctl_PRE_PROSSOR_RENDER_RESOLUTION) ) {
		
		THE_CONFIG->setRenderResolution(p->GetValueAsString());
		
	}
	else if ( Cnc::Config::compare(*p, CncWork_Ctl_ARTIFICIALLY_STEP_DELAY) ) {
	
		CncControl* cnc = APP_PROXY::getCncControl();
		if ( cnc )
			cnc->setStepDelay(p->GetValue().GetInteger());
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
	wxPGProperty* root			= NULL;
	wxString curCatLabel;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<double> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	//......................
	curCatLabel.assign("Working Settings");
	page = Cnc::Config::globlSetupGrid->AddPage(curCatLabel, ImageLibConfig().Bitmap(_("BMP_WORK_CFG")));
	root = page->Append( new wxPropertyCategory(curCatLabel, "xxx") );
	registerCategory(curCatLabel, root);
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatLabel);
	fps.propertyChanged 		= &CncConfig::pgChangedWorkingCfgPage;
	fps.propertyChanging		= &CncConfig::pgChangingWorkingCfgPage;
	fps.propertySelected		= &CncConfig::pgSelectedWorkingCfgPage;
	fps.propertyCommandButton	= &CncConfig::pgButtonWorkingCfgPage;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* prep = NULL;
		curCatLabel.assign("Preprocessor");
		prep = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, prep);
		{
			//...............
			pgParameterMgrArr.Clear();
			pgParameterMgrIntArr.Clear();
			
			auto appendList = [&](float resolution) {
				const unsigned int ppi = CncResolutionCalculator::getPointsPerInchForUnit(Unit::mm, resolution);
				pgParameterMgrArr.Add(wxString::Format(globalStrings.renderSelectorFormat, resolution, ppi));
			};
			auto formatValue = [](float resolution) {
				const unsigned int ppi = CncResolutionCalculator::getPointsPerInchForUnit(Unit::mm, resolution);
				return wxString::Format(globalStrings.renderSelectorFormat, resolution, ppi);
			};
			
			appendList(0.005); appendList(0.006); appendList(0.007); appendList(0.008); appendList(0.009); appendList(0.010);
			appendList(0.020); appendList(0.030); appendList(0.040); appendList(0.050); appendList(0.060); appendList(0.070);
			appendList(0.080); appendList(0.090); appendList(0.100); appendList(0.200); appendList(0.262); appendList(0.300);
			appendList(0.400); appendList(0.500); appendList(0.600); appendList(0.700); appendList(0.800); appendList(0.900);
			
			prop = prep->AppendChild(new wxEnumProperty("Render Resolution", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
			prop->SetHelpString("Defines the curve lib render resolution (EllipticalArc, QuadraticBezier and CubicBezier");
			prop->SetEditor( wxT("ComboBox") );
			prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
			prop->SetChoiceSelection(pgParameterMgrArr.Index(formatValue(0.262)));
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_RENDER_RESOLUTION, prop);
			
			//...............
			prop = prep->AppendChild( new wxBoolProperty("Analyse Pathes", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_ANALYSE, prop);
			{
				wxPGProperty* parent = prop;
				//...................
				const double begPitch = 0.1, endPitch = 100.0, sizPitch = CncCfgSliderProperty::calcSteps(begPitch, endPitch, 0.1);
				prop = parent->AppendChild(new CncCfgSliderProperty("Max XY Pitch", NEXT_PROP_ID, 15.0, begPitch, endPitch, sizPitch));
				prop->SetAttribute(wxPG_ATTR_UNITS, "Degree");
				prop->SetHelpString(_T("Max pitch between three path points within the XY plane to keep the path together."));
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(1);
				registerProperty(CncWork_Ctl_PRE_PROSSOR_MAX_XY_PITCH, prop);
				
				//...................
				prop = parent->AppendChild(new CncCfgSliderProperty("Max Z Pitch", NEXT_PROP_ID, 15.0, begPitch, endPitch, sizPitch));
				prop->SetAttribute(wxPG_ATTR_UNITS, "Degree");
				prop->SetHelpString(_T("Max pitch between the XY plane and the Z Axis to keep the path together."));
				wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
				wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(1);
				registerProperty(CncWork_Ctl_PRE_PROSSOR_MAX_Z_PITCH, prop);
			}
			
			//...............
			prop = prep->AppendChild( new wxBoolProperty("Skip empty moves", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_SKIP_EMPTY, prop);
			
			//...............
			prop = prep->AppendChild( new wxBoolProperty("Combine moves", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_COMBINE_MOVES, prop);
			
			//...............
			prop = prep->AppendChild( new wxBoolProperty("Display operating trace information by default", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_USE_OPERATING_TRACE, prop);

			//...............
			prop = prep->AppendChild( new wxBoolProperty("Connect List 'PathListEnties' by default", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_CNT_PATH_LIST_ENTRIES, prop);

			//...............
			prop = prep->AppendChild( new wxBoolProperty("Connect List 'MoveSequences' by default", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_PRE_PROSSOR_CNT_SEQUENCE_MOVES, prop);
		}
		
		//...................
		wxPGProperty* cCtl = NULL;
		curCatLabel.assign("Cnc-Controller");
		cCtl = root->AppendChild( new wxPropertyCategory(curCatLabel) );
		registerCategory(curCatLabel, cCtl);
		{
			//...................
			prop = cCtl->AppendChild(new CncCfgSliderProperty("Artificially Step Delay", NEXT_PROP_ID, 0.0, 0.0, 128.0, 128.0));
			prop->SetAttribute(wxPG_ATTR_UNITS, "ms");
			prop->SetHelpString(_T("Adds an artificially step delay after each move command. Debug only"));
			wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
			wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(0);
			registerProperty(CncWork_Ctl_ARTIFICIALLY_STEP_DELAY, prop);

			//...............
			prop = cCtl->AppendChild( new wxBoolProperty("Avoid sending of duplicate Setter values", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_AVOID_DUP_SETTER_VALUES, prop);
			
			//...............
			prop = cCtl->AppendChild( new wxBoolProperty("Position out of range -> Interrupt", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_INTERRUPT_BY_POS_OUTOFRANGE, prop);
			
			//...............
			prop = cCtl->AppendChild( new wxBoolProperty("Request idle requests", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_REQUEST_IDLE_REQUESTS, prop);
		}
		
		//...................
		wxPGProperty* mCtl = NULL;
		curCatLabel.assign("Micro-Controller");
		mCtl = root->AppendChild( new wxPropertyCategory(curCatLabel) );
		registerCategory(curCatLabel, mCtl);
		{
			//...............
			prop = mCtl->AppendChild( new wxBoolProperty("Inverse direction X", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_INVERSE_CTL_DIRECTION_X, prop);
			
			//...............
			prop = mCtl->AppendChild( new wxBoolProperty("Inverse direction Y", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_INVERSE_CTL_DIRECTION_Y, prop);
			
			//...............
			prop = mCtl->AppendChild( new wxBoolProperty("Inverse direction Z", NEXT_PROP_ID, true));
			prop->Enable(true);
			prop->SetHelpString(_T(""));
			prop->SetEditor( wxT("CheckBox") );
			CncConfig::registerProperty(CncWork_Ctl_INVERSE_CTL_DIRECTION_Z, prop);
			
			//...............
			prop = mCtl->AppendChild( new CncCfgStaticProperty("Position - Reply Threshold", NEXT_PROP_ID, ""));
			prop->SetValue("");
			prop->SetHelpString("Depends on the metric parameter below");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			prop->SetAttribute(wxPG_ATTR_UNITS, "impulse");
			registerProperty(CncWork_Ctl_REPLY_THRESHOLD_STEPS, prop);
			{
				//...............
				pgParameterMgrArr.Clear();
				pgParameterMgrIntArr.Clear();
				pgParameterMgrArr.Add(_("0.001")); pgParameterMgrArr.Add(_("0.005")); pgParameterMgrArr.Add(_("0.010")); pgParameterMgrArr.Add(_("0.020")); 
				pgParameterMgrArr.Add(_("0.030")); pgParameterMgrArr.Add(_("0.040")); pgParameterMgrArr.Add(_("0.050")); pgParameterMgrArr.Add(_("0.060")); 
				pgParameterMgrArr.Add(_("0.070")); pgParameterMgrArr.Add(_("0.080")); pgParameterMgrArr.Add(_("0.090")); pgParameterMgrArr.Add(_("0.100"));
				pgParameterMgrArr.Add(_("0.200")); pgParameterMgrArr.Add(_("0.300")); pgParameterMgrArr.Add(_("0.400")); pgParameterMgrArr.Add(_("0.500"));
				pgParameterMgrArr.Add(_("0.600")); pgParameterMgrArr.Add(_("0.700")); pgParameterMgrArr.Add(_("0.800")); pgParameterMgrArr.Add(_("0.900"));
				pgParameterMgrArr.Add(_("1.000")); pgParameterMgrArr.Add(_("2.000")); pgParameterMgrArr.Add(_("5.000"));
				
				prop = prop->AppendChild( new wxEnumProperty("Position - Reply Threshold", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 15));
				prop->SetHelpString("Defines the metric difference at which the controller continuous reply the current position as well as the measured speed back to the application");
				prop->SetEditor( wxT("ComboBox") );
				prop->SetAttribute(wxPG_ATTR_UNITS, "mm");
				CncConfig::registerProperty(CncWork_Ctl_REPLY_THRESHOLD_METRIC, prop);
			}
			
			
			//...............
			prop = mCtl->AppendChild( new CncCfgSliderProperty("Slider Test", NEXT_PROP_ID, 48.1, 0.0, 100.0, 1000));
			prop->SetAttribute(wxPG_ATTR_UNITS, "impulse");
			registerProperty("HackiWimmer1", prop);
			
			//...............
			prop = mCtl->AppendChild( new CncCfgSliderProperty("Slider Test", NEXT_PROP_ID, 0.0, -100.0, 100.0, 1000));
			prop->SetAttribute(wxPG_ATTR_UNITS, "impulse");
			prop->SetAttribute(wxPG_ATTR_UNITS, "impulse");
			//prop->SetAttribute(Attribute_READONLY, "TRUE");
			wxDynamicCast(prop, CncCfgSliderProperty)->setEditable(false);
			wxDynamicCast(prop, CncCfgSliderProperty)->setDecimalPlaces(4);
			registerProperty("HackiWimmer2", prop);
			
		}
	}
}
