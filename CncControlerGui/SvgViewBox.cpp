#include "CncConfig.h"
#include "SvgViewBox.h"

#define FormatViewBoxString(inputUnit) \
 wxString::Format("0 0 %lf %lf", CncUnitCalculator<float>(inputUnit, Unit::px).convert(width), \
								 CncUnitCalculator<float>(inputUnit, Unit::px).convert(height))

//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode() 
: width(1000)
, height(1000)
, viewBox(FormatViewBoxString(Unit::px))
, scaleX(1.0f)
, scaleY(1.0f)
, unitCalculator(Unit::px, Unit::px)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(double svgWidth, double svgHeight, Unit unit) 
: width(svgWidth)
, height(svgHeight)
, viewBox(FormatViewBoxString(unit))
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
, viewBox(vb.IsEmpty() ? FormatViewBoxString(unit) : vb)
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
, unitCalculator(n.getInputUnit(), n.getOutputUnit())
//////////////////////////////////////////////////////////////////
{
	// already done by the given node
	// setup();
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::setup() {
//////////////////////////////////////////////////////////////////
	// to be independent from the given width and height unit 
	// convert a values to px
	convertToPx();
	
	// determine scaling
	if ( viewBox.isValid() ) {
		scaleX = getWidth()  / ( viewBox.getW() ? viewBox.getW() : 1.0f );
		scaleY = getHeight() / ( viewBox.getH() ? viewBox.getH() : 1.0f );
	}
}
//////////////////////////////////////////////////////////////////
const wxString& SVGRootNode::getRootTransformation(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	ret.clear();
	
	// The scene have to be moved in the special case the svg should 
	// be converted to a right hand coord system 
	if ( CncConfig::getGlobalCncConfig()->getSvgConvertToRightHandFlag() )
		ret.append(wxString::Format("translate(%lf,%lf) ", 0.0, getHeight()));
	
	// The target display area is always a right hand coord system. 
	// Therefore the Y axis for an svg always must be reversed! 
	ret.append(wxString::Format("scale(%lf,%lf)", scaleX, scaleY * (-1) ));
	
	return ret;
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::convertToUnit(const Unit unit) {
//////////////////////////////////////////////////////////////////
	unitCalculator.changeOutputUnit(unit);
	
	width  = unitCalculator.convert(width);
	height = unitCalculator.convert(height);
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
