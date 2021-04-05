#ifndef SVG_TRANSFORM_MATRIX_H
#define SVG_TRANSFORM_MATRIX_H

#include <vector>
#include <wx/tokenzr.h>
#include <wx/gdicmn.h>
#include "CncCommon.h"

typedef std::vector<double> TransformParameterList;

class SVGTransformMatrix {
	
	private:
		double A;
		double B;
		double C;
		double D;
		double E;
		double F;
		
		wxRealPoint result;
		
		/////////////////////////////////////////////////////////
		int evaluateTransformParameters(wxString& pStr, TransformParameterList & p) {
			p.clear();
			
			wxStringTokenizer tokenizer(pStr, " ,");
			while ( tokenizer.HasMoreTokens() ) {
				wxString s = tokenizer.GetNextToken();
				if ( s.IsEmpty() == false ) {
					double d;
					s.ToDouble(&d);
					p.push_back(d);
				}
			}

			return p.size();
		}
		
		/////////////////////////////////////////////////////////
		bool performTransform(const wxString& cmd, TransformParameterList parameters) {
			SVGTransformMatrix tmp;
			bool ret = false;
			bool trace = false;
			
			if ( trace ) std::clog << cmd.c_str() << std::endl;
			
			if (      cmd == "translate" ) 	ret = tmp.translate (parameters);
			else if ( cmd == "scale" )		ret = tmp.scale     (parameters);
			else if ( cmd == "rotate" ) 	ret = tmp.rotate    (parameters);
			else if ( cmd == "skewX" ) 		ret = tmp.skewX     (parameters);
			else if ( cmd == "skewY" ) 		ret = tmp.skewY     (parameters);
			else if ( cmd == "matrix" )		ret = tmp.matrix    (parameters);
			else {
				std::cerr << "SVGTransformMatrix::performTransform: " << cmd.c_str() << " is currently not supported" << std::endl;
			}
			
			if ( ret == true ) {
				multiply(tmp);
			
				if ( trace ) std::clog << *this;
			}
			
			if ( ret == false ) {
				std::cerr << "SVGTransformMatrix::performTransform: Failed" << std::endl;
				std::cerr << "Command: " << cmd.c_str() << std::endl;
				for (TransformParameterList::iterator it=parameters.begin(); it!=parameters.end(); ++it) {
					std::cerr << " Para  : " << *it << std::endl;
				}
			}
			
			return ret;
		}
		
	public:
		/////////////////////////////////////////////////////////
		SVGTransformMatrix()
		: A(0.0)
		, B(0.0)
		, C(0.0)
		, D(0.0)
		, E(0.0)
		, F(0.0)
		{
			// | a c e |
			// | b d f |
			// | 0 0 1 |
			unchanged();
		}
		
		/////////////////////////////////////////////////////////
		~SVGTransformMatrix()
		{}

		/////////////////////////////////////////////////////////
		const double getA() const { return A; }
		const double getB() const { return B; }
		const double getC() const { return C; }
		const double getD() const { return D; }
		const double getE() const { return E; }
		const double getF() const { return F; }
		
		bool unchanged();
		
		void multiply(const SVGTransformMatrix& m);
		wxRealPoint& transform(double& x, double& y);
		
		bool translate(TransformParameterList p);
		bool translate(double x, double y = 0.0);
		
		bool scale(TransformParameterList p);
		bool scale(double x, double y = 0.0);
		
		bool rotate(TransformParameterList p);
		bool rotate(double a, double x = 0.0, double y = 0.0);
		
		bool skewX(TransformParameterList p);
		bool skewX(double a);
		
		bool skewY(TransformParameterList p);
		bool skewY(double a);
		
		bool matrix(TransformParameterList p);
		bool matrix(double a, double b, double c, double d, double e, double f);
		
		bool performTransformAsStringList(const wxString& ts);
		
		/////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const SVGTransformMatrix &a) {
			ostr << "SVGTransformMatrix: ";
			ostr << "A=" << a.getA() << ", ";
			ostr << "B=" << a.getB() << ", ";
			ostr << "C=" << a.getC() << ", "; 
			ostr << "D=" << a.getD() << ", "; 
			ostr << "E=" << a.getE() << ", "; 
			ostr << "F=" << a.getF() << std::endl; 
			return ostr;
		}
};

/////////////////////////////////////////////////////////////////
class SVGTransformer {
	
	protected:
		SVGTransformMatrix matrix;
		SVGTransformMatrix helper;
		
	public:
		/////////////////////////////////////////////////////////
		SVGTransformer()
		: matrix()
		{
		}
		
		/////////////////////////////////////////////////////////
		void transformPoint(wxRealPoint& p) { transformPoint(p.x, p.y); }
		void transformPoint(double& x, double& y) {
			matrix.transform(x, y);
		}
		
		/////////////////////////////////////////////////////////
		const wxRealPoint& transform(const wxRealPoint& p) { return transform(p.x, p.y); }
		const wxRealPoint& transform(double x, double y) {
			return matrix.transform(x, y);
		}
		
		/////////////////////////////////////////////////////////
		void applyUnchanged() { 
			matrix.unchanged(); 
		}
		
		/////////////////////////////////////////////////////////
		void applyTranslate(double xy) { applyTranslate(xy, xy); }
		void applyTranslate(double x, double y) {
			helper.unchanged();
			helper.translate(x, y);
			matrix.multiply(helper);
		}
		
		/////////////////////////////////////////////////////////
		void applyScale(double xy) { applyScale(xy, xy); }
		void applyScale(double x, double y) {
			helper.unchanged();
			helper.scale(x, y);
			matrix.multiply(helper);
		}
		
		/////////////////////////////////////////////////////////
		void applyRotate(double a) { applyRotate(a, 0.0, 0.0); }
		void applyRotate(double a, double xy) { applyRotate(a, xy, xy); }
		void applyRotate(double a, double x, double y) {
			helper.unchanged();
			helper.rotate(a, x, y);
			matrix.multiply(helper);
		}
		
		/////////////////////////////////////////////////////////
		void applySkewX(double a) {
			helper.unchanged();
			helper.skewX(a);
			matrix.multiply(helper);
		}
		
		/////////////////////////////////////////////////////////
		void applySkewY(double a) {
			helper.unchanged();
			helper.skewX(a);
			matrix.multiply(helper);
		}
		
		/////////////////////////////////////////////////////////
		void applyMatrix(double a, double b, double c, double d, double e, double f) {
			helper.unchanged();
			helper.matrix(a, b, c, d, e, f);
			matrix.multiply(helper);
		}
		
};

#endif
