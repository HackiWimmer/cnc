#include <wx/propgrid/manager.h>
#include <wx/config.h>
#include "wxcrafter.h"
#include "CncConfig.h"


wxPropertyGridManager* globlSetupGrid 	= NULL;;

wxPGProperty* rootGCode					= NULL;
wxPGProperty* rootSVG					= NULL;
wxPGProperty* rootEnv					= NULL;
wxPGProperty* rootSpeed					= NULL;


wxPGProperty* propMaxSpeedZ 				= NULL;
wxPGProperty* propMaxSpeedXY 				= NULL;



////////////////////////////////////////////////////////////////////////
CncConfig* CncConfig::globalCncConfig = NULL;
class GlobalConfigManager {
	public:
		
		GlobalConfigManager() {
			CncConfig::globalCncConfig = new CncConfig();
		}
		
		~GlobalConfigManager() {
			if ( CncConfig::globalCncConfig != NULL )
				delete CncConfig::globalCncConfig;
		}
} __gcm;
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void CncConfig::setSetupGrid(wxPropertyGridManager* sg, wxConfigBase* cfg) {
////////////////////////////////////////////////////////////////////////
	wxASSERT(sg);
	wxASSERT(cfg);
	
	globlSetupGrid = sg;
	globlSetupGrid->Clear();
	
	unsigned int propCounter;
	wxPropertyGridPage* page;
	
	
	page = globlSetupGrid->AddPage(_("GCode Properties"),	ImageLibConfig().Bitmap(_("BMP_GCODE_CFG")));
	rootGCode = page->Append( new wxPropertyCategory("GCode Properties") );
	
	page = globlSetupGrid->AddPage(_("SVG Properties"),  	ImageLibConfig().Bitmap(_("BMP_SVG_CFG")));
	rootSVG = page->Append( new wxPropertyCategory("SVG Properties") );
	
	page = globlSetupGrid->AddPage(_("Environment"),  		ImageLibConfig().Bitmap(_("BMP_ENV_CFG")));
	rootEnv = page->Append( new wxPropertyCategory("Environment") );
		
	
	
	
	page = globlSetupGrid->AddPage(_("Speed"),  			ImageLibConfig().Bitmap(_("BMP_SPEED_CFG")));
	rootSpeed = page->Append( new wxPropertyCategory(_("Speed")));
	{
		wxPGProperty* prop;
		
		//...................
		prop = rootSpeed->AppendChild( new wxIntProperty("Max speed XY [rpm]", wxString::Format("%d", propCounter++), cfg->ReadLong(_T("CncConfig/MaxSpeedYX"), 100)));
		prop->Enable(false);
		prop->SetHelpString(_T("Max speed [rpm] of the stepper motors for the x and y axis."));
		propMaxSpeedXY = prop;
		
		//...................
		prop = rootSpeed->AppendChild( new wxIntProperty("Max speed Z [rpm]", wxString::Format("%d", propCounter++), cfg->ReadLong(_T("CncConfig/MaxSpeedZ"), 100)));
		prop->Enable(false);
		prop->SetHelpString(_T("Max speed [rpm] of the stepper motor for the z axis."));
		propMaxSpeedZ = prop;
	}
	
}

////////////////////////////////////////////////////////////////////////
// config getters
int CncConfig::getMaxSpeedZ() 				{ wxASSERT ( propMaxSpeedZ ); 				return propMaxSpeedZ->GetValue().GetInteger();}
int CncConfig::getMaxSpeedXY() 				{ wxASSERT ( propMaxSpeedXY ); 				return propMaxSpeedXY->GetValue().GetInteger(); }





















wxComboBox* CncConfig::gblCurveLibSelector = NULL; 

////////////////////////////////////////////////////////////////////////
float CncConfig::getDefaultCurveLibResolution() {
////////////////////////////////////////////////////////////////////////
	return CncSvgCurveLib::getDefaultResolution();
}
////////////////////////////////////////////////////////////////////////
float CncConfig::getCurveLibResolution() { 
////////////////////////////////////////////////////////////////////////
	return CncSvgCurveLib::getResolution(); 
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setCurveLibResolution(double v) { 
////////////////////////////////////////////////////////////////////////
	CncSvgCurveLib::setResolution((float)v); 
	updateCurveLibResolutionSelector();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::updateCurveLibResolutionSelector() { 
////////////////////////////////////////////////////////////////////////
	if ( CncConfig::gblCurveLibSelector != NULL ) {
		wxString label(wxString::Format("%.3f", CncSvgCurveLib::getResolution()));
		CncConfig::gblCurveLibSelector->SetStringSelection(label);
	}
}

