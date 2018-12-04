#ifndef GCODE_FIELD_H
#define GCODE_FIELD_H

#include <iostream>
#include <limits>
#include <cfloat>

#define INVALID_GCODE_COMMAND_CMD 	'\0'
#define INVALID_GCODE_COMMAND_NUM 	LONG_MAX
#define INVALID_GCODE_FIELD_VALUE	DBL_MAX
#define INVALID_GCODE_FIELD_NUM		LONG_MAX

/////////////////////////////////////////////////////////////
class GCodeField {
	private:
		char cmd			= INVALID_GCODE_COMMAND_CMD;
		long num			= INVALID_GCODE_FIELD_NUM;
		long subNum			= INVALID_GCODE_FIELD_NUM;
		double value		= INVALID_GCODE_FIELD_VALUE;
		
		wxString token;
		
		////////////////////////////////////////////////////////
		GCodeField& breakDownValue();
		
	public:

		////////////////////////////////////////////////////////
		GCodeField() {
			cmd			= INVALID_GCODE_COMMAND_CMD;
			num			= INVALID_GCODE_FIELD_NUM;
			subNum		= INVALID_GCODE_FIELD_NUM;
			value		= INVALID_GCODE_FIELD_VALUE;
		}
		
		////////////////////////////////////////////////////////
		GCodeField(const GCodeField& field) {
			setParmeters(field.getCmd(), field.getNum(), field.getSubNum());
		}

		////////////////////////////////////////////////////////
		GCodeField(char c, long n, long sn) {
			setParmeters(c, n, sn);
		}
		
		////////////////////////////////////////////////////////
		GCodeField(const char c) {
			wxString tt(c);
			setToken(tt);
		}
		////////////////////////////////////////////////////////
		GCodeField(const char* t) {
			if ( t != NULL ) {
				wxString tt(t);
				setToken(tt);
			} else {
				wxString tt("");
				setToken(tt);
			}
		}
		////////////////////////////////////////////////////////
		GCodeField(wxString& t) {
			setToken(t);
		}
		
		////////////////////////////////////////////////////////
		GCodeField& setParmeters(char c, long n, long sn) {
			cmd 	= c;
			num 	= n;
			subNum 	= sn;
			return * this;
		}
		
		////////////////////////////////////////////////////////
		GCodeField& setToken(const wxString& t) {
			token = t;
			breakDownValue();
			return * this;
		}
		
		////////////////////////////////////////////////////////
		const wxString& getToken() const {
			return token;
		}
		
		////////////////////////////////////////////////////////
		const char getCmd() const { return cmd; }
		const long getNum() const { return num; }
		const long getSubNum() const { return subNum; }
		
		////////////////////////////////////////////////////////
		const double getValue() const { return value; }
		
		////////////////////////////////////////////////////////
		const bool isValid() const { 
			return ( cmd != INVALID_GCODE_COMMAND_CMD && num != INVALID_GCODE_FIELD_NUM ); 
		}
		
		////////////////////////////////////////////////////////
		const bool hasNum() const { 
			return ( num != INVALID_GCODE_FIELD_NUM ); 
		}
		
		////////////////////////////////////////////////////////
		const bool hasSubNum() const { 
			return ( subNum != INVALID_GCODE_FIELD_NUM ); 
		}
		
		////////////////////////////////////////////////////////
		const bool hasValue() const { 
			return ( value != INVALID_GCODE_FIELD_VALUE ); 
		}
		////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const GCodeField &a) {
			if ( a.getCmd() == INVALID_GCODE_COMMAND_CMD ) {
				ostr << "<Empty field>";
				return ostr;
			}
			
			ostr << a.getCmd();
			
			if ( a.hasNum() )
				ostr << a.getNum();
				
			if ( a.hasSubNum() )
				ostr << "." << a.getSubNum();
				
			if ( a.hasValue() ) 
				ostr << ", Value=" << a.getValue();
				
			return ostr;
		}
};

/////////////////////////////////////////////////////////////
struct GCodeFieldComparer {
	bool operator()(const GCodeField& first, const GCodeField& second) const {
		
		if ( first.getCmd() < second.getCmd() ) 	{ return true; }
		if ( first.getCmd() > second.getCmd() ) 	{ return false; }
		
		if ( first.getNum() < second.getNum()) 		{ return true; }
		if ( first.getNum() > second.getNum()) 		{ return false; }
		
		return first.getSubNum() < second.getSubNum();
	}
};

#endif