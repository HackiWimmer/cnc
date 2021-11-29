#include <wx/propgrid/manager.h>
#include "wxCrafterImages.h"
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
void CncConfig::pgChangedApplicationCfgPage(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupApplicationCfgPage(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxPropertyGridPage* page	= NULL;
	wxPGProperty* root 			= NULL;
	wxString curCatLabel;
	
	wxArrayString pgParameterMgrArr;
	wxArrayInt pgParameterMgrIntArr;
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	curCatLabel.assign("Application");
	page = Cnc::Config::globlSetupGrid->AddPage(curCatLabel, ImageLibConfig().Bitmap(_("BMP_APP_CFG")));
	root = page->Append( new wxPropertyCategory(curCatLabel) );
	registerCategory(curCatLabel, root);
	
	PGFuncPtrStore fps;
	fps.name.assign(curCatLabel);
	fps.propertyChanged 		= &CncConfig::pgChangedApplicationCfgPage;
	fps.propertyChanging		= NULL;
	fps.propertySelected		= NULL;
	fps.propertyCommandButton	= NULL;
	CncConfig::registerPageRoot(root, fps);
	{
		wxPGProperty* prop = NULL;
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Show splash screen on startup", NEXT_PROP_ID, true));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_SHOW_SPLASH_SCREEN, prop);
		
		//...............
		pgParameterMgrArr.Clear();
		pgParameterMgrIntArr.Clear();
		pgParameterMgrArr.Add(_("mm")); 
		pgParameterMgrArr.Add(_("steps"));
		prop = root->AppendChild( new wxEnumProperty("Default display unit", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("ComboBox") );
		registerProperty(CncApplication_DEF_DISPLAY_UNIT, prop);
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Auto connect port on startup", NEXT_PROP_ID, true));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_AUTO_CONNECT, prop);
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Auto open last template on startup", NEXT_PROP_ID, false));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_AUTO_OPEN_LAST, prop);
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Auto process template on startup", NEXT_PROP_ID, false));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_AUTO_PROCESS, prop);
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Auto open OpenGl Context Observer on startup", NEXT_PROP_ID, false));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_AUTO_OPEN_OGL_OBSERVER, prop);

		//...............
		prop = root->AppendChild( new wxBoolProperty("Show Test menu", NEXT_PROP_ID, true));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_SHOW_TEST_MENU, prop);
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Use Main preview", NEXT_PROP_ID, true));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_USE_MAIN_PREVIEW, prop);
		
		//...............
		prop = root->AppendChild( new wxBoolProperty("Use Monitor Preview", NEXT_PROP_ID, true));
		prop->Enable(true);
		prop->SetHelpString(_T("Restart required"));
		prop->SetEditor( wxT("CheckBox") );
		registerProperty(CncApplication_USE_MONITOR_PREVIEW, prop);
		
		//...............
		pgParameterMgrArr.Clear();
		pgParameterMgrIntArr.Clear();
		pgParameterMgrArr.Add(_("Always")); 
		pgParameterMgrArr.Add(_("Never"));
		pgParameterMgrArr.Add(_("Serial Port only"));
		prop = root->AppendChild( new wxEnumProperty("Run confirmation mode", NEXT_PROP_ID, pgParameterMgrArr, pgParameterMgrIntArr, 0));
		prop->Enable(true);
		prop->SetHelpString(_T(""));
		prop->SetEditor( wxT("ComboBox") );
		registerProperty(CncApplication_CONFIRMATION_MODE, prop);
		
		//...................
		wxPGProperty* port = NULL;
		curCatLabel.assign("Port");
		port = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, port);
		{
			//...............
			prop = port->AppendChild( new wxStringProperty("Default port", NEXT_PROP_ID, ""));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Com_DEFAULT_PORT, prop);
		}
		collapse(port);
		
		//...................
		wxPGProperty* tpl = NULL;
		curCatLabel.assign("Template");
		tpl = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, tpl);
		{
			//...............
			prop = tpl->AppendChild( new wxDirProperty("Default directory", NEXT_PROP_ID, ""));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tpl_DEFAULT_DIRECTORY, prop);
			
			//...............
			prop = tpl->AppendChild( new wxFileProperty("Default template", NEXT_PROP_ID, ""));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tpl_DEFAULT_FILE, prop);
		}
		collapse(tpl);

		//...................
		wxPGProperty* tools = NULL;
		curCatLabel.assign("Tools");
		tools = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, tools);
		{
			//...............
			prop = tools->AppendChild( new wxFileProperty("File Browser", NEXT_PROP_ID, ""));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_FILE_BROWSER, prop);

			//...............
			prop = tools->AppendChild( new wxFileProperty("Binary File Viewer", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_BIN_FILE_VIEWER, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("SVG File Viewer", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_SVG_FILE_VIEWER, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("GCode File Viewer", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_GCODE_FILE_VIEWER, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("XML File Viewer", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_XML_FILE_VIEWER, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("Browser", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_BROWSER, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("External Text Editor", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_EXTERNAL_EDITOR, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("External Hex Editor", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_EXTERNAL_HEX_EDITOR, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("External Veusz Plotter", NEXT_PROP_ID, "notepad"));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_VEUSZ_PLOTTER, prop);
			
			//...............
			prop = tools->AppendChild( new wxFileProperty("PyCam", NEXT_PROP_ID, ""));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tool_PY_CAM, prop);
		}
		collapse(tools);
		
		//...................
		wxPGProperty* test = NULL;
		curCatLabel.assign("Test");
		test = root->AppendChild( new wxPropertyCategory(curCatLabel));
		registerCategory(curCatLabel, test);
		{
			//...............
			prop = test->AppendChild( new wxDirProperty("Default test templates directory", NEXT_PROP_ID, ""));
			prop->Enable(true);
			prop->SetHelpString("");
			registerProperty(CncApplication_Tpl_DEFAULT_TEST_DIRECTORY, prop);
		}
	}
}
