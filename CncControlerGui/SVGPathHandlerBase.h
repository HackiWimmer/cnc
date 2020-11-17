#ifndef SVG_PATH_HANDLER_BASE_H
#define SVG_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "SvgViewBox.h"
#include "SvgCncContext.h"
#include "SvgTransformMatrix.h"

//////////////////////////////////////////////////////////////////
class SVGPathHandlerBase : public PathHandlerBase {
	
	public:
		
		SVGPathHandlerBase();
		virtual ~SVGPathHandlerBase();
		
		virtual const char* getName()  						{ return "SVGPathHandlerBase"; }
		SVGTransformMatrix& getSvgTransformMatrix() 		{ return currentSvgTransformMatrix; }
		
		virtual void setSvgRootNode(const SVGRootNode& srn);
		
		SvgCncContext& getSvgCncContext();
		void setSvgCncContext(SvgCncContext& cwp);
		
	protected:
		
		SVGRootNode			svgRootNode;
		SVGTransformMatrix 	currentSvgTransformMatrix;
		SvgCncContext		currentCncContext;
		
		virtual void transform(double& xAbs, double& yAbs)  { currentSvgTransformMatrix.transform(xAbs, yAbs); }
		virtual bool callback(const CncCurveLib::Point& p);
		virtual bool processLinearMove(bool alreadyRendered);
		
	private:
		
		struct LinearMoveParam {
			bool alreadyRendered 	= false;
			bool alreadyTransformed = false;
			
			double absX = 0.0;
			double absY = 0.0;
			double absZ = 0.0;
		};
		
		inline bool processLinearMove(const LinearMoveParam& p);
};

#endif
