#include <iostream>
#include <wx/string.h>
#include <wx/event.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include "wxCrafterImages.h"
#include "CncToolMagazine.h"

const unsigned int TM_COL_ID			= 1;
const unsigned int TM_COL_TYPE			= 2;
const unsigned int TM_COL_DIAMETER		= 3;
const unsigned int TM_COL_LENGTH		= 4;
const unsigned int TM_COL_OFFSET		= 5;
const unsigned int TM_COL_COMMENT		= 6;

const char* defaultTool					= "-1";
////////////////////////////////////////////////////////////////////
// app defined event table

wxBEGIN_EVENT_TABLE(CncToolMagazine, CncToolMagazineBase)
	EVT_COMMAND(wxID_ANY, wxEVT_CONFIG_UPDATE_NOTIFICATION, CncToolMagazine::configurationUpdated)
wxEND_EVENT_TABLE()
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
CncToolMagazine::CncToolMagazine(wxWindow* parent)
: CncToolMagazineBase(parent)
, lastSelectedItem(-1)
, insertState(false)
////////////////////////////////////////////////////////////////////////////
{
	// decorate
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibConfig().Bitmap("BMP_TOOL_MAGAZINE_ENTRY"));
	
	m_toolMagazine->SetImageList(imageList, wxIMAGE_LIST_SMALL);
	m_toolMagazine->AppendColumn("", 			wxLIST_FORMAT_RIGHT , 24);
	m_toolMagazine->AppendColumn("ID", 			wxLIST_FORMAT_RIGHT , wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Type", 		wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Diameter", 	wxLIST_FORMAT_RIGHT,  wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Length", 		wxLIST_FORMAT_RIGHT,  wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Offset", 		wxLIST_FORMAT_RIGHT,  wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Comment", 	wxLIST_FORMAT_LEFT,   250);
	
	wxFloatingPointValidator<float> idVal(0, NULL, wxNUM_VAL_DEFAULT );
	idVal.SetRange(0, 999);
	m_toolMagazineId->SetValidator(idVal);

	wxFloatingPointValidator<float> doubleValidator(3, NULL, wxNUM_VAL_DEFAULT );
	doubleValidator.SetRange(0.0, 25.0);
	m_toolMagazineDiameter->SetValidator(doubleValidator);
	
	doubleValidator.SetRange(0.0, 60.0);
	m_toolMagazineLength->SetValidator(doubleValidator);
	
	doubleValidator.SetRange(0.0, 60.0);
	m_toolMagazineOffset->SetValidator(doubleValidator);
	
	getToolMagazineConfig();
	selectedItem(0);
	
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	if ( cc != NULL ) 
		cc->registerWindowForConfigNotification(this);
}
////////////////////////////////////////////////////////////////////////////
CncToolMagazine::~CncToolMagazine() {
////////////////////////////////////////////////////////////////////////////
	m_toolMagazine->DeleteAllItems();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::getToolMagazineConfig() {
////////////////////////////////////////////////////////////////////////////
	m_toolMagazine->DeleteAllItems();
	m_cbDefaultMappedTo->Clear();
	
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	if ( cc != NULL ) {
		
		CncConfig::ToolMagazine& tm = cc->getToolMagazine();
		for ( auto it = tm.begin(); it != tm.end(); ++it) {
			int id = it->first;
			
			if ( id >= TOOL_MAGAZINE_MIN_ID && id <= TOOL_MAGAZINE_MAX_ID ) {
				CncConfig::ToolMagazineEntry entry = it->second;
				
				unsigned int index = m_toolMagazine->GetItemCount();
				
				m_toolMagazine->InsertItem(index, "",  0);
				m_toolMagazine->SetItem(index, TM_COL_ID, 			wxString::Format("%d", 		id));
				m_toolMagazine->SetItem(index, TM_COL_TYPE, 		wxString::Format("%s", 		entry.type));
				m_toolMagazine->SetItem(index, TM_COL_DIAMETER, 	wxString::Format("%.3lf", 	entry.diameter));
				m_toolMagazine->SetItem(index, TM_COL_LENGTH, 		wxString::Format("%.3lf", 	entry.length));
				m_toolMagazine->SetItem(index, TM_COL_OFFSET, 		wxString::Format("%.3lf", 	entry.offset));
				m_toolMagazine->SetItem(index, TM_COL_COMMENT, 		wxString::Format("%s", 		entry.comment));
				
				m_cbDefaultMappedTo->Append(wxString::Format("%d", id));
			}
		}
	}
	
	// add default tool - on demand
	if ( checkIfIdAlreadyExists(-1) == false ) {
		m_toolMagazine->InsertItem(0, "",  0);
		m_toolMagazine->SetItem(0, TM_COL_ID, 			defaultTool);
		m_toolMagazine->SetItem(0, TM_COL_TYPE, 		"PEN");
		m_toolMagazine->SetItem(0, TM_COL_DIAMETER, 	"0.000");
		m_toolMagazine->SetItem(0, TM_COL_LENGTH, 		"0.000");
		m_toolMagazine->SetItem(0, TM_COL_OFFSET, 		"0.000");
		m_toolMagazine->SetItem(0, TM_COL_COMMENT, 		"Default Tool");
		
		m_cbDefaultMappedTo->Append(defaultTool);
	}
	
	// tool magazine parameter
	if ( cc != NULL ) {
		CncConfig::ToolMagazineParameter& tmp = cc->getToolMagazineParameter();
	
		m_cbDefaultToolUsage->SetValue(tmp.useDefaultTool);
		
		if ( tmp.defaultMappedTo.IsEmpty() ) tmp.defaultMappedTo = defaultTool;
		m_cbDefaultMappedTo->SetStringSelection(tmp.defaultMappedTo);
	}
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::setToolMagazineConfig() {
////////////////////////////////////////////////////////////////////////////
	CncConfig* cc = CncConfig::getGlobalCncConfig();
	if ( cc == NULL )
		return;
		
	// tool magazine parameter
	CncConfig::ToolMagazineParameter& tmp = cc->getToolMagazineParameter();
	tmp.useDefaultTool 	= m_cbDefaultToolUsage->GetValue();
	tmp.defaultMappedTo	= m_cbDefaultMappedTo->GetStringSelection();
	if ( tmp.defaultMappedTo.IsEmpty() ) tmp.defaultMappedTo	= defaultTool;
	
	// tool magazine
	CncConfig::ToolMagazine& tm = cc->getToolMagazine();
	tm.clear();
	
	for (int i=0; i<m_toolMagazine->GetItemCount(); i++ )  {
		CncConfig::ToolMagazineEntry entry;
		wxString cell;
		
		//type
		entry.type.assign(m_toolMagazine->GetItemText(i, TM_COL_TYPE));
		
		// diameter
		cell.assign(m_toolMagazine->GetItemText(i, TM_COL_DIAMETER));
		cell.ToDouble(&entry.diameter);
		
		// length
		cell.assign(m_toolMagazine->GetItemText(i, TM_COL_LENGTH));
		cell.ToDouble(&entry.length);
		
		// offset
		cell.assign(m_toolMagazine->GetItemText(i, TM_COL_OFFSET));
		cell.ToDouble(&entry.offset);
		
		// comment
		entry.comment.assign(m_toolMagazine->GetItemText(i, TM_COL_COMMENT));
		
		// id
		cell.assign(m_toolMagazine->GetItemText(i, TM_COL_ID));
		long id; cell.ToLong(&id);
		
		tm[id] = entry;
		
	}
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::configurationUpdated(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	getToolMagazineConfig();
	//std::clog << "CncToolMagazine::configurationUpdated(wxCommandEvent& event)" << std::endl;
}
////////////////////////////////////////////////////////////////////////////
bool CncToolMagazine::checkIfIdAlreadyExists(const int newId) {
////////////////////////////////////////////////////////////////////////////
	for (int i=0; i<m_toolMagazine->GetItemCount(); i++ )  {
		wxString cell(m_toolMagazine->GetItemText(i, TM_COL_ID));
		long id; cell.ToLong(&id);
		
		if ( newId == id )
			return true;
	}
	
	return false;
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::setInsertState(bool state) {
////////////////////////////////////////////////////////////////////////////
	insertState = state;
	m_btToolMagazineEdit->SetLabel(( state == true ? "Insert" : "Update" ));
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::completeTool(unsigned int index) {
////////////////////////////////////////////////////////////////////////////
	// deselect all items
	for ( int i=0; i<m_toolMagazine->GetItemCount(); i++)
		m_toolMagazine->SetItemState(i, 0, wxLIST_STATE_SELECTED);
		
	// select the new one
	selectedItem(index);
	setInsertState(true);
	
	m_toolMagazine->Enable(false);
	m_btToolMagazineAdd->Enable(false);
	m_btToolMagazineDuplicate->Enable(false);
	m_btToolMagazineRemove->Enable(false);
	
	m_btToolMagazineEdit->Enable(true);
	m_btToolMagazineCancle->Enable(true);
	
	m_toolMagazineId->Enable(true);
	m_toolMagazineType->Enable(true);
	m_toolMagazineComment->Enable(true);
	
	// do this after m_toolMagazineType is updated
	enableInputFields();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::addTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	unsigned int index = m_toolMagazine->GetItemCount();
	m_toolMagazine->InsertItem(index, "",  0);
	m_toolMagazine->SetItem(index, TM_COL_ID, 			"");
	m_toolMagazine->SetItem(index, TM_COL_TYPE, 		m_toolMagazineType->GetStringSelection());
	m_toolMagazine->SetItem(index, TM_COL_DIAMETER, 	"0.000");
	m_toolMagazine->SetItem(index, TM_COL_LENGTH, 		"0.000");
	m_toolMagazine->SetItem(index, TM_COL_OFFSET, 		"0.000");
	m_toolMagazine->SetItem(index, TM_COL_COMMENT, 		"");
	
	completeTool(index);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::duplicateTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	if ( lastSelectedItem < 0 )
		addTool(event);
	
	unsigned int index = m_toolMagazine->GetItemCount();
	m_toolMagazine->InsertItem(index, "",  0);
	m_toolMagazine->SetItem(index, TM_COL_ID, 			"");
	m_toolMagazine->SetItem(index, TM_COL_TYPE, 		m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_TYPE));
	m_toolMagazine->SetItem(index, TM_COL_DIAMETER, 	m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_DIAMETER));
	m_toolMagazine->SetItem(index, TM_COL_LENGTH, 		m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_LENGTH));
	m_toolMagazine->SetItem(index, TM_COL_OFFSET, 		m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_OFFSET));
	m_toolMagazine->SetItem(index, TM_COL_COMMENT, 		m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_COMMENT));
	
	completeTool(index);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::editTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	unsigned int index = lastSelectedItem;
	wxString cell;
	cell.assign(m_toolMagazineId->GetValue());
	if ( cell.IsEmpty() ) {
		cnc::trc.logError(wxString::Format("Tool Id can't be empty!"));
		return;
	}
	
	long id; cell.ToLong(&id);
	if ( insertState == true && checkIfIdAlreadyExists(id) == true ) {
		cnc::trc.logError(wxString::Format("Tool with Id '%ld' already exists!", id));
		return;
	}
	
	wxString ID(wxString::Format("%ld", id));
	
	m_toolMagazine->SetItem(index, TM_COL_ID, 			ID);
	m_toolMagazine->SetItem(index, TM_COL_TYPE, 		m_toolMagazineType->GetStringSelection());
	m_toolMagazine->SetItem(index, TM_COL_DIAMETER, 	m_toolMagazineDiameter->GetValue());
	m_toolMagazine->SetItem(index, TM_COL_LENGTH, 		m_toolMagazineLength->GetValue());
	m_toolMagazine->SetItem(index, TM_COL_OFFSET, 		m_toolMagazineOffset->GetValue());
	m_toolMagazine->SetItem(index, TM_COL_COMMENT, 		m_toolMagazineComment->GetValue());
	
	m_toolMagazineId->Enable(false);
	setInsertState(false);
	setToolMagazineConfig();
	
	if ( m_cbDefaultMappedTo->FindString(ID) < 0  )
		m_cbDefaultMappedTo->Append(ID);
	
	m_toolMagazine->Enable(true);
	m_btToolMagazineEdit->Enable(true);
	m_btToolMagazineAdd->Enable(true);
	m_btToolMagazineDuplicate->Enable(true);
	m_btToolMagazineRemove->Enable(true);
	m_btToolMagazineCancle->Enable(false);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::clickUseDefaultTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	// tool magazine parameter
	wxString lastMappedTo = m_cbDefaultMappedTo->GetStringSelection();
	
	// select mapped to  . . .
	int sel = m_cbDefaultMappedTo->FindString(lastMappedTo);
	
	if ( sel >= 0 )	m_cbDefaultMappedTo->Select(sel);
	else			m_cbDefaultMappedTo->SetStringSelection(defaultTool);
	
	setToolMagazineConfig();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::selectDefaultToolMappedTo(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	clickUseDefaultTool(event);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::cancel(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	unsigned int index = m_toolMagazine->GetItemCount() - 1;
	if ( index > 0 )
		m_toolMagazine->DeleteItem(index);

	m_toolMagazine->Enable(true);
	
	selectedItem(0);
	setInsertState(false);
	setToolMagazineConfig();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::removeTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	if ( lastSelectedItem < 0 || lastSelectedItem > m_toolMagazine->GetItemCount() - 1 )
		return;
	
	wxString ID = m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_ID);
	m_toolMagazine->DeleteItem(lastSelectedItem);
	setToolMagazineConfig();
	
	if ( m_cbDefaultMappedTo->FindString(ID) > 0  )
		m_cbDefaultMappedTo->Delete(m_cbDefaultMappedTo->FindString(ID));
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::selectedTool(wxListEvent& event) {
////////////////////////////////////////////////////////////////////////////
	unsigned int index = event.m_itemIndex;
	lastSelectedItem = index;
	
	m_toolMagazineId->Enable(false);
	
	m_toolMagazineComment->Enable(index != 0);
	m_btToolMagazineRemove->Enable(index != 0);
	
	m_toolMagazineType->Enable(true);
	
	m_btToolMagazineEdit->Enable(true);
	m_btToolMagazineAdd->Enable(true);
	m_btToolMagazineDuplicate->Enable(true);
	m_btToolMagazineCancle->Enable(false);
	
	m_toolMagazineId->SetValue(m_toolMagazine->GetItemText(index, TM_COL_ID));
	m_toolMagazineDiameter->SetValue(m_toolMagazine->GetItemText(index, TM_COL_DIAMETER));
	m_toolMagazineLength->SetValue(m_toolMagazine->GetItemText(index, TM_COL_LENGTH));
	m_toolMagazineOffset->SetValue(m_toolMagazine->GetItemText(index, TM_COL_OFFSET));
	m_toolMagazineComment->SetValue(m_toolMagazine->GetItemText(index, TM_COL_COMMENT));
	m_toolMagazineType->SetStringSelection(m_toolMagazine->GetItemText(index, TM_COL_TYPE));
	
	// do this after m_toolMagazineType is updated
	enableInputFields();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::selectedItem(const unsigned int index) {
////////////////////////////////////////////////////////////////////////////
	m_toolMagazine->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::selectType(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	enableInputFields();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::enableInputFields() {
////////////////////////////////////////////////////////////////////////////
	wxString sel(m_toolMagazineType->GetStringSelection());
	if ( sel == "PEN" ) {
		m_toolMagazineDiameter->SetValue("0.000");
		m_toolMagazineLength->SetValue("0.000");
		m_toolMagazineOffset->SetValue("0.000");
	} else {
		if ( lastSelectedItem >= 0 ) {
			m_toolMagazineDiameter->SetValue(m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_DIAMETER));
			m_toolMagazineLength->SetValue(m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_LENGTH));
			m_toolMagazineOffset->SetValue(m_toolMagazine->GetItemText(lastSelectedItem, TM_COL_OFFSET));
		}
	}
	
	m_toolMagazineDiameter->Enable(sel != "PEN");
	m_toolMagazineLength->Enable(sel != "PEN");
	m_toolMagazineOffset->Enable(sel != "PEN");
}
