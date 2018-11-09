#ifndef SVG_PATH_HANDLER_BASE_H
#define SVG_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "SvgTransformMatrix.h"

//////////////////////////////////////////////////////////////////
class SVGPathHandlerBase : public PathHandlerBase {
	
	public:
	
		SVGPathHandlerBase()
		: PathHandlerBase()
		, currentSvgTransformMatrix()
		{
			unitCalculator.changeInputUnit(Unit::px);
		}
		
		virtual ~SVGPathHandlerBase()
		{}
		
		virtual const char* getName() { return "SVGPathHandlerBase"; }
		
		SVGTransformMatrix& getSvgTransformMatrix() { 
			return currentSvgTransformMatrix; 
		}
		
	protected:
		
		SVGTransformMatrix currentSvgTransformMatrix;
		
		virtual void transform(double& xAbs, double& yAbs) {
			currentSvgTransformMatrix.transform(xAbs, yAbs);
		}
	
};

#endif
