#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
#include "CncFileNameService.h"
#include "CncUnitCalculator.h"
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
void CncConfig::setupEnvironmentCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatLabel;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	curCatLabel.assign("Environment");
	page = Cnc::Config::globlSetupGrid->AddPage(curCatLabel, ImageLibConfig().Bitmap(_("BMP_ENV_CFG")));
	root = page->Append( new wxPropertyCategory(curCatLabel) );
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatLabel);
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty*	prop = NULL;
		
		//...................
		wxPGProperty* factors = NULL;
		curCatLabel.assign("Calculated Factors");
		factors = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, factors);
		{
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Display Factor X axis", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_DISPLAY_FACT_X, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Display Factor Y axis", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_DISPLAY_FACT_Y, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Display Factor Z axis", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_DISPLAY_FACT_Z, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Display Factor X axis 3D", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_DISPLAY_FACT_3D_X, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Display Factor Y axis 3D", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_DISPLAY_FACT_3D_Y, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Display Factor Z axis 3D", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_DISPLAY_FACT_3D_Z, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Calculation Factor X axis 3D", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_CALCULATION_FACT_X, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Calculation Factor Y axis 3D", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_CALCULATION_FACT_Y, prop);
			
			//...............
			prop = factors->AppendChild( new CncCfgStaticProperty("Calculation Factor Z axis 3D", NEXT_PROP_ID, ""));
			prop->SetHelpString("");
			prop->SetAttribute(Attribute_READONLY, "TRUE");
			registerProperty(CncRuntime_CALCULATION_FACT_Z, prop);
		}
		
		//...................
		wxPGProperty* unitCalc = NULL;
		curCatLabel.assign("Unit Calculator");
		unitCalc = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, unitCalc);
		{
			//...............
			std::stringstream stream;
			CncUnitCalculatorBase::trace(stream);
			wxString fns(stream.str().c_str());
			{
				wxStringTokenizer lines(fns, "\n");
				while ( lines.HasMoreTokens() ) {
					wxString token = lines.GetNextToken();
					token.Trim(true).Trim(false);
					wxString entry(token.BeforeFirst(':'));
					wxString value(token.AfterFirst(':'));
					
					//...............
					wxString name(wxString::Format("#|CncRuntime|%s", NEXT_PROP_ID));
					prop = unitCalc->AppendChild( new CncCfgStaticProperty(entry, name, ""));
					prop->SetValue(value);
					prop->SetHelpString("");
					prop->SetAttribute(Attribute_READONLY, "TRUE");
					registerProperty(name, prop);
				}
			}
		}
		
		//...................
		wxPGProperty* fileNames = NULL;
		curCatLabel.assign("File Name Service");
		fileNames = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, fileNames);
		{
			//...............
			std::stringstream stream;
			CncFileNameService::trace(stream);
			wxString fns(stream.str().c_str());
			{
				wxStringTokenizer lines(fns, "\n");
				while ( lines.HasMoreTokens() ) {
					wxString token = lines.GetNextToken();
					token.Trim(true).Trim(false);
					wxString entry(token.BeforeFirst(':'));
					wxString value(token.AfterFirst(':'));
					
					//...............
					wxString name(wxString::Format("#|CncRuntime|%s", NEXT_PROP_ID));
					prop = fileNames->AppendChild( new CncCfgStaticProperty(entry, name, ""));
					prop->SetValue(value);
					prop->SetHelpString("");
					prop->SetAttribute(Attribute_READONLY, "TRUE");
					registerProperty(name, prop);
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::updateCalculatedFactors() {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* prop = NULL;
	
	{ prop = getProperty(CncRuntime_DISPLAY_FACT_X); 		if (prop != NULL) prop->SetValue(getDisplayFactX(CncMetric)); }
	{ prop = getProperty(CncRuntime_DISPLAY_FACT_Y); 		if (prop != NULL) prop->SetValue(getDisplayFactY(CncMetric)); }
	{ prop = getProperty(CncRuntime_DISPLAY_FACT_Z); 		if (prop != NULL) prop->SetValue(getDisplayFactZ(CncMetric)); }
	
	{ prop = getProperty(CncRuntime_DISPLAY_FACT_3D_X); 	if (prop != NULL) prop->SetValue(getDispFactX3D()); }
	{ prop = getProperty(CncRuntime_DISPLAY_FACT_3D_Y); 	if (prop != NULL) prop->SetValue(getDispFactY3D()); }
	{ prop = getProperty(CncRuntime_DISPLAY_FACT_3D_Z); 	if (prop != NULL) prop->SetValue(getDispFactZ3D()); }
	
	{ prop = getProperty(CncRuntime_CALCULATION_FACT_X); 	if (prop != NULL) prop->SetValue(getCalculationFactX(CncMetric)); }
	{ prop = getProperty(CncRuntime_CALCULATION_FACT_Y); 	if (prop != NULL) prop->SetValue(getCalculationFactY(CncMetric)); }
	{ prop = getProperty(CncRuntime_CALCULATION_FACT_Z); 	if (prop != NULL) prop->SetValue(getCalculationFactZ(CncMetric)); }
}
////////////////////////////////////////////////////////////////////////
void CncConfig::updateCalculatedZAxisValues() {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* prop = NULL;
	
	wxString list("Z-Feed-Table:\n");
	for (unsigned int i=0; i<getMaxDurations(); i++) {
		list.append(wxString::Format(" Duration step[% 2d] = % 2.3lf\n", i, getDurationThickness(i)));
	}
	
	{ prop = getProperty(CncRuntime_Z_MAX_DURATIONS); 			if (prop != NULL) prop->SetValue((int)getMaxDurations()); }
	{ prop = getProperty(CncRuntime_Z_WORKPIECE_OFFSET); 		if (prop != NULL) prop->SetValue(getWorkpieceOffset()); }
	{ prop = getProperty(CncRuntime_Z_MAX_DURATION_THICKNESS); 	if (prop != NULL) prop->SetValue(getMaxDurationThickness()); }
	{ prop = getProperty(CncRuntime_Z_CALCULATED_DURATIONS); 	if (prop != NULL) prop->SetValue((int)getDurationCount()); }
	{ prop = getProperty(CncRuntime_Z_CURRENT_Z_DISTANCE); 		if (prop != NULL) prop->SetValue(getCurZDistance()); }
	{ prop = getProperty(CncRuntime_Z_WORKPIECE_INCLUDED); 		if (prop != NULL) prop->SetValue(getReferenceIncludesWpt()); }
	{ prop = getProperty(CncRuntime_Z_DURATION_THICKNESS); 		if (prop != NULL) prop->SetValue("See description below"); prop->SetHelpString(list); }
}
