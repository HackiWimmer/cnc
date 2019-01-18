#include <iostream>
#include <wx/textdlg.h>
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncPerspective.h"

const char* configGroupName 				= "Perspectives";
const char* paneListSuffix 					= ".PaneList";
const char* userPerspectivePrefix			= "*";

const char  nameEnclose						= '\'';

///////////////////////////////////////////////////////////////////
CncPerspective::CncPerspective(wxConfigBase* cfg, wxMenu* m) 
: config(cfg)
, menu(m)
///////////////////////////////////////////////////////////////////
{
	wxASSERT(menu != NULL);
}
///////////////////////////////////////////////////////////////////
CncPerspective::~CncPerspective() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
const char CncPerspective::getNameEnclose() {
///////////////////////////////////////////////////////////////////
	return nameEnclose;
}
///////////////////////////////////////////////////////////////////
const char* CncPerspective::getConfigGroupName() {
///////////////////////////////////////////////////////////////////
	return configGroupName;
}
///////////////////////////////////////////////////////////////////
const char* CncPerspective::getPaneListSuffix() {
///////////////////////////////////////////////////////////////////
	return paneListSuffix;
}
///////////////////////////////////////////////////////////////////
const char* CncPerspective::getUserPerspectivePrefix() {
///////////////////////////////////////////////////////////////////
	return userPerspectivePrefix;
}
///////////////////////////////////////////////////////////////////
int CncPerspective::getNextUserPerspectiveInsertIndex(unsigned int sepIndex) {
///////////////////////////////////////////////////////////////////
	wxASSERT(menu != NULL);
	
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	
	unsigned int itemCounter = 0;
	unsigned int sepCounter  = 0;
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
		
		// find corresponding separator
		if ( mi->IsSeparator() == true ) {
			sepCounter++;
			
			if ( sepCounter == sepIndex )
				return itemCounter;
		}
		
		itemCounter++;
	}
	
	return -1;
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::isUserPerspective(const wxString& menuLabel) {
///////////////////////////////////////////////////////////////////
	if ( menuLabel.Contains("'") == false )
		return false;
	
	if ( menuLabel.Contains(userPerspectivePrefix) == false )
		return false;
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::getAllUserPerspectiveNamesFromMenuLabels(wxArrayString& items) {
///////////////////////////////////////////////////////////////////
	wxASSERT(menu != NULL);
	
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
			
		wxString label(mi->GetItemLabelText());
		if ( isUserPerspective(label) == false )
			continue;
		
		if ( label.Contains("-") == false )
			continue;
		
		// strip name
		label.assign(label.AfterFirst('\''));
		label.assign(label.BeforeFirst('\''));
		
		items.Add(label);
	}
	
	return ( items.GetCount() > 0 );
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::checkIfPerspectiveAlreadyExists(const wxString& name) {
///////////////////////////////////////////////////////////////////
	wxASSERT(menu != NULL);
	
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
		
		// consider only items which are containing a '-'
		wxString label(mi->GetItemLabelText());
		if ( label.Contains("-") == false )
			continue;
		
		// strip name
		label = label.AfterFirst('\'');
		label = label.BeforeLast('\'');
		
		// check if already exists
		if ( name.Upper() == label.Upper() )
			return true;
	}
	
	GBL_CONFIG->getTheApp()->GetAuimgrMain();
	
	return false;
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::insertNextUserPerspective(const wxString& newLabel) {
///////////////////////////////////////////////////////////////////
	wxASSERT(menu != NULL);
	
	// safty - corresponding error messages have to be processed outside
	if ( checkIfPerspectiveAlreadyExists(newLabel) == true )
		return false;
	
	bool ret = false;
	int idx1 = getNextUserPerspectiveInsertIndex(1);
	int idx2 = getNextUserPerspectiveInsertIndex(2) + 1; // + 1 because the new item above have to be considered too
	
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	wxMenuItem* mi = NULL;
	if ( idx1 >=0 && idx2 > idx1 ) {
		mi = menu->Insert(idx1, wxID_ANY, wxString::Format("* User Perspective - '%s'", newLabel));
		mi->SetBitmap(ImageLibPerspective().Bitmap("BMP_USER_PERSPECTIVE"));
		mf->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::loadPerspective, mf, mi->GetId());
		THE_APP->registerMenuItem(mi);
		
		mi = menu->Insert(idx2, wxID_ANY, wxString::Format("* Save User Perspective as '%s'", newLabel));
		mi->SetBitmap(ImageLibPerspective().Bitmap("BMP_USER_PERSPECTIVE_SAVE"));
		mf->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::savePerspective, mf, mi->GetId());
		
		THE_APP->registerMenuItem(mi);
		ret = true;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncPerspective::setupUserPerspectives() {
///////////////////////////////////////////////////////////////////
	config->SetPath(wxString::Format("/%s", configGroupName));
	
	// enumeration variables
	wxString entryName;
	long entryIndex;

	// over all perspectives
	bool bEntry = config->GetFirstEntry(entryName, entryIndex);
	while ( bEntry ) {
		// if user prespective
		if ( entryName.StartsWith(userPerspectivePrefix) == true ) {
			// if no pane list
			if ( entryName.Contains(paneListSuffix) == false ) {
				// remove asterix
				wxString label(entryName.SubString(1, entryName.length()-1));
				if ( insertNextUserPerspective(label) == false ) {
					std::cerr << "CncPerspective::setupUserPerspectives() failed: Entry: " << entryName << std::endl;
				}
			}
		}
		
		bEntry = config->GetNextEntry(entryName, entryIndex);
	}
	
	// reset config path
	config->SetPath("/");
}
///////////////////////////////////////////////////////////////////
void CncPerspective::destroyUserPerspectives() {
///////////////////////////////////////////////////////////////////
	wxASSERT(menu != NULL);
	
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
		
		wxString label(mi->GetItemLabelText());
		if ( isUserPerspective(label) == false )
			continue;
			
		MainFrame* mf = GBL_CONFIG->getTheApp();
			
		// unbind event
		if ( label.Contains("Save Perspective") == true )	mf->Unbind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::savePerspective, mf, mi->GetId());
		else												mf->Unbind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::loadPerspective, mf, mi->GetId());
	}
}
/////////////////////////////////////////////////////////////////////
bool CncPerspective::renameUserPerspective(const wxString& from, const wxString& to) {
/////////////////////////////////////////////////////////////////////
	wxASSERT( config != NULL );
	wxASSERT( menu != NULL );
	
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
			
		wxString label(mi->GetItemLabelText());
		
		if ( label.Upper().Contains(wxString::Format("'%s'", from.Upper())) == false )
			continue;
			
		// rename menu
		if ( label.Contains("Save User Perspective") == true )	mi->SetItemLabel(wxString::Format("* Save User Perspective as '%s'", to));
		else													mi->SetItemLabel(wxString::Format("* User Perspective - '%s'", to));
		
		// rename config
		config->SetPath("/Perspectives");
		config->RenameEntry(wxString::Format("*%s", from), 			wxString::Format("*%s", to));
		config->RenameEntry(wxString::Format("*%s.PaneList", from), wxString::Format("*%s.PaneList", to));
		config->SetPath("/");
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::addUserPerspective() {
///////////////////////////////////////////////////////////////////
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	wxString newLabel;
	do {
		wxTextEntryDialog dlg(mf, "Name:", "Add User Perspective . . .");
		dlg.SetMaxLength(32);
		dlg.SetTextValidator(wxFILTER_ASCII);
		dlg.SetValue(newLabel);
		
		if ( dlg.ShowModal() != wxID_OK  )
			return false;
		
		// check content
		newLabel.assign(dlg.GetValue());
		newLabel.Trim(true).Trim(false);
		if ( newLabel.IsEmpty() == true )
			return false;
			
		newLabel.Replace("'", "", true);
		newLabel.Replace("-", "", true);
	
		if ( checkIfPerspectiveAlreadyExists(newLabel) == true ) {
			wxString msg(wxString::Format("A user perspective with the name '%s' already exists. Please choose a different name", newLabel));
			wxMessageDialog dlg(mf, msg, _T("Add User Perspective failed. . ."), wxOK|wxCENTRE|wxICON_ERROR);
			dlg.ShowModal();
		}
		
	} while ( checkIfPerspectiveAlreadyExists(newLabel) == true );
	
	// insert
	if ( insertNextUserPerspective(newLabel) == false ) {
		std::cerr << "Can't insert the user perspective '" << newLabel << "'. insert failed!" << std::endl;
		return false;
	}
	
	savePerspective(wxString::Format("*%s", newLabel), false);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::removeUserPerspective() {
///////////////////////////////////////////////////////////////////
	wxASSERT( menu != NULL );
	MainFrame* mf = GBL_CONFIG->getTheApp();

	wxArrayString items;
	if ( getAllUserPerspectiveNamesFromMenuLabels(items) == false ) {
		wxString msg("No user perspectives available");
		wxMessageDialog eDlg(mf, msg, _T("Remove User Perspective failed . . ."), wxOK|wxCENTRE|wxICON_ERROR);
		eDlg.ShowModal();
		return false;
	}
	
	wxSingleChoiceDialog lDlg(mf, "Select perspective to remove:", "Remove User Perspective . . .", items);
	
	if ( lDlg.ShowModal() != wxID_OK )
		return false; 
	
	// check content
	wxString rmvLabel(lDlg.GetStringSelection());
	rmvLabel.Trim(true).Trim(false);
	if ( rmvLabel.IsEmpty() == true )
		return false;
	
	wxString msg(wxString::Format("Do you really want to remove the the user perspective: '%s'", rmvLabel));
	wxMessageDialog qDlg(mf, msg, _T("Remove User Perspective . . ."), wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
	if ( qDlg.ShowModal() != wxID_OK ) 
		return false;
	
	// remove
	wxMenuItemList mil = menu->GetMenuItems(); 
	mil.GetFirst();
	wxString search(wxString::Format("'%s'", rmvLabel));
	for (wxMenuItemList::iterator iter = mil.begin(); iter != mil.end(); ++iter) {
		wxMenuItem* mi = *iter;
		if ( mi == NULL )
			continue;
		
		if ( mi->GetItemLabelText().Contains(search) == true ) {
			// unbind event
			if ( mi->GetItemLabelText().Contains("Save Perspective") == true )	mf->Unbind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::savePerspective, mf, mi->GetId());
			else																mf->Unbind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::loadPerspective, mf, mi->GetId());
			
			// remove menu
			menu->Remove(mi->GetId());
			THE_APP->unregisterMenuItem(mi);
			
			// remove config
			wxASSERT(config != NULL);
			config->DeleteEntry(wxString::Format("/Perspectives/*%s", rmvLabel));
			config->DeleteEntry(wxString::Format("/Perspectives/*%s.PaneList", rmvLabel));
		}
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncPerspective::renameUserPerspective() {
///////////////////////////////////////////////////////////////////
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	wxArrayString items;
	if ( getAllUserPerspectiveNamesFromMenuLabels(items) == false ) {
		wxString msg("No user perspectives available");
		wxMessageDialog eDlg(mf, msg, _T("Rename User Perspective failed . . ."), wxOK|wxCENTRE|wxICON_ERROR);
		eDlg.ShowModal();
		return false;
	}
	
	// determine old name
	wxSingleChoiceDialog lDlg(mf, "Select perspective to rename:", "Rename User Perspective . . .", items);
	
	if ( lDlg.ShowModal() != wxID_OK )
		return false; 
	
	// check content
	wxString oldLabel(lDlg.GetStringSelection());
	oldLabel.Trim(true).Trim(false);
	if ( oldLabel.IsEmpty() == true )
		return false;
	
	// determine new name
	wxString newLabel;
	do {
		wxTextEntryDialog nDlg(mf, "New name:", "Rename User Perspective . . .", "");
		nDlg.SetMaxLength(32);
		nDlg.SetTextValidator(wxFILTER_ASCII);
		nDlg.SetValue(oldLabel);
		
		if ( nDlg.ShowModal() != wxID_OK  )
			return false;
		
		// check content
		newLabel.assign(nDlg.GetValue());
		newLabel.Trim(true).Trim(false);
		if ( newLabel.IsEmpty() == true )
			return false;
		
		newLabel.Replace("'", "", true);
		newLabel.Replace("-", "", true);
		
		if ( checkIfPerspectiveAlreadyExists(newLabel) == true ) {
			wxString msg("A user perspectives with this name already exists. Choose a different name.");
			wxMessageDialog eDlg(mf, msg, _T("Rename User Perspective failed . . ."), wxOK|wxCENTRE|wxICON_ERROR);
			eDlg.ShowModal();
		}
		
	} while ( checkIfPerspectiveAlreadyExists(newLabel) == true );
	
	// rename
	return renameUserPerspective(oldLabel, newLabel);
}
/////////////////////////////////////////////////////////////////////
bool CncPerspective::loadPerspective(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	wxString id(wxString::Format("%s/%s", CncPerspective::getConfigGroupName(), name));
	wxString perspective;
	
	config->Read(id, &perspective, "");
	bool ret = (perspective.IsEmpty() == false);
	
	if ( ret == false )	mf->viewAllAuiPanes();
	else				mf->m_auimgrMain->LoadPerspective(perspective);
	
	mf->decorateViewMenu();
	
	return ret;
}
/////////////////////////////////////////////////////////////////////
void CncPerspective::savePerspective(const wxString& name, bool withQuestion) {
/////////////////////////////////////////////////////////////////////
	wxASSERT(config);
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	if ( withQuestion == true ) {
		wxString msg(wxString::Format("Do you really want to update the '%s' perspective?", name));
		wxMessageDialog dlg(mf, msg, _T("Perspective save. . . "), wxOK|wxCANCEL|wxCENTRE|wxICON_QUESTION);
		
		if ( dlg.ShowModal() != wxID_OK ) 
			return;
	}
		
	wxString id(wxString::Format("/%s/%s", CncPerspective::getConfigGroupName(), name));
	config->Write(id, mf->m_auimgrMain->SavePerspective());
	
	// additional store a list of shown panes
	wxAuiPaneInfoArray panes = mf->m_auimgrMain->GetAllPanes();
	wxString list;
	for (unsigned int i = 0; i < panes.GetCount(); ++i) {
		if ( panes.Item(i).window->IsShown() )
			list.append(wxString::Format("%s|", panes.Item(i).name));
	}
	
	config->Write(wxString::Format("%s%s", id, CncPerspective::getPaneListSuffix()), list);
}
/////////////////////////////////////////////////////////////////////
void CncPerspective::ensureRunPerspectiveMinimal() {
/////////////////////////////////////////////////////////////////////
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	if ( mf->m_winMonitorView->IsShown() == false )
		ensureAllPanesFromPerspectiveAreShown("Run");
}
/////////////////////////////////////////////////////////////////////
void CncPerspective::ensureDebugPerspectiveMinimal() {
/////////////////////////////////////////////////////////////////////
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	if ( mf->m_winMonitorView->IsShown() == false || mf->m_debuggerView->IsShown() == false )
		ensureAllPanesFromPerspectiveAreShown("Debug");
}
/////////////////////////////////////////////////////////////////////
void CncPerspective::ensureAllPanesFromPerspectiveAreShown(const wxString& name) {
/////////////////////////////////////////////////////////////////////
	MainFrame* mf = GBL_CONFIG->getTheApp();
	
	wxString id(wxString::Format("%s/%s%s", CncPerspective::getConfigGroupName(), name, CncPerspective::getPaneListSuffix()));
	wxString paneListSuffix;
	
	config->Read(id, &paneListSuffix, "");
	if ( paneListSuffix.IsEmpty() == false ) {
		
		wxStringTokenizer tokenizer(paneListSuffix, "|");
		while ( tokenizer.HasMoreTokens() ) {
			wxString name = tokenizer.GetNextToken();
			
			name.Trim(true).Trim(false);
			if ( name.IsEmpty() == false ) {
				mf->showAuiPane(name, false);
			}
		}
		
		mf->decorateViewMenu();
		mf->GetAuimgrMain()->Update();
	}
}

