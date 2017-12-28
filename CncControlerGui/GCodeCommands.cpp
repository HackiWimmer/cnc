#include <wx/tokenzr.h>
#include "GCodeBlock.h"
#include "GCodeCommands.h"

//////////////////////////////////////////////////////////////////
CommandStore GCodeCommands::commandStore;
wxString GCodeCommands::returnValue;

//////////////////////////////////////////////////////////////////
class CommandStoreInitializer {
	
	private:
	
		int ig = CMD_DESRIPTION_INVALID_GROUP;
		
		//////////////////////////////////////////////////////////////////
		inline void registerCommand(GCodeField field, const char type, const int group, const bool modal, const char* desc) {
			CmdDescription cd;
			cd.type 		= type;
			cd.group		= group;
			cd.modal		= modal;
			cd.description 	= desc;
			cd.field 		= field;
			
			GCodeCommands::commandStore[field] = cd;
		}
		
		//////////////////////////////////////////////////////////////////
		inline void registerMCommands() {
			// GCode M Commands
			registerCommand(GCodeField("M00"), 		'C', ig,	false, "Stop Or Unconditional Stop");
			registerCommand(GCodeField("M01"), 		'C', ig,	false, "Sleep Or Conditional Stop");
			registerCommand(GCodeField("M02"), 		'C', ig,	false, "Program End");
			registerCommand(GCodeField("M03"), 		'C', ig,	false, "Spindle On Clockwise");
			registerCommand(GCodeField("M04"), 		'C', ig,	false, "Spindle On Counter Clockwise");
			registerCommand(GCodeField("M05"), 		'C', ig,	false, "Spindle Off");
			registerCommand(GCodeField("M06"), 		'C', ig,	false, "Tool Change");
			registerCommand(GCodeField("M07"), 		'C', ig,	false, "Mist Coolant On");
			registerCommand(GCodeField("M08"), 		'C', ig,	false, "Flood Coolant On");
			registerCommand(GCodeField("M09"), 		'C', ig,	false, "Coolant Off");
			
			registerCommand(GCodeField("M30"), 		'C', ig,	false, "Program End And Rewind");
		}
		
		//////////////////////////////////////////////////////////////////
		inline void registerGCommands() {
			// GCode G Commands
			registerCommand(GCodeField("G00"), 		'C', 1,		true,  "Rapid Linear Move");
			registerCommand(GCodeField("G01"), 		'C', 1,		true,  "Linear Move");
			registerCommand(GCodeField("G02"), 		'C', 1,		true,  "Controlled Arc Move Clockwise");
			registerCommand(GCodeField("G03"), 		'C', 1,		true,  "Controlled Arc Move Counter Clockwise");
			registerCommand(GCodeField("G04"), 		'C', 0,		false, "Dwell");
			
			registerCommand(GCodeField("G17"), 		'C', 2,		false, "XY Plane Select");
			registerCommand(GCodeField("G18"), 		'C', 2,		false, "ZX Plane Select");
			registerCommand(GCodeField("G19"), 		'C', 2,		false, "YZ Plane Select");
			
			registerCommand(GCodeField("G20"), 		'C', 6,		false, "Set Units To Inches");
			registerCommand(GCodeField("G21"), 		'C', 6,		false, "Set Units To Millimeters");
			registerCommand(GCodeField("G28"), 		'C', 0,		false, "Move To Origin");
			
			registerCommand(GCodeField("G30"), 		'C', 0,		false, "Zero Return");
			
			registerCommand(GCodeField("G40"), 		'C', 0,		false, "Cutter Compensation off");
			registerCommand(GCodeField("G41"), 		'C', 0,		false, "Cutter Compensation on - static left");
			registerCommand(GCodeField("G41.1"), 	'C', 0,		false, "Cutter Compensation on - dynamic left");
			registerCommand(GCodeField("G42"), 		'C', 0,		false, "Cutter Compensation on - static right");
			registerCommand(GCodeField("G42.1"), 	'C', 0,		false, "Cutter Compensation on - dynamic right");
			
			registerCommand(GCodeField("G43"),	 	'C', 0,		false, "Tool Length offset");
			
			registerCommand(GCodeField("G80"),	 	'C', 0,		false, "Cancel Canned Cycle");
			
			registerCommand(GCodeField("G90"), 		'C', 3,		false, "Absolute Positioning");
			registerCommand(GCodeField("G91"), 		'C', 3,		false, "Relative Positioning");
			registerCommand(GCodeField("G90.1"), 	'C', 4,		false, "Arc Center Absolute Mode");
			registerCommand(GCodeField("G91.1"), 	'C', 4,		false, "Arc Center Releativ Mode");
			registerCommand(GCodeField("G92"), 		'C', 0,		false, "Set Position");
		}
		
