#include "SvgPathString.h"

////////////////////////////////////////////////////////////////////////////
void SvgPathString::initPath() {
////////////////////////////////////////////////////////////////////////////
	pathStr = "<path d=\"";
}
////////////////////////////////////////////////////////////////////////////
void SvgPathString::finishPath() {
	pathStr << wxString::Format("\" %s\n />\n", getDefaultSvgElementEnd());
}
////////////////////////////////////////////////////////////////////////////
void SvgPathString::add(const wxString& pf) {
	pathStr << pf << ' ';
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathString::get() {
////////////////////////////////////////////////////////////////////////////
	finishPath();
	static wxString s;
	s = pathStr;
	
	initPath();
	return s;
}
////////////////////////////////////////////////////////////////////////////
bool SvgPathString::addPocketWhole(double td, double rx, double ry, bool correctRadius) {
////////////////////////////////////////////////////////////////////////////
	if ( td < 1.0 ) 		{ addError(wxString::Format("Invalid tool diameter: %lf", td)); 			return false; }
	if ( rx < td * 2)		{ addError(wxString::Format("Invalid radius rx: %lf", rx)); 				return false; }
	if ( ry < td * 2)		{ addError(wxString::Format("Invalid radius ry: %lf", ry)); 				return false; }
	
	// correctd raduius 
	double rxc = rx - (correctRadius == true ? (td / 2) : 0.0);
	double ryc = ry - (correctRadius == true ? (td / 2) : 0.0);
	
	// First absolute move, center is always 0,0 . . .
	add(fGen().absMove(-rxc, 0.0));
	add(fGen().ellipticalArg(rxc, ryc));
	
	// . . . then start at second step
	for ( int i=1; i<128; i++ ) {
		double nrx = rxc - td * 0.9 * i;
		double nry = ryc - td * 0.9 * i;
		
		if ( nrx < td ) break;
		if ( nry < td ) break;
		
		add(fGen().relLine(td, 0.0));
		add(fGen().ellipticalArg(rxc - td * i, ryc - td * i));
	}
	
	return true;
}