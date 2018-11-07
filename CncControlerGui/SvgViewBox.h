#ifndef SVG_VIEW_BOX_H
#define SVG_VIEW_BOX_H

#include <cfloat>
#include <wx/tokenzr.h>

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
		
		float scaleX;
		float scaleY;
		
	public:
		///////////////////////////////////////////////////////////////////
		SVGViewbox(const wxString& vb, double svgWidth, double svgHeight) 
		: viewBoxStr(vb)
		, x(DBL_MAX)
		, y(DBL_MAX)
		, w(DBL_MAX)
		, h(DBL_MAX)
		, minX(0.0)
		, maxX(0.0)
		, minY(0.0)
		, maxY(0.0)
		, scaleX(1.0f)
		, scaleY(1.0f)
		{
			wxStringTokenizer tokenizer(viewBoxStr, " ,");
			unsigned int counter = 0;
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				switch ( counter++ ) {
					case 0:		token.ToDouble(&x); break;
					case 1:		token.ToDouble(&y); break;
					case 2:		token.ToDouble(&w); break;
					case 3:		token.ToDouble(&h); break;
					default:	std::cerr << "SvgViewBox::SvgViewBox: Invalid token count: "<< counter << std::endl;;
				}
			}
			
			// calculate further members
			if ( isValid() ) {
				minX = x;
				minY = y;
				maxX = minX + w;
				maxY = minY + h;
				
				scaleX = w/(svgWidth  ? svgWidth  : 1.0);
				scaleY = h/(svgHeight ? svgHeight : 1.0);
			}
		}
		
		///////////////////////////////////////////////////////////////////
		~SVGViewbox() {}
		
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const SVGViewbox &a) {
			ostr << a.getViewBoxStr() << ": " 	<< a.getX() 	<< ", " 
												<< a.getY() 	<< ", " 
												<< a.getW() 	<< ", " 
												<< a.getH() 	<< " | "
												<< a.getMinX() 	<< ", "
												<< a.getMaxX() 	<< ", "
												<< a.getMinY() 	<< ", "
												<< a.getMaxY() 	<< " | "
												<< a.getScaleX() << ", "
												<< a.getScaleY();
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
		
		const float getScaleX()	const 	{ return scaleX; }
		const float getScaleY()	const 	{ return scaleY; }

		const wxString& getViewBoxStr() const { return viewBoxStr; }
};

#endif
