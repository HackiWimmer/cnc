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
#include "MainFrameProxy.h"
#include "CncFileNameService.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncControl.h"
#include "CncStringLogger.h"
#include "CncTemplateContext.h"
#include "SVGPathHandlerCnc.h"
#include "SvgColourScheme.h"
#include "SVGFileParser.h"

//////////////////////////////////////////////////////////////////
SVGFileFormatter::SVGFileFormatter(const wxString& ifn)
: inboundFileName	(ifn)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
bool SVGFileFormatter::removeCncTags(wxXmlNode* child) {
//////////////////////////////////////////////////////////////////
	// -----------------------------------------------------------
	auto isCncTag = [&](const wxString& name) {
		
		if      ( name.IsSameAs(SvgNodeTemplates::CncParameterResetBlockNodeName) )	return true;
		else if ( name.IsSameAs(SvgNodeTemplates::CncParameterPrintBlockNodeName) )	return true;
		else if ( name.IsSameAs(SvgNodeTemplates::CncParameterBlockNodeName) )		return true;
		else if ( name.IsSameAs(SvgNodeTemplates::CncVariablesBlockNodeName) )		return true;
		else if ( name.IsSameAs(SvgNodeTemplates::CncMacroBlockNodeName) )			return true;
		else if ( name.IsSameAs(SvgNodeTemplates::CncBreakBlockNodeName) )			return true;
		else if ( name.IsSameAs(SvgNodeTemplates::CncPauseBlockNodeName) )			return true;
		
		return false;
	};
	
	// -----------------------------------------------------------
	while ( child ) {
		
		if ( removeCncTags(child->GetChildren()) == false )
			return false;
			
		wxXmlNode* next = child->GetNext();
		
		const wxString& name = child->GetName();
		if ( name.StartsWith("Cnc") && isCncTag(name) )  {
			const int ln = child->GetLineNumber();
			
			if ( child->GetParent()->RemoveChild(child) == false ) {
				std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": RemoveChild() failed! [%d] [%s]\n", ln, name));
				return false;
			}
		}
		
		child = next;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileFormatter::removeCncTags(wxXmlDocument& doc) {
//////////////////////////////////////////////////////////////////
	if ( doc.IsOk() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": wxXmlDocument::isOk() failed\n"));
		return false;
	}
	
	wxXmlNode* child = doc.GetRoot()->GetChildren();
	return removeCncTags(child);
}
//////////////////////////////////////////////////////////////////
bool SVGFileFormatter::removeCncTags(const wxString& ofn) {
//////////////////////////////////////////////////////////////////
	return convert(CNV_PRETTY_WITHOUT_CNC, ofn);
}
//////////////////////////////////////////////////////////////////
bool SVGFileFormatter::format(const wxString& ofn) {
//////////////////////////////////////////////////////////////////
	return convert(CNV_PRETTY_WITH_CNC, ofn);
}
//////////////////////////////////////////////////////////////////
bool SVGFileFormatter::compact(const wxString& ofn, bool rmvCncTags) {
//////////////////////////////////////////////////////////////////
	return convert(rmvCncTags ? CNV_COMPACT_WITHOUT_CNC : CNV_PRETTY_WITH_CNC, ofn);
}
//////////////////////////////////////////////////////////////////
bool SVGFileFormatter::convert(SVGFileFormatter::Mode m, const wxString& ofn) {
//////////////////////////////////////////////////////////////////
	if ( wxFileName(inboundFileName).Exists() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": Inbound file '%s' didn't exist!\n", inboundFileName));
		return false;
	}
	
	wxXmlDocument doc;
	{
		wxLogNull dummyToSuppressXmlDocErrorMessages;
		if ( doc.Load(inboundFileName) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": wxXmlDocument::Load('%s') failed!\n", inboundFileName));
			return false;
		}
	}
	
	int indent		= 2;
	bool rmvCncTags	= false;
	
	switch ( m ) {
		case CNV_PRETTY_WITHOUT_CNC:
			rmvCncTags	= true;
			indent		= 2;
			break;
		case CNV_PRETTY_WITH_CNC:
			rmvCncTags	= false;
			indent		= 2;
			break;
		case CNV_COMPACT_WITHOUT_CNC:
			rmvCncTags	= true;
			indent		= wxXML_NO_INDENTATION;
			break;
		case CNV_COMPACT_WITH_CNC:
			rmvCncTags	= false;
			indent		= wxXML_NO_INDENTATION;
			break;
	}
	
	if ( rmvCncTags == true ) {
		if ( removeCncTags(doc) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(wxString::Format(":removeCncTags('%s') failed!\n", inboundFileName));
			return false;
		}
	}
	
	if ( doc.Save(ofn, indent) == false ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": wxXmlDocument::Save('%s') failed!\n", ofn));
		return false;
	}
	
	return true;
}





