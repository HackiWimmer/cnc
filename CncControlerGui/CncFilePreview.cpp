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
, svgPreview(NULL)
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
	
	svgPreview = new CncSvgViewer(this);
	GblFunc::replaceControl(m_svgPreviewPlaceholder, svgPreview);
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectEmptyPreview() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > 0 );
	m_previewBook->SetSelection(0);
	
	wxString fileName(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialPreviewAvailable.svg"));
	if ( wxFileName::Exists(fileName) == false )
		fileName.assign("about:blank");
	
	svgPreview->loadFile(fileName);
	svgPreview->Update();
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectSVGPreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > 0 );
	m_previewBook->SetSelection(0);
	
	svgPreview->loadFile(fileName);
	svgPreview->Update();
	
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
							
		case TplText:		selectEmptyPreview();
							break;
							
		default:			std::cerr << "CncFilePreview::selectPreview(): No preview registered for: " 
									  << cnc::getTemplateFormatAsString(tf)
									  << std::endl;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::activate3DPerspectiveButton(wxButton* bt) {
///////////////////////////////////////////////////////////////////
	static wxColour active(171, 171, 171);
	static wxColour inactive(240, 240, 240);

	m_3D_Top->SetBackgroundColour(inactive);
	m_3D_Bottom->SetBackgroundColour(inactive);
	m_3D_Front->SetBackgroundColour(inactive);
	m_3D_Rear->SetBackgroundColour(inactive);
	m_3D_Left->SetBackgroundColour(inactive);
	m_3D_Right->SetBackgroundColour(inactive);
	m_3D_Perspective1->SetBackgroundColour(inactive);
	m_3D_Perspective2->SetBackgroundColour(inactive);
	m_3D_Perspective3->SetBackgroundColour(inactive);
	m_3D_Perspective4->SetBackgroundColour(inactive);
	
	if ( bt != NULL ) {
		bt->SetBackgroundColour(active);
	}
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::show3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	
	if ( bt == m_3D_Perspective1 ) {
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso1();
		
	} else if ( bt == m_3D_Perspective2 ) {
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso2();
		
	} else if ( bt == m_3D_Perspective3 ) {
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso3();
		
	} else if ( bt == m_3D_Perspective4 ) {
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso4();
		
	}
	
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::showFromBottom3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	gcodePreview->viewBottom();
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::showFromFront3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	gcodePreview->viewFront();
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::showFromLeft3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	gcodePreview->viewLeft();
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::showFromRear3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	gcodePreview->viewRear();
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::showFromRight3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	gcodePreview->viewRight();
	Refresh();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::showFromTop3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	activate3DPerspectiveButton((wxButton*)event.GetEventObject());
	gcodePreview->viewTop();
	Refresh();
}
///////////////////////////////////////////////////////////////////
const char* CncFilePreview::getBlankHtmlPage() {
///////////////////////////////////////////////////////////////////
	wxFileName fn(CncFileNameService::getBlankHtmlPageFileName());
	std::fstream html;
	
	html.open(fn.GetFullPath().c_str().AsChar(), std::ios::out | std::ios::trunc);
	if ( html.is_open() ) {
		
		html << "<HTML>" << std::endl;
		html << "<HEAD>" << std::endl;
		html << "<TITLE>Blank Page</TITLE>" << std::endl;
		html << "</HEAD>" << std::endl;
		html << "<BODY BGCOLOR=\"FFFFFF\">" << std::endl;
		html << "<HR>" << std::endl;
		html << "Default HTML Page" << std::endl;
		html << "<H1>Empty Content (about:blank)</H1>" << std::endl;
		html << "<H2>No further information available</H2>" << std::endl;
		html << "<HR>" << std::endl;
		html << "</BODY>" << std::endl;
		html << "</HTML>" << std::endl;
		
		html.flush();
		html.close();
		
	} else {
		return "about:blank";
	}

	return fn.GetFullPath();
}
///////////////////////////////////////////////////////////////////
const char* CncFilePreview::getErrorHtmlPage(const wxString& errorInfo) {
//////////////////////////////////////////////////////////////////
	wxFileName fn(CncFileNameService::getErrorHtmlPageFileName());
	std::fstream html;
	
	wxString ei(errorInfo);
	if ( errorInfo.IsEmpty() )
		ei = "No further information available";
	
	html.open(fn.GetFullPath().c_str().AsChar(), std::ios::out | std::ios::trunc);
	if ( html.is_open() ) {
		
		html << "<HTML>" << std::endl;
		html << "<HEAD>" << std::endl;
		html << "<TITLE>Error Page</TITLE>" << std::endl;
		html << "</HEAD>" << std::endl;
		html << "<BODY BGCOLOR=\"FFFFFF\">" << std::endl;
		html << "<HR>" << std::endl;
		html << "Default HTML Page" << std::endl;
		html << "<H1>";
		html << ei;
		html << "/H1>" << std::endl;
		html << "<H2>No further information availiable</H2>" << std::endl;
		html << "<HR>" << std::endl;
		html << "</BODY>" << std::endl;
		html << "</HTML>" << std::endl;
		
		html.flush();
		html.close();
		
	} else {
		return "about:blank";
	}

	return fn.GetFullPath();
}

