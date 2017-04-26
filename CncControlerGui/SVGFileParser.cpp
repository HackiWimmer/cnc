#include <iostream>
#include <wx/string.h>
#include <wx/evtloop.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/dataview.h>
#include <wx/stc/stc.h>
#include <wx/webview.h>
#include <wx/filename.h>
#include "SvgEditPopup.h"
#include "SvgUnitCalculator.h"
#include "CncFileNameService.h"
#include "CncControl.h"
#include "SVGPathHandler.h"
#include "SVGFileParser.h"

//////////////////////////////////////////////////////////////////
SVGFileParser::SVGFileParser(const char* fn, CncControl* cnc) 
: cncControl(cnc)
, cncNodeBreak(false)
, pathHandler(new SVGPathHandlerCnc(cnc))
, fileName(fn)
, svgTraceControl(NULL)
, debugBase(NULL)
, debugPath(NULL)
, debugDetail(NULL)
, svgSourceControl(NULL)
{
//////////////////////////////////////////////////////////////////
	createSvgTraceRoot();
}
//////////////////////////////////////////////////////////////////
SVGFileParser::~SVGFileParser() {
//////////////////////////////////////////////////////////////////
	delete pathHandler;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::createSvgTraceRoot() {
//////////////////////////////////////////////////////////////////
	wxXmlNode* root = svgTrace.DetachRoot();
	if ( root )
		delete root;
		
	root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "Root");
	svgTrace.SetRoot(root);
}

