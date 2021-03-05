#include "CncConfig.h"
#include "SvgViewBox.h"

#define FORMAT_VIEWBOX_STRING \
 wxString::Format("0 0 %lf %lf", width, height)

//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode() 
: width(1000)
, height(1000)
, viewBox(FORMAT_VIEWBOX_STRING)
, scaleX(1.0f)
, scaleY(1.0f)
, unitCalculator(Unit::px, Unit::mm)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(double svgWidth, double svgHeight, Unit unit) 
: width(svgWidth)
, height(svgHeight)
, viewBox(FORMAT_VIEWBOX_STRING)
, scaleX(1.0f)
, scaleY(1.0f)
, unitCalculator(unit, unit)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(double svgWidth, double svgHeight, Unit unit, const wxString& vb) 
: width(svgWidth)
, height(svgHeight)
, viewBox(vb.IsEmpty() ? FORMAT_VIEWBOX_STRING : vb)
, scaleX(1.0f)
, scaleY(1.0f)
, unitCalculator(unit, unit)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(const SVGRootNode& n) 
: width(n.getWidth())
, height(n.getHeight())
, viewBox(getViewbox().getViewBoxStr())
, scaleX(n.getScaleX())
, scaleY(n.getScaleY())
, unitCalculator(n.getInputUnit(), Unit::mm)
//////////////////////////////////////////////////////////////////
{
	// already done by the given node
	// setup();
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::setup() {
//////////////////////////////////////////////////////////////////
	// determine scaling
	if ( THE_CONFIG->getSvgConsiderViewboxFlag() ) {
		if ( viewBox.isValid() ) {
			scaleX = getWidth()  / ( viewBox.getW() ? viewBox.getW() : 1.0f );
			scaleY = getHeight() / ( viewBox.getH() ? viewBox.getH() : 1.0f );
		}
	}
}
//////////////////////////////////////////////////////////////////
const wxString& SVGRootNode::getRootTransformation(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	ret.clear();
	
	double h  = 0.0;
	double sx = 1.0;
	double sy = 1.0;
	
	if ( THE_CONFIG->getSvgConvertToRightHandFlag() ) {
	
		// The scene has to be moved in the special case the svg should 
		// be converted to a right hand coord system 
		h  = getHeight();
		
		// The target display area (Cnc App) is always a right hand coord system.
		// Therefore, the Y axis for an svg always must be reversed! 
		sx = scaleX;
		sy = scaleY * (-1);
	}
	
	ret.append(wxString::Format("translate(%lf,%lf) ", 0.0, h));
	ret.append(wxString::Format("scale(%lf,%lf)", sx, sy ));
	return ret;
}


//////////////////////////////////////////////////////////////////
void SVGRootNodeTest::test() {
//////////////////////////////////////////////////////////////////
	typedef CncUnitCalculatorBase::Unit Unit;
	
	{
		SVGRootNode srn(10, 12, Unit::pc);
		std::cout << srn << std::endl;
	}
}