		//////////////////////////////////////////////////////////////////
		inline void registerParameters() {
			// GCode Parameters
			registerCommand(GCodeField("X"), 		'P', ig,	false, "Coordinate");
			registerCommand(GCodeField("Y"), 		'P', ig,	false, "Coordinate");
			registerCommand(GCodeField("Z"), 		'P', ig,	false, "Coordinate");
			registerCommand(GCodeField("I"), 		'P', ig,	false, "Coordinate");
			registerCommand(GCodeField("J"), 		'P', ig,	false, "Coordinate");
			
			registerCommand(GCodeField("N"), 		'P', ig,	false, "Line number");
			registerCommand(GCodeField("S"), 		'P', ig,	false, "Spindle speed setup");
			registerCommand(GCodeField("F"), 		'P', ig,	false, "Feed rate");
			registerCommand(GCodeField("T"), 		'P', ig,	false, "Tool ID");
			registerCommand(GCodeField("E"), 		'P', ig,	false, "Extruder value");
			registerCommand(GCodeField("T"), 		'P', ig,	false, "Tool ID");
		}
	
	public:
		//////////////////////////////////////////////////////////////////
		CommandStoreInitializer() {
			
			registerMCommands();
			registerGCommands();
			registerParameters();
		}
		
}; CommandStoreInitializer CSI;

//////////////////////////////////////////////////////////////////
const char* GCodeCommands::getGCodeHelpHint(wxString& curLine) {
//////////////////////////////////////////////////////////////////
	curLine.Replace("\r","\0");
	curLine.Replace("\n","\0");
	
	if ( curLine.IsEmpty() )
		return "";
	
	bool openComment = false;
	if ( GCodeBlock::removeComments(curLine, openComment) < 1 )
		return "";
		
	wxString cmd;
	for ( unsigned int i=0; i< curLine.length(); i++) {
		
		if ( isalpha(curLine[i]) ) {
			cmd = curLine.Mid(i);
			break;
		}
	}
	
	wxStringTokenizer tokenizerBlank(cmd, " ");
	if ( tokenizerBlank.HasMoreTokens() )	cmd = tokenizerBlank.GetNextToken();
	else									cmd = "";
	
	return explainGCodeCommand(cmd);
}
//////////////////////////////////////////////////////////////////
const char* GCodeCommands::explainGCodeCommand(wxString& cmd) {
//////////////////////////////////////////////////////////////////
	GCodeField field(cmd);
	return explainGCodeCommand(field);
}
//////////////////////////////////////////////////////////////////
const char* GCodeCommands::explainGCodeParameter(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	if ( GCodeCommands::isParameter(field.getCmd()) == false )
		return "";
		
	if ( field.hasValue() == false )
		return "";

	returnValue  = "GCode Parameter: ";
	returnValue += field.getCmd();
	
	GCodeField f(field.getCmd());
	const CmdDescription* cd = getCmdDescription(f);
	if ( cd != NULL ) {
		returnValue += wxString::Format(" > %s", cd->description);
		return returnValue.c_str();
	}
	
	returnValue += "Unknown!";
	return returnValue.c_str();
}
//////////////////////////////////////////////////////////////////
const char* GCodeCommands::explainGCodeCommand(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	if ( GCodeCommands::isBlockCommand(field.getCmd()) == false )
		return explainGCodeParameter(field);
	
	if ( field.isValid() == false )
		return "";
	
	returnValue  = "GCode: ";
	returnValue += field.getCmd();
	
	if ( field.getNum() != INVALID_GCODE_FIELD_NUM ) {
		if ( field.hasSubNum() ) 	returnValue += wxString::Format("%i.%i > ",field.getNum(), field.getSubNum());
		else 						returnValue += wxString::Format("%i > ",field.getNum());
	} else {
		returnValue += "? > ";
	}
	
	const CmdDescription* cd = getCmdDescription(field);
	if ( cd != NULL ) {
		returnValue += cd->description;
		return returnValue.c_str();
	}

	returnValue += "Unknown!";
	return returnValue.c_str();
}
//////////////////////////////////////////////////////////////////
const bool GCodeCommands::isRegistered(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	CommandStore::iterator it = GCodeCommands::commandStore.find(field);
	if ( it != GCodeCommands::commandStore.end() ) {
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////
const bool GCodeCommands::isBlockCommand(const GCodeField& field) {
	switch ( field.getCmd() ) {
		case 'G':
		case 'M': return true;
		default:;
	}
	return false;
}
//////////////////////////////////////////////////////////////////
const bool GCodeCommands::isParameter(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	GCodeField f(field.getCmd());
	CommandStore::iterator it = GCodeCommands::commandStore.find(f);
	if ( it != GCodeCommands::commandStore.end() ) {
		return (*it).second.type == 'P';
	}
	return false;
}
//////////////////////////////////////////////////////////////////
const CmdDescription* GCodeCommands::getCmdDescription(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	CommandStore::iterator it = GCodeCommands::commandStore.find(field);
	if ( it != GCodeCommands::commandStore.end() ) {
		return &(*it).second;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////
const bool GCodeCommands::isModal(const GCodeField& field) {
//////////////////////////////////////////////////////////////////
	CommandStore::iterator it = GCodeCommands::commandStore.find(field);
	if ( it != GCodeCommands::commandStore.end() ) {
		return ((*it).second).modal;
	}
	return false;
}
