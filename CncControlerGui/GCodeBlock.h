#ifndef GCODE_BLOCK_H
#define GCODE_BLOCK_H

#include "CncUnitCalculator.h"
#include "DataControlModel.h"
#include "CncUnitCalculator.h"
#include "CncPosition.h"
#include "GCodeField.h"
#include "GCodeCommands.h"

/////////////////////////////////////////////////////////////
class GCodeBlock {
	
	typedef CncUnitCalculatorBase::Unit Unit;
	
	private:
		/////////////////////////////////////////////////////
		inline double getMoveRelative(double thisValue, double currentAxisValue) {
			if ( thisValue == INVALID_GCODE_FIELD_VALUE )
				return 0.0d;
			
			return ( isAbsolutePositioningXYZ() ? thisValue - currentAxisValue : thisValue);
		}
		
		/////////////////////////////////////////////////////
		inline double getMoveAbsolute(double thisValue, double currentAxisValue) {
			if ( thisValue == INVALID_GCODE_FIELD_VALUE )
				return currentAxisValue;
				
			return ( isAbsolutePositioningXYZ() ? thisValue : currentAxisValue + thisValue );
		}
		
	public:
		/////////////////////////////////////////////////////////////
		enum GCodePositioning	{GC_Relative, GC_Absolute};
		enum GCodePlaneSelect	{GC_XY_PLANE, GC_ZX_PLANE, GC_YZ_PLANE};
		
		/////////////////////////////////////////////////////////////
		long				clientID		= -1;
		
		char 				cmdCode			= INVALID_GCODE_COMMAND_CMD;
		long		 		cmdNumber		= INVALID_GCODE_COMMAND_NUM;
		long				cmdSubNumber	= INVALID_GCODE_COMMAND_NUM;
		
		char 				prevCmdCode		= INVALID_GCODE_COMMAND_CMD;
		long		 		prevCmdNumber	= INVALID_GCODE_COMMAND_NUM;
		long				prevCmdSubNumber= INVALID_GCODE_COMMAND_NUM;
		
		bool				openComment		= false;
		
		wxString			nodeName;
		wxString			block;
		
		Unit				unit			= Unit::mm;
		GCodePositioning	posModeXYZ		= GC_Absolute;
		GCodePositioning	posModeIJ		= GC_Absolute;
		
		GCodePlaneSelect	plane 			= GC_XY_PLANE;
		
		double				x 				= INVALID_GCODE_FIELD_VALUE;
		double				y 				= INVALID_GCODE_FIELD_VALUE;
		double 				z 				= INVALID_GCODE_FIELD_VALUE;
		
		double 				i				= INVALID_GCODE_FIELD_VALUE;
		double 				j				= INVALID_GCODE_FIELD_VALUE;
		
		double				s				= INVALID_GCODE_FIELD_VALUE;
		double				e				= INVALID_GCODE_FIELD_VALUE;
		double 				f				= INVALID_GCODE_FIELD_VALUE;
		
		double 				p				= INVALID_GCODE_FIELD_VALUE;
		
		double 				h				= INVALID_GCODE_FIELD_VALUE;

		/////////////////////////////////////////////////////
		void reInit();
		
		/////////////////////////////////////////////////////
		static int removeComments(wxString& block, bool& openComment);
		
		/////////////////////////////////////////////////////
		void copyPrevCmdToCmd() {
			cmdCode			= prevCmdCode;
			cmdNumber		= prevCmdNumber;
			cmdSubNumber	= prevCmdSubNumber;
		}
		/////////////////////////////////////////////////////
		void copyCmdToPrevCmd() {
			prevCmdCode 		= cmdCode;
			prevCmdNumber 		= cmdNumber;
			prevCmdSubNumber 	= cmdSubNumber;
		}
		
		/////////////////////////////////////////////////////
		const bool isValid() const	{ return (cmdCode != INVALID_GCODE_COMMAND_CMD && cmdNumber != INVALID_GCODE_COMMAND_NUM);}
		
