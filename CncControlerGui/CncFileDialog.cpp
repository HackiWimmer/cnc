#include <wx/sizer.h>
#include <wx/event.h>
#include "wxCrafterImages.h"
#include "CncFileDialog.h"

wxBEGIN_EVENT_TABLE(CncFileDialogLRUList, wxListCtrl)
	EVT_LIST_ITEM_SELECTED	(wxID_ANY, 	CncFileDialogLRUList::onSelectListItem)
	EVT_LEFT_DCLICK 		(			CncFileDialogLRUList::onLeftDClick)
	EVT_KEY_DOWN			(			CncFileDialogLRUList::onKeyDown)
wxEND_EVENT_TABLE()

CncFileDialog* glblCncFileDialogPointer = NULL;

/////////////////////////////////////////////////////////////
CncFileDialogLRUList::CncFileDialogLRUList(wxWindow *parent)
: wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT)
, lastSelection(-1)
/////////////////////////////////////////////////////////////
{
	AppendColumn("Last recently used  . . .", 	wxLIST_FORMAT_LEFT , 600);
	
	wxImageList* imageList = new wxImageList(16, 16, true);
	imageList->RemoveAll();
	imageList->Add(ImageLibFile().Bitmap("BMP_LRU_FILE"));
	SetImageList(imageList, wxIMAGE_LIST_SMALL);
}
/////////////////////////////////////////////////////////////
CncFileDialogLRUList::~CncFileDialogLRUList() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
CncFileDialog* CncFileDialogLRUList::getFileDialog() {
/////////////////////////////////////////////////////////////
	#warning
	wxWindow* panel = GetParent();
	return glblCncFileDialogPointer;//panel != NULL ? static_cast<CncFileDialog*>(panel->GetParent()) : NULL ;
}
/////////////////////////////////////////////////////////////
void CncFileDialogLRUList::addFileName(const wxString& fn) {
/////////////////////////////////////////////////////////////
	unsigned int index = GetItemCount();
	InsertItem(index, fn,  0);
	SetItem(index, COL_FILE, fn);
}
/////////////////////////////////////////////////////////////
bool CncFileDialogLRUList::processSelectedItem() {
////////////////////////////////////////////////////////////
	if ( lastSelection < 0 )
		return false;

	std::cout << GetItemText(lastSelection, COL_FILE) << std::endl;
	std::cout << getFileDialog() << std::endl;
	
	CncFileDialog* fd = getFileDialog();
	if ( fd != NULL ) {
		//fd->SetPath(GetItemText(lastSelection, COL_FILE));
		
		#warning
		fd->SetPath("C:\\@Development\\@Projekte\\CNC-Templates\\");
		fd->SetFilename("@NC-Test.gcode");
	}
	
	return true;
}
/////////////////////////////////////////////////////////////
void CncFileDialogLRUList::closeParentDialog(int code) {
/////////////////////////////////////////////////////////////
	CncFileDialog* fd = getFileDialog();
	if ( fd != NULL ) {
		//fd->EndModal(code);
		fd->SetReturnCode(code);
		fd->Show(false);
	}
}
/////////////////////////////////////////////////////////////
void CncFileDialogLRUList::onSelectListItem(wxListEvent& event) {
/////////////////////////////////////////////////////////////
	const long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;

	lastSelection = item;
	processSelectedItem();
}
/////////////////////////////////////////////////////////////
void CncFileDialogLRUList::onLeftDClick(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////
	if ( processSelectedItem() )
		closeParentDialog(wxID_OK);
}
/////////////////////////////////////////////////////////////
void CncFileDialogLRUList::onKeyDown(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////
	if ( processSelectedItem() )
		closeParentDialog(wxID_OK);
}



/////////////////////////////////////////////////////////////
CncFileDialogExtraPanel::CncFileDialogExtraPanel(wxWindow *parent)
: wxPanel(parent)
, lruListCtrl(new CncFileDialogLRUList(this))
/////////////////////////////////////////////////////////////
{
	wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
	sizerTop->Add(lruListCtrl,wxSizerFlags().Centre().Border());
	SetSizerAndFit(sizerTop);
}
/////////////////////////////////////////////////////////////
CncFileDialogExtraPanel::~CncFileDialogExtraPanel() {
/////////////////////////////////////////////////////////////
	//delete lruListCtrl;
}
/////////////////////////////////////////////////////////////
void CncFileDialogExtraPanel::addFileName(const wxString& fn) {
/////////////////////////////////////////////////////////////
	wxASSERT( lruListCtrl != NULL );
	lruListCtrl->addFileName(fn);
}


/////////////////////////////////////////////////////////////
CncFileDialog::CncFileDialog (
	wxWindow *		  	parent,
	const wxString &  	message,
	const wxString &  	defaultDir,
	const wxString &  	defaultFile,
	const wxString &  	wildcard,
	long  				style,
	const wxPoint &  	pos,
	const wxSize &  	size,
	const wxString &  	name 
)
: wxFileDialog(parent, message, defaultDir, defaultFile, wildcard, style, pos, size, name)
, lruList()
/////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
CncFileDialog::~CncFileDialog() {
/////////////////////////////////////////////////////////////
	lruList.clear();
}
/////////////////////////////////////////////////////////////
wxWindow* CncFileDialog::createExtraPanelForFileDialog(wxWindow* parent) {
/////////////////////////////////////////////////////////////
	// this has be always recreated, the ownership will be overtaken by wxFileDialog
	CncFileDialogExtraPanel* extraPanel = new CncFileDialogExtraPanel(parent);
	
	// fill LRU list control
	//CncFileDialog* fileDialog = static_cast<CncFileDialog*>(parent);
	CncFileDialog* fileDialog = glblCncFileDialogPointer ;
	if ( fileDialog != NULL )  {
		const LruList& lruList = fileDialog->getLruListContent();
		for ( auto it = lruList.begin(); it!= lruList.end(); ++it )
			extraPanel->addFileName(*it);
	}
	
	return extraPanel;
}
/////////////////////////////////////////////////////////////
void CncFileDialog::addLruFilename(const wxString& fn) {
/////////////////////////////////////////////////////////////
	lruList.push_back(fn);
}
/////////////////////////////////////////////////////////////
int CncFileDialog::ShowModal() {
/////////////////////////////////////////////////////////////
	if ( lruList.size() > 0 ) 
		;//SetExtraControlCreator( &CncFileDialog::createExtraPanelForFileDialog );
	
	glblCncFileDialogPointer = this;
		const bool ret = wxFileDialog::ShowModal();
	glblCncFileDialogPointer = NULL;
 
	return ret;
}
