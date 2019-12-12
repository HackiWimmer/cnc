#include <wx/arrstr.h>
#include <wx/dynlib.h>
#include "CncConfig.h"
#include "CncContext.h"
#include "CncFileNameService.h"
#include "CncContextListCtrl.h"
#include "GlobalFunctions.h"
#include "CncOSEnvironmentDialog.h"

//////////////////////////////////////////////////////////////////////////////
CncOSEnvironmentDialog::CncOSEnvironmentDialog(wxWindow* parent)
: CncOSEnvironmentDialogBase(parent)
, contextControl(NULL)
//////////////////////////////////////////////////////////////////////////////
{
	contextControl = new CncContextListCtrl(this, wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING);
	contextControl->setControls(m_cncContextPara, m_cncContextValue);
	GblFunc::replaceControl(m_cncContextListPlaceholder, contextControl);
	
	evaluateOSEnvrionemnt();
	evaluateAppEnvrionemnt();
	evaluateLoadedModules();
	evaluateVersionInfo();
	
	m_listbook->SetSelection(PAGE_ENVIRONMENT);
}
//////////////////////////////////////////////////////////////////////////////
CncOSEnvironmentDialog::~CncOSEnvironmentDialog() {
//////////////////////////////////////////////////////////////////////////////
	m_osEnvironmentList->DeleteAllItems();
	m_moduleList->DeleteAllItems();
	
	wxDELETE( contextControl );
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::evaluateVersionInfo() {
//////////////////////////////////////////////////////////////////////////////
	if ( m_versionInfoList->GetColumnCount() == 0 ) {
		m_versionInfoList->AppendColumn("Software", wxLIST_FORMAT_LEFT, 180);
		m_versionInfoList->AppendColumn("Version", 	wxLIST_FORMAT_LEFT, 500);
	}
	
	const VersionInfoMap& vim = THE_CONTEXT->versionInfoMap;
	for (auto it = vim.cbegin(); it != vim.cend(); ++it ) {
		
		const unsigned int index = m_versionInfoList->GetItemCount();
		m_versionInfoList->InsertItem(index, "");
		m_versionInfoList->SetItem(index, VER_COL_PARAM, it->first);
		m_versionInfoList->SetItem(index, VER_COL_VALUE, it->second);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::evaluateOSEnvrionemnt() {
//////////////////////////////////////////////////////////////////////////////
	if ( m_osEnvironmentList->GetColumnCount() == 0 ) {
		m_osEnvironmentList->AppendColumn("Parameter", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
		m_osEnvironmentList->AppendColumn("Value", 	 wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
	}
	
	m_osEnvironmentList->DeleteAllItems();
	
	wxEnvVariableHashMap envvars;
	wxGetEnvMap(&envvars);
	
	wxSortedArrayString vars;
	
	// Collect all environment vars
	for (auto it = envvars.begin(); it != envvars.end(); ++it)
		vars.Add(it->first);
	
	// ... sort it
	vars.Sort();
	
	// display key and value
	for (auto it = vars.begin(); it != vars.end(); ++it) {
		wxString const& envvarName  = *it;
		wxString const& envvarValue = envvars[*it];
		
		const unsigned int index = m_osEnvironmentList->GetItemCount();
		m_osEnvironmentList->InsertItem(index, "");
		m_osEnvironmentList->SetItem(index, ENV_COL_PARAM, envvarName);
		m_osEnvironmentList->SetItem(index, ENV_COL_VALUE, envvarValue);
	}
	
	sizeOSEnvironmentColumns();
	
	// select first item
	if ( m_osEnvironmentList->GetItemCount() > 0 )
		m_osEnvironmentList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::evaluateAppEnvrionemnt() {
//////////////////////////////////////////////////////////////////////////////
	if ( m_appEnvironmentList->GetColumnCount() == 0 ) {
		m_appEnvironmentList->AppendColumn("Parameter", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
		m_appEnvironmentList->AppendColumn("Value", 	 wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
	}
	
	m_appEnvironmentList->DeleteAllItems();

	std::stringstream stream;
	CncFileNameService::trace(stream);
	wxString fns(stream.str().c_str());
	
	wxStringTokenizer lines(fns, "\n");
	while ( lines.HasMoreTokens() ) {
		wxString token = lines.GetNextToken();
		token.Trim(true).Trim(false);
		wxString entry(token.BeforeFirst(':'));
		wxString value(token.AfterFirst(':'));
		
		const unsigned int index = m_appEnvironmentList->GetItemCount();
		m_appEnvironmentList->InsertItem(index, "");
		m_appEnvironmentList->SetItem(index, ENV_COL_PARAM, entry);
		m_appEnvironmentList->SetItem(index, ENV_COL_VALUE, value);
	}
	
	sizeAppEnvironmentColumns();
	
	// select first item
	if ( m_appEnvironmentList->GetItemCount() > 0 )
		m_appEnvironmentList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::evaluateLoadedModules() {
//////////////////////////////////////////////////////////////////////////////
	if ( m_moduleList->GetColumnCount() == 0 ) {
		m_moduleList->AppendColumn("Address (start:end)", 	wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
		m_moduleList->AppendColumn("Name", 	 				wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
		m_moduleList->AppendColumn("Version", 				wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
		m_moduleList->AppendColumn("Path", 					wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
	}
	
	m_moduleList->DeleteAllItems();
	
	wxDynamicLibraryDetailsArray modules(wxDynamicLibrary::ListLoaded());
	const size_t count = modules.size();
	if ( count <= 0 ) {
		const unsigned int index = m_moduleList->GetItemCount();
		m_moduleList->InsertItem(index, "");
		m_moduleList->SetItem(index, MOD_COL_ADDR, 		"");
		m_moduleList->SetItem(index, MOD_COL_NAME, 		"");
		m_moduleList->SetItem(index, MOD_COL_VERSION, 	"");
		m_moduleList->SetItem(index, MOD_COL_PATH, 		"Unable to evaluate loaded modules!");
		
		return;
	}
	
	wxSortedArrayString keys;
	const char separator = '|';
	
	// display details
	for ( size_t n = 0; n < count; n++ ) {
		const wxDynamicLibraryDetails& dll = modules[n];
		
		if ( m_modulesSortedBy->GetSelection() == 0 ) 	keys.Add(wxString::Format("%s%c%zu", wxFileName(dll.GetName()).GetFullName() , separator, n));
		else											keys.Add(wxString::Format("%s%c%zu", wxFileName(dll.GetPath()).GetPath() 	 , separator, n));
	}
	
	// ... sort it
	keys.Sort();
	
	// display key and value
	for (auto it = keys.begin(); it != keys.end(); ++it) {
		const wxString & key  = *it;
		const wxString & num  = key.AfterFirst(separator);
		long n; num.ToLong(&n);
		
		if ( n < 0 || n > (long)count - 1 )
			continue;
		
		const wxDynamicLibraryDetails& dll = modules[n];
		
		void *addr = 0;
		size_t len = 0;
		dll.GetAddress(&addr, &len);
		
		const unsigned int index = m_moduleList->GetItemCount();
		m_moduleList->InsertItem(index, "");
		m_moduleList->SetItem(index, MOD_COL_ADDR, 		wxString::Format("%p:%p", addr, static_cast<void*>(static_cast<char*>(addr) + len)));
		m_moduleList->SetItem(index, MOD_COL_NAME, 		wxFileName(dll.GetName()).GetFullName());
		m_moduleList->SetItem(index, MOD_COL_VERSION, 	dll.GetVersion());
		m_moduleList->SetItem(index, MOD_COL_PATH, 		wxFileName(dll.GetPath()).GetPath());
	}
	
	sizeModulesColumns();
	
	// select first item
	if ( m_moduleList->GetItemCount() > 0 )
		m_moduleList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::onClose(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////////////////
	EndModal(wxID_OK);
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::onSize(wxSizeEvent& event) {
//////////////////////////////////////////////////////////////////////////////
	event.Skip(true);
	
	sizeOSEnvironmentColumns(true);
	sizeModulesColumns(true);
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::sizeOSEnvironmentColumns(bool onlyLastColumn) {
//////////////////////////////////////////////////////////////////////////////
	const wxSize size = GetClientSize();
	
	if ( onlyLastColumn == false ) {
		m_osEnvironmentList->SetColumnWidth(ENV_COL_PARAM, wxLIST_AUTOSIZE);
		m_osEnvironmentList->SetColumnWidth(ENV_COL_PARAM, m_osEnvironmentList->GetColumnWidth(ENV_COL_PARAM) + 16 );
	}
	
	m_osEnvironmentList->SetColumnWidth(ENV_COL_VALUE, size.GetWidth() - m_osEnvironmentList->GetColumnWidth(ENV_COL_PARAM));
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::sizeAppEnvironmentColumns(bool onlyLastColumn) {
//////////////////////////////////////////////////////////////////////////////
	const wxSize size = GetClientSize();
	
	if ( onlyLastColumn == false ) {
		m_appEnvironmentList->SetColumnWidth(ENV_COL_PARAM, wxLIST_AUTOSIZE);
		m_appEnvironmentList->SetColumnWidth(ENV_COL_PARAM, m_appEnvironmentList->GetColumnWidth(ENV_COL_PARAM) + 16 );
	}
	
	m_appEnvironmentList->SetColumnWidth(ENV_COL_VALUE, size.GetWidth() - m_appEnvironmentList->GetColumnWidth(ENV_COL_PARAM));
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::sizeModulesColumns(bool onlyLastColumn) {
//////////////////////////////////////////////////////////////////////////////
	const wxSize size = GetClientSize();
	
	if ( onlyLastColumn == false ) {
		m_moduleList->SetColumnWidth(MOD_COL_ADDR, 		wxLIST_AUTOSIZE);
		m_moduleList->SetColumnWidth(MOD_COL_ADDR, 		m_moduleList->GetColumnWidth(MOD_COL_ADDR) + 8);
		m_moduleList->SetColumnWidth(MOD_COL_NAME, 		wxLIST_AUTOSIZE);
		m_moduleList->SetColumnWidth(MOD_COL_VERSION, 	wxLIST_AUTOSIZE);
	}
	
	m_moduleList->SetColumnWidth(MOD_COL_PATH, size.GetWidth() - ( 
																	+ m_moduleList->GetColumnWidth(MOD_COL_ADDR)
																	+ m_moduleList->GetColumnWidth(MOD_COL_NAME)
																	+ m_moduleList->GetColumnWidth(MOD_COL_VERSION)
																 )
	);
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::selectOSEnvironmentItem(wxListEvent& event) {
//////////////////////////////////////////////////////////////////////////////
	unsigned int index = event.m_itemIndex;
	
	m_osEnvParam->SetValue(m_osEnvironmentList->GetItemText(index, ENV_COL_PARAM));
	m_osEnvValue->SetValue(m_osEnvironmentList->GetItemText(index, ENV_COL_VALUE));
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::selectAppEnvironmentItem(wxListEvent& event) {
//////////////////////////////////////////////////////////////////////////////
	unsigned int index = event.m_itemIndex;
	
	m_appEnvParam->SetValue(m_appEnvironmentList->GetItemText(index, ENV_COL_PARAM));
	m_appEnvValue->SetValue(m_appEnvironmentList->GetItemText(index, ENV_COL_VALUE));
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::selectModulesItem(wxListEvent& event) {
//////////////////////////////////////////////////////////////////////////////
	unsigned int index = event.m_itemIndex;
	
	m_modAddress->SetValue(m_moduleList->GetItemText(index, MOD_COL_ADDR));
	m_modName->SetValue(m_moduleList->GetItemText(index, MOD_COL_NAME));
	m_modVersion->SetValue(m_moduleList->GetItemText(index, MOD_COL_VERSION));
	m_modPath->SetValue(m_moduleList->GetItemText(index, MOD_COL_PATH));
}
//////////////////////////////////////////////////////////////////////////////
void CncOSEnvironmentDialog::sortModules(wxCommandEvent& event) {
//////////////////////////////////////////////////////////////////////////////
	evaluateLoadedModules();
}
