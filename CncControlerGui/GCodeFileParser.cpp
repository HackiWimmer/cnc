#include <iostream>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/stc/stc.h>
#include "GCodeFileParser.h"

//////////////////////////////////////////////////////////////////
GCodeFileParser::GCodeFileParser(const wxString& fn, CncControl* cnc) 
: SVGFileParser(fn, cnc)
, currentBlockNode(NULL)
, programEnd(false)
, previewMode(false)
, resumeOnError(true)
, curPxPos({0, 0, 0})
{
//////////////////////////////////////////////////////////////////
	
}
//////////////////////////////////////////////////////////////////
GCodeFileParser::~GCodeFileParser() {
//////////////////////////////////////////////////////////////////
	if ( currentBlockNode != NULL )
		delete currentBlockNode;
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::streamErrorInfo() {
//////////////////////////////////////////////////////////////////
	if ( userMessages.size() == 0 )
		return;
		
	unsigned int x = 20, y = 20;
	preview << "<text x=\"" << x << "\" y=\"" << y << "\" style=\"fill:black;font-weight:bold;\">Processing information:" << std::endl;
	
	for ( UserMessages::iterator it = userMessages.begin(); it != userMessages.end(); ++it ) {
		y += 15;
		std::stringstream ss;
		ss << "<tspan x=\"" << x << "\" y=\"" << y << "\" style=\"fill:%s;font-weight:normal;\">" << (*it).msg.c_str() << "</tspan>" << std::endl;
		
		switch ( (*it).type ) {
			case wxICON_ERROR:		preview << wxString::Format(ss.str().c_str(), "red"); break;
			case wxICON_WARNING:	preview << wxString::Format(ss.str().c_str(), "blue"); break;
			default:				preview << wxString::Format(ss.str().c_str(), "black"); break;
		}
	}
	
	preview << "</text>";
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::createPreview(const wxString& resultingFileName, bool withErrorInfo) {
//////////////////////////////////////////////////////////////////
	bool ret = false;
	previewMode = true;
	
	preview.open(resultingFileName, std::ios::out | std::ios::trunc);
	if ( preview.is_open() ) {
		
		startPreview();
		preprocess();
		if ( withErrorInfo == true )
			streamErrorInfo();
		endPreview();
		
		CncDoublePosition xy;
		curPxPos.getMaxWatermarks(xy);
		//todo remove clog
		//std::clog << xy << std::endl;
		
		preview.flush();
		preview.close();
		ret = true;
	}
	
	previewMode = false;
	return ret;
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::startPreview() {
//////////////////////////////////////////////////////////////////
	preview << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
	preview << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"" << std::endl;
	preview << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << std::endl;
	preview << "<svg xmlns=\"http://www.w3.org/2000/svg\" "; 
	preview << "width=\""  << wxString::Format("%f%s",getDefaultWidth(), getDefaultUnit()) << "\" "; 
	preview << "height=\"" << wxString::Format("%f%s",getDefaultHeight(), getDefaultUnit()) << "\" ";
	preview << "xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
	preview << ">" << std::endl;
	preview.flush();
	
	userMessages.clear();
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::endPreview() {
//////////////////////////////////////////////////////////////////
	preview << "</svg>"; 
	preview.flush();
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayMessage(const wxString& msg, int type) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << msg;
	
	return displayMessage(ss, type);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayMessage(std::stringstream& ss, int type) {
//////////////////////////////////////////////////////////////////
	std::stringstream msg;
	msg << wxString::Format("[%04d]: ", pathHandler->getCncWorkingParameters().currentLineNumber);
	msg << ss.str();
	
	if ( previewMode == false ) {
		switch ( type ) {
			case wxICON_ERROR:		std::cerr << msg.str() << ", GCode program will be stopped." << std::endl; break;
			case wxICON_WARNING:	std::clog << msg.str() << ", GCode program will be continued." << std::endl; break;
			default:				std::cout << msg.str() << std::endl; break;
		}
	} else {
		GCodeFileParserMsgInfo mi;
		mi.msg 	= msg.str();
		mi.type	= type;
		userMessages.push_back(mi);
	}
	
	return (resumeOnError == true );
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnhandledBlockCommand(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not handled GCode command: " << GCodeField(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber);
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnsupportedBlockCommand(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported GCode block command: " << field;
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnhandledParameter(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported GCode parameter: " << field.getCmd();
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayToolChangeDetected(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported tool change: " << field;
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
template<typename T> inline void GCodeFileParser::appendBlockAttribute(const char* key, T value, const char* format) {
//////////////////////////////////////////////////////////////////
	if ( currentBlockNode == NULL )
		return;
	
	if ( value != INVALID_GCODE_FIELD_VALUE )
		currentBlockNode->AddAttribute(key, wxString::Format(format, value));
}
//////////////////////////////////////////////////////////////////
wxXmlAttribute* GCodeFileParser::getAttributes(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( currentBlockNode == NULL )
		return NULL;
	
	appendBlockAttribute("@X", 		gcb.x, "%6.3f");
	appendBlockAttribute("@Y", 		gcb.y, "%6.3f");
	appendBlockAttribute("@Z",		gcb.z, "%6.3f");
	appendBlockAttribute("I", 		gcb.i, "%6.3f");
	appendBlockAttribute("J", 		gcb.j, "%6.3f");
	appendBlockAttribute("E", 		gcb.e, "%6.3f");
	appendBlockAttribute("F", 		(long)gcb.f, "%d");
	
	currentBlockNode->AddAttribute("Unit", wxString::Format("%s", SvgUnitCalculator::getUnitAsStr(gcb.unit)));

	return currentBlockNode->GetAttributes();
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::createXmlBlockNode() {
//////////////////////////////////////////////////////////////////
	if ( currentBlockNode != NULL )
		delete currentBlockNode;
		
	currentBlockNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "GCodeBlock");
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::setDefaultParameters() {
//////////////////////////////////////////////////////////////////
	wxString w(wxString::Format("%f%s",getDefaultWidth(), getDefaultUnit()));
	wxString h(wxString::Format("%f%s",getDefaultHeight(), getDefaultUnit()));
	setSVGWH(w, h);
	programEnd = false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processRelease() {
//////////////////////////////////////////////////////////////////
	return SVGFileParser::processRelease();
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processDebug() {
//////////////////////////////////////////////////////////////////
	return SVGFileParser::processDebug();
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::process() {
//////////////////////////////////////////////////////////////////
	// todo
	if ( true )
		return SVGFileParser::process();
	
	{ // test
		cncControl->moveLinearMetricXYZ(100, 100, 40, false);
		cncControl->moveLinearMetricXYZ(-20, -40, 0, false);
		
		cncControl->moveLinearMetricXYZ(-80, -60, -40, false);
		
		//cncControl->reset();
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::preprocess() {
//////////////////////////////////////////////////////////////////
	setDefaultParameters();

	wxFileInputStream input(fileName);
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	
	pathHandler->getCncWorkingParameters().currentLineNumber = 0;
	GCodeBlock gcb;
	
	if ( input.IsOk() ) {
		while( input.IsOk() && !input.Eof() ) {
			wxString line = text.ReadLine();
			line.Trim(false).Trim(true);
			
			pathHandler->getCncWorkingParameters().currentLineNumber++;
			
			if ( line.IsEmpty() == false ) {
				if ( processBlock(line, gcb) == false ) {
					std::cerr << "GCodeFileParser::preprocess(): Failed " <<std::endl;
					std::cerr << " Line number: " << pathHandler->getCncWorkingParameters().currentLineNumber << std::endl;
					return false;
				}
			}
			
			if ( programEnd == true )
				break;
		}
		return true;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processBlock(wxString& block, GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( GCodeBlock::removeComments(block, gcb.openComment) < 1 )
		return true;
	
	createXmlBlockNode();
	gcb.reInit();
	gcb.block = block;
	
	wxStringTokenizer tokenizerBlank(block, " \t");
	while ( tokenizerBlank.HasMoreTokens() ) {
		wxString token = tokenizerBlank.GetNextToken();
		
		if ( token.IsEmpty() )
			continue;
			
		if ( isalpha(token[0]) == false )
			continue;
			
		GCodeField nextField(token);
	
		if ( gcb.isValid() && GCodeCommands::isBlockCommand(nextField.getCmd()) ) {
			if ( performBlock(gcb) == false )
				return false;
			
			gcb.reInit();
		} 
		
		processField(nextField, gcb);
	}
	
	return performBlock(gcb);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processField(const GCodeField& field, GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( GCodeCommands::isBlockCommand(field.getCmd()) ) {
		if ( GCodeCommands::isRegistered(field) == true ) {
			gcb.cmdCode 		= field.getCmd();
			gcb.cmdNumber 		= field.getNum();
			gcb.cmdSubNumber	= field.getSubNum();
			gcb.nodeName		= field.getToken();
			return true;
		} else {
			displayUnsupportedBlockCommand(field);
			return false;
		}
	}
	
	switch ( field.getCmd() ) {
		case 'X':	gcb.x 			= field.getValue();
					return true; 
					
		case 'Y':	gcb.y 			= field.getValue();
					return true; 
					
		case 'Z':	gcb.z 			= field.getValue();
					return true; 
					
		case 'I':	gcb.i 			= field.getValue();
					return true; 
					
		case 'J':	gcb.j 			= field.getValue();
					return true; 
					
		case 'S':	gcb.s 			= field.getValue();
					return true; 
					
		case 'E':	gcb.e 			= field.getValue();
					return true; 
					
		case 'F':	gcb.f 			= field.getValue();
					return true; 
					
		case 'T':	displayToolChangeDetected(field);
					return true;
		case 'N':
		case 'H':
		case '*':	//skip this fields
					return true;
					
		default: 	return displayUnhandledParameter(field);
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::performBlock(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.isValid() == false && gcb.hasMoveCmd() == true ) {
		gcb.copyPrevCmdToCmd();
	}
	
	if ( gcb.isValid() == false ) {
		if ( gcb.hasMoveCmd() ) {
			std::cerr << "GCodeFileParser::processBlock: Invalid GCode block:" << std::endl;
			std::cerr << " Command:     " << GCodeField(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber) << std::endl;
			std::cerr << " Block:       " << gcb  << std::endl;
			std::cerr << " Line number: " << pathHandler->getCncWorkingParameters().currentLineNumber << std::endl;
		} 
		
		return true;
	}
	
	svgUserAgent.setNodeType(gcb.nodeName);
	svgUserAgent.addXMLAttributes(getAttributes(gcb));
	
	//gcb.trace(std::clog);
	
	switch ( gcb.cmdCode ) {
		case 'G':	return processG(gcb);
		case 'M':	return processM(gcb);
		//....
		default: 	return displayUnhandledBlockCommand(gcb);
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processG(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	switch ( gcb.cmdNumber ) {
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		case 0: 	// GC_G: Rapid Linear Move
		{
			return processRapidLinearMove(gcb);
		} //....................................................
		case 1: 	// GC_G: Linear Move
		{
			return processLinearMove(gcb);
		} //....................................................
		case 2:		// GC_G: Controlled Arc Move Clockwise
		{
			return processArcMove(gcb, false);
		} //....................................................
		case 3: 	// GC_G: Controlled Arc Move Counter Clockwise
		{
			return processArcMove(gcb, true);
		}
		//....................................................
		case 20:	// GC_G: Set Units To Inches
		{
			gcb.unit = in; 
			return true;
		} //....................................................
		case 21:	// GC_G_SetUnitsToMillimeters
		{	
			gcb.unit = mm; 
			return true;
		} //....................................................
		case 17: 	// GC_G: XY Plane Select
		{
			gcb.plane = GCodeBlock::GC_XY_PLANE;
			return true;
		} //....................................................
		case 18: 	// GC_G: ZX Plane Select
		{
			gcb.plane = GCodeBlock::GC_ZX_PLANE;
			return true;
		} //....................................................
		case 19: 	// GC_G: YZ Plane Select
		{
			gcb.plane = GCodeBlock::GC_YZ_PLANE;
			return true;
		} //....................................................
		case 28: 	// GC_G: Move To Origin:
		{
			// todo inplement GCode: Move To Origin:
			return evaluatePath("M 0 0");
		} //....................................................
		case 90: 	// GC_G: Absolute Positioning:
		{
			if ( gcb.cmdSubNumber == 1L ) { // G90.1
				gcb.posModeIJ = GCodeBlock::GC_Absolute;
			} else {
				gcb.posModeXYZ = GCodeBlock::GC_Absolute;
				gcb.posModeIJ  = GCodeBlock::GC_Absolute;
			}
			return true;
		} //....................................................
		case 91:	// GC_G: Set To Relative Positioning:
		{
			if ( gcb.cmdSubNumber == 1L ) { // G91.1
				gcb.posModeIJ = GCodeBlock::GC_Relative;
			} else {
				gcb.posModeXYZ = GCodeBlock::GC_Relative;
				gcb.posModeIJ  = GCodeBlock::GC_Relative;
			}
			return true;
		} //....................................................
		case 92:	// GC_G: Set Position:
		{
			//todo implement GCode: Set Position:
			return true;
		} //....................................................
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		// Commands to skip:
		case 4: 	// GC_G: Dwell:
		{ 
			return true;
		} //....................................................
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		default: {
			return displayUnhandledBlockCommand(gcb);
		}
	}

	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processM(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	switch ( gcb.cmdNumber ) {
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		case 0:		// GC_M_StopOrUnconditionalStop
		case 1:		// GC_M_SleepOrConditionalStop
		case 2:		// GC_M_ProgramEnd
		{
			programEnd = true; 
			return true;
		} //....................................................
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		// Commands to skip:
		case 3: 	// GC_M_SpindleOnClockwise
		case 4:		// GC_M_SpindleOnCounterClockwise
		case 5: 	// GC_M_SpindleOff
		case 6:		// GC_M_ToolChange
		case 7:		// GC_M_MistCoolantOn
		case 8:		// GC_M_FloodCoolantOn
		case 9:		// GC_M_CoolantOff
		{
			// will all be handled by class SVGFileParser
			return true;
		} //....................................................
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		default: {
			if ( gcb.cmdNumber > 30 )
				return true; // skip all SD card commands
			
			return displayUnhandledBlockCommand(gcb);
		}
	}

	return false;
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::updateCurrentPxPosition(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.hasX() ) curPxPos.incX(gcb.getXMoveRelative(curPxPos));
	if ( gcb.hasY() ) curPxPos.incY(gcb.getYMoveRelative(curPxPos));
	if ( gcb.hasZ() ) curPxPos.incZ(gcb.getZMoveRelative(curPxPos));
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::processSvgPath() {
//////////////////////////////////////////////////////////////////
	if ( previewMode == true ) {
		preview << "<path d=\"";
		preview << svgPath.getPath();
		
// todo
//clog << svgPath.getPath() << endl;
		preview << "\"";
		preview << " fill=\"none\" stroke=\"black\" stroke-width=\"1.0\"";
		preview << " transform=\"scale(0.5)\""; //todo mirrow svg
		preview << "/>" << std::endl;
	} else {
		evaluatePath(svgPath.getPath());
	}
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processRapidLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	updateCurrentPxPosition(gcb);
	
			//todo
			if ( gcb.hasZ() )
				//clog << gcb.block << endl;
	
	if ( svgPath.available() ) {
		if ( svgPath.hasLinearMove() ) {
			processSvgPath();
			svgPath.initNextPath();
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processLinearMove(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	bool ret = svgPath.addGCB_AsLinearMove(curPxPos, gcb);
	updateCurrentPxPosition(gcb);
	return ret;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processArcMove(GCodeBlock& gcb, bool sweep) {
//////////////////////////////////////////////////////////////////
	bool ret = svgPath.addGCB_AsArcMove(curPxPos, gcb, sweep);
	updateCurrentPxPosition(gcb);
	return ret;
}
