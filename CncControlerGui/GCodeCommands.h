#ifndef GCODE_COMMANDS_H
#define GCODE_COMMANDS_H

#include <map>
#include <wx/string.h>
#include "GCodeField.h"

////////////////////////////////////////////////////////////////
#define CMD_DESRIPTION_INVALID_TYPE 			'\0'
#define CMD_DESRIPTION_INVALID_GROUP 			-1

struct CmdDescription {
	// \0=unknown, C=Command, P=Paramaeter
	char 		type			= CMD_DESRIPTION_INVALID_TYPE;
	int			group			= CMD_DESRIPTION_INVALID_GROUP;
	bool 		modal			= false;
	wxString 	description		= wxT("");
	
	//Corresponding field
	GCodeField 	field			= GCodeField("");
};

////////////////////////////////////////////////////////////////
typedef std::map<GCodeField, CmdDescription, GCodeFieldComparer> CommandStore;
class GCodeCommands {
	private:
		static wxString returnValue;
		
		////////////////////////////////////////////////////////////////
		static const char* explainGCodeParameter(const GCodeField& field);
		
	public:
		////////////////////////////////////////////////////////////////
		static const char* getGCodeHelpHint(wxString& curLine);
		static const char* explainGCodeCommand(wxString& cmd);
		static const char* explainGCodeCommand(const GCodeField& field);
		
		////////////////////////////////////////////////////////////////
		static const bool isRegistered(const GCodeField& field);
		static const bool isRegistered(const char* cmd) { GCodeField field(cmd); return isRegistered(field); }
		static const bool isRegistered(char cmd, long num, long subNum) { GCodeField field(cmd, num, subNum); return isRegistered(field); };
		
		////////////////////////////////////////////////////////////////
		static const bool isBlockCommand(const GCodeField& field);
		static const bool isBlockCommand(const char* cmd) { GCodeField field(cmd[0]); return isBlockCommand(field); }
		static const bool isBlockCommand(char cmd) { GCodeField field(cmd); return isBlockCommand(field); };
		
		////////////////////////////////////////////////////////////////
		static const bool isParameter(const GCodeField& field);
		static const bool isParameter(const char* cmd) { GCodeField field(cmd[0]); return isParameter(field); }
		static const bool isParameter(char cmd) { GCodeField field(cmd); return isParameter(field); };
		
		////////////////////////////////////////////////////////////////
		static const CmdDescription* getCmdDescription(const GCodeField& field);
		static const CmdDescription* getCmdDescription(const char* cmd) { GCodeField field(cmd); return getCmdDescription(field); }
		static const CmdDescription* getCmdDescription(char cmd, long num, long subNum) { GCodeField field(cmd, num, subNum); return getCmdDescription(field); }
		
		////////////////////////////////////////////////////////////////
		static const bool isModal(const GCodeField& field);
		static const bool isModal(const char* cmd) { GCodeField field(cmd); return isModal(field); }
		static const bool isModal(char cmd, long num, long subNum) { GCodeField field(cmd, num, subNum); return isModal(field); };
		
		////////////////////////////////////////////////////////////////
		// Command store
		static CommandStore commandStore;
	
};

#endif