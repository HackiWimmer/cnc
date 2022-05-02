#include <iostream>
#include <sstream>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include "CncSourceEditor.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "MainFrame.h"
#include "CncAutoFreezer.h"
#include "CncAutoProgressDialog.h"
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
	const GCodeField field(gcb.cmdCode, gcb.cmdNumber, gcb.cmdSubNumber);
	
	if ( gcb.cmdNumber >= 20 && gcb.cmdNumber <= 30)
		return true; // skip all SD card commands

	if ( GCodeCommands::canBeIgnored(field) )
		return true; // skip such commands

	std::stringstream ss;
	ss	<< "Not handled GCode command: " 
		<< field
	;
	
	if ( additionalInfo != NULL )
		ss << " [" << additionalInfo << "]";
		
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnsupportedBlockCommand(const GCodeField& field, const char* additionalInfo) {
//////////////////////////////////////////////////////////////////
	if ( GCodeCommands::canBeIgnored(field) )
		return true; // skip such commands
		
	std::stringstream ss;
	ss	<< "Not supported GCode block command: " 
		<< field
	;
	
	if ( additionalInfo != NULL )
		ss << " [" << additionalInfo << "]";
		
	return displayMessage(ss, resumeOnError ? wxICON_WARNING : wxICON_ERROR);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::displayUnhandledParameter(const GCodeField& field, const char* additionalInfo) {
//////////////////////////////////////////////////////////////////
	if ( GCodeCommands::canBeIgnored(field) )
		return true; // skip such commands

	std::stringstream ss;
	ss	<< "Not supported GCode parameter: "
		<< field.getCmd()
	;
	
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
bool GCodeFileParser::spool() {
//////////////////////////////////////////////////////////////////
	// -----------------------------------------------------------
	auto triggerEnd = [&](bool success)
	{
		const Trigger::EndRun endRun(success);
		deligateTrigger(endRun);
		return success;
	};

	if ( pathHandler->isPathListUsed() == false )
		return triggerEnd(true);
	
	// prepare . . . 
	CncGCodeSequenceListCtrl* ctrl = THE_APP->getGCodeSequenceList();
	CncAutoFreezer caf(ctrl);
	
	programEnd  = false;
	bool failed = false;
	
	CncAutoProgressDialog progressDlg(THE_APP->getMotionMonitor(), "Preprocessing GCode");
	progressDlg.Show();
	
	// over all collected gcode commands
	CNC_CEX2_A("Run parsed GCode commands (entries=%zu)", gCodeSequence.size())
	FORCE_LOGGER_UPDATE
	
	const long modVal = (long)( gCodeSequence.size() / 1000 );
	for ( auto it = gCodeSequence.begin(); it != gCodeSequence.end(); ++it)
	{
		const long distance = std::distance(gCodeSequence.begin(), it);
		if ( modVal > 0 && distance % modVal == 0 )
		{
			// from time to time update the progress
			// to keep a good usage feeling
			const double val = gCodeSequence.size() ? ((double)distance / gCodeSequence.size()) * 100 : 0.0;
			UPDATE_PROGRESS_DLG(wxString::Format("%.1lf %%", val));
		}
		
		GCodeBlock& gcb = (*it);
		
		// reconstruct the line number to be right also during the spooling
		// e. g. in case of error messages etc.
		setCurrentLineNumber(gcb.clientID / CLIENT_ID.TPL_FACTOR);

		if ( performBlock(gcb) == false )
		{
			std::cerr << CNC_LOG_FUNCT_A(" performBlock() failed at line %ld !\n", ClientIds::lineNumber(gcb.clientID));
			gcb.trace(std::cerr);
			gcb.traceMore(std::cerr);
			
			failed = true;
			break;
		}
		
		// error handling
		if ( programEnd == true )
		{
			const long remaining = std::distance(it, gCodeSequence.end());
			
			if ( remaining > 1 )
			{
				std::cout	<< "Info: Program end detected at line " 
							<< ClientIds::lineNumber(gcb.clientID)
							<< ", but still " 
							<< remaining 
							<< " GCode blocks remaining \n"
				;
			}
			
			break;
		}
		
		if ( THE_APP->isDisplayParserDetails() == true )
			ctrl->addBlock(gcb);
	}
	
	progressDlg.Hide();
	FORCE_LOGGER_UPDATE
	
	// finalize . . .
	if ( failed == true )
		return triggerEnd(false);
		
	triggerEnd(true);
	return pathHandler->spoolWorkflow();
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::preprocess() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler->prepareWork() == false ) 
	{
		std::cerr << CNC_LOG_FUNCT_A(": pathHandler->prepareWork() failed!\n");
		return false;
	}
	
	setDefaultParameters();
	setCurrentLineNumber(0);
	GCodeBlock gcb;
	
	CncGCodeSequenceListCtrl* ctrl = THE_APP->getGCodeSequenceList();
	ctrl->clearAll();
	
	std::string line;
	std::ifstream file (fileName.c_str().AsChar());
	
	// get length of file
	struct stat stat_buf;
	int rc = stat(fileName.c_str(), &stat_buf);
	long totalLength = rc == 0 ? stat_buf.st_size : -1;
	
	// read the file content
	if ( file.is_open() )
	{
		UPDATE_PROGRESS_DLG("");
		
		const long modVal = (long)( totalLength / 1000 );
		
		// over all file lines
		while ( getline (file, line) )
		{
			incCurrentLineNumber();
			
			if ( modVal > 0 && getCurrentLineNumber() % modVal == 0 )
			{
				// from time to time update the progress
				// to keep a good usage feeling
				const double val = ((double)file.tellg() / totalLength) * 100;
				UPDATE_PROGRESS_DLG(wxString::Format("%.1lf %%", val));
			}
			
			// for all lines with content
			if ( line.length() > 0 )
			{
				// strip it to the essential information
				wxString gcodeLine(line);
				gcodeLine.Trim(false).Trim(true);
				
				// process the gcode line
				if ( processBlock(gcodeLine, gcb) == false )
				{
					std::cerr << CNC_LOG_FUNCT_A(wxString::Format(": Failed! Line number: %ld\n", getCurrentLineNumber()));
					return false;
				}
				
				if ( evaluateProcessingState() == false )
					return false;

				if ( evaluateDebugState() == false )
					return false;
			}
			
			if ( programEnd == true )
				break;
		}
		
		file.close();
	}
	
	FORCE_LOGGER_UPDATE
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
	
	bool blockCmdAvialable = false;
	
	wxStringTokenizer tokenizerBlank(block, " \t");
	while ( tokenizerBlank.HasMoreTokens() )
	{
		const wxString token = tokenizerBlank.GetNextToken();
		
		if ( token.IsEmpty() )
			continue;
			
		if ( isalpha(token[0]) == false )
			continue;
		
		// the valid gcode token processing starts form here on
		GCodeField nextField(token);
		
		if ( GCodeCommands::isBlockCommand(nextField.getCmd()) )
		{
			if ( blockCmdAvialable == true ) 
			{
				// in this case contains the current gcode file line
				// a second block command and the collected fields 
				// (cmd + parameters) have to be performed 
				if ( prepareBlock(gcb) == false )
					return false;
					
				gcb.reInit();
				
				// now the next block command can be collected . . .
			}
			
			if ( GCodeCommands::isRegistered(nextField) == true )
			{
				// (re)setup the gcb block command structure
				gcb.cmdCode 		= nextField.getCmd();
				gcb.cmdNumber 		= nextField.getNum();
				gcb.cmdSubNumber	= nextField.getSubNum();
				gcb.nodeName		= nextField.getToken();
				
				// if something goes wrong reset the gcb structure
				blockCmdAvialable = gcb.isValid();
				if ( blockCmdAvialable == false )
				{
					displayUnsupportedBlockCommand(nextField);
					gcb.reInit();
				}
				else
				{
					if ( GCodeCommands::isStandaloneCommand(nextField) )
					{
						// in this case all further tokens are content of such commands
						// e. g. M117 <message token A message token B .....>
						// and content are already part of gcb.block
						
						// break and not return here to call prepareBlock(gcb);
						// after the while loop
						break;
					}
				}
			}
			else
			{
				displayUnsupportedBlockCommand(nextField);
				continue;
			}
		}
		else
		{
			// if the gcode token isn't a block command
			// process it as parameter
			processField(nextField, gcb);
			
			// processField() == false is already handled and 
			// has no further impact in this routine
		}
		
		if ( evaluateProcessingState() == false )
			return false;
			
		if ( programEnd == true )
			return true;
	}
	
	// processes the collected g cpode tokens
	// (cmd + parameters)
	return prepareBlock(gcb);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::processField(const GCodeField& field, GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	switch ( field.getCmd() ) 
	{
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
					
		case 'T':	gcb.t			= field.getValue();
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
	// first initialize the client id context
	gcb.clientID = getCurrentLineNumber() * CLIENT_ID.TPL_FACTOR;
	
	// work with intermediate step on demand
	if ( pathHandler->isPathListUsed() )
	{
		gCodeSequence.push_back(gcb);
		
		// in this case the processing stops here.
		// performBlock() will be called later while
		// spooling the gCodeSequence list
		return true;
	}
	
	// this is only done without a path list - see above
	// Normally for previews etc.
	if ( THE_APP->isDisplayParserDetails() == true ) 
		THE_APP->getGCodeSequenceList()->addBlock(gcb);
	
	return performBlock(gcb);
}
//////////////////////////////////////////////////////////////////
bool GCodeFileParser::performBlock(GCodeBlock& gcb) {
//////////////////////////////////////////////////////////////////
	// check the new path trigger and initialize it before 
	// calling initNextClientId()
	if ( gcb.cmdCode == 'G' && gcb.cmdNumber == 0 )
		pathHandler->initNextPath();
	
	initNextClientId(gcb.clientID);
	
	// first detect tool changes
	if ( THE_CONFIG->getCurrentToolId() != int(gcb.t) )
		setNextToolID(gcb.t);
	
	// then setup these parameters
	bool ret = false;
	if ( ( ret = pathHandler->processParameterEFS(gcb) ) == true )
	{
		// then process the block command if any exits
		if ( gcb.isValid() == true )
		{
			switch ( gcb.cmdCode )
			{
				case 'G':	ret = processG(gcb);	break;
				case 'M':	ret = processM(gcb);	break;
				//....
				default: 	return displayUnhandledBlockCommand(gcb);
			}
		}
		// else do nothing more
	}
	
	// the perform debug information on demand
	if ( THE_CONTEXT->processingInfo->getCurrentDebugState() == true )
	{
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
	
	switch ( gcb.cmdNumber ) 
	{
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
			if ( gcb.hasCmdSubNumber() == false ) 
			{
				pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_STATIC_RIGHT);
			}
			else
			{
				if ( gcb.cmdSubNumber == 1L ) 
				{ // G42.1
					pathHandler->setCutterCompensationMode(GCodePathHandlerBase::CC_DYNAMIC_RIGHT);
				}
				else
				{
					return false;
				}
			}
			
			return true;
		} //....................................................
		case 43: 	// GC_G: Tool length offset
		{
			if ( gcb.hasH() ) 
			{
				int id = (int)gcb.h;
				if ( id > 0 ) 
				{
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
			if ( gcb.cmdSubNumber == 1L ) 
			{ // G90.1
				gcb.posModeIJ = GCodeBlock::GC_Absolute;
			} 
			else
			{
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
	switch ( gcb.cmdNumber ) 
	{
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		case  0:		// GC_M_StopOrUnconditionalStop
		case  1:		// GC_M_SleepOrConditionalStop
		case  2:		// GC_M_ProgramEnd
		{
			programEnd = true; 
			return true;
		} //....................................................
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		case 3: 	// GC_M_SpindleOnClockwise
		{
			pathHandler->switchSpindleState(true);
			return true;
		}
		case 4:		// GC_M_SpindleOnCounterClockwise
		{
			return displayUnhandledBlockCommand(gcb);
		}
		case 5:		// GC_M_SpindleOff
		{
			pathHandler->switchSpindleState(false);
			return true;
		}
		case 6:		// GC_M_ToolChange
		{
			// a tool change isn't yet supported
			// a corresponding message will be display by setNextToolID(....)
			return true;
		}
		case 7:		// GC_M_MistCoolantOn
		{
			// mist coolant isn't yet supported
			// the command will skipped silent
			return true;
		}
		case 8:		// GC_M_FloodCoolantOn
		{
			// float coolant isn't yet supported
			// the command will skipped silent
			return true;
		}
		case 9:		// GC_M_CoolantOff
		{
			// coolant isn't yet supported
			// the command will skipped silent
			return true;
		} //....................................................
		
		case 117:	// Marlin: Set LCD Message
		{
			const CncProcessingInfo::RunPhase curRunPhase = THE_CONTEXT->processingInfo->getCurrentRunPhase();
			if ( curRunPhase == CncProcessingInfo::RunPhase::RP_Spool )
			{
				std::cout	<< "GCode Marlin LCD Message: "
							<< gcb.block
							<< std::endl;
				;
			}
			
			return true;
		}
		
		//::::::::::::::::::::::::::::::::::::::::::::::::::::::
		default: 
		{
			return displayUnhandledBlockCommand(gcb);
		}
	}

	return false;
}





