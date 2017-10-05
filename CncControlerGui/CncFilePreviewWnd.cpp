#include <iostream>
#include <wx/filename.h>
#include "GlobalFunctions.h"
#include "CncFilePreviewWnd.h"

///////////////////////////////////////////////////////////////////
CncFilePreviewWnd::CncFilePreviewWnd(wxWindow* parent)
: CncFilePreviewWndBase(parent)
, preview(NULL)
///////////////////////////////////////////////////////////////////
{
	installContent();
	previewFile("about:blank", PT_SVG);
}
///////////////////////////////////////////////////////////////////
CncFilePreviewWnd::~CncFilePreviewWnd() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncFilePreviewWnd::installContent() {
///////////////////////////////////////////////////////////////////
	preview = new CncFilePreview(this);
	GblFunc::replaceControl(m_previewPlaceHolder, preview);
}
///////////////////////////////////////////////////////////////////
CncFilePreviewWnd::PreviewType CncFilePreviewWnd::autoDetectPreviewType(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	//todo
	return PT_AUTO;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreviewWnd::previewFile(const wxString& fileName, PreviewType pt) {
///////////////////////////////////////////////////////////////////
	wxFileName fn(fileName);
	if ( fn.Exists() == false ) {
		std::cerr << "CncFilePreviewWnd::previewFile: File: '" << fileName << "' doesn't exists!" << std::endl;
		return false;
	}
	
	m_previewFileName->SetValue(fileName);
	m_previewFileName->SetToolTip(fileName);
	
	if ( pt == PT_AUTO )
		pt = autoDetectPreviewType(fileName);
	
	bool ret = false;
	switch ( pt ) {
		case PT_AUTO: 	std::cerr << "CncFilePreviewWnd::previewFile: Cant detect file type for: '" << fileName << "'!" << std::endl;
						m_previewFileType->SetValue("???");
						ret = false;
						break;
						
		case PT_SVG:	ret = preview->selectSVGPreview(fileName);
						m_previewFileType->SetValue("SVG");
						break;
						
		case PT_GCODE:	ret = preview->selectGCodePreview(fileName);
						m_previewFileType->SetValue("GCODE");
						break;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncFilePreviewWnd::onClose(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	Show(false);
}
