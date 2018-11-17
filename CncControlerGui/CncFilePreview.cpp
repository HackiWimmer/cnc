#include "GlobalFunctions.h"
#include "GCodePathHandlerGL.h"
#include "GCodeFileParser.h"
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


