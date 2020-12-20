#include <iostream>
#include <sstream>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include "CncSourceEditor.h"
#include "CncConfig.h"
#include "MainFrame.h"
#include "CncAutoFreezer.h"
#include "CncGCodeSequenceListCtrl.h"
#include "GCodePathHandlerBase.h"
#include "GCodeFileParser.h"

//////////////////////////////////////////////////////////////////
GCodeFileParser::GCodeFileParser(const wxString& fn, GCodePathHandlerBase* ph) 
: FileParser		(fn)
, pathHandler		(ph)
, gCodeSequence		()
, programEnd		(false)
, displayWarnings	(true)
, resumeOnError		(true)
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
	msg << wxString::Format(" [%04d]: ", getCurrentLineNumber());
	msg << ss.str();
	
	switch ( type ) {
		case wxICON_ERROR:		if ( true )				{ std::cerr << msg.str() << ", GCode program will be stopped."   << std::endl; } break;
		case wxICON_WARNING:	if ( displayWarnings )	{ std::cout << msg.str() << ", GCode program will be continued." << std::endl; } break;
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
void GCodeFileParser::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	pathHandler->logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void GCodeFileParser::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	pathHandler->logMeasurementEnd();
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::preprocess() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler->prepareWork() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(": pathHandler->prepareWork() failed!\n");
		return false;
	}
	
	setDefaultParameters();
	
	// read the file content
	wxFileInputStream input(fileName);
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	
	setCurrentLineNumber(0);
	GCodeBlock gcb;
	
	CncGCodeSequenceListCtrl* ctrl = THE_APP->getGCodeSequenceList();
	ctrl->clearAll();
	
	if ( input.IsOk() ) {
		while( input.IsOk() && !input.Eof() ) {
			wxString line = text.ReadLine();
			line.Trim(false).Trim(true);
			
			incCurrentLineNumber();
			
			if ( line.IsEmpty() == false ) {
				if ( processBlock(line, gcb) == false ) {
					std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": Failed! Line number: %ld\n", getCurrentLineNumber()));
					return false;
				}
				
				if ( evaluateDebugState() == false )
					return false;
			}
			
			if ( programEnd == true )
				break;
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::spool() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler->isPathListUsed() == false )
		return true;
	
	// over all commands
	CncGCodeSequenceListCtrl* ctrl = THE_APP->getGCodeSequenceList();
	CncAutoFreezer caf(ctrl);
	
	for ( auto it = gCodeSequence.begin(); it != gCodeSequence.end(); ++it) {
		performBlock(*it);
		
		if ( THE_APP->isDisplayParserDetails() == true )
			ctrl->addBlock(*it);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::postprocess() {
//////////////////////////////////////////////////////////////////
	return pathHandler->finishWork();
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
			if ( prepareBlock(gcb) == false )
				return false;
			
			gcb.reInit();
		} 
		
		processField(nextField, gcb);
		
		if ( evaluateProcessingState() == false )
			return false;
	}
	
	return prepareBlock(gcb);
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
					
		case 'T':	//displayMessage(wxString::Format("Tool change prepared: ID: %.0lf", field.getValue()), wxICON_INFORMATION);
					setNextToolID(field.getValue());
					return true;
					
		case 'P':	gcb.p 			= field.getValue();
					return true; 
					
		case 'H':	gcb.h 			= field.getValue();
					return true; 
					
		case 'N':
		case '*':	//skip this fields
					return true;
					
		default: 	return displayUnhandledParameter(field);
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::prepareBlock(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	gcb.clientID = getCurrentLineNumber() * CLIENT_ID.TPL_FACTOR;
	
	if ( gcb.isValid() == false && gcb.hasMoveCmd() == true ) {
		gcb.copyPrevCmdToCmd();
	}
	
	if ( gcb.isValid() == false ) {
		if ( gcb.hasMoveCmd() ) {
			std::cerr << "GCodeFileParser::prepareBlock: Invalid GCode block:" << std::endl;
			std::cerr << " Command:     " << GCodeField(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber) << std::endl;
			std::cerr << " Block:       " << gcb  << std::endl;
			std::cerr << " Line number: " << getCurrentLineNumber() << std::endl;
		} 
		
		return true;
	}
	
	// work with intermediate step on demand
	if ( pathHandler->isPathListUsed() ) {
		gCodeSequence.push_back(gcb);
		
		// in this case stop here 
		return true;
	}
	
	// this is only done withou a path list - see above
	// Normaly for previews etc.
	if ( THE_APP->isDisplayParserDetails() == true ) {
		CncGCodeSequenceListCtrl* ctrl = THE_APP->getGCodeSequenceList();
		ctrl->addBlock(gcb);
	}
	
	return performBlock(gcb);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::performBlock(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	//gcb.trace(std::clog);
	bool ret = false;
	
	// check the new path trigger and init it before 
	// calling initNextClientId()
	if ( gcb.cmdCode == 'G' && gcb.cmdNumber == 0 )
		pathHandler->initNextPath();
	
	initNextClientId(gcb.clientID);
	
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
		case 4: 	// GC_G: Dwell
		{
			return pathHandler->processDwell(gcb);
		}
		//....................................................
		case 20:	// GC_G: Set Units To Inches
		{
			typedef CncUnitCalculatorBase::Unit Unit;
			
			gcb.unit = Unit::in; 
			return true;
		} //....................................................
		case 21:	// GC_G_SetUnitsToMillimeters
		{	
			typedef CncUnitCalculatorBase::Unit Unit;
			
			gcb.unit = Unit::mm; 
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
		case 40: 	// GC_G: Cutter Compensation off
		{
			pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_OFF);
			return true;
		} //....................................................
		case 41: 	// GC_G: Cutter Compensation (Left) on
		{	
			if ( gcb.hasCmdSubNumber() == false ) {
				pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_STATIC_LEFT);
			} else  {
				if ( gcb.cmdSubNumber == 1L ) { // G41.1
					pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_DYNAMIC_LEFT);
				} else {
					return false;
				}
			}
			
			return true;
		} //....................................................
		case 42: 	// GC_G: Cutter Compensation (Right) on
		{	
			if ( gcb.hasCmdSubNumber() == false ) {
				pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_STATIC_RIGHT);
			} else  {
				if ( gcb.cmdSubNumber == 1L ) { // G42.1
					pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_DYNAMIC_RIGHT);
				} else {
					return false;
				}
			}
			
			return true;
		} //....................................................
		case 43: 	// GC_G: Tool length offset
		{
			if ( gcb.hasH() ) {
				int id = (int)gcb.h;
				if ( id > 0 ) {
					pathHandler->setToolLengthOffsetId(id);
					return true;
				}
			}
			
			return false;
		} //
		case 80: 	// GC_G: Cancle Canned Cycle
		{
			// does currntly nothing beause G81 to G89 are also not supported
			return true;
		} //
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
			pathHandler->switchToolState(true);
			return true;
		}
		case 4:		// GC_M_SpindleOnCounterClockwise
		{
			return displayUnhandledBlockCommand(gcb);
		}
		case 5:		// GC_M_SpindleOff
		{
			pathHandler->switchToolState(false);
			return true;
		}
		case 6:		// GC_M_ToolChange
		{
			// a tool change isn't yet supported
			// a correponding message will be display by setNextToolID(....)
			return true;
		}
		case 7:		// GC_M_MistCoolantOn
		{
			// mist coolant isn't yet supported
			// the command will skiped silent
			return true;
		}
		case 8:		// GC_M_FloodCoolantOn
		{
			// float coolant isn't yet supported
			// the command will skiped silent
			return true;
		}
		case 9:		// GC_M_CoolantOff
		{
			// coolant isn't yet supported
			// the command will skiped silent
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





