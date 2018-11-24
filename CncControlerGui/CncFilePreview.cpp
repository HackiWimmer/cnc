#include "GlobalFunctions.h"
#include "BinaryFileParser.h"
#include "GCodePathHandlerGL.h"
#include "GCodeFileParser.h"
#include "CncFileNameService.h"
#include "CncFilePreview.h"

///////////////////////////////////////////////////////////////////
CncFilePreview::CncFilePreview(wxWindow* parent)
: CncFilePreviewBase(parent)
, gcodePreview(NULL)
///////////////////////////////////////////////////////////////////
{
	installContent();
}
///////////////////////////////////////////////////////////////////
CncFilePreview::~CncFilePreview() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::installContent() {
///////////////////////////////////////////////////////////////////
	gcodePreview = new CncGCodePreview(this);
	GblFunc::replaceControl(m_gcodePreviewPlaceholder, gcodePreview);
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectEmptyPreview() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > 0 );
	m_previewBook->SetSelection(0);
	
	wxString fileName(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoPreviewAvailable.svg"));
	if ( wxFileName::Exists(fileName) == false )
		fileName.assign("about:blank");
	
	m_svgPreview->LoadURL(fileName);
	m_svgPreview->Update();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectSVGPreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > 0 );
	m_previewBook->SetSelection(0);
	
	m_svgPreview->LoadURL(fileName);
	m_svgPreview->Update();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectGCodePreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > 1 );
	m_previewBook->SetSelection(1);
	
	wxFileName fn(fileName);
	if ( fn.Exists() == false ) {
		std::cerr << " CncFilePreview::selectGCodePreview: Invalid file: " << fileName << std::endl;
		return false;
	}
	
	GCodeFileParser gfp(fileName, new GCodePathHandlerGL(gcodePreview));
	gfp.setDisplayWarnings(false);
	return gfp.processRelease();
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectBinaryPreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxString externalFile;
	if ( BinaryFileParser::extractSourceContentAsFile(fileName, externalFile) == false ) {
		std::cerr << "CncFilePreview::selectBinaryPreview(): Can't create preview for file: '" 
				  << fileName
				  << "'" << std::endl;
		return false;
	}
	
	return selectPreview(externalFile);
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectPreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	TemplateFormat tf = cnc::getTemplateFormatFromFileName(fileName);
	
	switch ( tf ) {
		case TplSvg:		selectSVGPreview(fileName);
							break;
							
		case TplGcode:		selectGCodePreview(fileName);
							break;
							
		case TplBinary:		selectBinaryPreview(fileName);
							break;
							
		default:			std::cerr << "CncFilePreview::selectPreview(): No preview registered for: " 
									  << cnc::getTemplateFormatAsString(tf)
									  << std::endl;
	}
	
	return true;
}
