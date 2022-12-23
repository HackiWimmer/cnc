#include "CncConfig.h"
#include "CncCommon.h"
#include "SvgTransformMatrix.h"
#include "SvgViewBox.h"

namespace ViewBox
{
	typedef CncUnitCalculatorBase::Unit Unit;
	wxString generate(double w, double h, Unit u)
	{
		CncUnitCalculator<float> uc(Unit::px, u);
		const float width  = uc.convert(w);
		const float height = uc.convert(h);
		return wxString::Format("%lf %lf %lf %lf", 0.0, 0.0, width, height);
	}
};

//////////////////////////////////////////////////////////////////
void SVGViewbox::setup(const wxString& vb) {
//////////////////////////////////////////////////////////////////
	viewBoxStr.assign(vb);
	
	wxStringTokenizer tokenizer(viewBoxStr, " ,");
	unsigned int counter = 0;
	while ( tokenizer.HasMoreTokens() ) 
	{
		wxString token = tokenizer.GetNextToken();
		if ( token.IsEmpty() == false )
		{
			switch ( counter++ )
			{
				case 0:		token.ToDouble(&x); break;
				case 1:		token.ToDouble(&y); break;
				case 2:		token.ToDouble(&w); break;
				case 3:		token.ToDouble(&h); break;
				default:	CNC_CERR_FUNCT_A(" Invalid token count: %u", counter);
			}
		}
	}
	
	// calculate further members
	if ( isValid() ) 
	{
		minX = x;
		minY = y;
		maxX = x + w;
		maxY = y + h;
	}
}

//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode() 
: width					(1000.0)
, height				(1000.0)
, viewBox				(ViewBox::generate(width, height, Unit::px))
, scaleX				(1.0f)
, scaleY				(1.0f)
, rootTransformation	()
, unitCalculator		(Unit::px, Unit::mm)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(double svgWidth, double svgHeight, Unit unit) 
: width					(svgWidth)
, height				(svgHeight)
, viewBox				(ViewBox::generate(width, height, unit))
, scaleX				(1.0f)
, scaleY				(1.0f)
, rootTransformation	()
, unitCalculator		(unit, unit)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(double svgWidth, double svgHeight, Unit unit, const wxString& vb) 
: width					(svgWidth)
, height				(svgHeight)
, viewBox				(vb.IsEmpty() ? ViewBox::generate(width, height, unit) : vb)
, scaleX				(1.0f)
, scaleY				(1.0f)
, rootTransformation	()
, unitCalculator		(unit, unit)
//////////////////////////////////////////////////////////////////
{
	setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(const SVGRootNode& n) 
: width					(n.getWidth())
, height				(n.getHeight())
, viewBox				(getViewbox().getViewBoxStr())
, scaleX				(n.getScaleX())
, scaleY				(n.getScaleY())
, rootTransformation	(n.getRootTransformation())
, unitCalculator		(n.getInputUnit(), Unit::mm)
//////////////////////////////////////////////////////////////////
{
	// already done by the given node
	// setup();
}
//////////////////////////////////////////////////////////////////
SVGRootNode::SVGRootNode(const wxString& vb)
//////////////////////////////////////////////////////////////////
: width					(0.0f)
, height				(0.0f)
, viewBox				(vb)
, scaleX				(1.0f)
, scaleY				(1.0f)
, rootTransformation	()
, unitCalculator		(Unit::px, Unit::px)
{
	width  = viewBox.getW();
	height = viewBox.getH();
	
	setup();
}
//////////////////////////////////////////////////////////////////
void SVGRootNode::setup() {
//////////////////////////////////////////////////////////////////
	// determine viewBox scaling
	scaleX = 1.0;
	scaleY = 1.0;
	
	if ( THE_CONFIG->getSvgConsiderViewboxFlag() )
	{
		if ( viewBox.isValid() )
		{
			if ( cnc::dblCmp::nu(viewBox.getW()) == false && cnc::dblCmp::nu(width) == false )
				scaleX = viewBox.getW() / width;
				
				
			if ( cnc::dblCmp::nu(viewBox.getH()) == false && cnc::dblCmp::nu(height) == false )
				scaleY = viewBox.getH() / height;
		}
	}
	
	// initialize root transformation
	rootTransformation.clear();
	
	// determine right hand transformation
	if ( THE_CONFIG->getSvgConvertToRightHandFlag() )
	{
		// The target display area (Cnc App) is always a right hand coordinate system.
		// Therefore, the Y axis for an svg always must be mirrored! 
		scaleY *= (-1);
		
		rootTransformation.append(wxString::Format(" scale(%lf,%lf)", scaleX, scaleY));
	}
	
	if ( THE_CONFIG->getSvgConsiderViewboxFlag() )
	{
		if ( viewBox.isValid() )
		{
			const double dx = -viewBox.getX();
			const double dy = -viewBox.getY();
			
			// consider viewBox offset
			rootTransformation.append(wxString::Format(" translate(%lf,%lf) ", dx, dy));
			
			if ( THE_CONFIG->getSvgConvertToRightHandFlag() )
				rootTransformation.append(wxString::Format(" translate(%lf,%lf) ", 0.0, -height));
		}
	}
}
//////////////////////////////////////////////////////////////////
CncDoublePosition SVGRootNode::getViewboxOffset() const {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition ret(
				getViewbox().getX(), 
				getViewbox().getY(), 
				0.0
	);
	
	return ret;
}
//////////////////////////////////////////////////////////////////
CncDoublePosition SVGRootNode::getViewboxOffset_MM() const {
//////////////////////////////////////////////////////////////////
	CncUnitCalculator<float> uc(getInputUnit(), Unit::mm);
	
	const CncDoublePosition ret(
				uc.convert(getViewbox().getX()), 
				uc.convert(getViewbox().getY()), 
				0.0
	);
	
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
