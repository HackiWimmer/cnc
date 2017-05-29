#include "CncClipperWrapper.h"


///////////////////////////////////////////////////////////////////
const unsigned int CncPolygonPoints::doubleToIntFactor = 1000;
IntPoint CncPolygonPoints::glbTempIntPointRetVal;
wxRealPoint CncPolygonPoints::gblTmpRealPointRetVal;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
CncPolygonPoints::CncPolygonPoints() 
: Path()
, minPos(LONG_MAX, LONG_MAX)
, maxPos(LONG_MIN, LONG_MIN)
{
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
CncPolygonPoints::CncPolygonPoints(const CncPolygonPoints& p) 
: Path(p)
, minPos(p.getMinIntPos())
, maxPos(p.getMaxIntPos())
{
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
CncPolygonPoints::CncPolygonPoints(const ClipperLib::Path& p) 
: Path()
, minPos(LONG_MAX, LONG_MAX)
, maxPos(LONG_MIN, LONG_MIN)
{
///////////////////////////////////////////////////////////////////
	for ( auto it=p.begin(); it!=p.end(); ++it )
		append(*it);
}
///////////////////////////////////////////////////////////////////
CncPolygonPoints::~CncPolygonPoints() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
double CncPolygonPoints::convertToDouble(const cInt& v) {
///////////////////////////////////////////////////////////////////
	return ((double)v/doubleToIntFactor);
}
///////////////////////////////////////////////////////////////////
cInt CncPolygonPoints::convertToInt(double v) {
///////////////////////////////////////////////////////////////////
	return v * doubleToIntFactor;
}
///////////////////////////////////////////////////////////////////
const IntPoint& CncPolygonPoints::convertToIntPoint(const wxRealPoint& p) {
///////////////////////////////////////////////////////////////////
	glbTempIntPointRetVal.X = p.x * doubleToIntFactor;
	glbTempIntPointRetVal.Y = p.y * doubleToIntFactor;
	
	return glbTempIntPointRetVal;
}
///////////////////////////////////////////////////////////////////
const wxRealPoint& CncPolygonPoints::convertToRealPoint(const IntPoint& p) {
///////////////////////////////////////////////////////////////////
	gblTmpRealPointRetVal.x = ((double)p.X / doubleToIntFactor);
	gblTmpRealPointRetVal.y = ((double)p.Y / doubleToIntFactor);
	
	return gblTmpRealPointRetVal;
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::evaluateAdditionalValues() {
///////////////////////////////////////////////////////////////////
	minPos = {LONG_MAX, LONG_MAX};
	maxPos = {LONG_MIN, LONG_MIN};
	
	for ( auto it=begin(); it!=end(); ++it ) {
		updateMinMaxPos(*it);
	}
}
///////////////////////////////////////////////////////////////////
inline void CncPolygonPoints::updateMinMaxPos(const IntPoint& p) {
///////////////////////////////////////////////////////////////////
	minPos.X = std::min(p.X, minPos.X);
	minPos.Y = std::min(p.Y, minPos.Y);
	maxPos.X = std::max(p.X, maxPos.X);
	maxPos.Y = std::max(p.Y, maxPos.Y);
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::append(const IntPoint& p) {
///////////////////////////////////////////////////////////////////
	updateMinMaxPos(p);
	push_back(p);
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::append(const wxRealPoint& p) {
///////////////////////////////////////////////////////////////////
	append(CncPolygonPoints::convertToIntPoint(p));
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::append(double x, double y) {
///////////////////////////////////////////////////////////////////
	append(CncPolygonPoints::convertToIntPoint(wxRealPoint(x, y)));
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::append(long x, long y) {
///////////////////////////////////////////////////////////////////
	append(IntPoint(x * doubleToIntFactor, y * doubleToIntFactor));
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::append(int x, int y) {
///////////////////////////////////////////////////////////////////
	append(IntPoint(x * doubleToIntFactor, y * doubleToIntFactor));
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::append(const ClipperLib::Path& p) {
///////////////////////////////////////////////////////////////////
	if ( p.size() == 0 )
		return;
	
	for ( auto it=p.begin(); it!=p.end(); ++it ) {
		append(*it);
	}
	
	if ( *p.begin() != *(p.end() - 1) )
		append(*p.begin());
}
///////////////////////////////////////////////////////////////////
bool CncPolygonPoints::isPolygonClosed() {
///////////////////////////////////////////////////////////////////
	if ( size() == 0 )
		return true;
		
	return ( *begin() == *(end() - 1) );
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::closePolygon() {
///////////////////////////////////////////////////////////////////
	if ( size() > 0 && isPolygonClosed() == false )
		append(*begin());
}
///////////////////////////////////////////////////////////////////
const char* CncPolygonPoints::getDataPointsAsWktString(wxString& ret) {
///////////////////////////////////////////////////////////////////
	ret.assign("POLYGON((");

	unsigned int cnt = 0;
	CncPolygonPoints::iterator it;
	for ( it = begin(); it != end(); ++it ) {
		if ( cnt == 0 )	ret.Append(wxString::Format("%.3lf %.3lf",  convertToDouble(it->X), convertToDouble(it->Y)));
		else			ret.Append(wxString::Format(",%.3lf %.3lf", convertToDouble(it->X), convertToDouble(it->Y)));
		cnt++;
	}
	
	// append first point at the end too
	it = begin();
	ret.Append(wxString::Format(",%.3lf %.3lf", convertToDouble(it->X), convertToDouble(it->Y)));

	ret.Append("))");
	return ret;
}
//////////////////////////////////////////////////////////////////
const char* CncPolygonPoints::getAsSvgPathRepresentation(SVGUnit inputUnit, bool withElement, const wxString& style) const {
//////////////////////////////////////////////////////////////////
	static wxString s;
	s.Alloc(1024 * 4);
	
	if ( size() == 0 ) {
		s.assign("<!-- no data available -->");
		return s.c_str();
	}
	
	if ( withElement == true )	s.assign("<path d=\"M");
	else						s.assign("M");
	
	wxString x((size() > 1 ? " L" : ""));
		
	unsigned int cnt = 0;
	double pxFactor = SvgUnitCalculator::getFactorMM2Unit(inputUnit);
	
	for (auto it = begin(); it != end(); ++it) {
		if ( cnt == 0 ) s.append(wxString::Format("%.3lf,%.3lf%s", SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->X)), 
		                                                           SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->Y)), 
																   x));
		else			s.append(wxString::Format(" %.3lf,%.3lf",  SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->X)), 
		                                                           SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->Y))));
		cnt++;
	}
	
	if ( withElement == true )	
		s.append(" z\" ");
	
	if ( style.IsEmpty() == false ) 
		s.append(style);
	
	if ( withElement == true )
		s.append("/>");
		
	return s.c_str();
}



///////////////////////////////////////////////////////////////////
CncPolygons::CncPolygons() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
CncPolygons::~CncPolygons() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool CncPolygons::getPolygonPoints(unsigned int idx, CncPolygonPoints& ret) {
///////////////////////////////////////////////////////////////////
	ret.clear();
	
	if ( idx > size() - 1 )
		return false;
		
	// creates a copy!
	ret = at(idx);
		
	return true;
}




///////////////////////////////////////////////////////////////////
CncClipperWrapper::CncClipperWrapper() {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
CncClipperWrapper::~CncClipperWrapper() {
///////////////////////////////////////////////////////////////////
	
}
///////////////////////////////////////////////////////////////////
bool CncClipperWrapper::offsetPath(const CncPolygonPoints& in, CncPolygons& out, double offset, 
                                  CncClipperCornerType joinType, 
								  CncClipperEndType endType) {
///////////////////////////////////////////////////////////////////
	try {
		ClipperOffset co;
		co.AddPath(in, convertCornerType(joinType), convertEndType(endType));
		co.Execute(out, offset * CncPolygonPoints::doubleToIntFactor);
		
	} catch (std::runtime_error e ) {
		std::cerr << "CncClipperWrapper::offsetPath: Runtime error: " << e.what() << endl;
		return false;
		
	} catch(...) {
		std::cerr << "CncClipperWrapper::offsetPath: Unkown exception" << endl;
		return false;
	}
	
	return true;
}
