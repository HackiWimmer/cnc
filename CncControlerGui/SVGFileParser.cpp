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
#include "SVGElementConverter.h"
#include "SvgUnitCalculator.h"
#include "CncFileNameService.h"
#include "CncControl.h"
#include "SVGPathHandlerCnc.h"
#include "SVGFileParser.h"

//////////////////////////////////////////////////////////////////
SVGFileParser::SVGFileParser(const char* fn, CncControl* cnc) 
: SVGNodeParser()
, cncControl(cnc)
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
void SVGFileParser::setPathHandler(SVGPathHandlerBase* ph) {
//////////////////////////////////////////////////////////////////
	std::cerr << "SVGFileParser::setPathHandler: Invalid call, this class didn't support this method!" << endl;
	std::cerr << "Nothig will be set." << endl;
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
long SVGFileParser::getCurrentLineNumber() {
//////////////////////////////////////////////////////////////////
	return pathHandler->getCncWorkingParameters().currentLineNumber;
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
bool SVGFileParser::shouldStop() {
//////////////////////////////////////////////////////////////////
	return runInfo.getStopFlag();
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
			if ( SVGElementConverter::convertCircleToPathData(child, ret) )
				if  ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "ELLIPSE" ) {
			clearDebugControlBase();
			debugXMLNode(child);
			
			wxString ret; 
			if ( SVGElementConverter::convertEllipseToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "LINE" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( SVGElementConverter::convertLineToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "POLYGON" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( SVGElementConverter::convertPolygonToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "POLYLINE" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( SVGElementConverter::convertPolylineToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
					return false;
					
		} else if ( child->GetName().Upper() == "RECT" ) {
			clearDebugControlBase();
			debugXMLNode(child);

			wxString ret; 
			if ( SVGElementConverter::convertRectToPathData(child, ret) )
				if ( evaluatePath(ret) == false )
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
void SVGFileParser::initNextPath(const wxString& data) {
//////////////////////////////////////////////////////////////////
	svgUserAgent.initNextPath(pathHandler->getCncWorkingParameters(), data);
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