		const bool hasAll_XYZ() const 	{ return (hasX() && hasY() && hasZ()); }
		const bool hasAll_XY()  const 	{ return (hasX() && hasY()); }
		
		const bool hasOneOf_XYZ() const { return (hasX() || hasY() || hasZ()); }
		const bool hasOneOf_XY()  const { return (hasX() || hasY()); }
		
		const bool hasOneOf_IJ()  const { return (hasI() || hasJ()); }
		const bool hasOneOf_SEF() const { return (hasS() || hasE() || hasF()); }
		
		const bool hasX() const		{ return x != INVALID_GCODE_FIELD_VALUE; }
		const bool hasY() const 	{ return y != INVALID_GCODE_FIELD_VALUE; }
		const bool hasZ() const 	{ return z != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasI() const		{ return i != INVALID_GCODE_FIELD_VALUE; }
		const bool hasJ() const		{ return j != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasS() const		{ return s != INVALID_GCODE_FIELD_VALUE; }
		const bool hasE() const		{ return e != INVALID_GCODE_FIELD_VALUE; }
		const bool hasF() const		{ return f != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasP() const		{ return p != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasH() const		{ return h != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasMoveCmd() const { 
			return (hasOneOf_XYZ() || hasI() || hasJ()); 
		}
		
		const bool hasCmdSubNumber() const { 
			return cmdSubNumber != INVALID_GCODE_COMMAND_NUM; 
		}
		
		/////////////////////////////////////////////////////
		const bool isAbsolutePositioningXYZ() { return posModeXYZ == GC_Absolute; }
		const bool isAbsolutePositioningIJ() { return posModeIJ == GC_Absolute; }
		
		/////////////////////////////////////////////////////
		const double getXMoveRelative(const CncDoublePosition& curPos) { return getMoveRelative(x, curPos.getX()); }
		const double getYMoveRelative(const CncDoublePosition& curPos) { return getMoveRelative(y, curPos.getY()); }
		const double getZMoveRelative(const CncDoublePosition& curPos) { return getMoveRelative(z, curPos.getZ()); }
		
		const double getXMoveAbsolute(const CncDoublePosition& curPos) { return getMoveAbsolute(x, curPos.getX()); }
		const double getYMoveAbsolute(const CncDoublePosition& curPos) { return getMoveAbsolute(y, curPos.getY()); }
		const double getZMoveAbsolute(const CncDoublePosition& curPos) { return getMoveAbsolute(z, curPos.getZ()); }
		
		const double ensureUnit(double value) {
			if ( unit == Unit::in )
				return value * CncUnitCalculatorBase::getFact_in2mm();
			
			wxASSERT( unit == Unit::mm );
			return value;
		}
		
		//////////////////////////////////////////////////////////////////
		const double getCurrentFeedSpeed() {
			if ( hasF() )
				return f;
				
			return 0.0;
		}
		
		//////////////////////////////////////////////////////////////////
		const double getCurrentSpindleSpeed() {
			if ( hasS() )
				return s;
				
			return 0.0;
		}
		
		//////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const GCodeBlock &a);
		
		const wxString getCmdAsString(wxString& ret) const;
		
		const std::ostream& trace(std::ostream &ostr) const;
		const DcmItemList& trace(DcmItemList& rows) const;
		
		const std::ostream& traceMore(std::ostream& ostr) const;
		const wxString& traceMore(wxString& ret) const;
		
		//////////////////////////////////////////////////////////////////
		const char* getUnitAsString() const { return CncUnitCalculatorBase::getUnitAsStr(unit); }
		const char* getPositioningAsString(GCodePositioning p) const {
			switch (p) {
				case GC_Relative: 	return "relative";
				case GC_Absolute:	return "absolute";
			}
			return "???";
		}
		
		const char* getPlaneAsString() const {
			switch (plane) {
				case GC_XY_PLANE: 	return "XY";
				case GC_ZX_PLANE:	return "ZX";
				case GC_YZ_PLANE:	return "YZ";
			}
			return "???";
		}
};

#endif

