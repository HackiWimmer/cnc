#ifndef SVG_PATH_FRAGMENT_H
#define SVG_PATH_FRAGMENT_H

#include "SvgTransformMatrix.h"
#include "SvgGeneratorBase.h"

////////////////////////////////////////////////////////////////////////////////////
class PolygonDataPoint {
	
	private:
		wxRealPoint	origPoint;
		wxRealPoint currPoint;
	
	public:
		///////////////////////////////////////////////////////////////////////////
		PolygonDataPoint(const wxRealPoint& p) {
			origPoint = p;
			currPoint = p;
		}
		
		///////////////////////////////////////////////////////////////////////////
		PolygonDataPoint(const PolygonDataPoint& pdp) {
			origPoint = pdp.getOriginalPoint();
			currPoint = pdp.getTransformedPoint();
		}
		
		///////////////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const PolygonDataPoint &a) {
			ostr << wxString::Format("%10.3lf,", a.getOriginalPoint().x);
			ostr << wxString::Format("%10.3lf",  a.getOriginalPoint().y);
			ostr << " | ";
			ostr << wxString::Format("%10.3lf,",a.getTransformedPoint().x);
			ostr << wxString::Format("%10.3lf", a.getTransformedPoint().y);
			return ostr;
		}
		
		///////////////////////////////////////////////////////////////////////////
		const wxRealPoint& getOriginalPoint() const 	{ return origPoint; }
		const wxRealPoint& getTransformedPoint() const 	{ return currPoint; }
		
		///////////////////////////////////////////////////////////////////////////
		void setOriginalPoint(const wxRealPoint& p) {
			origPoint = currPoint = p;
		}
		
		///////////////////////////////////////////////////////////////////////////
		void setTransformedPoint(const wxRealPoint& p) {
			currPoint = p;
		}
		
		///////////////////////////////////////////////////////////////////////////
		void transform(SVGTransformer& t) {
			currPoint = t.transform(origPoint.x, origPoint.y);
		}
		
		///////////////////////////////////////////////////////////////////////////
		void transformAgain(SVGTransformer& t) {
			currPoint = t.transform(currPoint.x, currPoint.y);
		}
};

typedef std::vector<PolygonDataPoint> PathFragmentPolygonData;

////////////////////////////////////////////////////////////////////////////////////
class SvgPathFragment : public SvgGeneratorBase {
	
	protected:
		SVGUnit inputUnit;
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& printf(double val);
		
	public:
		////////////////////////////////////////////////////////////////////////////
		SvgPathFragment(SVGUnit i = mm) 
		: SvgGeneratorBase()
		, inputUnit(i)
		{
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual ~SvgPathFragment() {
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setColour(const wxColour& c) {
			SvgGeneratorBase::setColour(c);
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setReferencePoint(const wxRealPoint& p) {
			SvgGeneratorBase::setReferencePoint(p);
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setXmlPattern(const wxString& pattern) {
			SvgGeneratorBase::setXmlPattern(pattern);
		}
		
		////////////////////////////////////////////////////////////////////////////
		static const wxString& convertToString(SVGUnit inputUnit, double val);
		static double convertToDouble(SVGUnit inputUnit, double val);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& absMove(double toX, double toY);
		const wxString& relMove(double dx, double dy);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& absLine(double toX, double toY);
		const wxString& relLine(double dx, double dy);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& ellipticalArg(double r);
		const wxString& ellipticalArg(double rx, double ry);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& addRect(double td, double width, double height, double rx, double ry, bool correctRadius);
		const wxString& addSquareRect(double td, double width, double height, bool correctRadius);
		const wxString& addRoundRect(double td, double width, double height, bool correctRadius);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& addPoint(double x, double y);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& addPolygon(const PathFragmentPolygonData& pd);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& addLine(double td, double x1, double y1, double x2, double y2, bool correctRadius);
		
};

#endif