
#include <iostream>
#include <fstream>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/stdpaths.h>
#include "PathGeneratorFrame.h"

const char* PD_Preview_fileName			= "PathGeneratorPreview.svg";

///////////////////////////////////////////////////////////////////
PathGeneratorFrame::PathGeneratorFrame(wxWindow* parent)
: PathGeneratorFrameBase(parent)
, previewHeight(1500)
, previewWidth(1500)
, viewBoxX(0)
, viewBoxY(0)
, viewBoxW(0)
, viewBoxH(0)
, errorText("")
{
///////////////////////////////////////////////////////////////////
	setupPathSelector();
	initControls();
}
///////////////////////////////////////////////////////////////////
PathGeneratorFrame::~PathGeneratorFrame() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::closeWindow() {
///////////////////////////////////////////////////////////////////
	Show(false);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onCloseWindow(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	closeWindow();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onCloseWindowFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	closeWindow();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::copyPath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_generatedPath->SelectAll();
	m_generatedPath->Copy();
	m_generatedPath->SetSelection(0,0);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupPathSelector() {
///////////////////////////////////////////////////////////////////
	
	pathGeneratorStore.registerPathGenerator(new xyz());
	pathGeneratorStore.registerPathGenerator(new abc());
	pathGeneratorStore.registerPathGenerator(new pgRoundPoketWhole());
	//...
	
	
	// Init path selector
	pathGeneratorStore.setupSelector(m_pgPathSelector);
	m_pgPathSelector->SetSelection(0);
	wxCommandEvent dummy;
	selectPathSelector(dummy);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::initControls() {
///////////////////////////////////////////////////////////////////
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT );
	
	validator.SetPrecision(3);
	validator.SetRange(1, 30);
	m_pgPropToolDiameter->SetValidator(validator);
	
	validator.SetPrecision(3);
	validator.SetRange(-400, 400);
	m_pgPropTranslateX->SetValidator(validator);
	m_pgPropTranslateY->SetValidator(validator);
	
	validator.SetPrecision(1);
	validator.SetRange(0, 10);
	m_pgPropScaleX->SetValidator(validator);
	m_pgPropScaleY->SetValidator(validator);
	
	validator.SetPrecision(1);
	validator.SetRange(-360, 360);
	m_pgPropRotateA->SetValidator(validator);
	validator.SetPrecision(3);
	validator.SetRange(-400, 400);
	m_pgPropRotateX->SetValidator(validator);
	m_pgPropRotateY->SetValidator(validator);
	
	validator.SetPrecision(1);
	validator.SetRange(-360, 360);
	m_pgPropSkewX->SetValidator(validator);
	m_pgPropSkewY->SetValidator(validator);
	
	validator.SetPrecision(0);
	validator.SetRange(0, 1500);
	m_svgW->SetValidator(validator);
	m_svgH->SetValidator(validator);
	m_svgW->SetValue(wxString::Format("%d", previewWidth));
	m_svgH->SetValue(wxString::Format("%d", previewHeight));
	
	validator.SetPrecision(0);
	validator.SetRange(0, 1500);
	m_vieBoxX->SetValidator(validator);
	m_vieBoxY->SetValidator(validator);
	m_vieBoxW->SetValidator(validator);
	m_vieBoxH->SetValidator(validator);
	m_vieBoxX->SetValue(wxString::Format("%d", viewBoxX));
	m_vieBoxY->SetValue(wxString::Format("%d", viewBoxY));
	m_vieBoxW->SetValue(wxString::Format("%d", viewBoxW));
	m_vieBoxH->SetValue(wxString::Format("%d", viewBoxH));
	
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateSvgValues(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxString w  = m_svgW->GetValue();
	wxString h  = m_svgH->GetValue();
	
	wxString vx = m_vieBoxX->GetValue();
	wxString vy = m_vieBoxY->GetValue();
	wxString vw = m_vieBoxW->GetValue();
	wxString vh = m_vieBoxH->GetValue();
	
	w.ToLong(&previewWidth);
	h.ToLong(&previewHeight);
	
	vx.ToLong(&viewBoxX);
	vy.ToLong(&viewBoxY);
	vw.ToLong(&viewBoxW);
	vh.ToLong(&viewBoxH);
	
	updatePreview();
}
///////////////////////////////////////////////////////////////////
int PathGeneratorFrame::getPathSelection() {
///////////////////////////////////////////////////////////////////
	wxString item = m_pgPathSelector->GetStringSelection();
	wxString idStr = item.SubString(0,3);
	long id;
	idStr.ToLong(&id);
	
	if ( id >= 0 && id <= 999 )
		return id;
	
	return -1;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectPathSelector(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clearParameters();
	m_generatedPath->Clear();
	
	int id = getPathSelection();
	if ( id >= 0 ) {
		pathGeneratorStore.setupParameter(id, 0, m_pgProp01);
		pathGeneratorStore.setupParameter(id, 1, m_pgProp02);
		pathGeneratorStore.setupParameter(id, 2, m_pgProp03);
		pathGeneratorStore.setupParameter(id, 3, m_pgProp04);
		pathGeneratorStore.setupParameter(id, 4, m_pgProp05);
		pathGeneratorStore.setupParameter(id, 5, m_pgProp06);
		pathGeneratorStore.setupParameter(id, 6, m_pgProp07);
		pathGeneratorStore.setupParameter(id, 7, m_pgProp08);
	}
	
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearParameter(wxPGProperty* p) {
///////////////////////////////////////////////////////////////////
	if ( p == NULL )
		return;
		
	p->SetLabel("");
	p->Enable(false);
	p->SetHelpString("");
	p->Hide(true);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearParameters() {
///////////////////////////////////////////////////////////////////
	clearParameter(m_pgProp01);
	clearParameter(m_pgProp02);
	clearParameter(m_pgProp03);
	clearParameter(m_pgProp04);
	clearParameter(m_pgProp05);
	clearParameter(m_pgProp06);
	clearParameter(m_pgProp07);
	clearParameter(m_pgProp08);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearView(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_generatedPath->Clear();
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	int id = getPathSelection();
	if ( id >= 0 ) {
		
		PathGenertorBase::CommonValues cv;
		cv.toolDiameter = m_pgPropToolDiameter->GetValue();
		cv.pathColour	= ((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour;
		pathGeneratorStore.setCommonValues(id, cv);
		
		PathGenertorBase::TransformValues tv;
		tv.translateX 	= m_pgPropTranslateX->GetValue();
		tv.translateY 	= m_pgPropTranslateY->GetValue();
		tv.scaleX		= m_pgPropScaleX->GetValue();
		tv.scaleY		= m_pgPropScaleY->GetValue();
		tv.rotateA		= m_pgPropRotateA->GetValue();
		tv.rotateX		= m_pgPropRotateX->GetValue();
		tv.rotateY		= m_pgPropRotateY->GetValue();
		tv.skewX		= m_pgPropSkewX->GetValue();
		tv.skewY		= m_pgPropSkewY->GetValue();
		pathGeneratorStore.setTransformValues(id, tv);
		
		pathGeneratorStore.setParameterValue(id, 0, m_pgProp01);
		pathGeneratorStore.setParameterValue(id, 1, m_pgProp02);
		pathGeneratorStore.setParameterValue(id, 2, m_pgProp03);
		pathGeneratorStore.setParameterValue(id, 3, m_pgProp04);
		pathGeneratorStore.setParameterValue(id, 4, m_pgProp05);
		pathGeneratorStore.setParameterValue(id, 5, m_pgProp06);
		pathGeneratorStore.setParameterValue(id, 6, m_pgProp07);
		pathGeneratorStore.setParameterValue(id, 7, m_pgProp08);
	}
	
	m_generatedPath->SetValue(pathGeneratorStore.generatePath(getPathSelection()));
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setPath(wxString path) {
///////////////////////////////////////////////////////////////////
	//obsolete
	wxString value("<path d=\"");
	value += path;
	value += "\"  stroke=\"black\" fill=\"none\" stroke-width=\"1.0\"/>";
	m_generatedPath->SetValue(value);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updatePreview() {
///////////////////////////////////////////////////////////////////
	wxString pfn(PD_Preview_fileName);
	wxFileName fn(pfn);
	if ( fn.HasVolume() == false ) {
		wxString exPath = wxStandardPaths::Get().GetExecutablePath();
		pfn = wxFileName(exPath).GetPath(wxPATH_GET_SEPARATOR) + pfn;
	} 
	
	std::fstream fs;
	fs.open(pfn.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
	if ( fs.is_open() ) {
		fs << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
		fs << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"" << std::endl;
		fs << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << std::endl;
		fs << "<svg xmlns=\"http://www.w3.org/2000/svg\" "; 
		fs << "width=\"" << previewWidth << "mm\" "; 
		fs << "height=\"" << previewHeight << "mm\" ";
		fs << "viewBox=\"" << viewBoxX << " " <<  viewBoxY << " " << viewBoxW << " " <<  viewBoxH << "\" ";
		fs << "xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
		fs << "<title>SVG Path Generator</title>" << std::endl;
		fs << "<desc>Preview</desc>" << std::endl;
		
		if ( errorText.length() == 0 ) {
			fs << m_generatedPath->GetValue() << std::endl;
		} else {
			fs << "<text x=\"8\" y=\"40\" style=\"font-family:verdana; font-size:14px; font-weight:bold;\">";
			fs << errorText.c_str();
			fs << "</text>" << std::endl;
		}
		
		fs << "</svg>" << std::endl;
		fs.close();
	}

	wxFileName check(pfn);
	if ( !check.Exists() )
		pfn = "about:blank";

	m_pgPreview->LoadURL(pfn);
}










///////////////////////////////////////////////////////////////////
bool PathGeneratorFrame::generateSymetricCirclePoints(std::vector<wxRealPoint>& pl, 
                                                      double xOffset, double yOffset, 
													  double sections, double radius) {
///////////////////////////////////////////////////////////////////
	// todo check parameter
	pl.clear();

	double steps = 360.0/(sections);

	for (double i=0; i<=360; i+=steps ) {
		double x = cos(i*PI/180) * radius;
		double y = sin(i*PI/180) * radius;
		
		x += xOffset;
		y += yOffset;
		
		x = round(x * 1000.0) / 1000.0;
		y = round(y * 1000.0) / 1000.0;
				
		pl.push_back({x,y});
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePolygon() {
///////////////////////////////////////////////////////////////////
	errorText.Clear();
	
	wxString val;
	double radius = 0;
	long sections = 1;
	
	val = "10.0";//m_polygonRadius->GetValue();
	if ( val.length() > 0 ) val.ToDouble(&radius);

	val = "5";//m_polygonSections->GetValue();
	if ( val.length() > 0 ) val.ToLong(&sections);
	
	double xOffset = + 15;
	double yOffset = radius + 15;
	std::vector<wxRealPoint> pl;
	
	if ( generateSymetricCirclePoints(pl, xOffset, yOffset, sections, radius) == false )
		return;
	
	wxString p("M ");
	std::vector<wxRealPoint>::iterator it = pl.begin();
	wxRealPoint p1 = *it; ++it;
	wxRealPoint p2;

	p << p1.x; p << " "; p << p1.y;
	for ( ; it != pl.end(); ++it) {
		p2 = *it;
		
		double dx = p1.x - p2.x;
		double dy = p1.y - p2.y;
		p << " l ";
		p << dx; p << " "; p << dy;
		
		p1 = p2;
	}
	
	setPath(p);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generateKnob() {
///////////////////////////////////////////////////////////////////
	errorText.Clear();

	wxString val;
	double radius = 100;
	long sections = 8;

	val = "10.00";//m_knobRadius->GetValue();
	if ( val.length() > 0 ) val.ToDouble(&radius);

	val = "10";//m_knobSections->GetValue();
	if ( val.length() > 0 ) val.ToLong(&sections);
	
	double xOffset = radius + 25;
	double yOffset = radius + 25;
	std::vector<wxRealPoint> pl;
	
	if ( generateSymetricCirclePoints(pl, xOffset, yOffset, sections, radius) == false )
		return;

	std::vector<wxRealPoint>::iterator it = pl.begin();
	wxRealPoint p1 = *it; ++it;
	wxRealPoint p2;

	wxString p("M ");
	p << p1.x; p << " "; p << p1.y;
	for ( ; it != pl.end(); ++it) {
		p2 = *it;
		
		double dx = p1.x - p2.x;
		double dy = p1.y - p2.y;
		double dh = sqrt(dx*dx + dy*dy);
		
		p << "M "; p << p1.x; p << " "; p << p1.y;
		
		p << " A"; p << dh/2; p << " "; p << dh/2; 
		
		if ( std::distance(pl.begin(), it)%2  == 0 ) p << " 0 1 0 ";
		else										 p << " 0 1 1 ";

		p << p2.x; p << " "; p << p2.y; p << " ";

		p1 = p2;
	}
	
	setPath(p);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePocketWhole() {
///////////////////////////////////////////////////////////////////

	SvgPathGroup spg(mm);
	//SvgPathString sps;
	//SvgPathFragment spf;
	
	//spg.pGen().add(spg.fGen().ellipticalArg(25.0));
	
	spg.pGen().addPocketWhole(4.0, 40.0, 40.0);
	
	spg.add(spg.pGen().get());
	spg.translate(100.0);
	//spg.scale(1.5);
	
	
	if ( spg.hasError() ) {
		errorText = spg.getErrorInfo();
	} else {
		m_generatedPath->SetValue(spg.get());
	}

}

