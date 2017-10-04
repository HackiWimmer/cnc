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
	wxASSERT( m_gcodePreviewPlaceholder );
		
	wxWindow* parent = m_gcodePreviewPlaceholder->GetParent();
	wxSizer* sizer   = m_gcodePreviewPlaceholder->GetContainingSizer();
	
	wxASSERT( parent != NULL );
	wxASSERT( sizer != NULL );

	gcodePreview = new CncGCodePreview(parent, NULL);
	gcodePreview->SetPosition(m_gcodePreviewPlaceholder->GetPosition());

	sizer->Replace(m_gcodePreviewPlaceholder, gcodePreview, true);
	sizer->Layout();
	
	// remove the placeholder
	m_gcodePreviewPlaceholder->Destroy();
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
	
	return true;
}