//////////////////////////////////////////////////////////////////
#define SFP_ADD_SEP(msg)	APP_PROXY::parsingSynopsisTraceAddSeparator(wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));
#define SFP_LOG_INF(msg)	APP_PROXY::parsingSynopsisTraceAddInfo     (wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));
#define SFP_LOG_WAR(msg)	APP_PROXY::parsingSynopsisTraceAddWarning  (wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));
#define SFP_LOG_ERR(msg)	APP_PROXY::parsingSynopsisTraceAddError    (wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));

//////////////////////////////////////////////////////////////////
class svgUserAgent;

SVGFileParser::SVGFileParser(const wxString& fn, CncControl* cnc) 
: SVGParserBase		(new SVGPathHandlerCnc(cnc))
, FileParser		(fn)
, cncControl		(cnc)
, svgUserAgent		()
, currentNodeName	()
, debugBase			(NULL)
, debugPath			(NULL)
, debugDetail		(NULL)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	pathHandler->setFileParser(this);
}
//////////////////////////////////////////////////////////////////
SVGFileParser::~SVGFileParser() {
//////////////////////////////////////////////////////////////////
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
		soc.detailInfo 			= APP_PROXY::GetDvListCtrlSvgUADetailInfo();
		soc.inboundPathList		= APP_PROXY::GetDvListCtrlSvgUAInboundPathList();
		soc.useDirectiveList	= APP_PROXY::GetDvListCtrlSvgUAUseDirective();
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
	std::cerr	<< CNC_LOG_FUNCT << ": Invalid call, this class didn't support this method!" 
				<< std::endl
				<< "Nothig will be set." 
				<< std::endl
				;
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
	
	std::stringstream ss; ss << svgRootNode;
	APP_PROXY::GetSvgRootNode()->ChangeValue(ss.str());

	// reporting
	typedef CncUnitCalculator<float> UC;
	SFP_ADD_SEP("RootNode:");
	SFP_LOG_INF(wxString::Format("Translated RootNode: %s",  ss.str()));
	SFP_LOG_INF(wxString::Format(" Input Unit        : %s",  UC::getUnitAsStr(svgRootNode.getInputUnit())));
	SFP_LOG_INF(wxString::Format(" Width        [px] : %12.3lf", svgRootNode.getWidth()));
	SFP_LOG_INF(wxString::Format(" Heigth       [px] : %12.3lf", svgRootNode.getHeight()));
	SFP_LOG_INF(wxString::Format(" Width        [mm] : %12.3lf", svgRootNode.getWidth_MM()));
	SFP_LOG_INF(wxString::Format(" Heigth       [mm] : %12.3lf", svgRootNode.getHeight_MM()));
	SFP_LOG_INF(wxString::Format(" Scale X           : %12.3lf", svgRootNode.getScaleX()));
	SFP_LOG_INF(wxString::Format(" Scale Y           : %12.3lf", svgRootNode.getScaleY()));
	
	pathHandler->setSvgRootNode(svgRootNode);
	
	wxString ret;
	svgUserAgent.initalize();
	svgUserAgent.addTransform(svgRootNode.getRootTransformation(ret));
	
	return true;
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::broadcastDebugState(bool state) {
//////////////////////////////////////////////////////////////////
	getPathHandler()->setDebugState(state);
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
void SVGFileParser::registerXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	wxXmlAttribute* attr = child->GetAttributes();
	svgUserAgent.setNodeType(child->GetName());
	svgUserAgent.addXMLAttributes(attr);
	
	SvgCncContext& cwp = pathHandler->getSvgCncContext();
	cwp.setCurrentLineNumber(getCurrentLineNumber());
	
	// ----------------------------------------------------------
	if ( runInfo.getCurrentDebugState() == true ) {
		wxString content;
		debugXMLAttribute(attr, content);
		appendDebugValueBase("Attributes", content);
	}
}
//////////////////////////////////////////////////////////////////
void SVGFileParser::registerMovementNode() {
//////////////////////////////////////////////////////////////////
	static SvgColourAttributeDecoder	cad;
	static SvgColourDecoder				cs;
	
	SvgCncContext& cwp	= pathHandler->getSvgCncContext();
	const bool ucs		= cwp.useColourScheme();
	SFP_ADD_SEP(wxString::Format("Parse next Movement Node"));
	SFP_LOG_INF(wxString::Format("Use Colour Scheme    : %s", ucs ? "Yes" : "No"));
	
	if ( ucs == true ) {
		
		const wxString& style = svgUserAgent.getCurrentAttribute("style", "");
		if ( style.IsEmpty() == false ) {
			cad.reset();
			cad.decode(style);
			cwp.setFillColour(cad.getFillColour());
			cwp.setStrokeColour(cad.getStrokeColour());
		}
		
		const wxString& fill = svgUserAgent.getCurrentAttribute("fill", "");
		if ( fill.IsEmpty() == false ) {
			cs.setColour(fill);
			cwp.setFillColour(cs.getColour());
		}
		
		const wxString& stroke = svgUserAgent.getCurrentAttribute("stroke", "");
		if ( stroke.IsEmpty() == false ) {
			cs.setColour(stroke);
			cwp.setStrokeColour(cs.getColour());
		}
		
		cwp.determineColourEffects();
	}
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
	getPathHandler()->setDebugState(runInfo.getCurrentDebugState());
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::spool() {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	
	const UserAgentVector& uav = svgUserAgent.getList();
	pathHandler->prepareWork();
	
	SvgCncContextSummary sumCtx;
	
	// over all stored pathes
	for ( auto itUav = uav.begin(); itUav != uav.end(); ++itUav ) {
		const SVGUserAgentInfo& uai = *itUav;
		
		// ----------------------------------------------------------------------
		if ( uai.nodeName == SvgNodeTemplates::CncBreakBlockNodeName ) {
			std::cout << " CncBreak at line " << ( uai.lineNumber / CLIENT_ID.TPL_FACTOR )<< " detected. Processing will stop here." << std::endl;
			break;
		}
		
		// ----------------------------------------------------------------------
		if ( uai.nodeName == SvgNodeTemplates::CncPauseBlockNodeName ) {
			pathHandler->processWait(uai.cncPause.getMicroSeconds());
		}
		
		// ----------------------------------------------------------------------
		if ( uai.nodeName == SvgNodeTemplates::CncMacroBlockNodeName ) {
			// is already processed to the current cnc parameter
			continue;
		}
		
		// ----------------------------------------------------------------------
		if ( uai.nodeName == SvgNodeTemplates::CncParameterBlockNodeName ) {
			sumCtx.add(uai.cncParameters);
		}
		
		pathHandler->setSvgCncContext(uai.cncParameters);
		
		// important! the current node name has to be set before setCurrentLineNumer() 
		// to get a correct result in this overlaoded function
		currentNodeName.assign(uai.nodeName);
		
		if ( runInfo.getCurrentDebugState() == true ) {
			registerNextDebugNode(uai.nodeName);
			
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
				
			std::cerr	<< CNC_LOG_FUNCT << ": Failed" 
						<< std::endl
						<< "Line number: " << uai.lineNumber / CLIENT_ID.TPL_FACTOR 
						<< ", Node Type: " << uai.nodeName 
						<< std::endl
						;
			return false;
		}
		
		if ( evaluateDebugState() == false )
			return false;
	}
	
	THE_TPL_CTX->registerToolTotList(sumCtx.getToolTotList());
	THE_TPL_CTX->registerToolSelList(sumCtx.getToolSelList());
	
	return pathHandler->finishWork();
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::performPath(const SVGUserAgentInfo& uai) {
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
bool SVGFileParser::prepareTransformMatrix(const SVGUserAgentInfo& uai) {
//////////////////////////////////////////////////////////////////
	for (auto it=uai.transformList.begin(); it!=uai.transformList.end(); ++it) {
		if ( pathHandler->getSvgTransformMatrix().performTransformAsStringList(*it) == false ) {
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::performPathByIds(const SVGUserAgentInfo& uai) {
//////////////////////////////////////////////////////////////////
	DoubleStringMap ids = uai.ids;
	
	for ( auto itIds = ids.begin(); itIds != ids.end(); ++itIds ) {
		// over all use directives
		UseDirectiveVector& udv = svgUserAgent.getUseInfoVector();

		for ( auto it=udv.begin(); it!=udv.end(); ++it ) {
			UseDirective& ud = *it;
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
bool SVGFileParser::performUse(const SVGUserAgentInfo& uai, UseDirective& ud) {
//////////////////////////////////////////////////////////////////
	if ( auto it = ud.attributes.find("transform"); it != ud.attributes.end() ) {
		
		
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
bool SVGFileParser::spoolPath(const SVGUserAgentInfo& uai, const wxString& transform) {
//////////////////////////////////////////////////////////////////
	if ( uai.shouldProceed() == false )
		return true;
		
	if ( pathHandler->activateNextPath(uai.lineNumber) == false )
		return false;
	
	const PathInfoVector& pil = uai.getPathInfoList();
	for ( auto itPiv = pil.cbegin(); itPiv != pil.cend(); ++itPiv ) {
		const PathInfo& pi = *itPiv;
		
		if ( pathHandler->processCommand_2DXY(itPiv->cmd, itPiv->cnt, itPiv->values) == false ) {
			uai.debug(pi, std::cerr);
			std::cerr << CNC_LOG_FUNCT_A(": failed") << std::endl;
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
	
	SFP_ADD_SEP("Start SVG file parsing")
	SFP_LOG_INF(wxString::Format("File name: '%s'", fileName))
	
	wxXmlDocument doc;
	{
		CncStringLogger tmpLogger;
		if ( !doc.Load(fileName) ) {
			wxMessageBox(tmpLogger.GetBuffer(), CNC_LOG_FUNCT, wxICON_ERROR);
			return false;
		}
	}
	
	// Start processing the XML file.
	if ( doc.GetRoot()->GetName().Upper() != "SVG") {
		std::cerr << CNC_LOG_FUNCT << ": Can't evaluate svg tag\n";
		return false;
	}
	
	const wxString w = doc.GetRoot()->GetAttribute("width");
	const wxString h = doc.GetRoot()->GetAttribute("height");
	const wxString v = doc.GetRoot()->GetAttribute("viewBox");
	
	if ( setSVGRootNode(w, h, v) == false ) {
		std::cerr << CNC_LOG_FUNCT << ": setSVGRootNode() failed\n";
		return false;
	}
	
	// main entry point to evaluate all further XML nodes
	const bool ret = processXMLNode(doc.GetRoot()->GetChildren());
	
	if ( ret == false ) {
		std::cerr	<< CNC_LOG_FUNCT << " will return false"						<< std::endl
					<< " Current line numer: "	<< getCurrentLineNumber()			<< std::endl
					<< " Duration counter: "	<< cncControl->getDurationCounter()	<< std::endl
					<< " File parsing stopped"	<< std::endl
					;
	}
	else {
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
	
	const SVGRootNode& rn = getPathHandler()->getSvgRootNode();
	CncUnitCalculator<float> to_mm(rn.getInputUnit(), Unit::mm);
	
	const CncDoublePosition min(cncControl->getMinPositionsMetric());
	const CncDoublePosition max(cncControl->getMaxPositionsMetric());
	
	const double cncDistX 	= max.getX() - min.getX();
	const double cncDistY 	= max.getY() - min.getY();
	const double svgDistX 	= rn.getWidth_MM()  * rn.getScaleX();
	const double svgDistY 	= rn.getHeight_MM() * rn.getScaleY();
	
	const double cncMinX	= min.getX();
	const double cncMaxX	= max.getX();

	// if the Y axis will be reversed svg is also a right hand coord system
	const bool   cnv		= CncConfig::getGlobalCncConfig()->getSvgConvertToRightHandFlag();
	const double cncMinY	= min.getY() * (cnv ? +1 : -1);
	const double cncMaxY	= max.getY() * (cnv ? +1 : -1);
	
	const double svgMinX	= to_mm.convert(rn.getViewbox().getMinX());
	const double svgMaxX	= to_mm.convert(rn.getViewbox().getMaxX());
	const double svgMinY	= to_mm.convert(rn.getViewbox().getMinY());
	const double svgMaxY	= to_mm.convert(rn.getViewbox().getMaxY());
	
	// --------------------------------------------------------
	auto check_1_Less_2 = [&](double d1, double d2, const char* msg) {
		if ( d1 > d2 ) {
			SFP_LOG_ERR(wxString::Format("%s %12.3lf > %12.3lf", msg, d1, d2));
		}
	};
	
	SFP_ADD_SEP("Boundings:");
	SFP_LOG_INF(wxString::Format(" CNC distance x, y  [mm]: %12.3lf, %12.3lf", cncDistX, cncDistY));
	SFP_LOG_INF(wxString::Format(" SVG distance x, y  [mm]: %12.3lf, %12.3lf", svgDistX, svgDistY));
	SFP_LOG_INF(wxString::Format(" CNC min      x, y  [mm]: %12.3lf, %12.3lf", cncMinX, cncMinY));
	SFP_LOG_INF(wxString::Format(" SVG min      x, y  [mm]: %12.3lf, %12.3lf", svgMinX, svgMinY));
	SFP_LOG_INF(wxString::Format(" CNC max      x, y  [mm]: %12.3lf, %12.3lf", cncMaxX, cncMaxY));
	SFP_LOG_INF(wxString::Format(" SVG max      x, y  [mm]: %12.3lf, %12.3lf", svgMaxX, svgMaxY));

	check_1_Less_2(cncDistX, svgDistX, " CNC distance X out of range:  ");
	check_1_Less_2(cncDistY, svgDistY, " CNC distance Y out of range:  ");
	check_1_Less_2(svgMinX,  cncMinX,  " CNC min bound X out of range: ");
	check_1_Less_2(svgMinY,  cncMinY,  " CNC min bound Y out of range: ");
	check_1_Less_2(cncMaxX,  svgMaxX,  " CNC max bound X out of range: ");
	check_1_Less_2(cncMaxY,  svgMaxY,  " CNC max bound Y out of range: ");
	
	const bool ret = APP_PROXY::parsingSynopsisTraceHasErrorEntries() == false;
	
	if ( ret == false ) {
		SFP_ADD_SEP("Post Processing Error Summary:");
		std::cerr << "SVG post processing decteced error(s). For more details please visit the parsing synopsis trace\n";
	}
	else {
		if ( APP_PROXY::parsingSynopsisTraceHasWarnEntries() == true )
		cnc::cex1 << "SVG post processing decteced warnig(s). For more details please visit the parsing synopsis trace\n";
	}
	
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processXMLNode(wxXmlNode *child) {
//////////////////////////////////////////////////////////////////
	
	// -----------------------------------------------------------
	#define ADD_ATTR_ID \
	{ \
		const char * attrName = "id"; \
		if ( child->HasAttribute(attrName) ) \
			svgUserAgent.addID(child->GetAttribute("id", ""), child->GetName().c_str()); \
	}
	
	// -----------------------------------------------------------
	#define ADD_ATTR_TRANSFORM \
	{ \
		const char * attrName = "transform"; \
		if ( child->HasAttribute(attrName) ) \
			svgUserAgent.addTransform(child->GetAttribute(attrName)); \
	}
	// -----------------------------------------------------------
	#define ADD_ATTR_STYLE \
	{ \
		const char * attrName = "style"; \
		if ( child->HasAttribute(attrName) )  \
			svgUserAgent.addStyle(child->GetAttribute(attrName)); \
	}

	// -----------------------------------------------------------
	#define ADD_BASIC_SHAPE(function) \
	{ \
		registerMovementNode(); \
		registerNextDebugNode(currentNodeName); \
		\
		ADD_ATTR_TRANSFORM \
		\
		wxString ret; \
		if ( SVGElementConverter::function(child, ret) ) {\
			if ( false ) { \
				std::cout << "Constructed Path: " << ret << std::endl; \
			} \
			 \
			if ( evaluatePath(ret) == false ) \
				return false; \
		} \
	}
	
	
	// -----------------------------------------------------------
	// entry point
	while ( child ) {
		bool nodeConsidered = false;
		// important! the current node name has to be set before setCurrentLineNumer() 
		// to get a correct result in this overloaded functions
		currentNodeName.assign(child->GetName());
		setCurrentLineNumber(child->GetLineNumber());
		registerXMLNode(child);
		
		// ----------------------------------------------------------
		if ( currentNodeName.IsSameAs("PATH", false) ) {
			nodeConsidered  = true;
			
			registerMovementNode();
			registerNextDebugNode(currentNodeName);
			
			ADD_ATTR_TRANSFORM
			
			const wxString data(child->GetAttribute("d", ""));
			if ( evaluatePath(data)  == false )
				return false;
		}
		// ----------------------------------------------------------
		else if ( currentNodeName.IsSameAs("SYMBOL", false) ) {
			nodeConsidered  = true;
			
			ADD_ATTR_ID
			ADD_ATTR_TRANSFORM
		}
		// ----------------------------------------------------------
		else if ( currentNodeName.IsSameAs("G", false) ) {
			nodeConsidered  = true;
			
			ADD_ATTR_ID
			ADD_ATTR_TRANSFORM
			ADD_ATTR_STYLE
		}
		// ----------------------------------------------------------
		else if ( currentNodeName.IsSameAs("CIRCLE",	false) )	{ nodeConsidered  = true; ADD_BASIC_SHAPE(convertCircleToPathData)   }
		else if ( currentNodeName.IsSameAs("ELLIPSE",	false) )	{ nodeConsidered  = true; ADD_BASIC_SHAPE(convertEllipseToPathData)  }
		else if ( currentNodeName.IsSameAs("LINE",		false) )	{ nodeConsidered  = true; ADD_BASIC_SHAPE(convertLineToPathData)     }
		else if ( currentNodeName.IsSameAs("POLYGON",	false) )	{ nodeConsidered  = true; ADD_BASIC_SHAPE(convertPolygonToPathData)  }
		else if ( currentNodeName.IsSameAs("POLYLINE",	false) )	{ nodeConsidered  = true; ADD_BASIC_SHAPE(convertPolylineToPathData) }
		else if ( currentNodeName.IsSameAs("RECT",		false) )	{ nodeConsidered  = true; ADD_BASIC_SHAPE(convertRectToPathData)     }
		// ----------------------------------------------------------
		else if ( currentNodeName.IsSameAs("USE", false) ) {
			nodeConsidered  = true; 
			registerNextDebugNode(currentNodeName);
			
			UseDirectiveVector& udv = svgUserAgent.getUseInfoVector();
			UseDirective ud;
			evaluateUse(child->GetAttributes(), ud.attributes);
			udv.push_back(svgUserAgent.evaluateUseDirective(ud));
		}
		// ----------------------------------------------------------
		else if ( currentNodeName.StartsWith("Cnc") ) { 
			nodeConsidered  = true; 
			
			if ( processCncParameter(child) == false )
				return false; 
		}
		
		// check the debug state before the next node
		if ( evaluateDebugState() == false )
			return false;
			
		// recursion call to get the complete depth
		wxXmlNode* last = child;
		
		// GetChildren() returns the first child of this node
		if ( processXMLNode(child->GetChildren()) == false )
			return false;
		
		// close the id, transform and style attribute (on demand)
		if ( last != NULL ) {
			if ( nodeConsidered == true ) {
				if ( last->HasAttribute("id") )			svgUserAgent.removeId(last->GetAttribute("id"));
				if ( last->HasAttribute("transform") )	svgUserAgent.removeLastTransform();
				
				// only style attributes are collected from <g> before
				if ( last->GetName().IsSameAs("g", false) )
					if ( last->HasAttribute("style") )	svgUserAgent.removeLastStyle();
			}
		}
		
		if ( cncControl->isInterrupted() == true )
			return false;
		
		//swap
		child = child->GetNext();
		
		// check the debug state before the next node
		if ( evaluateDebugState() == false )
			return false;
	}
	
	return true;
	
	// -----------------------------------------------------------
	#undef ADD_ATTR_ID
	#undef ADD_ATTR_TRANSFORM
	#undef ADD_ATTR_STYLE
	#undef ADD_BASIC_SHAPE
}
//////////////////////////////////////////////////////////////////
bool SVGFileParser::processCncParameter(wxXmlNode* node) {
//////////////////////////////////////////////////////////////////
	if ( node == NULL )
		return false;
		
	SvgCncContext& cwp = pathHandler->getSvgCncContext();
	
	// ------------------------------------------------------
	if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncParameterResetBlockNodeName) ) {
		cwp.reset();
		
		registerNextDebugNode(currentNodeName);
		svgUserAgent.initNextCncParameterNode(pathHandler->getSvgCncContext());
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncParameterBlockNodeName) ) {
		registerNextDebugNode(currentNodeName);
		
		// replace cnc macros on demand
		const wxString macroKey(SvgCncContextMacro().MACRO_IDENTIFIER);
		wxString macroId;
		if ( node->GetAttribute(macroKey, &macroId) ) {
			node->DeleteAttribute(macroKey);
			
			const SvgCncContextMacro* macro = svgUserAgent.getMarcoWithId(macroId);
			if ( macro != NULL )	cwp.expand(*macro);
			else					cwp.addExternWarning(wxString::Format("Can't find macro '%s'", macroId));
		}
		
		if ( cwp.provide(node) == false )
			return false;
			
		svgUserAgent.initNextCncParameterNode(pathHandler->getSvgCncContext());
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncVariablesBlockNodeName) ) {
		registerNextDebugNode(currentNodeName);
		
		if ( cwp.provideVariables(node) == false )
			return false;
			
		svgUserAgent.initNextCncVaribalesNode(pathHandler->getSvgCncContext());
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncMacroBlockNodeName) ) {
		registerNextDebugNode(currentNodeName);
	
		SvgCncContextMacro scm; 
		scm.setCurrentLineNumber(getCurrentLineNumber());
		if ( scm.provide(node) == false )
			return false;
		
		svgUserAgent.initNextCncMacroNode(scm);
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncPauseBlockNodeName) ) {
		registerNextDebugNode(currentNodeName);
	
		SvgCncPause scp; 
		scp.setCurrentLineNumber(getCurrentLineNumber());
		if ( scp.provide(node) == false )
			return false;
		
		svgUserAgent.initNextCncPauseNode(scp);
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncBreakBlockNodeName) ) {
		registerNextDebugNode(currentNodeName);
		
		SvgCncBreak scb; 
		scb.setCurrentLineNumber(getCurrentLineNumber());
		
		svgUserAgent.initNextCncBreakNode(scb);
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs(SvgNodeTemplates::CncParameterPrintBlockNodeName) ) {
		cwp.traceTo(std::cout, 1);
			
		registerNextDebugNode(currentNodeName);
		svgUserAgent.initNextCncParameterNode(pathHandler->getSvgCncContext());
	}
	// ------------------------------------------------------
	else if ( currentNodeName.IsSameAs("CNC", false) ) {
		cnc::cex1 << "Obsolete Node. <CNC> isn't longer supported. Line number: " << node->GetLineNumber() << std::endl;
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
	svgUserAgent.initNextPath(pathHandler->getSvgCncContext(), data);
}
