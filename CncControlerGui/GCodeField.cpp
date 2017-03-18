#include "GCodeCommands.h"
#include "GCodeField.h"

/////////////////////////////////////////////////////
GCodeField& GCodeField::breakDownValue() {
/////////////////////////////////////////////////////
	if ( token.length() > 0 ) {
		cmd = token[0];
		
		if ( token.length() > 1 ) {
			if ( GCodeCommands::isBlockCommand(cmd) == true ) {
				int pos = wxNOT_FOUND;
				if ( (pos = token.find('.')) == wxNOT_FOUND ) {
					if ( token.Mid(1).ToLong(&num) == false)
						num = INVALID_GCODE_FIELD_NUM;
				} else {
					token.SubString(1, pos - 1).ToLong(&num);
					token.Mid(pos + 1).ToLong(&subNum);
				}
			} else {
				token.Mid(1).ToDouble(&value);
			}
		}
	}
	
	return *this;
}