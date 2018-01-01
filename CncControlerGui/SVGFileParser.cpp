#include <iostream>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/stc/stc.h>
#include <wx/webview.h>
#include <wx/filename.h>
#include "SvgEditPopup.h"
#include "SVGElementConverter.h"
#include "SvgUnitCalculator.h"
#include "CncFileNameService.h"
#include "CncControl.h"
#include "SVGPathHandlerCnc.h"
#include "SVGFileParser.h"

//////////////////////////////////////////////////////////////////
SVGFileParser::SVGFileParser(const wxString& fn, CncControl* cnc) 
: SVGNodeParser()
, FileParser(fn)
, cncControl(cnc)
, pathHandler(new SVGPathHandlerCnc(cnc))
, svgUserAgent()
, currentNodeName()
, debugBase(NULL)
, debugPath(NULL)
, debugDetail(NULL)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	pathHandler->setFileParser(this);
}
//////////////////////////////////////////////////////////////////
SVGFileParser::~SVGFileParser() {
//////////////////////////////////////////////////////////////////
	delete pathHandler;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	pathHandler->logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	pathHandler->logMeasurementEnd();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	pathHandler->initNextClientId(id);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::evaluateProcessingCallback() {
//////////////////////////////////////////////////////////////////
	return evaluateProcessingState();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::selectSourceControl(unsigned long pos) {
//////////////////////////////////////////////////////////////////
	// default handling
	if ( inboundSourceControl == NULL ) {
		FileParser::selectSourceControl(pos);
		return;
	}
	
	// debug only
	bool debug = false;
	if ( debug ) {
		std::clog << pos << ": "<< currentNodeName << std::endl;
		std::clog << inboundSourceControl->GetCurrentPos() << std::endl;
		std::clog << inboundSourceControl->GetLine(pos) << std::endl;
	}
	
	bool ok = false;
	
	long backStep = 1;
	wxString searchStart(currentNodeName);
	
	// only if the currentNodeName exists use it as serach start point
	// this is only the case if the parser runs preprocess() or spool()
	if ( inboundSourceControl->GetLine(pos).Contains(currentNodeName) == false ) {
		backStep = 0;
		searchStart.assign("<");
	}
	
	// sets the position to the start of the given line
	inboundSourceControl->GotoLine(pos);
	inboundSourceControl->Home();
	inboundSourceControl->SearchAnchor();
	// find start
	long sp = inboundSourceControl->SearchNext(0, searchStart);
	long ep = wxNOT_FOUND;
	
	if ( sp != wxNOT_FOUND ) {
		inboundSourceControl->SetCurrentPos(sp);
		inboundSourceControl->SearchAnchor();
		// find end
		ep = inboundSourceControl->SearchNext(0, ">");
		
		if ( ep != wxNOT_FOUND ) {
			// make the end visible
			inboundSourceControl->GotoPos(ep);
			// select
			inboundSourceControl->SetSelection(sp - backStep, ep + 1);
			ok = true;
		}
	}
	
	// debug only
	if ( debug )
		std::clog << sp << ", " << ep << "\n";
	
	// on error use the default handling
	if ( ok == false )
		FileParser::selectSourceControl(pos);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::setPathHandler(PathHandlerBase* ph) {
//////////////////////////////////////////////////////////////////
	std::cerr << "SVGFileParser::setPathHandler: Invalid call, this class didn't support this method!" << endl;
	std::cerr << "Nothig will be set." << endl;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::addPathElement(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	return svgUserAgent.addPathElement(c, count, values);
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
void SVGFileParser::broadcastDebugState(bool state) {
//////////////////////////////////////////////////////////////////
	pathHandler->setDebugState(state);
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
void SVGFileParser::registerXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	initXMLNode(child);
	
	if ( runInfo.getCurrentDebugState() == false )
		return;

	SvgCncParameters cwp = pathHandler->getSvgCncParameters();
	appendDebugValueBase("Reverse Path", cwp.getCorrectionType());
	
	wxString content;
	wxXmlAttribute* attr = child->GetAttributes();
	debugXMLAttribute(attr, content);
	appendDebugValueBase("Attributes", content);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::clearControls() {
//////////////////////////////////////////////////////////////////
	FileParser::clearControls();
	svgUserAgent.clearControls();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::initNextRunPhase(FileParserRunInfo::RunPhase p) {
//////////////////////////////////////////////////////////////////
	FileParser::initNextRunPhase(p);
	pathHandler->setDebugState(runInfo.getCurrentDebugState());
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::spool() {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	UserAgentVector uav = svgUserAgent.getList();
	
	pathHandler->prepareWork();

	// over all stored pathes#
	SVGUserAgentInfo uai;
	for ( UserAgentVector::iterator itUav = uav.begin(); itUav != uav.end(); ++itUav ) {
		uai = *itUav;
		
		if ( uai.nodeName == SvgNodeTemplates::CncBreakBlockNodeName ) {
			std::cout << " CncBreak at line " << uai.lineNumber << " detected. Processing will stop here." << std::endl;
			break;
		}
		
		if ( uai.nodeName == SvgNodeTemplates::CncPauseBlockNodeName ) {
			pathHandler->processWait(uai.cncPause.microseconds);
		}
		
		pathHandler->setCncWorkingParameters(uai.cncParameters);
		// important! the current node name has to be set before setCurrentLineNumer() 
		// to get a correct result in this overlaoded function
		currentNodeName.assign(uai.nodeName);
		setCurrentLineNumber(uai.lineNumber);
		registerNextDebugNode(uai.nodeName);
		
		if ( runInfo.getCurrentDebugState() == true ) {
			DcmItemList dil;
			uai.getBaseDetails(dil);
			appendDebugValueBase(dil);
			
			dil.clear();
			uai.getPathDetails(dil);
			appendDebugValuePath(dil);
		}
		
		if ( performPath(uai) == false ) {
			
			// in this case to stop here is valid
			if ( runInfo.getStopFlag() == true )
				return true;
				
			std::cerr << "SVGFileParser::performPath: Failed" << std::endl;
			std::cerr << "Line number: " << uai.lineNumber << ", Node Type: " << uai.nodeName << std::endl;
			return false;
		}
		
		if ( evaluateDebugState() == false )
			return false;
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
	sopi.pathData.assign(uai.originalPath);
	sopi.transformInfo.assign(uai.getTransformInfoAsString());
	sopi.useTransformInfo.assign(transform);
	
	if ( pathHandler->initNextPath(sopi) == false )
		return false;
		
	//PathInfoVector piv = uai.pathInfoList;
	for ( PathInfoVector::iterator itPiv = uai.pathInfoList.begin(); itPiv != uai.pathInfoList.end(); ++itPiv ) {
		if ( pathHandler->process(itPiv->cmd, itPiv->count, itPiv->values) == false ) {
			std::cerr << "SVGFileParser::spoolPath failed" << std::endl;
			uai.debug(*itPiv, std::cerr);
			return false;
		}
		
		if ( evaluateProcessingState() == false )
			return false;
	}
	
	if ( pathHandler->finishCurrentPath() == false )
		return false;
		
	// path is now recorded and tried out, now run the controller 
	if ( pathHandler->runCurrentPath() == false )
		return false;
		
	if ( evaluateProcessingState() == false )
		return false;
		
	return true;
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
	
	// main entry point foor evaluateing all XML nodes
	wxXmlNode *child = doc.GetRoot()->GetChildren();
	bool ret = processXMLNode(child);
	
	if ( ret == false ) {
		std::cerr << "SVGFileParser: processXMLNode return false \n";
		std::cerr << " Current line numer: " << getCurrentLineNumber() << std::endl;
		std::cerr << " Duration counter: " << cncControl->getDurationCounter() << std::endl;
		std::cerr << " File parsing stopped" << std::endl;
	} else {
		// fill the user agent controls
		svgUserAgent.expand();
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	while ( child ) {
		// important! the current node name has to be set before setCurrentLineNumer() 
		// to get a correct result in this overlaoded function
		currentNodeName.assign(child->GetName());
		setCurrentLineNumber(child->GetLineNumber());
		registerXMLNode(child);
		
		pathHandler->getSvgCncParameters().currentLineNumber = child->GetLineNumber();

		if ( child->GetName() == SvgNodeTemplates::CncParameterBlockNodeName ) {
			if ( evaluateCncParameters(child) == false )
				return false;
				
			registerNextDebugNode(currentNodeName);
			svgUserAgent.initNextCncParameterNode(pathHandler->getSvgCncParameters());
				
		} else if (child->GetName() == SvgNodeTemplates::CncBreakBlockNodeName ) {
			registerNextDebugNode(currentNodeName);
			
			SvgCncBreak scb;
			scb.currentLineNumber = getCurrentLineNumber();
			svgUserAgent.initNextCncBreakNode(scb);
			
		} else if (child->GetName() == SvgNodeTemplates::CncPauseBlockNodeName ) {
			registerNextDebugNode(currentNodeName);
		
			SvgCncPause scp;
			scp.currentLineNumber = getCurrentLineNumber();
			double p = 0.0;
			if ( child->GetAttribute("p", "0.0").ToDouble(&p) )
				scp.microseconds = (int64_t)(p * 1000 * 1000);
			
			svgUserAgent.initNextCncPauseNode(scp);
			
		} else if (child->GetName().Upper() == "SYMBOL" ) {
			wxString a(child->GetAttribute("id", ""));
			svgUserAgent.addID(a, child->GetName().c_str());
			
			a.assign(child->GetAttribute("transform", ""));
			svgUserAgent.addTransform(a);
			
		} else if (child->GetName().Upper() == "G" ) {
			wxString a(child->GetAttribute("id", ""));
			svgUserAgent.addID(a, child->GetName().c_str());
			
			a.assign(child->GetAttribute("transform", ""));
			svgUserAgent.addTransform(a);
			
			a.assign(child->GetAttribute("style", ""));
			svgUserAgent.addStyle(a);
			
		} else if ( child->GetName().Upper() == "PATH" ) {
			registerNextDebugNode(currentNodeName);
			
			wxString data(child->GetAttribute("d", ""));
			if ( evaluatePath(data)  == false )
				return false;
				
		} else if ( child->GetName().Upper() == "CIRCLE" ) {
			registerNextDebugNode(currentNodeName);
			
			wxString ret; 
			if ( SVGElementConverter::convertCircleToPathData(child, ret) )
				if  ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "ELLIPSE" ) {
			registerNextDebugNode(currentNodeName);
			
			wxString ret; 
			if ( SVGElementConverter::convertEllipseToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "LINE" ) {
			registerNextDebugNode(currentNodeName);

			wxString ret; 
			if ( SVGElementConverter::convertLineToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "POLYGON" ) {
			registerNextDebugNode(currentNodeName);

			wxString ret; 
			if ( SVGElementConverter::convertPolygonToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "POLYLINE" ) {
			registerNextDebugNode(currentNodeName);

			wxString ret; 
			if ( SVGElementConverter::convertPolylineToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "RECT" ) {
			registerNextDebugNode(currentNodeName);

			wxString ret; 
			if ( SVGElementConverter::convertRectToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "USE" ) {
			registerNextDebugNode(currentNodeName);
			
			UseDirectiveVector& udv = svgUserAgent.getUseInfoVector();
			UseDirective ud;
			evaluateUse(child->GetAttributes(), ud.attributes);
			udv.push_back(svgUserAgent.evaluateUseDirective(ud));
			
		} else if (child->GetName().Upper() == "CNC") {
			cnc::cex1 << "Obsolete Node. <CNC> isn't longer supported. Line number: " << child->GetLineNumber() << std::endl;
		}

		// check the debug state before the next node
		if ( evaluateDebugState() == false )
			return false;
			
		// recursion call to get the complete depth
		wxXmlNode* last = child;
		if ( processXMLNode(child->GetChildren()) == false ) {
			return false;
		}
		
		// close the id, transform and style attribute (on demand)
		if ( last != NULL ) {
			wxString a(last->GetAttribute("id", ""));
			if ( a != "" )
				svgUserAgent.removeId(a);
				
			a.assign(last->GetAttribute("transform", ""));
			if ( a != "" )
				svgUserAgent.removeLastTransform();
			
			// only style attributes frmom <g> are colleted before
			if ( last->GetName().Upper() == "G" ) {
				a.assign(last->GetAttribute("style", ""));
				if ( a != "" )
					svgUserAgent.removeLastStyle();
			}
		}
		
		if ( cncControl->isInterrupted() == true )
			return false;
		
		child = child->GetNext();
		
		// check the debug state before the next node
		if ( evaluateDebugState() == false )
			return false;
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
void SVGFileParser::initNextPath(const wxString& data) {
//////////////////////////////////////////////////////////////////
	svgUserAgent.initNextPath(pathHandler->getSvgCncParameters(), data);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::evaluateCncParameters(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	wxASSERT(cncControl);
	SvgCncParameters& cwp = pathHandler->getSvgCncParameters();
	
	wxString attr = child->GetAttribute("reverse", "no");
	cwp.setReverseFlag(attr);
	
	attr = child->GetAttribute("correction", "none");
	cwp.setCorrectionType(attr);
	
	attr = child->GetAttribute("depth", wxString::Format("Z%lf", 0.0));
	cwp.setCurrentZDepth(attr);
	
	return true;
}
