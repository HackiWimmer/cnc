
#include <iostream>
#include <fstream>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/stdpaths.h>
#include "PathGeneratorFrame.h"

const char* PD_Preview_fileName						= "PathGeneratorPreview.svg";
wxString PathGeneratorFrame::globaGeneratedPath		= _T("");

///////////////////////////////////////////////////////////////////
PathGeneratorFrame::PathGeneratorFrame(wxWindow* parent, wxStyledTextCtrl* tplEditor)
: PathGeneratorFrameBase(parent)
, prevSizeHeight(-1)
, templateEditor(tplEditor)
, previewHeight(1500)
, previewWidth(1500)
, viewBoxX(0)
, viewBoxY(0)
, viewBoxW(0)
, viewBoxH(0)
{
///////////////////////////////////////////////////////////////////
	globaGeneratedPath.clear();
	initControls();
	
	SetMinSize(wxSize(-1, minSizeHeight));
	SetSize(wxSize(-1, -1));
	decorateSizeButton();
}
///////////////////////////////////////////////////////////////////
PathGeneratorFrame::~PathGeneratorFrame() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onStartupTimer(wxTimerEvent& event) {
///////////////////////////////////////////////////////////////////
	setupPathSelector();
	
	wxCommandEvent dummy;
	selectPathSelector(dummy);
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
	m_generatedResult->SelectAll();
	m_generatedResult->Copy();
	m_generatedResult->SetSelection(0,0);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupPathSelector() {
///////////////////////////////////////////////////////////////////
	// Init path selector
	pathGeneratorStore.setupSelector(m_pgPathSelector);
	if ( pathGeneratorStore.getGenertorCount() > 0 )
		m_pgPathSelector->SetSelection(0);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::enableControls(bool state){
///////////////////////////////////////////////////////////////////
	m_btPGGenerate->Enable(state);
	m_btPGClear->Enable(state);
	m_btPGCopy->Enable(state);
	m_btPGClose->Enable(state);
	
	state == true ? m_generatedPreview->Thaw() : m_generatedPreview->Freeze();
	state == true ? m_generatedResult->Thaw() : m_generatedResult->Freeze();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::initControls() {
///////////////////////////////////////////////////////////////////
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_RETURN);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_PAGEDOWN);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_PREV_PROPERTY, WXK_PAGEUP);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_DOWN);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_PREV_PROPERTY, WXK_UP);
	
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_RETURN);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_PAGEUP);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_PAGEDOWN);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_UP);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_DOWN);
	
	m_pgParameterMgr->GetGrid()->ResetColours();
	m_pgParameterMgr->GetGrid()->SetCaptionBackgroundColour(wxColour(255,255,128));
	m_pgParameterMgr->GetGrid()->SetCellBackgroundColour(wxColour(255,255,255));
	
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	validator.SetPrecision(3);
	validator.SetRange(0, 30);
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
	m_pgPropSvgWidth->SetValidator(validator);
	m_pgPropSvgHeight->SetValidator(validator);
	m_pgPropSvgWidth->SetValue(wxString::Format("%d", previewWidth));
	m_pgPropSvgHeight->SetValue(wxString::Format("%d", previewHeight));
	
	validator.SetPrecision(0);
	validator.SetRange(0, 1500);
	m_pgPropSvgVbX->SetValidator(validator);
	m_pgPropSvgVbY->SetValidator(validator);
	m_pgPropSvgVbW->SetValidator(validator);
	m_pgPropSvgVbH->SetValidator(validator);
	m_pgPropSvgVbX->SetValue(wxString::Format("%d", viewBoxX));
	m_pgPropSvgVbY->SetValue(wxString::Format("%d", viewBoxY));
	m_pgPropSvgVbW->SetValue(wxString::Format("%d", viewBoxW));
	m_pgPropSvgVbH->SetValue(wxString::Format("%d", viewBoxH));
	
	validator.SetPrecision(3);
	validator.SetRange(-20, 100);
	m_pgPropCncZDepth->SetValidator(validator);
	m_pgPropCncZDepth->SetValue(wxString::Format("%.3lf", 0.0));
	
	validator.SetPrecision(0);
	validator.SetRange(1, 100);
	m_pgPropGridSize->SetValidator(validator);
	m_pgPropGridSize->SetValue(wxString::Format("%.0lf", 5.0));
	
	m_pgCatViewBox->SetExpanded(false);
	m_pgCatTranslate->SetExpanded(true);
	m_pgCatScale->SetExpanded(false);
	m_pgCatRotate->SetExpanded(false);
	m_pgCatSkrew->SetExpanded(false);
	
	setHelpInfos();
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
	selectPathSelector(getPathSelection());
	
	generatePath();
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectPathSelector(int id) {
///////////////////////////////////////////////////////////////////
	clearParameters();
	m_generatedPreview->Clear();
	m_generatedResult->Clear();
	
	if ( id != getPathSelection() )
		m_pgPathSelector->SetSelection(id);
	
	if ( id >= 0 ) {
		pathGeneratorStore.initPathGenerator(id);
		setupCommonValues(pathGeneratorStore.getCommonValues(id));
		setupCncParameterValues(pathGeneratorStore.getCncParameterValues(id));
		pathGeneratorStore.setupParameter(id, m_pgCatPath);
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearParameters() {
///////////////////////////////////////////////////////////////////
	if ( m_pgParameterMgr->IsShownOnScreen() == false)
		return;
	
	m_pgCatPath->DeleteChildren();
	m_pgParameterMgr->Refresh();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearView(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clearView();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearView() {
///////////////////////////////////////////////////////////////////
	m_generatedPreview->Clear();
	m_generatedResult->Clear();
	globaGeneratedPath.Clear();
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onActivate(wxActivateEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( templateEditor == NULL )
		return;
		
	m_btPGReplaceEdit->Enable(templateEditor->IsShownOnScreen());
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::relaceEditControlSelection(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( templateEditor == NULL )
		return;
		
	if ( templateEditor->IsShownOnScreen() == false )
		return;
		
	templateEditor->ReplaceSelection(m_generatedResult->GetValue());
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::toogleSize(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxSize s = GetSize();
	int height = s.GetHeight();
	
	if ( height > minSizeHeight ) {
		prevSizeHeight = height;
		SetSize(-1, minSizeHeight);
	} else {
		SetSize(-1, prevSizeHeight);
	}
	
	decorateSizeButton();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::decorateSizeButton() {
///////////////////////////////////////////////////////////////////
	if ( GetSize().GetHeight() > minSizeHeight )	m_btPGMinimize->SetBitmap(ImageLib16().Bitmap("BMP_MINUS"));
	else											m_btPGMinimize->SetBitmap(ImageLib16().Bitmap("BMP_PLUS"));
		
	m_btPGMinimize->Refresh();
	m_btPGMinimize->Update();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
	decorateSizeButton();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
bool PathGeneratorFrame::regenerateSvgBlock(PathGeneratorStore::RegenerateParameter & rp) {
///////////////////////////////////////////////////////////////////
	rp.out.errorInfo.clear();
	
	
	
	PathGeneratorBase::XmlPatternResult result;
	if ( PathGeneratorBase::decodeXmlPattern(rp.in.cncPattern, result) == false ) {
		rp.out.errorInfo << result.errorInfo << "\n";
		return false;
	}
	
	PathGeneratorBase* pGen = pathGeneratorStore.getPathGenerator(result.tplName);
	if ( pGen == NULL) {
		rp.out.errorInfo << "Cant find Template Generator: " << result.tplName << "\n";
		return false;
	}
	
	if ( pGen->getCheckSumAsString() != result.checkSum ) {
		rp.out.errorInfo << "Invalid CheckSum detected: " << pGen->getCheckSumAsString() << "!=" << result.checkSum  << "\n";
		return false;
	}
	
	selectPathSelector(pGen->getSelectorIndex());
	updateParameterValues(result.pMap);
	result.setToolDiameter(rp.in.toolDiameter);
	updateCommonValues(result.commonValues);
	updateCncParameterValues(result.cncParameterValues);
	updateTransformValues(result.transformValues);
	
	generatePath();
	updatePreview();
	
	//todo replace selection
	
	return true;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	generatePath();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePath() {
///////////////////////////////////////////////////////////////////
	enableControls(false);
		
	int id = getPathSelection();
	if ( id >= 0 ) {
		evaluateCommonValues(id);
		evaluateSvgParameterValues(id);
		evaluateCncParameterValues(id);
		evaluatePathParameterValues(id);

		// generate path
		cnc::pg::trc.clear();
		pathGeneratorStore.resetErrorInfo(id);

		// prepare preview and result
		wxString svgFragment(pathGeneratorStore.generatePath(id));
		
		wxString preview(svgFragment);
		wxString result(svgFragment);
		
		SvgGeneratorBase::finalizeSvgFragment(preview, true);
		SvgGeneratorBase::finalizeSvgFragment(result, false);
		
		// update result
		m_generatedResult->SetValue(result);
		
		// update global Value
		globaGeneratedPath = result;
		
		// update preview
		cnc::pg::trc.blue();
		cnc::pg::trc << preview;

		if ( pathGeneratorStore.hasErrorInfo(id) == true ) {
			cnc::pg::trc.red();
			cnc::pg::trc << wxString::Format("\n<--\nError info:\n%s-->\n", pathGeneratorStore.getErrorInfo(id));
		}

		// update transform values
		if ( m_pgPropTransformMode->GetValue().GetBool() == true )
			updateTransformValues(pathGeneratorStore.getTransformValues(id));
			
	} else {
		clearView();
		cnc::pg::trc.red();
		cnc::pg::trc << wxString::Format("\n<--\nError info:\n%s%d-->\n", "PathGeneratorFrame::generatePath: Invalid id: ", id);
	}
	
	updatePreview();
	enableControls(true);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateCommonValues(const PathGeneratorBase::CommonValues& cv) {
///////////////////////////////////////////////////////////////////
	m_pgPropCorrection->SetValue(cv.toolCorrection);
	m_pgPropToolDiameter->SetValue(cv.toolDiameter);
	//((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour;
	m_pgPropConfigBlock->SetValue(cv.configBlock);
	m_pgPropRefCross->SetValue(cv.referenceCross);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateCommonValues(int id) {
///////////////////////////////////////////////////////////////////
	PathGeneratorBase::CommonValues cv;
	cv.toolCorrection	= m_pgPropCorrection->GetValue().GetBool();
	cv.toolDiameter 	= m_pgPropToolDiameter->GetValue();
	cv.pathColour		= ((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour;
	cv.configBlock		= m_pgPropConfigBlock->GetValue().GetBool();
	cv.referenceCross	= m_pgPropRefCross->GetValue().GetBool();
	
	pathGeneratorStore.setCommonValues(id, cv);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateTransformValues(const PathGeneratorBase::TransformValues& tv) {
///////////////////////////////////////////////////////////////////
	m_pgPropTransformMode->SetValue(tv.autoMode);
	
	m_pgPropTranslateX->SetValue(tv.translateX);
	m_pgPropTranslateY->SetValue(tv.translateY);
	m_pgPropScaleX->SetValue(tv.scaleX);
	m_pgPropScaleY->SetValue(tv.scaleY);
	m_pgPropRotateA->SetValue(tv.rotateA);
	m_pgPropRotateX->SetValue(tv.rotateX);
	m_pgPropRotateY->SetValue(tv.rotateY);
	m_pgPropSkewX->SetValue(tv.skewX);
	m_pgPropSkewY->SetValue(tv.skewY);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateSvgParameterValues(int id) {
///////////////////////////////////////////////////////////////////
	previewWidth	= m_pgPropSvgWidth->GetValue().GetDouble();
	previewHeight	= m_pgPropSvgHeight->GetValue().GetLong();
	
	viewBoxX		= m_pgPropSvgVbX->GetValue().GetLong();
	viewBoxY		= m_pgPropSvgVbY->GetValue().GetLong();
	viewBoxW		= m_pgPropSvgVbW->GetValue().GetLong();
	viewBoxH		= m_pgPropSvgVbH->GetValue().GetLong();
	
	PathGeneratorBase::TransformValues tv;
	tv.autoMode		= m_pgPropTransformMode->GetValue().GetBool();
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
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateCncParameterValues(const PathGeneratorBase::CncParameterValues& cpv) {
///////////////////////////////////////////////////////////////////
	m_pgPropCncInclude->SetValue(cpv.include);
	m_pgPropCncZDepthMode->SetValue(cpv.zDepthMode);
	m_pgPropCncZDepth->SetValue(cpv.zDepth);
	m_pgPropCncReversePath->SetValue(cpv.reverse);
	m_pgPropCncToolCorrection->SetValue(cpv.correct);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateCncParameterValues(int id) {
///////////////////////////////////////////////////////////////////
	PathGeneratorBase::CncParameterValues cpv;
	cpv.include		= m_pgPropCncInclude->GetValue().GetBool();
	cpv.zDepthMode 	= m_pgPropCncZDepthMode->GetValue().GetInteger();
	cpv.zDepth 		= m_pgPropCncZDepth->GetValue().GetDouble();
	cpv.reverse 	= m_pgPropCncReversePath->GetValue().GetBool();
	cpv.correct 	= m_pgPropCncToolCorrection->GetValue().GetInteger();
	
	pathGeneratorStore.setCncParameterValues(id, cpv);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateParameterValues(const PathGeneratorBase::ParameterMap& pMap) {
///////////////////////////////////////////////////////////////////
	//update parameter values
	wxPropertyGrid* grid = m_pgParameterMgr->GetGrid();
	wxPGProperty* pathProp = grid->GetFirstChild(m_pgCatPath);
	if ( pathProp != NULL ) {
		
		// over all path properties
		wxPropertyGridIterator it = grid->GetIterator(wxPG_ITERATE_DEFAULT, pathProp);
		wxPGProperty* prop = NULL;
		while ( (prop = it.GetProperty()) != NULL && grid->GetPropertyCategory(prop) == m_pgCatPath) {
			unsigned int index = prop->GetIndexInParent();
			
			PathGeneratorBase::ParameterMap::const_iterator pmIt = pMap.find(index);
			if ( pmIt != pMap.end() ) {
				PathGeneratorBase::ParameterInfo pi = pmIt->second;

				// specialize and set value
				if ( pi.propertyType == wxPG_VARIANT_TYPE_DOUBLE ) {
					prop->SetValue(wxString::Format("%.3lf", pi.value.GetDouble()));
					
				} else if ( pi.propertyType == wxPG_VARIANT_TYPE_LIST ) {
					prop->SetValueFromString(pi.value);
					
				} else {
					prop->SetValue(pi.value);
				}
			}
			
			it.Next();
		}
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluatePathParameterValues(int id) {
///////////////////////////////////////////////////////////////////
	wxPropertyGrid* grid = m_pgParameterMgr->GetGrid();
	wxPGProperty* pathProp = grid->GetFirstChild(m_pgCatPath);
	if ( pathProp != NULL ) {
		
		// over all path properties
		wxPropertyGridIterator it = grid->GetIterator(wxPG_ITERATE_DEFAULT, pathProp);
		wxPGProperty* prop = NULL;
		int cnt = 0;
		while ( (prop = it.GetProperty()) != NULL && grid->GetPropertyCategory(prop) == m_pgCatPath) {
			pathGeneratorStore.setParameterValue(id, cnt++, prop);
			it.Next();
		}
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupProperty(wxPGProperty* property, bool show) {
///////////////////////////////////////////////////////////////////
	if ( property == NULL )
		return;
		
	property->Hide(!show);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupProperty(wxPGProperty* property, bool show, wxVariant value) {
///////////////////////////////////////////////////////////////////
	if ( property == NULL )
		return;
		
	property->SetValue(value);
	property->Hide(!show);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupCommonValues(const PathGeneratorBase::CommonValues& cv) {
///////////////////////////////////////////////////////////////////
	setupProperty(m_pgPropCorrection, cv.canToolCorrection, cv.canToolCorrection);
	setupProperty(m_pgPropToolDiameter, cv.canToolDiameter, cv.toolDiameter);
	setupProperty(m_pgPropPathColour, cv.canPathColour);
	
	if ( cv.canPathColour == true )
		((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour = cv.pathColour;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupCncParameterValues(const PathGeneratorBase::CncParameterValues& cpv) {
///////////////////////////////////////////////////////////////////
	setupProperty(m_pgPropCncToolCorrection, cpv.canCorrect, cpv.correct);
	setupProperty(m_pgPropCncReversePath, cpv.canReverse, cpv.reverse);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::propertyChanged(wxPropertyGridEvent& event) {
///////////////////////////////////////////////////////////////////
	generatePath();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::displayGrid(std::fstream& fs){
///////////////////////////////////////////////////////////////////
	if ( m_pgPropGridDisplay->GetValue().GetBool() == false )
		return;

	double h = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(mm) * previewWidth);
	double w = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(mm) * previewHeight);
	double s = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(mm) * m_pgPropGridSize->GetValue().GetDouble());
	
	wxColour colour(0,0,0);
	wxColour c1(0,64,255);
	wxColour c2(255,64,0);
	
	int cnt = 0;
	for (double i=0; i<=h; i+=s, cnt++) {
		cnt%5 == 0 ? colour = c1 : colour = c2;
		fs << wxString::Format("<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"0.3\"/>", 0.0, i, w, i, colour.Red(), colour.Green(), colour.Blue());
	}
	
	cnt = 0;
	for ( double i=0; i<=w; i+=s, cnt++ ) {
		cnt%5 == 0 ? colour = c1 : colour = c2;
		fs << wxString::Format("<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"0.3\"/>", i, 0.0, i, h, colour.Red(), colour.Green(), colour.Blue());
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
		fs << "width=\"" << previewWidth << "mm\" "; 
		fs << "height=\"" << previewHeight << "mm\" ";
		fs << "viewBox=\"" << viewBoxX << " " <<  viewBoxY << " " << viewBoxW << " " <<  viewBoxH << "\" ";
		fs << "xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
		fs << "<title>SVG Path Generator</title>" << std::endl;
		fs << "<desc>Preview</desc>" << std::endl;
		
		displayGrid(fs);
		
		fs << m_generatedPreview->GetValue() << std::endl;
		
		fs << "</svg>" << std::endl;
		fs.close();
	}

	wxFileName check(pfn);
	if ( !check.Exists() )
		pfn = "about:blank";

	m_pgPreview->LoadURL(pfn);
}








///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setPath(wxString path) {
///////////////////////////////////////////////////////////////////
/*
	//obsolete
	std::clog << "Obsolete function PathGeneratorFrame::setPath"<< std::endl;
	wxString value("<path d=\"");
	value += path;
	value += "\"  stroke=\"black\" fill=\"none\" stroke-width=\"1.0\"/>";
	m_generatedPath->SetValue(value);
	 * */
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



