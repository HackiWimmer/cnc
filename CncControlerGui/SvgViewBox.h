#ifndef SVG_VIEW_BOX_H
#define SVG_VIEW_BOX_H

#include <cfloat>
#include <wx/tokenzr.h>

////////////////////////////////////////////////////////////////////////////
class SvgViewBox {
	private:
		wxString viewBoxStr;
		double x;
		double y;
		double w;
		double h;
		
	public:
		///////////////////////////////////////////////////////////////////
		SvgViewBox(wxString& vb) 
		: viewBoxStr(vb)
		, x(DBL_MAX)
		, y(DBL_MAX)
		, w(DBL_MAX)
		, h(DBL_MAX)
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
		}
		///////////////////////////////////////////////////////////////////
		~SvgViewBox() {
		}
		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const SvgViewBox &a) {
			ostr << a.getViewBoxStr() << ": " << a.getX() << ", " << a.getY() << ", " << a.getW() << ", " << a.getH();
			return ostr;
		}
		///////////////////////////////////////////////////////////////////
		const bool hasX() const 	{ return x != DBL_MAX; }
		const bool hasY() const 	{ return y != DBL_MAX; }
		const bool hasW() const		{ return w != DBL_MAX; }
		const bool hasH() const		{ return h != DBL_MAX; }
		const bool isValid() const	{ return (hasX() && hasY() && hasW() && hasH()); }
		///////////////////////////////////////////////////////////////////
		const double getX() const	{ return x; }
		const double getY() const	{ return y; }
		const double getW() const	{ return w; }
		const double getH() const	{ return h; }
		const wxString& getViewBoxStr() const { return viewBoxStr; }
};

#endif
