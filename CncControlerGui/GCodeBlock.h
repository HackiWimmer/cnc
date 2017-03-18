#ifndef GCODE_BLOCK_H
#define GCODE_BLOCK_H

#include "SvgUnitCalculator.h"
#include "CncPosition.h"
#include "GCodeField.h"
#include "GCodeCommands.h"

/////////////////////////////////////////////////////////////
class GCodeBlock {
		
	private:
		/////////////////////////////////////////////////////
		double getMoveRelative(double thisValue, double currentAxisValue) {
			if ( thisValue == INVALID_GCODE_FIELD_VALUE )
				return 0.0d;
				
			double ret = 0.0d;
				
			if ( isAbsolutePositioningXYZ() ) {
				ret =  thisValue - currentAxisValue;
			} else {
				ret = thisValue;
			}
			
			return ret;
		}
		
	public:
		/////////////////////////////////////////////////////////////
		enum GCodePositioning	{GC_Relative, GC_Absolute};
		enum GCodePlaneSelect	{GC_XY_PLANE, GC_ZX_PLANE, GC_YZ_PLANE};
		
		/////////////////////////////////////////////////////////////
		char 				cmdCode			= INVALID_GCODE_COMMAND_CMD;
		long		 		cmdNumber		= INVALID_GCODE_COMMAND_NUM;
		long				cmdSubNumber	= INVALID_GCODE_COMMAND_NUM;
		
		char 				prevCmdCode		= INVALID_GCODE_COMMAND_CMD;
		long		 		prevCmdNumber	= INVALID_GCODE_COMMAND_NUM;
		long				prevCmdSubNumber= INVALID_GCODE_COMMAND_NUM;
		
		bool				openComment		= false;
		
		wxString			nodeName;
		wxString			block;
		
		SVGUnit				unit			= mm;
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
		
		const bool hasX() const		{ return x != INVALID_GCODE_FIELD_VALUE; }
		const bool hasY() const 	{ return y != INVALID_GCODE_FIELD_VALUE; }
		const bool hasZ() const 	{ return z != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasI() const		{ return i != INVALID_GCODE_FIELD_VALUE; }
		const bool hasJ() const		{ return j != INVALID_GCODE_FIELD_VALUE; }
		
		const bool hasS() const		{ return s != INVALID_GCODE_FIELD_VALUE; }
		const bool hasE() const		{ return e != INVALID_GCODE_FIELD_VALUE; }
		const bool hasF() const		{ return f != INVALID_GCODE_FIELD_VALUE; }
		
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
		double getAsPx(double value) {
			if ( value == INVALID_GCODE_FIELD_VALUE )
				return INVALID_GCODE_FIELD_VALUE;
				
			switch ( unit ) {
				case mm:	return SvgUnitCalculator::convertMM2ReferenceUnit(value);
				case in:	return SvgUnitCalculator::convertMM2ReferenceUnit(value / 25.4);
				default: std::cerr << "GCodeBlock::getAsPx: Invalid GCode unit: " << SvgUnitCalculator::getUnitAsStr(unit) << std::endl;
			}
			return INVALID_GCODE_FIELD_VALUE;
		}
		
		/////////////////////////////////////////////////////
		double getXAsPx() { return getAsPx(x); }
		double getYAsPx() { return getAsPx(y); }
		double getZAsPx() { return getAsPx(z); }
		double getIAsPx() { return getAsPx(i); }
		double getJAsPx() { return getAsPx(j); }
		
		/////////////////////////////////////////////////////
		double getXMoveRelative(const CncDoublePosition& curPxPos) { return getMoveRelative(getAsPx(x), curPxPos.getX()); }
		double getYMoveRelative(const CncDoublePosition& curPxPos) { return getMoveRelative(getAsPx(y), curPxPos.getY()); }
		double getZMoveRelative(const CncDoublePosition& curPxPos) { return getMoveRelative(getAsPx(z), curPxPos.getZ()); }
		
		//////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const GCodeBlock &a) {
			ostr         << "X=" << (a.hasX() ? wxString::Format("%f",a.x) : "-");
			ostr << ", " << "Y=" << (a.hasY() ? wxString::Format("%f",a.y) : "-");
			ostr << ", " << "Z=" << (a.hasZ() ? wxString::Format("%f",a.z) : "-");
			ostr << ", " << "I=" << (a.hasI() ? wxString::Format("%f",a.i) : "-");
			ostr << ", " << "J=" << (a.hasJ() ? wxString::Format("%f",a.j) : "-");
			return ostr;
		}
		
		//////////////////////////////////////////////////////////////////
		void trace(std::ostream &ostr) {
			cmdCode			== INVALID_GCODE_COMMAND_CMD ? ostr << "?" : ostr << cmdCode;
			cmdNumber		== INVALID_GCODE_COMMAND_NUM ? ostr << "-" : ostr << cmdNumber;
			cmdSubNumber	== INVALID_GCODE_COMMAND_NUM ? ostr << ""  : ostr << cmdSubNumber;
			
			if ( hasMoveCmd() ) {
				ostr << ":";
				ostr << *this;
			}
		
			ostr << std::endl;
		}
};

#endif

