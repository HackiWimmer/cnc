#include "wxcrafter.h"
#include "MainFrame.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

extern wxPropertyGridManager* 	globlSetupGrid;

////////////////////////////////////////////////////////////////////////
void CncConfig::pgChangedSvgCfgPage(MainFrame* mf, wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
		
	wxString name(p->GetName());
	
	if        ( name == CncSvg_Emu_COPY_FACTOR ) {
		if ( mf != NULL )
			mf->releaseControllerSetupFromConfig();
		
	} else if ( name == CncSvg_Emu_RSLT_WITH_ORIG_PATH ) {
		if ( mf != NULL )
			mf->releaseControllerSetupFromConfig();
		
	} else if ( name == CncSvg_Emu_RSLT_ONLY_WITH_FIRST_CROSS ) {
		if ( mf != NULL )
			mf->releaseControllerSetupFromConfig();
		
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