#include <iostream>
#include <wx/dir.h>
#include <wx/filename.h>
#include "MainFrame.h"
#include "CncFileView.h"

enum FileListImage {
	FTI_FOLDER_UP = 0,
	FTI_FOLDER    = 1,
	FTI_FILE      = 2,
	FTI_ERROR     = 3
};
const char* ALL_FILES 		= "*.*";
const char* SVG_FILES 		= "*.svg";
const char* GCODE_FILES 	= "*.gcode";

/////////////////////////////////////////////////////////////////
CncFileView::CncFileView(MainFrame* parent)
: CncFileViewBase(parent)
, wxDirTraverser()
, defaultPath("")
, filterList()
, mainFrame(parent)
, avoidSelectListEvent(false)
, lastSelection()
/////////////////////////////////////////////////////////////////
{
	// the image list will be deletet by the list control 
	wxImageList* imageList = new wxImageList(16, 16);
	imageList->Add(ImageLibFile().Bitmap("BMP_FOLDER_UP"));
	imageList->Add(ImageLibFile().Bitmap("BMP_FOLDER"));
	imageList->Add(ImageLibFile().Bitmap("BMP_FILE"));
	imageList->Add(ImageLibFile().Bitmap("BMP_ERROR"));
	
	m_fileList->SetImageList(imageList, wxIMAGE_LIST_SMALL);
	m_fileList->AppendColumn("Workarea:", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
	
	filterList.push_back(ALL_FILES);
	filterList.push_back(SVG_FILES);
	filterList.push_back(GCODE_FILES);
	
	for ( auto it = filterList.begin(); it != filterList.end(); ++it )
		m_filterExtention->Append(*it);
		
	m_filterExtention->Select(0);
}
/////////////////////////////////////////////////////////////////
CncFileView::~CncFileView() {
/////////////////////////////////////////////////////////////////
	filterList.clear();
}
/////////////////////////////////////////////////////////////////
bool CncFileView::makePathValid(wxString& p) {
/////////////////////////////////////////////////////////////////
	wxFileName fn(p);
	if ( fn.Exists() == false || fn.MakeAbsolute() == false) {
		p.assign(wxFileName::GetHomeDir());
		if ( p.Last() != wxFileName::GetPathSeparator() ) 
			p.append(wxFileName::GetPathSeparator());
			
		return false;
	}
	
	if ( p.Last() != wxFileName::GetPathSeparator() ) {
		wxFileName tmp(wxString(p) + wxFileName::GetPathSeparator());
		// if p is a directory ensure it ends with a separator
		if ( tmp.Exists() ) {
			p.append(wxFileName::GetPathSeparator());
			return true;
		} 
		
		// if p is a file return the path part of p
		p.assign(fn.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
		return true;
	}
	
	// noting to do
	return true;
}
/////////////////////////////////////////////////////////////////
void CncFileView::setDefaultPath(const wxString& dirName) {
/////////////////////////////////////////////////////////////////
	defaultPath.assign(dirName);
	if ( makePathValid(defaultPath) == false )
		std::cerr << "CncFileView::setDefaultPath: Invalid path: " << dirName << std::endl;
}
/////////////////////////////////////////////////////////////////
void CncFileView::selectDefaultPath() {
/////////////////////////////////////////////////////////////////
	openDirectory(defaultPath);
}
/////////////////////////////////////////////////////////////////
wxDirTraverseResult CncFileView::OnFile(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	wxFileName fn(fileName);
	if ( m_filterExtention->GetStringSelection() == ALL_FILES ) {
		// add all files 
		m_fileList->InsertItem(m_fileList->GetItemCount(), fn.GetFullName(), FileListImage::FTI_FILE);
	} else {
		// consider the filter
		wxString ext(m_filterExtention->GetStringSelection());
		// a bad hack to remove "*. from the beginning
		ext = ext.SubString(2, ext.length() -1 );
		if ( fn.GetExt() == ext )
			m_fileList->InsertItem(m_fileList->GetItemCount(), fn.GetFullName(), FileListImage::FTI_FILE);
	}
	
	return wxDIR_CONTINUE;
}
/////////////////////////////////////////////////////////////////
wxDirTraverseResult CncFileView::OnDir(const wxString& dirName) {
/////////////////////////////////////////////////////////////////
	wxFileName fn(dirName);
	m_fileList->InsertItem(m_fileList->GetItemCount(),fn.GetFullName(), FileListImage::FTI_FOLDER);
	
	return wxDIR_IGNORE;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::openDirectory(const wxString& dirName) {
/////////////////////////////////////////////////////////////////
	m_fileList->DeleteAllItems();
	m_fileList->InsertItem(m_fileList->GetItemCount(), "..", FileListImage::FTI_FOLDER_UP);
	
	wxString dn(dirName);
	makePathValid(dn);
	
	wxDir dir(dn);
	if ( !dir.IsOpened() ) {
		std::cerr << "CncFileView::openDirectory: Cant open directory: " << dirName << std::endl;
		m_fileList->InsertItem(m_fileList->GetItemCount(), dirName, FileListImage::FTI_ERROR);
		return false;
	}
	
	m_currentDirectory->SetValue(dn);
	m_currentDirectory->SetToolTip(dn);
	m_currentDirectory->SetInsertionPointEnd();
	
	dir.Traverse(*this, wxEmptyString, wxDIR_DIRS | wxDIR_FILES | wxDIR_DOTDOT);
	
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::openFile(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	wxASSERT(mainFrame);
	mainFrame->openFileFromFileManager(fileName);
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::previewFile(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	wxASSERT(mainFrame);
	mainFrame->openMainPreview(fileName);
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::selectFileInList(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	wxFileName fn(fileName);
	if ( fn.Exists() ) {
		// first open the directory
		openDirectory(fn.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
		
		// manage the filter
		wxString ext("*.");
		ext.append(fn.GetExt());
		if ( m_filterExtention->SetStringSelection(ext) == false )
			m_filterExtention->SetStringSelection(ALL_FILES);
		
		// search the file
		long idx = m_fileList->FindItem(-1, fn.GetFullName());
		if ( idx == wxNOT_FOUND ) {
			return false;
		}
		
		// select the file
		avoidSelectListEvent = true;
		m_fileList->SetItemState(idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		m_fileList->EnsureVisible(idx);
		return true;
		
	}
	
	return false;
}
/////////////////////////////////////////////////////////////////
void CncFileView::fileListLeave(wxMouseEvent& event) {
	wxASSERT(mainFrame);
	
	if ( mainFrame->GetMainViewBook()->GetSelection() != (int)MainBookSelection::PREVIEW_PANEL )
		return;
		
	if ( mainFrame->GetKeepFileManagerPreview()->IsChecked() == true )
		return;
		
	mainFrame->selectMainBookSourcePanel();
}
/////////////////////////////////////////////////////////////////
void CncFileView::fileListActivated(wxListEvent& event) {
/////////////////////////////////////////////////////////////////
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;
	
	if ( m_fileList->GetItem(info) == false )
		return;
		
	wxString itemName = info.GetText();
	int itemTyp       = info.GetImage();
		
	wxFileName curDir(m_currentDirectory->GetValue());
	wxString name;
	
	switch ( (FileListImage)itemTyp ) {

		case FTI_FILE: 		name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
							name.append(itemName);
							openFile(name);
							break;
							
		default:			// do nothing
							break;
	}
}
/////////////////////////////////////////////////////////////////
void CncFileView::fileListSelected(wxListEvent& event) {
/////////////////////////////////////////////////////////////////
	// this appears if the selection doesn't came from a user action
	if ( avoidSelectListEvent == true ) {
		avoidSelectListEvent = false;
		event.Skip();
		return;
	}

	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;
	
	if ( m_fileList->GetItem(info) == false )
		return;
		
	wxString itemName = info.GetText();
	int itemTyp       = info.GetImage();
	
	wxFileName curDir(m_currentDirectory->GetValue());
	wxString name;
	
	switch ( (FileListImage)itemTyp ) {

		case FTI_FOLDER_UP:	if ( curDir.GetDirCount() > 0 )
								curDir.RemoveLastDir();
							name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
							
							lastSelection = name;
							openDirectory(name);
							break;
							
		case FTI_FOLDER:	name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
							name.append(itemName);
							
							lastSelection = name;
							openDirectory(name);
							break;
							
		case FTI_FILE: 		name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
							name.append(itemName);
							
							// avoid a second preview on activate
							if ( lastSelection == name )
								break;
								
							lastSelection = name;
							previewFile(name);
							break;
							
		case FTI_ERROR:		// do nothing
							break;
	}
}
/////////////////////////////////////////////////////////////////
void CncFileView::aFolderUp(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	wxFileName curDir(m_currentDirectory->GetValue());
	wxString name;
	
	if ( curDir.GetDirCount() > 0 )
		curDir.RemoveLastDir();
							
	name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
	openDirectory(name);
}
/////////////////////////////////////////////////////////////////
void CncFileView::update() {
/////////////////////////////////////////////////////////////////
	openDirectory(m_currentDirectory->GetValue());
}
/////////////////////////////////////////////////////////////////
void CncFileView::refresh(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	update();
}
/////////////////////////////////////////////////////////////////
void CncFileView::selectFilter(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	update();
}
/////////////////////////////////////////////////////////////////
void CncFileView::selectDefault(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	openDirectory(defaultPath);
}
/////////////////////////////////////////////////////////////////
void CncFileView::selectNewTemplate(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	GBL_CONFIG->getTheApp()->newTemplate(event);
}
/////////////////////////////////////////////////////////////////
void CncFileView::selectOpenTemplate(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	GBL_CONFIG->getTheApp()->openTemplate(event);
}
