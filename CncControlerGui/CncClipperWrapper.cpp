#include "CncClipperWrapper.h"


///////////////////////////////////////////////////////////////////
const unsigned int CncPolygonPoints::doubleToIntFactor = 1000;
IntPoint CncPolygonPoints::glbTempIntPointRetVal;
wxRealPoint CncPolygonPoints::gblTmpRealPointRetVal;


inline cInt Round(double val)
{
  if ((val < 0)) return static_cast<cInt>(val - 0.5); 
  else return static_cast<cInt>(val + 0.5);
}

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
CncDirection CncPolygonPoints::getOrientation() {
///////////////////////////////////////////////////////////////////
	if ( Orientation(*this) == true )	return CncClockwise;
	else 								return CncAnticlockwise; 
	
	return CncUndefDir;
}
///////////////////////////////////////////////////////////////////
CncDirection CncPolygonPoints::setOrientation(CncDirection dir) {
///////////////////////////////////////////////////////////////////
	if ( dir != CncUndefDir ) {
		if ( getOrientation() != dir )
			reverseOrientation();
	}
	
	return getOrientation();
}
///////////////////////////////////////////////////////////////////
void CncPolygonPoints::reverseOrientation() { 
///////////////////////////////////////////////////////////////////
	ReversePath(*this); 
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
bool CncPolygonPoints::remove(iterator position) {
///////////////////////////////////////////////////////////////////
	iterator ret = erase(position);
	evaluateAdditionalValues();
	
	return (ret != end());
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
		s.assign("");
		return s.c_str();
	}
	
	if ( withElement == true )	s.assign("<path d=\"M");
	else						s.assign("M");
	
	wxString x((size() > 1 ? " L" : ""));
		
	unsigned int cnt = 0;
	double pxFactor = SvgUnitCalculator::getFactorMM2Unit(inputUnit);
	
	for (auto it = begin(); it != end(); ++it) {
		
		if ( inputUnit == px ) {
			if ( cnt == 0 ) s.append(wxString::Format("%.6lf,%.6lf%s", CncPolygonPoints::convertToDouble(it->X), 
																	   CncPolygonPoints::convertToDouble(it->Y), 
																	   x));
			else			s.append(wxString::Format(" %.6lf,%.6lf",  CncPolygonPoints::convertToDouble(it->X), 
		                                                               CncPolygonPoints::convertToDouble(it->Y)));
		} else {
			if ( cnt == 0 ) s.append(wxString::Format("%.6lf,%.6lf%s", SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->X)), 
																	   SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->Y)), 
																	   x));
			else			s.append(wxString::Format(" %.6lf,%.6lf",  SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->X)), 
		                                                               SvgUnitCalculator::convertMM2ReferenceUnit(pxFactor * CncPolygonPoints::convertToDouble(it->Y))));
		}
		
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
CncPolygons::CncPolygons() 
: ClipperLib::Paths()
{
///////////////////////////////////////////////////////////////////
	// nothing more todo
}
///////////////////////////////////////////////////////////////////
CncPolygons::CncPolygons(const CncPolygons& ps) 
: ClipperLib::Paths(ps)
{
///////////////////////////////////////////////////////////////////
	// nothing more todo
}
///////////////////////////////////////////////////////////////////
CncPolygons::CncPolygons(const PolygonList& pl) 
: ClipperLib::Paths()
{
///////////////////////////////////////////////////////////////////
	for ( auto it=pl.begin(); it!=pl.end(); ++it) {
		CncPolygonPoints pp = *it;
		push_back(pp);
	}
}
///////////////////////////////////////////////////////////////////
CncPolygons::~CncPolygons() {
///////////////////////////////////////////////////////////////////
	// nothing todo
}
///////////////////////////////////////////////////////////////////
unsigned int CncPolygons::getTotalCount() {
///////////////////////////////////////////////////////////////////
	return size();
}
///////////////////////////////////////////////////////////////////
unsigned int CncPolygons::getOuterCount() {
///////////////////////////////////////////////////////////////////
	if ( size() == 0 )
		return 0;
	
	bool dir = Orientation(*begin());
	unsigned int ret = 0;

	for ( auto it=begin(); it!=end(); ++it) {
		if ( Orientation(*it) == dir )
			ret++;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
unsigned int CncPolygons::getHoleCount() {
///////////////////////////////////////////////////////////////////
	if ( size() == 0 )
		return 0;

	bool dir = Orientation(*begin());
	unsigned int ret = 0;

	for ( auto it=begin(); it!=end(); ++it) {
		if ( Orientation(*it) != dir )
			ret++;
	}
	
	return ret;
}






///////////////////////////////////////////////////////////////////
bool CncPolygons::getPolygonPoints(unsigned int idx, CncPolygonPoints& ret) {
///////////////////////////////////////////////////////////////////
	ret.clear();
	
	if ( size() == 0 )
		return false;
	
	if ( idx > size() -1 )
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
double CncClipperWrapper::correctEndPoint(IntPoint& p1, const IntPoint& p2, double offset) {
///////////////////////////////////////////////////////////////////
	if ( p1 == p2 )
		return 0.0;
		
	cInt dx = p2.X - p1.X;
	cInt dy = p2.Y - p1.Y;
	
	double length = sqrt(pow(dx, 2) + pow(dy, 2));

	if ( length < abs(offset * CncPolygonPoints::doubleToIntFactor) ) {
		return length;
	}
	
	// determine direction
	int mx = dx >= 0 ? +1 : -1;
	int my = dy >= 0 ? +1 : -1; 
	
	// special case: pure vertical correction
	if ( p1.X == p2.X ) {
		p1.Y -= (offset * CncPolygonPoints::doubleToIntFactor * my);
		
	// spcial case: pure horizontal correction
	} else if ( p1.Y == p2.Y ) {
		p1.X -= (offset * CncPolygonPoints::doubleToIntFactor * mx);
		
	// xy corrction
	} else {
		wxASSERT(dy != 0);
		double m = Round((1.0 * dx)/dy);
		p1.X -= (Round(sin(m)) * offset * mx * CncPolygonPoints::doubleToIntFactor);
		p1.Y -= (Round(cos(m)) * offset * my * CncPolygonPoints::doubleToIntFactor);
	}
	
	return offset;
}
///////////////////////////////////////////////////////////////////
bool CncClipperWrapper::correctEndPoints(CncPolygonPoints& in, double offset) {
///////////////////////////////////////////////////////////////////
	// nothing to do
	if ( cnc::dblCompareNull(offset) == true )
		return true;
		
	bool ret            = false;
	bool beginCorrected = false;
	bool endCorrected   = false;
	
	double beginOffset  = offset;
	double endOffset    = offset;
	
	unsigned int cnt    = 0;
	
	// on demand over several points
	do {
		cnt++;
		
		// check durations - safty
		if ( cnt > 1000 ) {
			std::cerr << "CncClipperWrapper::correctEndPoints: Too mutch durations: " << cnt << std::endl;
			ret = false;
			break;
		}
		
		// check CncPolygonPoints size - safty
		if ( in.size() < 2 ) {
			std::cerr << "CncClipperWrapper::correctEndPoints: Invalid size: " << in.size() << ". Duration: " << cnt << std::endl;
			ret = false;
			break;
		}
		
		// evaluate the tow first points at the begin 
		IntPoint p1 = *(in.begin());
		IntPoint p2 = *(in.begin() + 1);
		
		// correct the begin position
		if ( beginCorrected == false ) {
			beginOffset = correctEndPoint(p1, p2, beginOffset);
			
			if ( cnc::dblCompare(beginOffset, offset) == true) {
				*(in.begin()) = p1;
				beginCorrected = true;
			} else {
				in.remove(in.begin());
			}
		}
		
		// evaluate the two last points at the end
		IntPoint p3 = *(in.end() - 1);
		IntPoint p4 = *(in.end() - 2);
		
		// correct the end position
		if ( endCorrected == false ) {
			endOffset = correctEndPoint(p3, p4, endOffset);
			
			if ( cnc::dblCompare(endOffset, offset) == true) {
				*(in.end() -1) = p3;
				endCorrected = true;
			} else {
				in.remove(in.end() -1);
			}
		}
		
		// successful end/break
		if ( beginCorrected == true && endCorrected == true ) {
			ret = true;
			break;
		}
		
	} while ( true );
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncClipperWrapper::offsetPath(const CncPolygonPoints& in, CncPolygons& out, double offset, 
                                  CncClipperCornerType joinType, 
								  CncClipperEndType endType) {
///////////////////////////////////////////////////////////////////
	PolygonList list;
	list.push_back(in);
	
	return offsetPath(list, out, offset, joinType, endType);
}
///////////////////////////////////////////////////////////////////
bool CncClipperWrapper::offsetPath(const PolygonList& inList, CncPolygons& out, double offset, 
				                   CncClipperCornerType joinType, 
				                   CncClipperEndType endType) {
///////////////////////////////////////////////////////////////////
	try {
		ClipperOffset co;
		for ( auto it=inList.begin(); it!=inList.end(); ++it) {
			CncPolygonPoints pp = *it;
			co.AddPath(pp, convertCornerType(joinType), convertEndType(endType));
		}
		
		co.Execute(out, offset * CncPolygonPoints::doubleToIntFactor);
		
	} catch (std::runtime_error& e ) {
		std::cerr << "CncClipperWrapper::offsetPath: Runtime error: " << e.what() << std::endl;
		return false;
		
	} catch(...) {
		std::cerr << "CncClipperWrapper::offsetPath: Unkown exception" << std::endl;
		return false;
	}
	
	return true;
}
