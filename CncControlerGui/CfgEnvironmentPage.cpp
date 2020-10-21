#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
#include "CncFileNameService.h"
#include "CncUnitCalculator.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

extern wxPropertyGridManager* 	globlSetupGrid;

////////////////////////////////////////////////////////////////////////
void CncConfig::setupEnvironmentCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatName;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	curCatName.assign("Environment");
	page = globlSetupGrid->AddPage(curCatName, 
	ImageLibConfig().Bitmap(_("BMP_ENV_CFG")));
	root = page->Append( new wxPropertyCategory(curCatName) );
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatName);
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...................
		wxPGProperty* zaxis = NULL;
		curCatName.assign("Calculate Z Axis Values(Workpiece Thickness)");
		zaxis = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, zaxis);
		{
			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Max Durations [#]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_MAX_DURATIONS, prop);
			
			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Durations [#]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_CALCULATED_DURATIONS, prop);
			
			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Max Duration Thickness [mm]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_MAX_DURATION_THICKNESS, prop);
			
			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Workpiece included", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_WORKPIECE_INCLUDED, prop);

			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Workpiece offset [mm]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_WORKPIECE_OFFSET, prop);
			
			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Current Z Distance [mm]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_CURRENT_Z_DISTANCE, prop);

			//...............
			prop = zaxis->AppendChild( new wxStringProperty("Calculated Duration Array [mm]", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_Z_DURATION_THICKNESS, prop);
		}
		
		//...................
		wxPGProperty* factors = NULL;
		curCatName.assign("Calculated Factors");
		factors = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, factors);
		{
			//...............
			prop = factors->AppendChild( new wxStringProperty("Display Factor X axis", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_DISPLAY_FACT_X, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Display Factor Y axis", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_DISPLAY_FACT_Y, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Display Factor Z axis", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_DISPLAY_FACT_Z, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Display Factor X axis 3D", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_DISPLAY_FACT_3D_X, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Display Factor Y axis 3D", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_DISPLAY_FACT_3D_Y, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Display Factor Z axis 3D", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_DISPLAY_FACT_3D_Z, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Calculation Factor X axis 3D", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_CALCULATION_FACT_X, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Calculation Factor Y axis 3D", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_CALCULATION_FACT_Y, prop);
			
			//...............
			prop = factors->AppendChild( new wxStringProperty("Calculation Factor Z axis 3D", NEXT_PROP_ID, ""));
			prop->Enable(false);
			prop->SetHelpString("");
			prop->SetEditor( wxT("TextCtrl") );
			prop->SetValue("");
			registerProperty(CncRuntime_CALCULATION_FACT_Z, prop);
		}
		
		//...................
		wxPGProperty* fileNames = NULL;
		curCatName.assign("File Name Service");
		fileNames = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, fileNames);
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
					wxString name(wxString::Format("#/%s", NEXT_PROP_ID));
					prop = fileNames->AppendChild( new wxStringProperty(entry, name, ""));
					prop->Enable(false);
					prop->SetHelpString(value);
					prop->SetEditor( wxT("TextCtrl") );
					prop->SetValue(value);
					registerProperty(name, prop);
				}
			}
		}
		
		//...................
		wxPGProperty* unitCalc = NULL;
		curCatName.assign("CncUnitCalculator");
		unitCalc = root->AppendChild( new wxPropertyCategory(curCatName));
		registerCategory(curCatName, unitCalc);
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
					wxString name(wxString::Format("#/%s", NEXT_PROP_ID));
					prop = unitCalc->AppendChild( new wxStringProperty(entry, name, ""));
					prop->Enable(false);
					prop->SetHelpString(_T(""));
					prop->SetEditor( wxT("TextCtrl") );
					prop->SetValue(value);
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
	
	wxString list("<Readonly>\n");
	for (unsigned int i=0; i<getMaxDurations(); i++) {
		if ( getDurationThickness(i) != 0.0 )
			list.append(wxString::Format(" Duration step[%d] = %.3lf\n", i, getDurationThickness(i)));
	}
	
	{ prop = getProperty(CncRuntime_Z_MAX_DURATIONS); 			if (prop != NULL) prop->SetValue((int)getMaxDurations()); }
	{ prop = getProperty(CncRuntime_Z_WORKPIECE_OFFSET); 		if (prop != NULL) prop->SetValue(getWorkpieceOffset()); }
	{ prop = getProperty(CncRuntime_Z_MAX_DURATION_THICKNESS); 	if (prop != NULL) prop->SetValue(getMaxDurationThickness()); }
	{ prop = getProperty(CncRuntime_Z_CALCULATED_DURATIONS); 	if (prop != NULL) prop->SetValue((int)getDurationCount()); }
	{ prop = getProperty(CncRuntime_Z_CURRENT_Z_DISTANCE); 		if (prop != NULL) prop->SetValue(getCurZDistance()); }
	{ prop = getProperty(CncRuntime_Z_WORKPIECE_INCLUDED); 		if (prop != NULL) prop->SetValue(getReferenceIncludesWpt()); }
	{ prop = getProperty(CncRuntime_Z_DURATION_THICKNESS); 		if (prop != NULL) prop->SetValue("See description below"); prop->SetHelpString(list); }
}
