#include <iostream>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/stc/stc.h>
#include "GCodePathHandlerBase.h"
#include "GCodeFileParser.h"

//////////////////////////////////////////////////////////////////
GCodeFileParser::GCodeFileParser(const wxString& fn, GCodePathHandlerBase* ph) 
: FileParser(fn)
, pathHandler(ph)
, programEnd(false)
, displayWarnings(true)
, resumeOnError(true)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	pathHandler->setFileParser(this);
}
//////////////////////////////////////////////////////////////////
GCodeFileParser::~GCodeFileParser() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler != NULL )
		delete pathHandler;
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	pathHandler->initNextClientId(id);
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
	msg << wxString::Format("[%04d]: ", getCurrentLineNumber());
	msg << ss.str();
	
	switch ( type ) {
		case wxICON_ERROR:		if ( true )				{ std::cerr << msg.str() << ", GCode program will be stopped."   << std::endl; } break;
		case wxICON_WARNING:	if ( displayWarnings )	{ std::clog << msg.str() << ", GCode program will be continued." << std::endl; } break;
		default:				if ( displayWarnings )	{ std::cout << msg.str() << std::endl; } break;
	}
	
	return (resumeOnError == true );
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnhandledBlockCommand(GCodeBlock& gcb, const char* additionalInfo) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not handled GCode command: " << GCodeField(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber);
	if ( additionalInfo != NULL )
		ss << " [" << additionalInfo << "]";
		
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnsupportedBlockCommand(const GCodeField& field, const char* additionalInfo) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported GCode block command: " << field;
	if ( additionalInfo != NULL )
		ss << " [" << additionalInfo << "]";
		
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnhandledParameter(const GCodeField& field, const char* additionalInfo) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported GCode parameter: " << field.getCmd();
	if ( additionalInfo != NULL )
		ss << " [" << additionalInfo << "]";
		
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::setDefaultParameters() {
//////////////////////////////////////////////////////////////////
	programEnd = false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::preprocess() {
//////////////////////////////////////////////////////////////////
	pathHandler->prepareWork();
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::spool() {
//////////////////////////////////////////////////////////////////
	setDefaultParameters();

	wxFileInputStream input(fileName);
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	
	setCurrentLineNumber(0);
	GCodeBlock gcb;
	
	if ( input.IsOk() ) {
		while( input.IsOk() && !input.Eof() ) {
			wxString line = text.ReadLine();
			line.Trim(false).Trim(true);
			
			incCurrentLineNumber();
			
			if ( line.IsEmpty() == false ) {
				if ( processBlock(line, gcb) == false ) {
					std::cerr << "GCodeFileParser::preprocess(): Failed " <<std::endl;
					std::cerr << " Line number: " << getCurrentLineNumber() << std::endl;
					return false;
				}
				
				if ( evaluateDebugState() == false )
					return false;
			}
			
			if ( programEnd == true )
				break;
		}
		
		pathHandler->finishWork();
		return true;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::postprocess() {
//////////////////////////////////////////////////////////////////
#warning - something todo here?
/*
	// display the tool ID summary
	std::cout << wxString::Format("Tool ID Summary (count = %d):", (int)toolIds.size());
	for ( auto it = toolIds.begin(); it != toolIds.end(); ++it) {
		std::cout << " '" << (*it) << "'";
	}
	std::cout << std::endl;
*/
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processBlock(wxString& block, GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( GCodeBlock::removeComments(block, gcb.openComment) < 1 )
		return true;
	
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
		
		if ( evaluateProcessingState() == false )
			return false;
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
		case 'X':	gcb.x 			= gcb.ensureUnit(field.getValue());
					return true; 
					
		case 'Y':	gcb.y 			= gcb.ensureUnit(field.getValue());
					return true; 
					
		case 'Z':	gcb.z 			= gcb.ensureUnit(field.getValue());
					return true; 
					
		case 'I':	gcb.i 			= gcb.ensureUnit(field.getValue());
					return true; 
					
		case 'J':	gcb.j 			= gcb.ensureUnit(field.getValue());
					return true; 
					
		case 'S':	gcb.s 			= field.getValue();
					return true; 
					
		case 'E':	gcb.e 			= field.getValue();
					return true; 
					
		case 'F':	gcb.f 			= field.getValue();
					return true; 
					
		case 'T':	displayMessage(wxString::Format("Tool change prepared: ID: %.0lf", field.getValue()), wxICON_INFORMATION);
					setNextToolID(field.getValue());
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
			std::cerr << " Line number: " << getCurrentLineNumber() << std::endl;
		} 
		
		return true;
	}
	
	//gcb.trace(std::clog);
	bool ret = false;
	switch ( gcb.cmdCode ) {
		case 'G':	ret = processG(gcb);	break;
		case 'M':	ret = processM(gcb);	break;
		//....
		default: 	return displayUnhandledBlockCommand(gcb);
	}
	
	// perfom debug information 
	if ( runInfo.getCurrentDebugState() == true ) {
		registerNextDebugNode(GCodeCommands::explainGCodeCommand(gcb.nodeName)); 
		
		static DcmItemList rows;
		gcb.trace(rows);
		appendDebugValueBase(rows);
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processG(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
	
	switch ( gcb.cmdNumber ) {
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		case 0: 	// GC_G: Rapid Linear Move
		{
			return pathHandler->processRapidLinearMove(gcb);
		} //....................................................
		case 1: 	// GC_G: Linear Move
		{
			return pathHandler->processLinearMove(gcb);
		} //....................................................
		case 2:		// GC_G: Controlled Arc Move Clockwise
		{
			return pathHandler->processArcMove(gcb, false);
		} //....................................................
		case 3: 	// GC_G: Controlled Arc Move Counter Clockwise
		{
			return pathHandler->processArcMove(gcb, true);
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
			return pathHandler->moveToOrigin(gcb);
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
		case  0:		// GC_M_StopOrUnconditionalStop
		case  1:		// GC_M_SleepOrConditionalStop
		case  2:		// GC_M_ProgramEnd
		case 30:		// GC_M_ProgramEnd
		{
			programEnd = true; 
			return true;
		} //....................................................
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		case 3: 	// GC_M_SpindleOnClockwise
		{
			pathHandler->swichtToolOn();
			return true;
		}
		case 4:		// GC_M_SpindleOnCounterClockwise
		{
			return displayUnhandledBlockCommand(gcb);
		}
		case 5:		// GC_M_SpindleOff
		{
			pathHandler->swichtToolOff();
			return true;
		}
		case 6:		// GC_M_ToolChange
		{
			#warning todo - impl. tool change
			return true;
		}
		case 7:		// GC_M_MistCoolantOn
		case 8:		// GC_M_FloodCoolantOn
		case 9:		// GC_M_CoolantOff
		{
			return displayUnhandledBlockCommand(gcb);;
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





