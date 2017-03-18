#include <iostream>
#include "GCodeBlock.h"

//////////////////////////////////////////////////////////////////
int GCodeBlock::removeComments(wxString& block, bool& openComment) {
//////////////////////////////////////////////////////////////////
	int s = wxNOT_FOUND;
	if ( openComment == true ) {
		if ( ( s = block.find(')')) != wxNOT_FOUND ) {
			block = block.Remove(0, s);
			openComment = false;
			return block.length();
		} else {
			block.clear();
			return 0;
		}
	}
	
	if ( (int)block.find('%') != wxNOT_FOUND )
		block.clear();
		
	s = wxNOT_FOUND;
	if ( ( s = block.find(';')) != wxNOT_FOUND )
		block = block.SubString(0, s-1);

	s = wxNOT_FOUND;
	while ( (s = block.find('(')) != wxNOT_FOUND ) {
		
		int e = block.find(')');
		if ( e != wxNOT_FOUND ) {
			block = block.Remove(s, e - s + 1);
		} else {
			openComment = true;
			block = block.substr(0, s);
		}
	}
	return block.length();
}

//////////////////////////////////////////////////////////////////
void GCodeBlock::reInit() {
//////////////////////////////////////////////////////////////////
	// block header parameters
	if ( GCodeCommands::isModal(cmdCode, cmdNumber, cmdSubNumber) == true ) {
		copyCmdToPrevCmd();
	}
	
	cmdCode				= INVALID_GCODE_COMMAND_CMD;
	cmdNumber			= INVALID_GCODE_COMMAND_NUM;
	cmdSubNumber		= INVALID_GCODE_COMMAND_NUM;
	
	nodeName.clear();
	block.clear();
	
	// block body parameters
	x 				= INVALID_GCODE_FIELD_VALUE;
	y 				= INVALID_GCODE_FIELD_VALUE;
	z 				= INVALID_GCODE_FIELD_VALUE;
	i 				= INVALID_GCODE_FIELD_VALUE;
	j 				= INVALID_GCODE_FIELD_VALUE;
	//....
	
	// keep e, f, etc.
}