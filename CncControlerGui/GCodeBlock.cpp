#include <iostream>
#include <sstream>
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
	f 				= INVALID_GCODE_FIELD_VALUE;
	s 				= INVALID_GCODE_FIELD_VALUE;
	i 				= INVALID_GCODE_FIELD_VALUE;
	j 				= INVALID_GCODE_FIELD_VALUE;
	//....
	
	// keep e, s, etc.
}
//////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream &ostr, const GCodeBlock& a) {
//////////////////////////////////////////////////////////////////
	ostr         << "X=" << (a.hasX() ? wxString::Format("% 8.3f", a.x) : "-");
	ostr << ", " << "Y=" << (a.hasY() ? wxString::Format("% 8.3f", a.y) : "-");
	ostr << ", " << "Z=" << (a.hasZ() ? wxString::Format("% 8.3f", a.z) : "-");
	ostr << ", " << "F=" << (a.hasF() ? wxString::Format("% 8.3f", a.f) : "-");
	ostr << ", " << "S=" << (a.hasF() ? wxString::Format("% 8.3f", a.s) : "-");
	ostr << ", " << "I=" << (a.hasI() ? wxString::Format("% 8.3f", a.i) : "-");
	ostr << ", " << "J=" << (a.hasJ() ? wxString::Format("% 8.3f", a.j) : "-");
	return ostr;
}
//////////////////////////////////////////////////////////////////
const wxString GCodeBlock::getCmdAsString(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	ret.clear();
	
	cmdCode			== INVALID_GCODE_COMMAND_CMD ? ret << "?" : ret << cmdCode;
	cmdNumber		== INVALID_GCODE_COMMAND_NUM ? ret << "-" : ret << cmdNumber;
	cmdSubNumber	== INVALID_GCODE_COMMAND_NUM ? ret << ""  : ret << cmdSubNumber;
	
	return ret;
}
//////////////////////////////////////////////////////////////////
const std::ostream& GCodeBlock::trace(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	wxString cmd;
	ostr << getCmdAsString(cmd);
	
	if ( hasMoveCmd() ) {
		ostr << ":";
		ostr << *this;
	}

	ostr << std::endl;
	return ostr;
}
//////////////////////////////////////////////////////////////////
const DcmItemList& GCodeBlock::trace(DcmItemList& rows) const {
//////////////////////////////////////////////////////////////////
	rows.clear();
	
	DataControlModel::addKeyValueRow(rows, "Unit", 				getUnitAsString());
	DataControlModel::addKeyValueRow(rows, "Positioning XYZ", 	getPositioningAsString(posModeXYZ));
	DataControlModel::addKeyValueRow(rows, "Positioning IJ", 	getPositioningAsString(posModeIJ));
	DataControlModel::addKeyValueRow(rows, "Plane", 			getPlaneAsString());
	
	if ( hasOneOf_XYZ() ) {
		DataControlModel::addKeyValueRow(rows, "X", ( x != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", x) : "-"));
		DataControlModel::addKeyValueRow(rows, "Y", ( y != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", y) : "-"));
		DataControlModel::addKeyValueRow(rows, "Z", ( z != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", z) : "-"));
	}
	
	if ( hasOneOf_IJ() ) {
		DataControlModel::addKeyValueRow(rows, "I", ( i != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", i) : "-"));
		DataControlModel::addKeyValueRow(rows, "J", ( j != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", j) : "-"));
	}
	
	if ( hasOneOf_SEF() ) {
		DataControlModel::addKeyValueRow(rows, "S", ( s != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", s) : "-"));
		DataControlModel::addKeyValueRow(rows, "E", ( e != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", e) : "-"));
		DataControlModel::addKeyValueRow(rows, "F", ( f != INVALID_GCODE_FIELD_VALUE ? wxString::Format("%lf", f) : "-"));
	}
	//...
	
	return rows;
}
//////////////////////////////////////////////////////////////////
const std::ostream& GCodeBlock::traceMore(std::ostream& ostr) const{
//////////////////////////////////////////////////////////////////
	if ( hasOneOf_IJ() ) {
		ostr << ( i != INVALID_GCODE_FIELD_VALUE ? wxString::Format("I=%lf", i) : "") << ", ";
		ostr << ( j != INVALID_GCODE_FIELD_VALUE ? wxString::Format("J=%lf", j) : "");
	}
	
	/*
	if ( hasOneOf_SEF() ) {
		ostr << ( s != INVALID_GCODE_FIELD_VALUE ? wxString::Format("S=%lf", s) : "") << ", ";
		ostr << ( e != INVALID_GCODE_FIELD_VALUE ? wxString::Format("E=%lf", e) : "");
	}
	*/
	return ostr;
}
//////////////////////////////////////////////////////////////////
const wxString& GCodeBlock::traceMore(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	traceMore(ss);
	ret.assign(ss.str().c_str());
	
	return ret;
}
