#include <wx/sstream.h>
#include "CncPatternDefinitions.h"
#include "SvgPathGroup.h"

////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::translate(double xy) {
////////////////////////////////////////////////////////////////////////////
	translate(xy, xy);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::translate(double x, double y) {
////////////////////////////////////////////////////////////////////////////
	//always include a translate block
	double translateX = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(inputUnit) * x);
	double translateY = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(inputUnit) * y);
	
	transform << wxString::Format("translate(%.3lf,%.3lf)", translateX, translateY);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::scale(double xy) {
////////////////////////////////////////////////////////////////////////////
	scale(xy, xy);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::scale(double x, double y) {
////////////////////////////////////////////////////////////////////////////
	if ( x == 1 && y == 1 )
		return;
		
	transform << wxString::Format(" scale(%.3lf,%.3lf)", x, y);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::rotate(double a, double x, double y) {
////////////////////////////////////////////////////////////////////////////
	if ( a == 0 && x == 0 && y == 0 )
		return;
		
	double rotateX = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(inputUnit) * x);
	double rotateY = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(inputUnit) * y);
	
	transform << wxString::Format(" rotate(%.3lf,%.3lf,%.3lf)", a, rotateX, rotateY);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::skewX(double x) {
////////////////////////////////////////////////////////////////////////////
	if ( x == 0 )
		return;

	transform << wxString::Format(" skewX(%.3lf)", x);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::skewY(double y) {
////////////////////////////////////////////////////////////////////////////
	if ( y == 0 )
		return;

	transform << wxString::Format(" skewY(%.3lf)", y);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::initGroup() {
////////////////////////////////////////////////////////////////////////////
	pathGroup.clear();
	transform.clear();
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::finishGroup() {
////////////////////////////////////////////////////////////////////////////
	wxString prepend("<g>");
	
	if ( transform.IsEmpty() == false ) {
		prepend  = "<g transform=\"";
		prepend << transform;
		prepend << wxString::Format("\"\n  %s=\"%s\"", CncPatternRootName, xmlPattern); 
		prepend << "\n>";
	}
	
	prepend << SvgGeneratorBase::svgConfigPlaceholderBlock; 
	prepend << "\n\n <!-- SVG Path Generator: Generated Path Result: -->";
	
	pathGroup.Prepend(prepend);
	addReferenceCross(pathGroup);
	pathGroup << "\n</g>";
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::addReferenceCross(wxString& ps) {
////////////////////////////////////////////////////////////////////////////
	if ( includeRefCross == false )
		return;
		
	ps << "\n";
	
	wxColour colour(255,0,0);
	
	double l = 8.0;
	double x = 0.0; 
	double y = 0.0; 
	
	ps << "<!-- Reference cross -->\n";
	// horizontal
	ps << wxString::Format(" <%sline x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"1\"/%s>\n", 
						   SvgGeneratorBase::svgCommentPlaceholderStart, 
						   x - l, y, x + l, y, colour.Red(), colour.Green(), colour.Blue(),
						   SvgGeneratorBase::svgCommentPlaceholderEnd);
	// vertical
	ps << wxString::Format(" <%sline x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"1\"/%s>", 
						   SvgGeneratorBase::svgCommentPlaceholderStart,
						   x, y - l, x, y + l, colour.Red(), colour.Green(), colour.Blue(), 
						   SvgGeneratorBase::svgCommentPlaceholderEnd);
}
////////////////////////////////////////////////////////////////////////////
void SvgPathGroup::add(const wxString& ps) {
////////////////////////////////////////////////////////////////////////////
	pathGroup << "\n " << ps;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathGroup::get() {
////////////////////////////////////////////////////////////////////////////
	finishGroup();
	static wxString s;
	s = pathGroup;
	
	initGroup();
	return s;
}
////////////////////////////////////////////////////////////////////////////
wxXmlNode* SvgPathGroup::getXmlRoot() {
////////////////////////////////////////////////////////////////////////////
	wxStringInputStream stream(get());
	if ( xmlDoc.Load(stream) == true )
		return xmlDoc.DetachRoot();
		
	return NULL;
}
