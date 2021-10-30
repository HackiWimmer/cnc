#ifndef CNC_MOVE_DEFINITION
#define CNC_MOVE_DEFINITION

#include "CncCommon.h"

struct CncMoveDefinition {
	
	enum MoveMode { MM_1D, MM_2D, MM_3D };	
	struct Axis {
		bool	absolute	= false;
		double	value		= 0.0;
	};
	
	Axis x;
	Axis y;
	Axis z;
	
	CncSpeedMode speedMode	= CncSpeedMode::CncSpeedRapid;
	double speedValue		= 0.0;
	
	MoveMode moveMode		= MM_2D;
	
	bool toolState 			= false;
	bool correctLimit   	= true;
	
	// ------------------------------------------------------------------
	static MoveMode convert(const wxString& s, MoveMode defValue = MM_2D) {
		if      ( s.IsSameAs("1D") )	return MM_1D;
		else if ( s.IsSameAs("2D") )	return MM_2D;
		else if ( s.IsSameAs("3D") )	return MM_3D;
		else							return defValue;
	}
	
	// ------------------------------------------------------------------
	static const char* convert(MoveMode mm) {
		switch ( mm ) {
			case MM_1D: return "1D";
			case MM_2D: return "2D";
			case MM_3D: return "3D";
		}
		return "??";
	}
	
	// ------------------------------------------------------------------
	friend std::ostream &operator<< (std::ostream &ostr, const CncMoveDefinition& a) {
		ostr	<< cnc::getCncSpeedTypeAsString(a.speedMode) << " " << convert(a.moveMode)			<< std::endl
				<< (a.x.absolute ? 'X' : 'x')	<< ": " << wxString::Format("%+.3lf", a.x.value)	<< std::endl
				<< (a.y.absolute ? 'Y' : 'y')	<< ": " << wxString::Format("%+.3lf", a.y.value)	<< std::endl
				<< (a.z.absolute ? 'Z' : 'z')	<< ": " << wxString::Format("%+.3lf", a.z.value)	<< std::endl
				<< 'F' 							<< ": " << wxString::Format("%+.1lf", a.speedValue)	<< std::endl
		;
		
		return ostr;
	}
};

#endif