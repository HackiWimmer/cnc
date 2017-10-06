#include <iostream>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/stc/stc.h>
#include "3D/CncGCodePreview.h"
#include "GCodePathHandlerGL.h"
#include "GCodeFileParser.h"

//////////////////////////////////////////////////////////////////
GCodeFilePreview::GCodeFilePreview(const wxString& fn, GCodePathHandlerBase* ph) 
: FileParser(fn)
, pathHandler(ph)
, currentLineNumber(0)
, programEnd(false)
, resumeOnError(true)
{
//////////////////////////////////////////////////////////////////
	wxASSERT(pathHandler);
}
//////////////////////////////////////////////////////////////////
GCodeFilePreview::~GCodeFilePreview() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler != NULL )
		delete pathHandler;
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::displayMessage(const wxString& msg, int type) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << msg;
	
	return displayMessage(ss, type);
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::displayMessage(std::stringstream& ss, int type) {
//////////////////////////////////////////////////////////////////
	std::stringstream msg;
	msg << wxString::Format("[%04d]: ", currentLineNumber);
	msg << ss.str();
	
	switch ( type ) {
		case wxICON_ERROR:		std::cerr << msg.str() << ", GCode program will be stopped." << std::endl; break;
		case wxICON_WARNING:	std::clog << msg.str() << ", GCode program will be continued." << std::endl; break;
		default:				std::cout << msg.str() << std::endl; break;
	}
	
	return (resumeOnError == true );
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::displayUnhandledBlockCommand(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not handled GCode command: " << GCodeField(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber);
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::displayUnsupportedBlockCommand(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported GCode block command: " << field;
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::displayUnhandledParameter(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported GCode parameter: " << field.getCmd();
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::displayToolChangeDetected(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Not supported tool change: " << field;
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
void GCodeFilePreview::setDefaultParameters() {
//////////////////////////////////////////////////////////////////
	programEnd = false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::process() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler == NULL )
		return true;
	
	// todo
	pathHandler->prepareWork();
	
	bool ret = preprocess();
	
	pathHandler->finishWork();
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::preprocess() {
//////////////////////////////////////////////////////////////////
	setDefaultParameters();

	wxFileInputStream input(fileName);
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	
	currentLineNumber = 0;
	GCodeBlock gcb;
	
	if ( input.IsOk() ) {
		while( input.IsOk() && !input.Eof() ) {
			wxString line = text.ReadLine();
			line.Trim(false).Trim(true);
			
			currentLineNumber++;
			
			if ( line.IsEmpty() == false ) {
				if ( processBlock(line, gcb) == false ) {
					std::cerr << "GCodeFilePreview::preprocess(): Failed " <<std::endl;
					std::cerr << " Line number: " << currentLineNumber << std::endl;
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
bool GCodeFilePreview::processBlock(wxString& block, GCodeBlock& gcb) {
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
	}
	
	return performBlock(gcb);
}
//////////////////////////////////////////////////////////////////
bool GCodeFilePreview::processField(const GCodeField& field, GCodeBlock& gcb) {
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
bool GCodeFilePreview::performBlock(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	if ( gcb.isValid() == false && gcb.hasMoveCmd() == true ) {
		gcb.copyPrevCmdToCmd();
	}
	
	if ( gcb.isValid() == false ) {
		if ( gcb.hasMoveCmd() ) {
			std::cerr << "GCodeFilePreview::processBlock: Invalid GCode block:" << std::endl;
			std::cerr << " Command:     " << GCodeField(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber) << std::endl;
			std::cerr << " Block:       " << gcb  << std::endl;
			std::cerr << " Line number: " << currentLineNumber << std::endl;
		} 
		
		return true;
	}
	
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
bool GCodeFilePreview::processG(GCodeBlock& gcb) {
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
bool GCodeFilePreview::processM(GCodeBlock& gcb) {
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





