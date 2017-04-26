#include "SvgPathFragment.h"

////////////////////////////////////////////////////////////////////////////
double SvgPathFragment::convertToDouble(SVGUnit inputUnit, double val) {
////////////////////////////////////////////////////////////////////////////
	return SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(inputUnit) * val);
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::convertToString(SVGUnit inputUnit, double val) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("%.3lf", convertToDouble(inputUnit, val)));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::printf(double val) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(SvgPathFragment::convertToString(inputUnit, val));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::absMove(double toX, double toY) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format(" M %s,%s", printf(toX), printf(toY)));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::relMove(double dx, double dy) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format(" m %s,%s", printf(dx), printf(dy)));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::absLine(double toX, double toY) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format(" L %s,%s", printf(toX), printf(toY)));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::relLine(double dx, double dy) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format(" l %s,%s", printf(dx), printf(dy)));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::ellipticalArg(double r) {
////////////////////////////////////////////////////////////////////////////
	return ellipticalArg(r, r);
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::ellipticalArg(double rx, double ry) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	
	s.assign(wxString::Format(" a %s,%s  0 0 1 %s,0 a %s,%s 0 0 1 -%s,0", 
	                           printf(rx), 
							   printf(ry), 
							   printf(rx * 2),
							   printf(rx),
							   printf(ry),
							   printf(rx * 2)));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::addPoint(double x, double y) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	s.assign(wxString::Format("<path d=\"%s l 0.0,0.0\"%s/>", absMove(x,y), getDefaultSvgElementEnd()));
	return s;
}

////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::addLine(double td, double x1, double y1, double x2, double y2, bool correctRadius) {
////////////////////////////////////////////////////////////////////////////
	static wxString s;
	
	double xx1 = x1 + ( correctRadius == true ? td/2 : 0.0 );
	double yy1 = y1 + ( correctRadius == true ? td/2 : 0.0 );
	double xx2 = x2 + ( correctRadius == true ? td/2 : 0.0 );
	double yy2 = y2 + ( correctRadius == true ? td/2 : 0.0 );
		
	s.assign(wxString::Format("<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" %s\n />\n", 
								SvgPathFragment::convertToDouble(mm, xx1), 
								SvgPathFragment::convertToDouble(mm, yy1),
								SvgPathFragment::convertToDouble(mm, xx2),
								SvgPathFragment::convertToDouble(mm, yy2), 
								getDefaultSvgElementEnd()));
	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::addRect(double td, double width, double height, double rx, double ry, bool correctRadius) {
////////////////////////////////////////////////////////////////////////////

	//todo add error check
	//todo add inner pathes

	double w = width  - ( correctRadius == true ? td : 0.0 );
	double h = height - ( correctRadius == true ? td : 0.0 );
	double x = referencePoint.x + ( correctRadius == true ? td/2 : 0.0 );
	double y = referencePoint.y + ( correctRadius == true ? td/2 : 0.0 );
	
	static wxString s;
	s.assign(wxString::Format("<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" rx=\"%lf\" ry=\"%lf\" %s\n />\n", 
								SvgPathFragment::convertToDouble(mm, x), 
								SvgPathFragment::convertToDouble(mm, y), 
								SvgPathFragment::convertToDouble(mm, w),
								SvgPathFragment::convertToDouble(mm, h),
								SvgPathFragment::convertToDouble(mm, rx),
								SvgPathFragment::convertToDouble(mm, ry),
								getDefaultSvgElementEnd()));

	return s;
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::addSquareRect(double td, double width, double height, bool correctRadius) {
////////////////////////////////////////////////////////////////////////////
	return addRect(td, width, height, 0.0, 0.0, correctRadius);
}
////////////////////////////////////////////////////////////////////////////
const wxString& SvgPathFragment::addRoundRect(double td, double width, double height, bool correctRadius) {
////////////////////////////////////////////////////////////////////////////
	return addRect(td, width, height, td/2, td/2, correctRadius);;
}
