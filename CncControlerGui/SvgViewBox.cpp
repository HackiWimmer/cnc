#include "SvgViewBox.h"

//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode() 
: width(1000)
, height(1000)
, viewBox(wxString::Format("0 0 %lf %lf", width, height))
, resultSet()
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
, viewBox(wxString::Format("0 0 %lf %lf", width, height))
, resultSet()
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
, viewBox(vb.IsEmpty() ? wxString::Format("0 0 %lf %lf", width, height) : vb)
, resultSet()
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
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::setup() {
//////////////////////////////////////////////////////////////////
	if ( viewBox.isValid() ) {
		scaleX = getWidth()  / ( viewBox.getW() ? viewBox.getW() : 1.0f );
		scaleY = getHeight() / ( viewBox.getH() ? viewBox.getH() : 1.0f );
	}
}
//////////////////////////////////////////////////////////////////
const wxString& SVGRootNode::getRootTransformation(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	ret.assign(wxString::Format("scale(%lf,%lf)", scaleX, scaleY));
	return ret;
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::convertToUnit(const Unit unit) {
//////////////////////////////////////////////////////////////////
	unitCalculator.changeOutputUnit(unit);
	
	width  = unitCalculator.convert(width);
	height = unitCalculator.convert(height);
	
	const double vbX = unitCalculator.convert(viewBox.getX());
	const double vbY = unitCalculator.convert(viewBox.getY());
	const double vbW = unitCalculator.convert(viewBox.getW());
	const double vbH = unitCalculator.convert(viewBox.getH());
	
	viewBox.setup(wxString::Format("%lf %lf %lf %lf", vbX, vbY, vbW, vbH) );
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::convertToMillimeter() {
//////////////////////////////////////////////////////////////////
	convertToUnit(Unit::mm);
}
//////////////////////////////////////////////////////////////////
const SVGRootNode::ResultSet& SVGRootNode::getResult() {
//////////////////////////////////////////////////////////////////

	static ResultSet rs;


	return rs;
}

//////////////////////////////////////////////////////////////////
bool SVGRootNode::calculateResultSet(ResultSet& rs, float docW, float docH) {
//////////////////////////////////////////////////////////////////
	return calculateResultSet(rs, docW, docH, 0, 0, docW, docH);
}

//////////////////////////////////////////////////////////////////
bool SVGRootNode::calculateResultSet(ResultSet& rs, float docW, float docH, float vbX, float vbY, float vbW, float vbH) {
//////////////////////////////////////////////////////////////////

	rs.minX 	= vbX;
	rs.minY 	= vbY;

	rs.width 	= vbW;
	rs.height 	= vbH;

	rs.maxX 	= rs.minX + rs.width;
	rs.maxY 	= rs.minY + rs.height;

	rs.scaleX 	= vbW ? docW / vbW : 1.0;
	rs.scaleY 	= vbH ? docH / vbH : 1.0;


	return true;
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
