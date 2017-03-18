
#include <iostream>
#include <fstream>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/stdpaths.h>
#include "CncCommon.h"
#include "PathGeneratorFrame.h"

const char* PD_Preview_fileName			= "PathGeneratorPreview.svg";
const unsigned int PG_Page_Polygon 		= 0;
const unsigned int PG_Page_Knob 		= 1;
const unsigned int PG_Page_Gear 		= 2;

///////////////////////////////////////////////////////////////////
PathGeneratorFrame::PathGeneratorFrame(wxWindow* parent)
: PathGeneratorFrameBase(parent)
, previewHeight(1500)
, previewWidth(1500)
, errorText("")
{
///////////////////////////////////////////////////////////////////
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
void PathGeneratorFrame::initControls() {
///////////////////////////////////////////////////////////////////
	wxFloatingPointValidator<float> val1(3, NULL,wxNUM_VAL_DEFAULT );
	val1.SetRange(1, 500);
	m_polygonRadius->SetValidator(val1);

	wxIntegerValidator<unsigned long> val2;
	val2.SetRange(2, 100);
	m_polygonSections->SetValidator(val2);

	wxFloatingPointValidator<float> val3(3, NULL,wxNUM_VAL_DEFAULT );
	val3.SetRange(1, 500);
	m_knobRadius->SetValidator(val3);

	wxIntegerValidator<unsigned long> val4;
	val4.SetRange(2, 100);
	m_polygonSections->SetValidator(val4);

}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	unsigned int sel = m_pgNotebook->GetSelection();
	switch (sel) {
		case PG_Page_Polygon:
				generatePolygon();
				updatePreview();
				break;
		case PG_Page_Knob:
				generateKnob();
				updatePreview();
				break;
		default:
				errorText = "No path generator defined for this selection. Nothing will be generated";
				setPath("");
				updatePreview();
	}
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
		fs << "width=\"" << previewWidth << "\" "; 
		fs << "height=\"" << previewHeight << "\" ";
		//fs << "viewBox=\"0 0 " << width * zoomFact << " " <<  height * zoomFact << "\" ";
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
	
	val = m_polygonRadius->GetValue();
	if ( val.length() > 0 ) val.ToDouble(&radius);

	val = m_polygonSections->GetValue();
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

	val = m_knobRadius->GetValue();
	if ( val.length() > 0 ) val.ToDouble(&radius);

	val = m_knobSections->GetValue();
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
void PathGeneratorFrame::setPath(wxString path) {
///////////////////////////////////////////////////////////////////
	wxString value("<path d=\"");
	value += path;
	value += "\"  stroke=\"black\" fill=\"none\" stroke-width=\"1.0\"/>";
	m_generatedPath->SetValue(value);
}