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
		
		virtual const char*		getName()  								const	{ return "SVGPathHandlerBase"; }
		SVGTransformMatrix&		getSvgTransformMatrix() 						{ return currentSvgTransformMatrix; }
		virtual bool			activateNextPath(long clientId)					{ return true; }
		virtual void			setSvgRootNode(const SVGRootNode& srn);
		
		SvgCncContext&			getSvgCncContext();
		void					setSvgCncContext(const SvgCncContext& cwp);
		
	protected:
		
		SVGRootNode				svgRootNode;
		SVGTransformMatrix 		currentSvgTransformMatrix;
		SvgCncContext			currentCncContext;
		CncDoublePosition		svgZeroPosOffset;
		
		virtual void 			transform(double& xAbs, double& yAbs);
		virtual bool 			callback(const CncCurveLib::Point& p);
		virtual bool 			processLinearMove(bool alreadyRendered);
		
		void					setSvgRefPosOffset(double xOffset, double yOffset);
		
	private:
		
		struct LinearMoveParam 
		{
			bool alreadyRendered 	= false;
			bool alreadyTransformed = false;
			
			double absX = 0.0;
			double absY = 0.0;
			double absZ = 0.0;
		};
		
		inline bool 			processLinearMove(const LinearMoveParam& p);
};

#endif
