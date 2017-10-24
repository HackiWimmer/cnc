#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include "CncToolMagazine.h"

////////////////////////////////////////////////////////////////////////////
CncToolMagazine::CncToolMagazine(wxWindow* parent)
: CncToolMagazineBase(parent)
, lastSelectedItem(-1)
////////////////////////////////////////////////////////////////////////////
{
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibFile().Bitmap("BMP_LRU_FILE"));
	
	m_toolMagazine->SetImageList(imageList, wxIMAGE_LIST_SMALL);
	m_toolMagazine->AppendColumn("", 			wxLIST_FORMAT_RIGHT , 24);
	m_toolMagazine->AppendColumn("ID", 			wxLIST_FORMAT_RIGHT , wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Type", 		wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Diameter", 	wxLIST_FORMAT_RIGHT,  wxLIST_AUTOSIZE);
	m_toolMagazine->AppendColumn("Comment", 	wxLIST_FORMAT_LEFT,   250);
	
	m_toolMagazine->InsertItem(0, "",  0);
	m_toolMagazine->SetItem(0, 1, "-1");
	m_toolMagazine->SetItem(0, 2, "PEN");
	m_toolMagazine->SetItem(0, 3, "0.0");
	m_toolMagazine->SetItem(0, 4, "Default Tool");
	
	wxFloatingPointValidator<float> idVal(0, NULL, wxNUM_VAL_DEFAULT );
	idVal.SetRange(0, 999);
	m_toolMagazineId->SetValidator(idVal);

	wxFloatingPointValidator<float> diaVal(3, NULL, wxNUM_VAL_DEFAULT );
	diaVal.SetRange(0.0, 25.0);
	m_toolMagazineDiameter->SetValidator(diaVal);
	
	m_toolMagazine->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	itemSelected(0);
}
////////////////////////////////////////////////////////////////////////////
CncToolMagazine::~CncToolMagazine() {
////////////////////////////////////////////////////////////////////////////
	m_toolMagazine->DeleteAllItems();
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::addTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	unsigned int index = m_toolMagazine->GetItemCount();
	m_toolMagazine->InsertItem(index, "",  0);
	m_toolMagazine->SetItem(index, 1, "");
	m_toolMagazine->SetItem(index, 2, m_toolMagazineType->GetStringSelection());
	m_toolMagazine->SetItem(index, 3, "0.0");
	
	for ( int i=0; i<m_toolMagazine->GetItemCount(); i++)
		m_toolMagazine->SetItemState(i, 0, wxLIST_STATE_SELECTED);
		
	m_toolMagazine->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	itemSelected(index);
	
	m_toolMagazine->Enable(false);
	m_btToolMagazineEdit->Enable(true);
	m_btToolMagazineCancle->Enable(true);
	m_btToolMagazineAdd->Enable(false);
	m_btToolMagazineRemove->Enable(false);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::editTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////



	m_toolMagazine->Enable(true);
	m_btToolMagazineEdit->Enable(true);
	m_btToolMagazineAdd->Enable(true);
	m_btToolMagazineRemove->Enable(true);
	m_btToolMagazineCancle->Enable(false);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::cancle(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	m_toolMagazine->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	itemSelected(0);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::removeTool(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////////
	if ( lastSelectedItem < 0 || lastSelectedItem > m_toolMagazine->GetItemCount() - 1 )
		return;
	
	m_toolMagazine->DeleteItem(lastSelectedItem);
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::selectedTool(wxListEvent& event) {
////////////////////////////////////////////////////////////////////////////
	itemSelected(event.m_itemIndex);

	
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 1;
	info.m_mask = wxLIST_MASK_TEXT;
	
	/*
	if ( m_lruList->GetItem(info) )
		openMainPreview(wxString(lruFileList.getFileName(info.m_itemId)));
	*/
}
////////////////////////////////////////////////////////////////////////////
void CncToolMagazine::itemSelected(unsigned int index) {
////////////////////////////////////////////////////////////////////////////
	m_toolMagazineDiameter->Enable(index != 0);
	m_toolMagazineType->Enable(index != 0);
	m_toolMagazineId->Enable(index != 0);
	
	m_btToolMagazineRemove->Enable(index != 0);
	m_btToolMagazineEdit->Enable(index != 0);
	
	m_btToolMagazineAdd->Enable(true);
	m_btToolMagazineCancle->Enable(false);
	
	lastSelectedItem = index;
}

