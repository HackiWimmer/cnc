
#include <iostream>
#include <fstream>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/stdpaths.h>
#include "CncCommon.h"
#include "SvgUnitCalculator.h"
#include "SvgPathGenerator.h"
#include "PathGeneratorFrame.h"

const char* PD_Preview_fileName			= "PathGeneratorPreview.svg";
const unsigned int PG_Page_Polygon 		= 0;
const unsigned int PG_Page_Knob 		= 1;
const unsigned int PG_Page_Gear 		= 2;
const unsigned int PG_Page_PocketWhole	= 3;

///////////////////////////////////////////////////////////////////
PathGeneratorFrame::PathGeneratorFrame(wxWindow* parent)
: PathGeneratorFrameBase(parent)
, previewHeight(1500)
, previewWidth(1500)
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
	m_pgPathSelector->Append("001 - Polygon");
	m_pgPathSelector->Append("002 - Knob");
	m_pgPathSelector->Append("003 - Gear");
	m_pgPathSelector->Append("004 - Poket Whole");
	//...
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
	
	//todo
	
	//m_pgParameterMgr->Append()
	
	m_pgProp01->SetLabel("adadsdad");
	m_pgProp01->SetValidator(val3);
	wxVariant null("");
	m_pgProp01->SetLabel("");
	m_pgProp01->SetValue("");
	
	//m_pgProp01->S StringToValue(null);
	m_pgProp01->Enable(false);
	m_pgProp02->SetHelpString("adaddweewewqweqwqewqewqewwqeewqew\nqeqeqweqweqwe");

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
	errorText.clear();
	
	switch ( getPathSelection() ) {
		
		case 1:		setupPolygon();
					updatePreview();
					break;
		// todo
					
		default:	errorText = "No path generator defined for this selection. Nothing will be generated";
					setPath("");
					updatePreview();
	}
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
void PathGeneratorFrame::setupPolygon() {
///////////////////////////////////////////////////////////////////
	clearParameters();
	
	wxFloatingPointValidator<float> val1(3, NULL,wxNUM_VAL_DEFAULT );
	val1.SetRange(1, 400);
	
	wxIntegerValidator<unsigned long> val2;
	val2.SetRange(2, 100);

	m_pgProp01->Hide(false);
	m_pgProp01->SetLabel("Radius [mm]");
	m_pgProp01->SetValidator(val1);
	m_pgProp01->Enable(true);
	m_pgProp01->SetHelpString("Radius . . . .");
	m_pgProp01->SetValue(10.000);
	
	m_pgProp02->Hide(false);
	m_pgProp02->SetLabel("Count [number]");
	m_pgProp02->SetValidator(val2);
	m_pgProp02->Enable(true);
	m_pgProp02->SetHelpString("Corner count . . . .");
	m_pgProp02->SetValue(4);
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
		case PG_Page_PocketWhole:
				generatePocketWhole();
				updatePreview();
				break;
		default:
				errorText = "No path generator defined for this selection. Nothing will be generated";
				setPath("");
				updatePreview();
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setPath(wxString path) {
///////////////////////////////////////////////////////////////////
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

