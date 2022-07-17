#include "GlobalFunctions.h"
#include "BinaryFileParser.h"
#include "GCodePathHandlerGL.h"
#include "GCodeFileParser.h"
#include "CncFileNameService.h"
#include "CncAutoProgressDialog.h"
#include "CncTemplateContext.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncFilePreview.h"

///////////////////////////////////////////////////////////////////
CncFilePreview::CncFilePreview(wxWindow* parent, wxString name)
: CncFilePreviewBase	(parent)
, tplFormat				(TplUnknown)
, gcodePreview			(NULL)
, svgPreview			(NULL)
, previewName			(name)
, lastFileName			()
///////////////////////////////////////////////////////////////////
{
	gcodePreview = new CncGCodePreview(m_gcodePreviewPlaceholder->GetParent(), name);
	GblFunc::replaceControl(m_gcodePreviewPlaceholder, gcodePreview);
	
	svgPreview = new CncSvgViewer(m_svgPreviewPlaceholder->GetParent());
	svgPreview->SetBackgroundColour(*wxBLACK);
	GblFunc::replaceControl(m_svgPreviewPlaceholder, svgPreview);
	
	m_previewBook->SetBackgroundColour(*wxBLACK);
	svgPreview->GetParent()->Bind(wxEVT_MOUSEWHEEL,			&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Bind(wxEVT_LEFT_DOWN,			&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Bind(wxEVT_LEFT_UP,			&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Bind(wxEVT_LEFT_DCLICK,		&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Bind(wxEVT_MOTION,				&CncFilePreview::onMouse,	this);
}
///////////////////////////////////////////////////////////////////
CncFilePreview::~CncFilePreview() {
///////////////////////////////////////////////////////////////////
	svgPreview->GetParent()->Unbind(wxEVT_MOUSEWHEEL,		&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Unbind(wxEVT_LEFT_DOWN,		&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Unbind(wxEVT_LEFT_UP,			&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Unbind(wxEVT_LEFT_DCLICK,		&CncFilePreview::onMouse,	this);
	svgPreview->GetParent()->Unbind(wxEVT_MOTION,			&CncFilePreview::onMouse,	this);
	
	delete gcodePreview;
	delete svgPreview;
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::normalizeView() {
///////////////////////////////////////////////////////////////////
	if ( m_previewBook->GetSelection() == (int)GCODE_TAB_PAGE )
		gcodePreview->resetViewport();
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectEmptyPreview() {
///////////////////////////////////////////////////////////////////
	return selectPreview("Only_the_extention_is_relevant_here_to_get_an_empty_preview.unknown");
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectEmptyPreviewIntern() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > SVG_TAB_PAGE );
	m_previewBook->SetSelection(SVG_TAB_PAGE);
	
	svgPreview->setContentSizable(false);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectBinaryPreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	wxString externalFile;
	if ( BinaryFileParser::extractSourceContentAsFile(fileName, externalFile) == false )
	{
		std::cerr << "CncFilePreview::selectBinaryPreview(): Can't create preview for file: '"
				  << fileName
				  << "'" << std::endl;
		return false;
	}

	return selectPreview(externalFile);
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectSVGPreview() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > SVG_TAB_PAGE );
	m_previewBook->SetSelection(SVG_TAB_PAGE);
	
	svgPreview->setContentSizable(true);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectGCodePreview() {
///////////////////////////////////////////////////////////////////
	wxASSERT( m_previewBook->GetPageCount() > GCODE_TAB_PAGE );
	m_previewBook->SetSelection(GCODE_TAB_PAGE);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::loadFile() {
///////////////////////////////////////////////////////////////////
	if ( lastFileName.GetFullPath().IsEmpty() )
		return true;
	
	if ( lastFileName.Exists() == false ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(" : Invalid file: %s\n", lastFileName.GetFullPath());
		return false;
	}
	
	bool ret = false;
	if ( m_previewBook->GetSelection() == (int)GCODE_TAB_PAGE )
	{
		const unsigned long fileSizeInBytes	= lastFileName.GetSize().ToULong();
		const unsigned long waterMark		= 1000 * 1014; //1 MB
		
		if ( IsShownOnScreen() || fileSizeInBytes > waterMark )  
		{
			CncAutoProgressDialog progressDlg(this, "Loading File");
			progressDlg.Show();
		}
		
		gcodePreview->clear();
		gcodePreview->Refresh();
		
		GCodeFileParser gfp(lastFileName.GetFullPath(), new GCodePathHandlerGL(gcodePreview));
		gfp.setDisplayWarnings(false);
		ret = gfp.processRelease();
		
		gcodePreview->Refresh();
	}
	else if ( m_previewBook->GetSelection() == (int)SVG_TAB_PAGE )
	{
		ret = svgPreview->loadFile(lastFileName.GetFullPath(), "Cnc File Preview:" );
		
		if ( ret == true )	svgPreview->Update();
		else				svgPreview->Clear();
	}
	
	// always do this, because in case ret == false the boundaries have to be reseted;
	evaluateMetricBoundaries();
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncFilePreview::selectPreview(const wxString& fileName) {
///////////////////////////////////////////////////////////////////
	tplFormat = cnc::getTemplateFormatFromFileName(fileName);
	wxString fn(fileName);
	
	switch ( tplFormat )
	{
		case TplSvg:		selectSVGPreview();
							break;
							
		case TplGcode:		selectGCodePreview();
							break;
							
		case TplBinary:		// with respect to the recursive character of selectBinaryPreview
							// directly return here 
							return selectBinaryPreview(fn);
							
		case TplText:		selectEmptyPreviewIntern();
							fn.assign(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialPreviewAvailable.svg"));
							break;
							
		case TplUnknown:	selectEmptyPreviewIntern();
							fn.assign(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialPreviewAvailable.svg"));
							break;
							
		default:			selectEmptyPreviewIntern();
							fn.assign(wxString::Format("%s%s", CncFileNameService::getDatabaseDir(), "NoSerialPreviewAvailable.svg"));
							
							std::cerr << "CncFilePreview::selectPreview(): No preview registered for: " 
									  << cnc::getTemplateFormatAsString(tplFormat)
									  << std::endl;
	}
	
	// only if file changed (name, time, etc.) to the file
	wxFileName tmpFn(fn);
	if ( tmpFn != lastFileName )
	{
		lastFileName.Assign(fn);
		return loadFile();
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::onPreviewBookChanged(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	//std::cout << "X: " << m_previewBook->GetSelection() << ", " << m_previewBook->IsShown() << ", " << m_previewBook->IsShownOnScreen() << std::endl;
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::activate3DPerspectiveButton(wxButton* bt) {
///////////////////////////////////////////////////////////////////
	static wxColour active  (171, 171, 171);
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
	
	if ( bt != NULL ) 
		bt->SetBackgroundColour(active);
}
///////////////////////////////////////////////////////////////////
void CncFilePreview::show3D(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxButton* bt = (wxButton*)event.GetEventObject();
	
	if ( bt == m_3D_Perspective1 ) 
	{
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso1();
	}
	else if ( bt == m_3D_Perspective2 ) 
	{
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso2();
	}
	else if ( bt == m_3D_Perspective3 ) 
	{
		activate3DPerspectiveButton(bt);
		gcodePreview->viewIso3();
	}
	else if ( bt == m_3D_Perspective4 ) 
	{
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
///////////////////////////////////////////////////////////////////
bool CncFilePreview::evaluateMetricBoundaries() const {
///////////////////////////////////////////////////////////////////
	CncDoubleBoundaries box;
	
	switch ( tplFormat ) 
	{
		case TplSvg:		svgPreview->getMetricBoundaries(box);
							break;
							
		case TplGcode:		gcodePreview->getMetricBoundaries(box);
							break;
							
		case TplBinary:		// not available here, came as inbound svg - see selectPreview()
							box.reset();
							break;
							
		case TplText:		// not available here, came as default svg - see selectPreview()
							box.reset();
							break;
							
		case TplUnknown:	// not available here, came as default svg - see selectPreview()
							box.reset();
							break;
								
		default:			std::cerr << CNC_LOG_A(": Missing template type implementation. Last file name: %s\n", lastFileName.GetFullPath());
							box.reset();
	}
	
	// setup the template context with this preview size information
	THE_TPL_CTX->registerBoundaries(box, CncTemplateContext::BT_TEMPLATE);
	THE_TPL_CTX->updateGui(true);
	
	return box.hasBoundaries();
}
/////////////////////////////////////////////////////////////////////
void CncFilePreview::onMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( tplFormat == TplSvg )
		svgPreview->onMouse(event);
}
