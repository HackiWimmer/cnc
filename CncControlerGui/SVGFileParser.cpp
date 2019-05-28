#include <iostream>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/filename.h>
#include "CncSourceEditor.h"
#include "OSD/webviewOSD.h"
#include "SvgEditPopup.h"
#include "SvgViewBox.h"
#include "SVGElementConverter.h"
#include "CncUnitCalculator.h"
#include "MainFrame.h"
#include "CncFileNameService.h"
#include "CncConfig.h"
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
	wxASSERT(pathHandler);
	pathHandler->logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
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
	
	inboundSourceControl->selectLineNumber(pos, currentNodeName);
}
////////////////////////////////////////////////////////////////////////////
void SVGFileParser::enableUserAgentControls(bool state) {
////////////////////////////////////////////////////////////////////////////
	SvgUserAgentOutputControls soc;
	
	if ( state == true ) {
		soc.detailInfo 			= THE_APP->GetDvListCtrlSvgUADetailInfo();
		soc.inboundPathList		= THE_APP->GetDvListCtrlSvgUAInboundPathList();
		soc.useDirectiveList	= THE_APP->GetDvListCtrlSvgUAUseDirective();
	} 
	else {
		soc.detailInfo 			= NULL;
		soc.inboundPathList		= NULL;
		soc.useDirectiveList	= NULL;
	}
	
	svgUserAgent.setOutputControls(soc);
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::setPathHandler(PathHandlerBase* ph) {
//////////////////////////////////////////////////////////////////
	std::cerr << "SVGFileParser::setPathHandler: Invalid call, this class didn't support this method!" << std::endl;
	std::cerr << "Nothig will be set." << std::endl;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::addPathElement(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	// part of preprocessing
	// append to user agent only
	return svgUserAgent.addPathElement(c, count, values);
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::determineUnit(const wxString& uw, const wxString& uh, Unit& u) {
//////////////////////////////////////////////////////////////////
	Unit unitW = Unit::px; 
	Unit unitH = Unit::px; 

	if ( CncUnitCalculatorBase::determineUnit(uw, unitW) == false )
		return false;
		
	if ( CncUnitCalculatorBase::determineUnit(uh, unitH) == false )
		return false;
		
	if ( unitW == unitH ) {
		u = unitH;
		return true;
	}
		
	std::cerr << "unitW(" << unitW << ") != unitH(" << unitH << ")" << std::endl;
	return false;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::setSVGRootNode(const wxString& w, const wxString& h, const wxString& vb) {
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
	
	Unit unit;
	if ( determineUnit(unitW, unitH, unit) == false )
		return false;
	
	SVGRootNode svgRootNode(width, height, unit, vb);
	
	if ( THE_APP != NULL ) {
		std::stringstream ss; ss << svgRootNode;
		THE_APP->GetSvgRootNode()->ChangeValue(ss.str());
	}
	
	pathHandler->setSvgRootNode(svgRootNode);
	
	wxString ret;
	svgUserAgent.initalize();
	svgUserAgent.addTransform(svgRootNode.getRootTransformation(ret));
	
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
		
		// always reset the svg matrix
		pathHandler->getSvgTransformMatrix().unchanged();
		
		if ( uai.hasTransform() ) {
			// transform collected transformations
			if ( prepareTransformMatrix(uai) == false )
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
bool SVGFileParser::prepareTransformMatrix(SVGUserAgentInfo& uai) {
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
		
		#warning - the use transformation did not work correctly
		
		
		// reset the svg matrix
		pathHandler->getSvgTransformMatrix().unchanged();
		
		// first transform from use directive
		if ( pathHandler->getSvgTransformMatrix().performTransformAsStringList(it->second) == false )
			return false;
		
		// second transform rest
		if ( prepareTransformMatrix(uai) == false )
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
	
	initNextClientId(uai.lineNumber);
		
	for ( auto itPiv = uai.pathInfoList.begin(); itPiv != uai.pathInfoList.end(); ++itPiv ) {
		if ( pathHandler->processCommand_2DXY(itPiv->cmd, itPiv->count, itPiv->values) == false ) {
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
	
	if ( setSVGRootNode(w, h, v) == false ) {
		std::cerr << "SVGFileParser: setSVGRootNode() failed\n";
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
bool SVGFileParser::postprocess() {
//////////////////////////////////////////////////////////////////
	if ( cncControl == NULL )
		return true;
		
	#warning activate SVGFileParser::postprocess() again
	return true;
	
	const SVGRootNode& rn 	= pathHandler->getSvgRootNode();
	CncUnitCalculator<float> to_mm(rn.getInputUnit(), Unit::mm);
	
	const CncDoublePosition min(cncControl->getMinPositionsMetric());
	const CncDoublePosition max(cncControl->getMaxPositionsMetric());
	
	const double cncDistX 	= max.getX() - min.getX();
	const double cncDistY 	= max.getY() - min.getY();
	const double svgDistX 	= rn.getWidth_MM()  * rn.getScaleX();
	const double svgDistY 	= rn.getHeight_MM() * rn.getScaleY();
	
	const double cncMinX	= min.getX();
	const double cncMinY	= min.getY();
	const double cncMaxX	= max.getX();
	const double cncMaxY	= max.getY();
	
	const double svgMinX	= to_mm.convert(rn.getViewbox().getMinX());
	const double svgMinY	= to_mm.convert(rn.getViewbox().getMinY());
	const double svgMaxX	= to_mm.convert(rn.getViewbox().getMaxX());
	const double svgMaxY	= to_mm.convert(rn.getViewbox().getMaxY());
	
	// ....
	auto trace = [&](std::ostream& out) {
		
		out << " CNC distance x, y  [mm]: " << wxString::Format("%4.3lf, %4.3lf", cncDistX, cncDistY) 	<< std::endl;
		out << " SVG distance x, y  [mm]: " << wxString::Format("%4.3lf, %4.3lf", svgDistX, svgDistY) 	<< std::endl;
		out << " CNC min      x, y  [mm]: " << wxString::Format("%4.3lf, %4.3lf", cncMinX, cncMinY) 	<< std::endl;
		out << " SVG min      x, y  [mm]: " << wxString::Format("%4.3lf, %4.3lf", svgMinX, svgMinY) 	<< std::endl;
		out << " CNC max      x, y  [mm]: " << wxString::Format("%4.3lf, %4.3lf", cncMaxX, cncMaxY) 	<< std::endl;
		out << " SVG max      x, y  [mm]: " << wxString::Format("%4.3lf, %4.3lf", svgMaxX, svgMaxY) 	<< std::endl;
	};
	
	bool ret = true;
	auto check_1_Less_2 = [&](double d1, double d2, std::ostream& out, const char* msg) {
		if ( d1 > d2 ) {
			out << msg 
			    << wxString::Format("%4.3lf", d1)
			    << " > " 
				<< wxString::Format("%4.3lf", d2)
			    << std::endl;
				
			ret = false;
		}
	};
	
	check_1_Less_2(cncDistX, svgDistX, std::cerr, " CNC distance X out of range: ");
	check_1_Less_2(cncDistY, svgDistY, std::cerr, " CNC distance Y out of range: ");
	check_1_Less_2(svgMinX,  cncMinX,  std::cerr, " CNC min bound X out of range: ");
	check_1_Less_2(svgMinY,  cncMinY,  std::cerr, " CNC min bound Y out of range: ");
	check_1_Less_2(cncMaxX,  svgMaxX,  std::cerr, " CNC max bound X out of range: ");
	check_1_Less_2(cncMaxY,  svgMaxY,  std::cerr, " CNC max bound Y out of range: ");
	
	if ( ret == false ) 
		std::cerr << "\n SVG post processing report: Error(s) detected" << std::endl;
		
	if ( ret == false ) 
		trace( ret == true ? std::cout : std::cerr);
	
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
			if ( a.IsEmpty() == false )
				svgUserAgent.removeId(a);
				
			a.assign(last->GetAttribute("transform", ""));
			if ( a.IsEmpty() == false )
				svgUserAgent.removeLastTransform();
			
			// only style attributes frmom <g> are colleted before
			if ( last->GetName().Upper() == "G" ) {
				a.assign(last->GetAttribute("style", ""));
				if ( a.IsEmpty() == false )
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
