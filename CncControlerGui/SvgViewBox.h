#ifndef SVG_VIEW_BOX_H
#define SVG_VIEW_BOX_H

#include <cfloat>
#include <wx/tokenzr.h>
#include "CncUnitCalculator.h"

////////////////////////////////////////////////////////////////////////////
class SVGViewbox {
	
	private:
		wxString viewBoxStr;
		double x;
		double y;
		double w;
		double h;
		
		double minX;
		double maxX;
		double minY;
		double maxY;
		
	public:
	
		///////////////////////////////////////////////////////////////////
		explicit SVGViewbox(const wxString& vb) 
		: viewBoxStr	(vb)
		, x				(DBL_MAX)
		, y				(DBL_MAX)
		, w				(DBL_MAX)
		, h				(DBL_MAX)
		, minX			(0.0)
		, maxX			(0.0)
		, minY			(0.0)
		, maxY			(0.0)
		{
			setup(viewBoxStr);
		}
		
		SVGViewbox(const SVGViewbox& vb) 
		: viewBoxStr	(vb.getViewBoxStr())
		, x				(vb.getX())
		, y				(vb.getY())
		, w				(vb.getW())
		, h				(vb.getH())
		, minX			(vb.getMinX())
		, maxX			(vb.getMaxX())
		, minY			(vb.getMinY())
		, maxY			(vb.getMaxY())
		{}
		
		~SVGViewbox() {}
		void setup(const wxString& vb);
		
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const SVGViewbox &a)
		{
			ostr << a.getViewBoxStr() << ": " 	<< a.getX() 	<< ", " 
												<< a.getY() 	<< ", " 
												<< a.getW() 	<< ", " 
												<< a.getH() 	<< " | "
												<< a.getMinX() 	<< ", "
												<< a.getMaxX() 	<< ", "
												<< a.getMinY() 	<< ", "
												<< a.getMaxY();
			return ostr;
		}
		
		///////////////////////////////////////////////////////////////////
		const bool hasX() 		const 	{ return x != DBL_MAX; }
		const bool hasY() 		const 	{ return y != DBL_MAX; }
		const bool hasW() 		const	{ return w != DBL_MAX; }
		const bool hasH() 		const	{ return h != DBL_MAX; }
		const bool isValid() 	const	{ return (hasX() && hasY() && hasW() && hasH()); }
		
		///////////////////////////////////////////////////////////////////
		const double getX() 	const	{ return x; }
		const double getY() 	const	{ return y; }
		const double getW() 	const	{ return w; }
		const double getH() 	const	{ return h; }
		
		const double getMinX() 	const	{ return minX; }
		const double getMaxX() 	const	{ return maxX; }
		const double getMinY() 	const	{ return minY; }
		const double getMaxY() 	const	{ return maxY; }
		
		const wxString& getViewBoxStr() const { return viewBoxStr; }
};

////////////////////////////////////////////////////////////////////////////
class SVGRootNode {
	
	private:
		typedef CncUnitCalculatorBase::Unit Unit;
		
		double				width;
		double				height;
		
		SVGViewbox			viewBox;
		
		float				scaleX;
		float				scaleY;
		
		wxString			rootTransformation;
		
		CncUnitCalculator<float> unitCalculator;
		
		void setup();
		
	public:
		
		SVGRootNode();
		explicit SVGRootNode(const wxString& vb);
		SVGRootNode(double svgWidth, double svgHeight, Unit unit);
		SVGRootNode(double svgWidth, double svgHeight, Unit unit, const wxString& vb);
		SVGRootNode(const SVGRootNode& n); 
		~SVGRootNode() {}
		
		const SVGViewbox&	getViewbox()			const { return viewBox; }
		CncDoublePosition	getViewboxOffset()		const; 
		CncDoublePosition	getViewboxOffset_MM()	const; 
		const double		getWidth()				const { return width;   }
		const double		getHeight()				const { return height;  }
		
		const float			getScaleX()				const { return scaleX;  }
		const float			getScaleY()				const { return scaleY;  }
		
		const Unit			getInputUnit()			const { return unitCalculator.getInputUnit();  }
		const double		getWidth_MM()			const { CncUnitCalculator<float> uc(getInputUnit(), Unit::mm);  return uc.convert(width);  }
		const double		getHeight_MM()			const { CncUnitCalculator<float> uc(getInputUnit(), Unit::mm);  return uc.convert(height); }
		
		const wxString& getRootTransformation()		const { return rootTransformation; }
		
		friend std::ostream &operator<< (std::ostream &ostr, const SVGRootNode &a)
		{
			typedef CncUnitCalculator<float> UC;
			
			ostr	<< " <svg"
					<< " width=\""		<< a.getWidth_MM()	<< UC::getUnitAsStr(Unit::mm)	<< "\""
					<< " height=\""		<< a.getHeight_MM()	<< UC::getUnitAsStr(Unit::mm)	<< "\""
					<< " viewBox=\""	<< a.getViewbox().getViewBoxStr()					<< "\""
					<< " transform=\""	<< a.getRootTransformation()						<< "\""
					<< "/>"
			;
			
			return ostr;
		}
};

////////////////////////////////////////////////////////////////////////////
struct SVGRootNodeTest {
	
	static void test();
};

#endif