//////////////////////////////////////////////////////////////////
bool SVGFileParser::convertCircleToPathData(wxXmlNode* child, wxString ret) {
//////////////////////////////////////////////////////////////////
	// <circle cx="10" cy="10" r="5" fill="red" stroke="blue" stroke-width="10"  />
	// --> M 5,10 a 5,5 0 1,0 10,0 a 5,5 0 1,0 -10,0
	wxString cx = child->GetAttribute("cx", "");
	wxString cy = child->GetAttribute("cy", "");
	wxString r  = child->GetAttribute("r", "");

	double cxd, cyd, rd;
	cx.ToDouble(&cxd);
	cy.ToDouble(&cyd);
	r.ToDouble(&rd);

	wxString path("M ");
	path << (cxd - rd) << "," << cy;
	
	path << " a ";
	path << r   	<< ",";
	path << r   	<< " 0 0 1 ";
	path << (rd*2) 	<< ",0";
	
	path << " a ";
	path << r   	<< ",";
	path << r   	<< " 0 0 1 -";
	path << (rd*2)	<< ",0";

	//std::cerr << path.c_str() << std::endl;
	return evaluatePath(path);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::convertEllipseToPathData(wxXmlNode* child, wxString ret) {
//////////////////////////////////////////////////////////////////
	// <ellipse cx="10" cy="10" rx="5" ry="4" fill="red" stroke="blue" stroke-width="10"  />
	// --> M 5,10 a 5,4 0 1,0 10,0 a 5,4 0 1,0 -10,0
	wxString cx = child->GetAttribute("cx", "");
	wxString cy = child->GetAttribute("cy", "");
	wxString rx = child->GetAttribute("rx", "");
	wxString ry = child->GetAttribute("ry", "");
	
	double cxd, cyd, rxd, ryd;
	cx.ToDouble(&cxd);
	cy.ToDouble(&cyd);
	rx.ToDouble(&rxd);
	ry.ToDouble(&ryd);

	wxString path("M ");
	path << (cxd - rxd) << "," << cy;
	
	path << " a ";
	path << rx   	<< ",";
	path << ry   	<< " 0 0 1 ";
	path << (rxd*2) << ",0";
	
	path << " a ";
	path << rx   	<< ",";
	path << ry   	<< " 0 0 1 -";
	path << (rxd*2)	<< ",0";	
	
	//std::cerr << path.c_str() << std::endl;
	return evaluatePath(path);
} 
//////////////////////////////////////////////////////////////////
bool SVGFileParser::convertLineToPathData(wxXmlNode* child, wxString ret) {
//////////////////////////////////////////////////////////////////
	 // <line x1="100" y1="300" x2="300" y2="100" stroke-width="5"  />
	wxString path("M ");
	wxString x1 = child->GetAttribute("x1", "");
	wxString y1 = child->GetAttribute("y1", "");
	wxString x2 = child->GetAttribute("x2", "");
	wxString y2 = child->GetAttribute("y2", "");
	
	path << x1 << " ";
	path << y1 << " ";
	path << "L ";
	path << x2 << " ";
	path << y2;
	
	//std::cerr << path.c_str() << std::endl;
	return evaluatePath(path);
}
//////////////////////////////////////////////////////////////////
wxString& SVGFileParser::evaluatePolyPoints(wxString points, wxString& path) {
//////////////////////////////////////////////////////////////////
	wxStringTokenizer tokenizerBlank(points, " ");
	unsigned int cnt=0; 
	while ( tokenizerBlank.HasMoreTokens() ) {
		wxString valuePair = tokenizerBlank.GetNextToken();
		wxStringTokenizer tokenizerComma(valuePair, ",");

		while ( tokenizerComma.HasMoreTokens() ) {
			wxString token = tokenizerComma.GetNextToken();
			path << token << " ";

			if ( ++cnt == 2 )
				path << "L ";
		}
	}
	return path;
} 
//////////////////////////////////////////////////////////////////
bool SVGFileParser::convertPolygonToPathData(wxXmlNode* child, wxString ret) {
//////////////////////////////////////////////////////////////////
	//polygone fill="none" stroke="blue" stroke-width="10" 
	//	points="10,10
	//			15,15
 	//			20,10
	wxString path("M ");
	wxString points = child->GetAttribute("points", "");
	evaluatePolyPoints(points, path);
	path << "z";

	//std::cerr << path.c_str() << std::endl;
	return evaluatePath(path); 
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::convertPolylineToPathData(wxXmlNode* child, wxString ret) {
//////////////////////////////////////////////////////////////////
	//polyline fill="none" stroke="blue" stroke-width="10" 
	//	points="10,10
	//			15,15
 	//			20,10
 	//			25,15"
 
	wxString path("M ");
	wxString points = child->GetAttribute("points", "");
	evaluatePolyPoints(points, path);
	
	//std::cerr << path.c_str() << std::endl;
	return evaluatePath(path); 
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::convertRectToPathData(wxXmlNode* child, wxString ret) {
//////////////////////////////////////////////////////////////////
	//<rect x="1" y="1" width="1198" height="398" fill="none" stroke="blue" stroke-width="2"/>
	/*
	Let rx and ry be length values.
    If neither ‘rx’ nor ‘ry’ are properly specified, then set both rx and ry to 0. (This will result in square corners.)
    Otherwise, if a properly specified value is provided for ‘rx’, but not for ‘ry’, then set both rx and ry to the value of ‘rx’.
    Otherwise, if a properly specified value is provided for ‘ry’, but not for ‘rx’, then set both rx and ry to the value of ‘ry’.
    Otherwise, both ‘rx’ and ‘ry’ were specified properly. Set rx to the value of ‘rx’ and ry to the value of ‘ry’.
    If rx is greater than half of ‘width’, then set rx to half of ‘width’.
    If ry is greater than half of ‘height’, then set ry to half of ‘height’.
    The effective values of ‘rx’ and ‘ry’ are rx and ry, respectively.
	*/
	
	wxString xs = child->GetAttribute("x", "");
	wxString ys = child->GetAttribute("y", "");
	wxString width = child->GetAttribute("width", "");
	wxString height = child->GetAttribute("height", "");
	
	wxString rxs = child->GetAttribute("rx", "");
	wxString rys = child->GetAttribute("ry", "");
	long x=0, y=0, h=0, w=0, rx=0, ry=0;
	xs.ToLong(&x);
	ys.ToLong(&y);
	width.ToLong(&w);
	height.ToLong(&h);
	rxs.ToLong(&rx);
	rys.ToLong(&ry);

	// validation
	if ( rx > 0 && ry == 0)  ry = rx;
	if ( ry > 0 && rx == 0)  rx = ry;
	if ( rx > w/2 )			 rx = w/2;
	if ( ry > h/2 )			 ry = h/2;
	
	wxString path("M ");
	if ( rx == 0 ) {
		path << xs << " ";
		path << ys << " ";
		path << "l ";
		path << width  << " 0 ";
		path << "0 " << height;
		path << " -" << width << " 0 ";
		path << "z";
	} else {
		 
		//perform an absolute moveto operation to location (x+rx,y)
		path << x + rx << " " << y;
		//perform an absolute horizontal lineto operation to location (x+width-rx,y), 
		path << " L " << x + w - rx << " " << y;
		//perform an absolute elliptical arc operation to coordinate (x+width,y+ry), 
		//where rx and ry attributes on the elliptical arc command, respectively, 
		// the x-axis-rotation is set to zero, the large-arc-flag is set to zero, and the sweep-flag is set to one
		path << " A " << rx << " " << ry << " 0 0 1 " << x + w << " " << y + ry;
		//perform a absolute vertical lineto to location (x+width,y+height-ry)
		path << " L " << x + w << " " << y + h - ry;
		//perform an absolute elliptical arc operation to coordinate (x+width-rx,y+height)
		path << " A " << rx << " " << ry << " 0 0 1 " << x + w -rx << " " << y + h;
		//perform an absolute horizontal lineto to location (x+rx,y+height)
		path << " L " << x + rx << " " << y + h;
		//perform an absolute elliptical arc operation to coordinate (x,y+height-ry)
		path << " A " << rx << " " << ry << " 0 0 1 " << x << " " << y + h - ry;
		//perform an absolute vertical lineto to location (x,y+ry)
		path << " L " << x << " " << y + ry;
		//perform an absolute elliptical arc operation to coordinate (x+rx,y)		
		path << " A " << rx << " " << ry << " 0 0 1 " << x + rx << " " << y;
	}

	//std::clog << path.c_str() << std::endl;
	return evaluatePath(path);
}
///////////////////////////////////////////////////////////////////////
inline int SVGFileParser::getCommandParaCount(char c) {
/*
 Folgende Buchstaben bzw. Pfadverläufe sind möglich:

    moveto: M, m - Startpunkt (das Aufsetzen des imaginären Stiftes)
    lineto: L, l und H, h und V, v - eine gerade Linie
    closepath: Z, z - eine Pfad schließen
    cubic Bézier curve: C, c und S, s - eine kubische Bézier-Kurve
    quadratic Bézier curve: Q, q und T, t - eine quadratische Bézier-Kurve
    elliptical arc curve: A, a - einen elliptischen Bogen
*/
	int ret = -1;
	switch ( c ) {
		case 'z':
		case 'Z': ret = 0; break;
		case 'h':
		case 'H':
		case 'v':
		case 'V': ret = 1; break;
		case 'm':
		case 'M':
		case 'l':
		case 'L':
		case 't': 
		case 'T': ret = 2; break;
		case 'q':
		case 'Q':
		case 's':
		case 'S': ret = 4; break;
		case 'c':
		case 'C': ret = 6; break;
		case 'a':
		case 'A': ret = 7; break;

		default: ret = -1;
	} 
	return ret;
}
//////////////////////////////////////////////////////////////////
SVGUnit SVGFileParser::determineUnit (wxString uw, wxString uh) {
//////////////////////////////////////////////////////////////////
	SVGUnit unitW = SvgUnitCalculator::determineUnit(uw);
	SVGUnit unitH = SvgUnitCalculator::determineUnit(uh);

	if ( unitW == unitH)
		return unitH;
		
	std::cerr << "unitW(" << unitW << ") != unitH(" << unitH << ")" << std::endl;
	return unknown;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::setSVGWH(wxString w, wxString h) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	
	double width=0, height=0;
	wxString s, unitW, unitH;
	int sPos = -1;
	s = w;
	for ( int i=0; i<(int)s.Length();i++ ) {
		if ( s[i] == ' ' || s[i] == '\t' )
			continue;
		
		if ( isalpha(s[i]) && sPos < 0 ) {
			sPos = i;
			s.SubString(0, i - 1).ToDouble(&width);
			unitW = s.SubString(i, s.Length() -1);
		}
	}
	if ( sPos == -1 ) {
			s.SubString(0, s.Length() - 1).ToDouble(&width);
			unitW = "px";
	}

	sPos = -1;
	s = h;
	for ( int i=0; i<(int)s.Length();i++ ) {
		if ( s[i] == ' ' || s[i] == '\t' )
			continue;
		
		if ( isalpha(s[i]) && sPos < 0 ) {
			sPos = i;
			s.SubString(0, i - 1).ToDouble(&height);
			unitH = s.SubString(i, s.Length() -1);
		}
	}
	if ( sPos == -1 ) {
			s.SubString(0, s.Length() - 1).ToDouble(&height);
			unitH = "px";
	}
	
	SVGUnit unit = determineUnit(unitW, unitH);
	
	if ( unit == unknown )
		return false;
	
	pathHandler->setMaxDimensions(unit, width, height);
	svgUserAgent.initalize(unit, CncXYDoubleDimension(width, height));
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::setSVGViewBox(wxString vb) {
//////////////////////////////////////////////////////////////////
	pathHandler->setViewBox(vb);
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::debugNextPath() {
//////////////////////////////////////////////////////////////////
	CncWorkingParameters cwp = pathHandler->getCncWorkingParameters();
	runInfo.setLastLineNumber(cwp.currentLineNumber);
	runInfo.setNextFlag(true);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::debugNextStep() {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == true )
		runInfo.setNextFlag(true);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::debugStop() {
//////////////////////////////////////////////////////////////////
	runInfo.setCurrentDebugState(false);
	runInfo.setStopFlag(true);
	pathHandler->setDebugState(false);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::debugFinish() {
//////////////////////////////////////////////////////////////////
	runInfo.setCurrentDebugState(false);
	pathHandler->setDebugState(false);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::checkIfBreakpointIsActive() {
//////////////////////////////////////////////////////////////////
	//evaluate if current processing phase should be debugged
	if ( runInfo.getCurrentRunPhase() == SvgRunInfo::Svg_RP_Preprocesser && 
		 debugControls.debugPreprocessing != NULL && 
		 debugControls.debugPreprocessing->IsChecked() == false )
		return false;
		
	//evaluate if current processing phase should be debugged
	if ( runInfo.getCurrentRunPhase() == SvgRunInfo::Svg_RP_UserAgent && 
		 debugControls.debugUserAgent != NULL && 
		 debugControls.debugUserAgent->IsChecked() == false )
		return false;
		
	//evaluate if current processing phase should be debugged
	if ( runInfo.getCurrentRunPhase() == SvgRunInfo::Svg_RP_Spool && 
		 debugControls.debugSpooling != NULL && 
		 debugControls.debugSpooling->IsChecked() == false )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::freezeDebugControls(bool freeze) {
//////////////////////////////////////////////////////////////////
	//todo freezeDebugControls ???
	return;
	
	if ( freeze == true ) {
		if ( debugControls.debuggerControlBase ) {
			debugControls.debuggerControlBase->Update();
			debugControls.debuggerControlBase->Refresh();
			debugControls.debuggerControlBase->Freeze();
		}
	 
		if ( debugControls.debuggerControlPath ) {
			debugControls.debuggerControlPath->Update();
			debugControls.debuggerControlPath->Freeze();
		}
		
		if ( debugControls.debuggerControlDetail ) {
			debugControls.debuggerControlDetail->Update();
			debugControls.debuggerControlDetail->Freeze();
		}
	} else {
		if ( debugControls.debuggerControlBase && debugControls.debuggerControlBase->IsFrozen() ) {
			debugControls.debuggerControlBase->Thaw();
			debugControls.debuggerControlBase->Update();
		}
			
		if ( debugControls.debuggerControlPath && debugControls.debuggerControlPath->IsFrozen() ) {
			debugControls.debuggerControlPath->Thaw();
			debugControls.debuggerControlPath->Update();
		}
			
		if ( debugControls.debuggerControlDetail && debugControls.debuggerControlDetail->IsFrozen() ) {
			debugControls.debuggerControlDetail->Thaw();
			debugControls.debuggerControlDetail->Update();
		}
	}
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::evaluateDebugState(bool force) {
//////////////////////////////////////////////////////////////////
	// pause handling
	if ( runInfo.getPauseFlag() == true ) {
		wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
		while ( runInfo.getPauseFlag() == true ) {
			while ( evtLoop->Pending() )
				evtLoop->Dispatch();
				
			if ( cncControl->isInterrupted() == true )
				break;
				
			if ( runInfo.getStopFlag() == true )
				break;
		}
		runInfo.setPauseFlag(false);
		return;
	}

	// debug handling
	if ( runInfo.getCurrentDebugState() == true ) {
		
		if ( force == false ) {
			if ( checkIfBreakpointIsActive() == false )
				return;
		}
			
		wxEventLoopBase* evtLoop = wxEventLoopBase::GetActive();
		CncWorkingParameters cwp = pathHandler->getCncWorkingParameters();

		freezeDebugControls(false);
		displayCollectedTrace();
		//loop while next debug step should be appear (user event)
		while ( runInfo.getNextFlag() == false ) {
			while ( evtLoop->Pending() )
				evtLoop->Dispatch();
		
			if ( cncControl->isInterrupted() == true )
				break;
				
			if ( runInfo.getCurrentDebugState() == false )
				break;
				
			if ( runInfo.getLastLineNumber() >= 0  && runInfo.getLastLineNumber() == cwp.currentLineNumber) {
				break;
			} else {
				runInfo.setLastLineNumber(UNDEFINED_LINE_NUMBER);
			}
		}
		freezeDebugControls(true);
		runInfo.setNextFlag(false);
	}
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValuesToTrace(wxDataViewListCtrl* ctl, wxXmlNode* node) {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugMode() == false ) 
		return;
	
	wxASSERT(ctl);
	wxASSERT(node);
	
	for ( int i=0; i<ctl->GetItemCount(); i++ ) {
		wxXmlNode* n = new wxXmlNode();
		n->SetName("B");
		n->SetType(wxXML_CDATA_SECTION_NODE);

		wxString content = (ctl->GetTextValue(i, 0));
		content << "=";
		content << ctl->GetTextValue(i, 1);
		n->SetContent(content);
		node->AddChild(n);
	}
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::clearDebugControlBase() {
//////////////////////////////////////////////////////////////////
	if ( debugControls.debuggerControlBase == NULL )
		return;
	
	if ( debugControls.debuggerControlBase->GetItemCount() > 0 ) {
		debugBase = new wxXmlNode();
		debugBase->SetName("DebugBaseNode");
		debugBase->SetType(wxXML_ELEMENT_NODE);
		svgTrace.GetRoot()->AddChild(debugBase);
		
		appendDebugValuesToTrace(debugControls.debuggerControlBase, debugBase);
	}

	debugControls.debuggerControlBase->DeleteAllItems();
	debugControls.debuggerControlBase->Update();
	debugControls.debuggerControlBase->Refresh();
	clearDebugControlPath();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::clearDebugControlPath() {
//////////////////////////////////////////////////////////////////
	if ( debugControls.debuggerControlPath == NULL )
		return;
	
	if ( debugBase != NULL && debugControls.debuggerControlPath->GetItemCount() > 0 ) {
		debugPath = new wxXmlNode();
		debugPath->SetName("DebugPathNode");
		debugPath->SetType(wxXML_ELEMENT_NODE);
		debugBase->AddChild(debugPath);
		
		appendDebugValuesToTrace(debugControls.debuggerControlPath, debugPath);
	}

	debugControls.debuggerControlPath->DeleteAllItems();
	debugControls.debuggerControlPath->Update();
	debugControls.debuggerControlPath->Refresh();
	clearDebugControlDetail();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::clearDebugControlDetail() {
//////////////////////////////////////////////////////////////////
	if ( debugControls.debuggerControlDetail == NULL )
		return;

	if ( debugPath != NULL && debugControls.debuggerControlDetail->GetItemCount() > 0 ) {
		debugDetail = new wxXmlNode();
		debugDetail->SetName("DebugDetailNode");
		debugDetail->SetType(wxXML_ELEMENT_NODE);
		debugPath->AddChild(debugDetail);
		
		appendDebugValuesToTrace(debugControls.debuggerControlDetail, debugDetail);
	}
	
	debugControls.debuggerControlDetail->DeleteAllItems();
	debugControls.debuggerControlDetail->Update();
	debugControls.debuggerControlDetail->Refresh();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValue(wxDataViewListCtrl* ctl, const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == false )
		return;
		
	if ( checkIfBreakpointIsActive() == false )
		return;
		
	if ( ctl == NULL )
		return;
		
	wxVector<wxVariant> row;
	row.push_back(wxString(key));
	row.push_back(value.GetString());
	ctl->AppendItem(row);
	
	int itemCount = ctl->GetItemCount();
	ctl->EnsureVisible(ctl->RowToItem(itemCount - 1));
	ctl->EnsureVisible(ctl->RowToItem(0));
	ctl->Update();
	ctl->Refresh();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValue(wxDataViewListCtrl* ctl, DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugState() == false )
		return;
		
	if ( checkIfBreakpointIsActive() == false )
		return;
		
	for ( DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it ) {
		DcmRow row = *it;
		if ( row.size() == 2 ) {
			appendDebugValue(ctl, row.at(0).GetString(), row.at(1).GetString());
		}
	}
}
//////////////////////////////////////////////////////////////////
const char* SVGFileParser::convertToXmlString(wxString& value) {
//////////////////////////////////////////////////////////////////
	value.Replace(" ", "", true);
	return value;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValueBase( DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlBase, rows);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValuePath( DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlPath, rows);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValueDetail( DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlDetail, rows);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValueBase(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlBase, key, value);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValuePath(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlPath, key, value);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::appendDebugValueDetail(const char* key, wxVariant value) {
//////////////////////////////////////////////////////////////////
	appendDebugValue(debugControls.debuggerControlDetail, key, value);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::debugXMLAttribute(wxXmlAttribute *attribute, wxString& attrString) {
//////////////////////////////////////////////////////////////////
	if ( attribute == NULL )
		return;
	
	attrString << attribute->GetName();
	attrString << "=";
	attrString << attribute->GetValue();
	attrString << " ";
	
	// recursion call to get the compelete depth
	debugXMLAttribute(attribute->GetNext(), attrString);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::initXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	wxXmlAttribute* attr = child->GetAttributes();
	svgUserAgent.setNodeType(child->GetName());
	svgUserAgent.addXMLAttributes(attr);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::debugXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	initXMLNode(child);
	
	if ( runInfo.getCurrentDebugState() == false )
		return;

	CncWorkingParameters cwp = pathHandler->getCncWorkingParameters();
	appendDebugValueBase("Line Number", cwp.currentLineNumber);
	appendDebugValueBase("Reverse Path", cwp.getCorrectionType());
	appendDebugValueBase("Node", child->GetName());
	
	wxString content;
	wxXmlAttribute* attr = child->GetAttributes();
	debugXMLAttribute(attr, content);
	appendDebugValueBase("Attributes", content);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::clearControls() {
//////////////////////////////////////////////////////////////////
	clearDebugControlBase();
	svgUserAgent.clearControls();
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::createPreview(const wxString& resultingFileName, bool withErrorInfo) {
//////////////////////////////////////////////////////////////////
	return wxCopyFile(fileName, resultingFileName);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::pause() {
//////////////////////////////////////////////////////////////////
	runInfo.setPauseFlag(!runInfo.getPauseFlag());
	return runInfo.getPauseFlag();
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processRelease() {
//////////////////////////////////////////////////////////////////
	runInfo.releaseAllPhases();
	
	displayCollectedTrace(true);
	return process();
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processDebug() {
//////////////////////////////////////////////////////////////////
	runInfo.debugAllPhases();

	displayCollectedTrace(true);
	evaluateDebugState(true);
	return process();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::initNextRunPhase(SvgRunInfo::SvgRunPhase p) {
//////////////////////////////////////////////////////////////////
	runInfo.setCurrentRunPhase(p);
	pathHandler->setDebugState(runInfo.getCurrentDebugState());
	
	if ( debugControls.debugPhase == NULL )
		return;
		
	debugControls.debugPhase->SetLabel(runInfo.getCurrentDebugPhaseAsString());
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::process() {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);

	initNextRunPhase(SvgRunInfo::Svg_RP_Preprocesser);
	clearDebugControlBase();
	bool ret = preprocess();
	
	if ( runInfo.processMore() && ret == true ) {
		initNextRunPhase(SvgRunInfo::Svg_RP_UserAgent);
		svgUserAgent.expand();
		clearDebugControlBase();
		
		if ( runInfo.processMore() ) {
			initNextRunPhase(SvgRunInfo::Svg_RP_Spool);
			ret = spool();
		}
	} 
	
	initNextRunPhase(SvgRunInfo::Svg_RP_Unknown);
	freezeDebugControls(false);
	displayCollectedTrace();

	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::spool() {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	UserAgentVector uav = svgUserAgent.getList();
	
	pathHandler->prepareWork();

	// over all stored pathes
	for ( UserAgentVector::iterator itUav = uav.begin(); itUav != uav.end(); ++itUav ) {
		SVGUserAgentInfo uai  = *itUav;
		
		pathHandler->setCncWorkingParameters(uai.workingParameters);
	
		DcmItemList dil;
		uai.getBaseDetails(dil);
		appendDebugValueBase(dil);
		
		dil.clear();
		uai.getPathDetails(dil);
		appendDebugValuePath(dil);
		
		if ( performPath(uai) == false ) {
			std::cerr << "SVGFileParser::performPath: Failed" << std::endl;
			std::cerr << "Line number: " << uai.lineNumber << ", Node Type: " << uai.nodeName << std::endl;
			return false;
		}
		
		evaluateDebugState();
	}
	
	pathHandler->finishWork();
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::performPath(SVGUserAgentInfo& uai) {
//////////////////////////////////////////////////////////////////
	bool isSymbol = false;
	
	// Skip this special symbol generally
	if ( uai.isMemberOfSymbol("CncHelperGrid") ) {
		return true;
		
	// check if symbol to skip directly path processing
	} else if ( uai.isMemberOfSymbol() ) {
		isSymbol = true;
		
	// First prepare element
	} else {
		// reset current svg matrix
		pathHandler->getSvgTransformMatrix().unchanged();
		
		if ( uai.hasTransform() ) {
			// transfrom collected transformations
			if ( performTransform(uai) == false )
				return false;
		}
	} 
	
	if ( isSymbol == false ) {
		// spool this path directly
		if ( spoolPath(uai) == false )
			return false;
	}

	// spool this path by id (use directive)
	if ( performPathByIds(uai) == false ) {
		std::cerr << "SVGFileParser::performPathByIds: Failed" << std::endl;
		return false;
	}
			
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::performTransform(SVGUserAgentInfo& uai) {
//////////////////////////////////////////////////////////////////
	for (TransformVector::iterator it=uai.transformList.begin(); it!=uai.transformList.end(); ++it) {
		if ( pathHandler->getSvgTransformMatrix().performTransformAsStringList(*it) == false ) {
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::performPathByIds(SVGUserAgentInfo& uai) {
//////////////////////////////////////////////////////////////////
	DoubleStringMap ids = uai.ids;
	
	for ( DoubleStringMap::iterator itIds = ids.begin(); itIds != ids.end(); ++itIds ) {
		// over all use directives
		UseDirectiveVector udv = svgUserAgent.getUseInfoVector();

		for ( UseDirectiveVector::iterator it=udv.begin(); it!=udv.end(); ++it ) {
			UseDirective ud = *it;
			//cout << ud.id << endl;
			
			// if current path matches this the current use directive
			if ( ud.id == itIds->first ) {
				if ( performUse(uai, ud) == false )
					return false;
			}
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::performUse(SVGUserAgentInfo& uai, UseDirective& ud) {
//////////////////////////////////////////////////////////////////
	DoubleStringMap::iterator it;
	it = ud.attributes.find("transform");
	
	if ( it != ud.attributes.end() ) {
		
		cout << uai.lineNumber << endl;
		
		// reset current svg matrix
		pathHandler->getSvgTransformMatrix().unchanged();
		
		// first transform from use directive
		if ( pathHandler->getSvgTransformMatrix().performTransformAsStringList(it->second) == false ) {
			return false;
		}
		// second transform rest
		if ( performTransform(uai) == false )
			return false;
		
		//spool path 
		if ( spoolPath(uai, it->second) == false )
			return false;
			
	} else {
		//spool path 
		if ( spoolPath(uai, "") == false )
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::spoolPath(SVGUserAgentInfo& uai, const wxString& transform) {
//////////////////////////////////////////////////////////////////
	if ( uai.shouldProceed() == false )
		return true;
	
	SvgOriginalPathInfo sopi;
	sopi.pathData 			= uai.originalPath;
	sopi.transformInfo 		= uai.getTransformInfoAsString();
	sopi.useTransformInfo	= transform;
	
	if ( pathHandler->initNextPath(sopi) == false )
		return false;
		
	PathInfoVector piv = uai.pathInfoList;
	for ( PathInfoVector::iterator itPiv = piv.begin(); itPiv != piv.end(); ++itPiv ) {
		PathInfo pi = *itPiv;

		if ( pathHandler->process(pi.cmd, pi.count, pi.values) == false ) {
			std::cerr << "SVGFileParser::spoolPath failed" << std::endl;
			uai.debug(pi, std::cerr);
			return false;
		}
		evaluateDebugState();
	}
		
	if ( pathHandler->finishCurrentPath() == false )
		return false;
		
	// path is now recorded and tried out, now run the controller 
	if ( pathHandler->runCurrentPath() == false )
		return false;
		
	evaluateDebugState();
	clearDebugControlBase();

	if ( runInfo.getStopFlag() == true )
		return false;
		
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::selectSourceControl(wxStyledTextCtrl* ctl, unsigned long pos) {
//////////////////////////////////////////////////////////////////
	if ( ctl == NULL ) 
		return;
		
	ctl->GotoLine(pos);
	
	if ( pos == 0 ) {
		ctl->SetSelectionStart(0);
		ctl->SetSelectionEnd(0);
	} else {
		ctl->SetSelectionStart(ctl->GetCurrentPos());
		ctl->SetSelectionEnd(ctl->GetLineEndPosition(pos));
	}
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::selectSourceControl(unsigned long pos) {
//////////////////////////////////////////////////////////////////
	selectSourceControl(svgSourceControl, pos);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::preprocess() {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	
	wxXmlDocument doc;
	if ( !doc.Load(fileName) )
		return false;
		
	// Start processing the XML file.
	if ( doc.GetRoot()->GetName().Upper() != "SVG") {
		std::cerr << "SVGFileParser: Can't evaluate svg tag\n";
		return false;
	}
	
	wxString w = doc.GetRoot()->GetAttribute("width");
	wxString h = doc.GetRoot()->GetAttribute("height");
	wxString v = doc.GetRoot()->GetAttribute("viewBox");
	
	if ( setSVGWH(w, h) == false ) {
		std::cerr << "SVGFileParser: Unknown unit\n";
		return false;
	}
	if ( setSVGViewBox(v) == false ) {
		std::cerr << "SVGFileParser: Erorr while processing setSVGViewBox\n";
		return false;
	}

	clearDebugControlBase();
	
	// main entry point foor evaluateing all XML nodes
	wxXmlNode *child = doc.GetRoot()->GetChildren();
	cncNodeBreak = false;
	bool ret = processXMLNode(child);
	
	// reset the soure editor selection 
	selectSourceControl(0);

	if ( ret == false ) {
		std::cerr << "SVGFileParser: processXMLNode return false \n";
		std::cerr << " Current line numer: " << pathHandler->getCncWorkingParameters().currentLineNumber << std::endl;
		std::cerr << " Duration counter: " << cncControl->getDurationCounter() << std::endl;
		std::cerr << " File parsing stopped" << std::endl;
	} else {
		collectUserAgentTrace();
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	while ( child && cncNodeBreak == false) {
		pathHandler->getCncWorkingParameters().currentLineNumber = child->GetLineNumber();
		selectSourceControl(child->GetLineNumber() - 1);

		if ( child->GetName() == SvgNodeTemplates::CncParameterBlockNodeName ) {
			if ( evaluateCncParameters(child) == false )
				return false;
				
			debugXMLNode(child);
			svgUserAgent.initNextCncNode(pathHandler->getCncWorkingParameters());
				
		} else if (child->GetName() == SvgNodeTemplates::CncBreakBlockNodeName ) {
			cncNodeBreak = true;
			std::clog << SvgNodeTemplates::CncBreakBlockNodeName << " detected at line number: " << child->GetLineNumber() << std::endl;
			
			debugXMLNode(child);
			svgUserAgent.initNextCncNode(pathHandler->getCncWorkingParameters());
			
		} else if (child->GetName() == SvgNodeTemplates::CncPauseBlockNodeName ) {
			//todo
			std::clog << SvgNodeTemplates::CncPauseBlockNodeName << " isn't currently implemented. Line number: " << child->GetLineNumber() << std::endl;
			
			debugXMLNode(child);
			svgUserAgent.initNextCncNode(pathHandler->getCncWorkingParameters());
			
		} else if (child->GetName().Upper() == "SYMBOL" ) {
			wxString a = child->GetAttribute("id", "");
			svgUserAgent.addID(a, child->GetName().c_str());
			
			a = child->GetAttribute("transform", "");
			svgUserAgent.addTransform(a);
			
		} else if (child->GetName().Upper() == "G" ) {
			wxString a = child->GetAttribute("id", "");
			svgUserAgent.addID(a, child->GetName().c_str());
			
			a = child->GetAttribute("transform", "");
			svgUserAgent.addTransform(a);
			
			a = child->GetAttribute("style", "");
			svgUserAgent.addStyle(a);
			
		} else if ( child->GetName().Upper() == "PATH" ) {
			clearDebugControlBase();
			debugXMLNode(child);
			
			wxString data = child->GetAttribute("d", "");
			if ( evaluatePath(data)  == false )
				return false;
				
		} else if ( child->GetName().Upper() == "CIRCLE" ) {
			clearDebugControlBase();
			debugXMLNode(child);
			
			wxString ret; 
			if ( convertCircleToPathData(child, ret) )
				if ( processPathCommand(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "ELLIPSE" ) {
			clearDebugControlBase();
			debugXMLNode(child);
			
			wxString ret; 
			if ( convertEllipseToPathData(child, ret) )
				if ( processPathCommand(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "LINE" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( convertLineToPathData(child, ret) )
				if ( processPathCommand(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "POLYGON" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( convertPolygonToPathData(child, ret) )
				if ( processPathCommand(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "POLYLINE" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( convertPolylineToPathData(child, ret) )
				if ( processPathCommand(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "RECT" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( convertRectToPathData(child, ret) )
				if ( processPathCommand(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "USE" ) {
			clearDebugControlBase();
			
			UseDirectiveVector& udv = svgUserAgent.getUseInfoVector();
			UseDirective ud;
			evaluateUse(child->GetAttributes(), ud.attributes);
			udv.push_back(svgUserAgent.evaluateUseDirective(ud));
			
		} else if (child->GetName().Upper() == "CNC") {
			//todo
			std::clog << "Obsolete Node. <CNC> isn't longer implemented. Line number: " << child->GetLineNumber() << std::endl;
		}

		// recursion call to get the complete depth
		wxXmlNode* last = child;
		if ( processXMLNode(child->GetChildren()) == false ) {
			return false;
		}
		
		// close the id, transform and style attribute (on demand)
		if ( last != NULL ) {
			wxString a = last->GetAttribute("id", "");
			if ( a != "" )
				svgUserAgent.removeId(a);
				
			a = last->GetAttribute("transform", "");
			if ( a != "" )
				svgUserAgent.removeLastTransform();
			
			// only style attributes frmom <g> are colleted before
			if ( last->GetName().Upper() == "G" ) {
				a = last->GetAttribute("style", "");
				if ( a != "" )
					svgUserAgent.removeLastStyle();
			}
		}
		
		if ( cncControl->isInterrupted() == true )
			return false;
		
		child = child->GetNext();
	}

	return true;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::evaluateUse(wxXmlAttribute *attribute, DoubleStringMap& dsm) {
//////////////////////////////////////////////////////////////////
	if ( attribute == NULL )
		return;
		
	dsm[attribute->GetName()] = attribute->GetValue();
	evaluateUse(attribute->GetNext(), dsm);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::evaluatePath(wxString data) {
//////////////////////////////////////////////////////////////////
	appendDebugValueBase("Resulting Path", data);
	svgUserAgent.initNextPath(pathHandler->getCncWorkingParameters(), data);
	
	int sPos = -1;
	wxString token;
	for (unsigned int i=0; i<data.Length(); i++ ) {
		
		if ( data[i] == '-' || data[i] == '+' || data[i] == 'e' || data[i] == 'E' ) 
			continue;
			
		if ( isalpha(data[i]) && sPos < 0 ) {
			sPos = i;
		} else if ( isalpha(data[i]) ) {
			token = data.SubString(sPos, i - 1);
			if ( processPathCommand(token) == false )
				return false;
				
			sPos = i;
		} 
		
		if ( i == data.Length() - 1 ) {
			token = data.SubString(sPos, i);
			if ( processPathCommand(token) == false )
				return false;
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processPathCommand(wxString para) {
//////////////////////////////////////////////////////////////////
	if ( para.Length() == 0 )
		return true;

	clearDebugControlPath();
	appendDebugValuePath("Path Fragment", para);
	//std::clog << para.c_str() << std::endl;
	
	double values[MAX_PARAMETER_VALUES];
	
	wxString token;
	char c 						= '\0';
	unsigned int sPos 			= 0;
	unsigned int valueCounter 	= 0;
	unsigned int commandCounter = 0;
	int parameterCount 			= -1;

	para.Trim(true).Trim(false);
	for (unsigned int i=sPos; i<para.Length(); i++) {
		
		if ( i == 0 ) {
			c = para[0].GetValue();
			sPos++;
			if ( (parameterCount = getCommandParaCount(c) ) < 0 ) {
				std::cerr << "Not known command: " << c << std::endl;
				std::cerr << "Current line numer: " << pathHandler->getCncWorkingParameters().currentLineNumber << std::endl;
				break;
			}
		} else {
			if ( para[i] == ' ' || para[i] == ',' || para[i] == '-' || para[i] == '+' ) {
				// handle exponential presentation
				if ( (para[i] == '-' || para[i] == '+') && ( para[i-1] == 'e' || para[i-1] == 'E') ) 
					continue;
				
				if ( i != sPos ) {
					token = para.SubString(sPos, i - 1);
					token.ToDouble(&values[valueCounter++]);
					appendDebugValuePath("token", token);

					if ( valueCounter == MAX_PARAMETER_VALUES ) {
						std::cerr << "Max parameters count reached for: " << para.c_str() << std::endl;
						return false;
					}
				}
				if ( para[i] == '-'|| para[i] == '+' )	sPos = i;
				else									sPos = i + 1;
				
			} else if ( i == para.Length() - 1 ) {
				token = para.SubString(sPos, i);
				token.ToDouble(&values[valueCounter++]);
				appendDebugValuePath("token", token);
			}
		}
		
		if ( (int)valueCounter ==  parameterCount ) {
			commandCounter++;
			if ( commandCounter == 2 && ( c == 'm' || c == 'M' ) ) {
				// M - 1 = L or m - 1 = l
				c = c - 1;
			}
			
			bool ret = svgUserAgent.addPathElement(c, valueCounter, values);
			evaluateDebugState();
			
			if ( runInfo.getStopFlag() == true )
				return false;
			
			if ( ret == false )
				return false;

			valueCounter = 0;
		} 
	}
	
	bool ret = true;
	if ( valueCounter != 0 ) {
		std::cerr << "SVGFileParser:" << std::endl;
		std::cerr << "Parameters count error in: " << para.c_str() << std::endl;
		std::cerr << "Defined parameter count: " << parameterCount << "; Current value count: " << valueCounter << std::endl;
		std::cerr << "Current line numer: " << pathHandler->getCncWorkingParameters().currentLineNumber << std::endl;
		std::cerr << "Stored value list: " << std::endl;

		for (unsigned int i=0; i<valueCounter; i++) {
			std::cerr << "[" << i << "]=" << values[i] << "\t";
		}	
		std::cerr << std::endl;
		
		ret = false;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::evaluateCncParameters(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl && cncControl->getCncConfig());
	CncWorkingParameters& cwp = pathHandler->getCncWorkingParameters();
	
	wxString attr = child->GetAttribute("reverse", "no");
	cwp.setReverseFlag(attr);
	
	attr = child->GetAttribute("correction", "none");
	cwp.setCorrectionType(attr);
	
	attr = child->GetAttribute("depth", wxString::Format("Z%lf", 0.0));
	cwp.setCurrentZDepth(attr);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::collectUserAgentTrace() {
//////////////////////////////////////////////////////////////////
	if ( runInfo.getCurrentDebugMode() == false )
		return true;
	
	wxXmlNode* tr = new wxXmlNode();
	tr->SetName("Trace");
	tr->SetType(wxXML_ELEMENT_NODE);
	tr->AddAttribute("type", "User Agent Trace");
	svgTrace.GetRoot()->AddChild(tr);
	svgUserAgent.evaluateTraceInfo(tr);
	
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::displayCollectedTrace(bool blank) {
//////////////////////////////////////////////////////////////////
	if ( svgTraceControl == NULL )
		return;

	// create a temp file
	wxString tfn(CncFileNameService::getCncOutboundTraceFileName());
	
	if ( blank == true ) {
		
		createSvgTraceRoot();
		
		wxXmlNode* n = new wxXmlNode (svgTrace.GetRoot(), wxXML_ELEMENT_NODE, "Trace");
		n->AddAttribute("mode", "Empty");
		
	} else {
	
		if ( runInfo.getCurrentDebugMode() == false ) {
			createSvgTraceRoot();
			
			wxXmlNode* n = new wxXmlNode (svgTrace.GetRoot(), wxXML_ELEMENT_NODE, "Trace");
			n->AddAttribute("mode", runInfo.getDebugModeAsString());
		}
	}
	
	// Write the output 
	svgTrace.Save(tfn); 
	
	if ( svgTraceControl->IsShownOnScreen() == true ) {
		svgTraceControl->SetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT);
		svgTraceControl->SetZoom(wxWEBVIEW_ZOOM_TINY);
		svgTraceControl->LoadURL(tfn);
		svgTraceControl->Update();
	}
}