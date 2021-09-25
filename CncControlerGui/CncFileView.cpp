#include <iostream>
#include <wx/dir.h>
#include <wx/app.h>
#include <wx/filename.h>
#include "wxCrafterImages.h"
#include "OSD/CncAsyncKeyboardState.h"
#include "GlobalFunctions.h"
#include "MainFrameProxy.h"
#include "CncFileNameService.h"
#include "CncFileViewLists.h"
#include "CncFileView.h"

const char* ALL_FILES 		= "*.*";
const char* SVG_FILES 		= "*.svg";
const char* GCODE_FILES1 	= "*.gcode";
const char* GCODE_FILES2 	= "*.ngc";
const char* BIN_FILES 		= "*.bct";

#define CFV_PRINT_LOCATION_CTX_FILE			//	CNC_PRINT_LOCATION
#define CFV_PRINT_LOCATION_CTX_SOMETHING	//	CNC_PRINT_LOCATION

/////////////////////////////////////////////////////////////////
CncFileView::CncFileView(wxWindow* parent, bool sd)
: CncFileViewBase(parent)
, wxDirTraverser()
, defaultPath("")
, fileList(NULL)
, filterList()
, staticDir(sd)
, avoidSelectListEvent(false)
, lastSelection()
/////////////////////////////////////////////////////////////////
{
	// File List 
	fileList = new CncFileViewListCtrl(this, wxLC_SINGLE_SEL);
	GblFunc::replaceControl(m_fileListPlaceholder, fileList);
	
	fileList->setObserver(this);
	
	filterList.push_back(ALL_FILES);
	filterList.push_back(SVG_FILES);
	filterList.push_back(GCODE_FILES1);
	filterList.push_back(GCODE_FILES2);
	filterList.push_back(BIN_FILES);
	
	for ( auto it = filterList.begin(); it != filterList.end(); ++it )
		m_filterExtention->Append(*it);
		
	m_filterExtention->Select(0);
	
	if ( staticDir == true ) {
		m_btDirUp->Hide();
		m_btRefresh->Hide();
		m_btDefaultPath->Hide();
		m_btNewTemplate->Hide();
		m_btOpenTemplate->Hide();
		m_filterExtention->Hide();
		m_currentDirectory->Hide();
		Layout();
	}
}
/////////////////////////////////////////////////////////////////
CncFileView::~CncFileView() {
/////////////////////////////////////////////////////////////////
	filterList.clear();
	wxDELETE( fileList );
}
/////////////////////////////////////////////////////////////////
bool CncFileView::Enable(bool enable) {
/////////////////////////////////////////////////////////////////
	m_btDirUp->			Enable(enable);
	m_btRefresh->		Enable(enable);
	m_btDefaultPath->	Enable(enable);
	m_btNewTemplate->	Enable(enable);
	m_btOpenTemplate->	Enable(enable);
	m_btOpenTemplate->	Enable(enable);
	fileList->			Enable(enable);
	
	return true;
	
	//GblFunc::freeze(this, !enable);
	//return CncFileViewBase::Enable(enable); 
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
	const wxFileName fn(fileName);
	const wxString name(fn.GetFullName());
	
	if ( name.IsEmpty() == false )
		fileList->addFileEntry(name, CncFileViewListCtrl::FileListImage::FTI_FILE);
	
	return wxDIR_CONTINUE;
}
/////////////////////////////////////////////////////////////////
wxDirTraverseResult CncFileView::OnDir(const wxString& dirName) {
/////////////////////////////////////////////////////////////////
	if ( staticDir == true )
		return wxDIR_IGNORE;
		
	const wxFileName fn(dirName);
	const wxString name(fn.GetFullName());
	
	if ( name.IsEmpty() == false )
		fileList->addFileEntry(name, CncFileViewListCtrl::FileListImage::FTI_FOLDER);
		
	return wxDIR_IGNORE;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::openDirectory(const wxString& dirName) {
/////////////////////////////////////////////////////////////////
	fileList->deleteAllEntries();
	
	if ( staticDir == false )
		fileList->addFileEntry("..", CncFileViewListCtrl::FileListImage::FTI_FOLDER_UP);
	
	wxString dn(dirName);
	makePathValid(dn);
	
	wxDir dir(dn);
	if ( !dir.IsOpened() ) {
		std::cerr << "CncFileView::openDirectory: Can't open directory: " << dirName << std::endl;
		fileList->addFileEntry(dirName, CncFileViewListCtrl::FileListImage::FTI_ERROR);
		return false;
	}
	
	m_currentDirectory->SetValue(dn);
	m_currentDirectory->SetToolTip(dn);
	m_currentDirectory->SetInsertionPointEnd();
	
	const wxString filespec(m_filterExtention->GetStringSelection());
	dir.Traverse(*this, filespec, wxDIR_DIRS | wxDIR_FILES | wxDIR_DOTDOT);
	
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::openFile(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	APP_PROXY::openFileFromFileManager(fileName);
	return true;
}
/////////////////////////////////////////////////////////////////
bool CncFileView::previewFile(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	APP_PROXY::openMainPreview(fileName);
	return true;
}
/////////////////////////////////////////////////////////////////
void CncFileView::fileListLeave(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////
	APP_PROXY::filePreviewListLeave();
}
/////////////////////////////////////////////////////////////////
void CncFileView::fileListActivated(long item) {
/////////////////////////////////////////////////////////////////
	CFV_PRINT_LOCATION_CTX_FILE

	wxListItem info;
	info.m_itemId = item;
	info.m_col = 0;
	
	if ( fileList->GetItem(info) == false )
		return;
		
	const wxString itemName = info.GetText();
	const int itemTyp       = info.GetImage();
	
	wxFileName curDir(m_currentDirectory->GetValue());
	wxString name;
	
	typedef CncFileViewListCtrl::FileListImage FTI;
	switch ( (FTI)itemTyp ) {
		
		case FTI::FTI_FOLDER_UP:		if ( curDir.GetDirCount() > 0 )
											curDir.RemoveLastDir();

										name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));

										lastSelection = name;
										openDirectory(name);
										break;

		case FTI::FTI_FOLDER:			name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
										name.append(itemName);

										lastSelection = name;
										openDirectory(name);
										break;
		case FTI::FTI_FILE:
		case FTI::FTI_FILE_SELECTED:	name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
										name.append(itemName);

										openFile(name);
										break;
							
		default:						// do nothing
										break;
	}
}
/////////////////////////////////////////////////////////////////
bool CncFileView::selectFileInList(const wxString& fileName) {
/////////////////////////////////////////////////////////////////
	CFV_PRINT_LOCATION_CTX_FILE

	wxFileName fn(fileName);
	if ( fn.Exists() == false )
		return false;
		
	// manage the filter
	const wxString ext(wxString::Format("*.%s", fn.GetExt()));
	
	if ( m_filterExtention->SetStringSelection(ext) == false )
		m_filterExtention->SetStringSelection(ALL_FILES);
	
	// first open the directory
	if ( openDirectory(fn.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME)) == false )
		return false;

	// to avoid the call of fileListSelected()
	avoidSelectListEvent = true;
	// search the file
	avoidSelectListEvent = fileList->selectFileInList(fn.GetFullName());
	
	return avoidSelectListEvent;
}
/////////////////////////////////////////////////////////////////
void CncFileView::fileListSelected(long item) {
/////////////////////////////////////////////////////////////////
	CFV_PRINT_LOCATION_CTX_FILE
	
	// this appears if the selection doesn't came from a user action
	if ( avoidSelectListEvent == true ) { 
		avoidSelectListEvent = false; 
		return; 
	}
	
	wxListItem info;
	info.m_itemId = item;
	info.m_col = 0;
	
	if ( fileList->GetItem(info) == false )
		return;
		
	wxString itemName = info.GetText();
	int itemTyp       = info.GetImage();
	
	wxFileName curDir(m_currentDirectory->GetValue());
	wxString name;
	
	typedef CncFileViewListCtrl::FileListImage FTI;
	switch ( (FTI)itemTyp ) {

		case FTI::FTI_FILE:
		case FTI::FTI_FILE_SELECTED:	name.assign(curDir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
										name.append(itemName);
										
										// avoid a second preview on activate
										if ( lastSelection == name )
											break;
											
										lastSelection = name;
										previewFile(name);
										break;
							
		case FTI::FTI_FOLDER_UP:
		case FTI::FTI_FOLDER:
		case FTI::FTI_ERROR:			// do nothing
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
	APP_PROXY::newTemplate();
}
/////////////////////////////////////////////////////////////////
void CncFileView::selectOpenTemplate(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////
	APP_PROXY::openTemplate();
}


/////////////////////////////////////////////////////////////////
CncTransferFileView::CncTransferFileView(wxWindow* parent, bool staticDir)
: CncFileView	(parent, staticDir)
/////////////////////////////////////////////////////////////////
{
	openDirectory(CncFileNameService::getTransferDir());
	
	wxListItem item;
	getFileView()->GetColumn(CncFileViewListCtrl::COL_FILE, item);
	item.SetText("Transfer area:");
	getFileView()->SetColumn(CncFileViewListCtrl::COL_FILE, item);
	
}
/////////////////////////////////////////////////////////////////
CncTransferFileView::~CncTransferFileView() {
/////////////////////////////////////////////////////////////////
}