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
			
			if (      cmd == "translate" ) 	ret = tmp.translate(parameters);
			else if ( cmd == "scale" )		ret = tmp.scale(parameters);
			else if ( cmd == "rotate" ) 	ret = tmp.rotate(parameters);
			else if ( cmd == "skewX" ) 		ret = tmp.skewX(parameters);
			else if ( cmd == "skewY" ) 		ret = tmp.skewY(parameters);
			else if ( cmd == "matrix" )		ret = tmp.matrix(parameters);
			else {
				std::cerr << "SVGFileParser::performTransform: " << cmd.c_str() << " is currently not supported" << std::endl;
			}
			
			if ( ret == true ) {
				multiply(tmp);
			
				if ( trace ) std::clog << *this;
			}
			
			if ( ret == false ) {
				std::cerr << "SVGFileParser::performTransform: Failed" << std::endl;
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
		
		/////////////////////////////////////////////////////////
		void multiply(const SVGTransformMatrix& m) {
			double a, b, c, d , e, f;
			
			// | A C E |   | m.A m.C m.E |   | A*m.A + C*m.B + E*0   A*m.C + C*m.D + E*0   A*m.E + C*m.F + E*1 |
			// | B D F | x | m.B m.D m.F | = | B*m.A + D*m.B + F*0   B*m.C + D*m.D + F*0   B*m.E + D*m.F + F*1 |
			// | 0 0 1 |   |   0   0   1 |   | 0*m.A + 0*m.B + 1*0   0*m.C + 0*m.D + 1*0   0*m.E + 0*m.F + 1*1 | 
			
			a = A * m.getA() + C * m.getB();
			b = B * m.getA() + D * m.getB();
			c = A * m.getC() + C * m.getD();
			d = B * m.getC() + D * m.getD();
			e = A * m.getE() + C * m.getF() + E;
			f = B * m.getE() + D * m.getF() + F;
			
			A = a; B = b; C = c; D = d; E = e; F = f;
		}
		
		/////////////////////////////////////////////////////////
		wxRealPoint& transform(double& x, double& y) {
			result.x = A * x + C * y + E;
			result.y = B * x + D * y + F;
			x = result.x;
			y = result.y;
			
			return result;
		}
		
		/////////////////////////////////////////////////////////
		bool unchanged() {
			A = 1.0; B = 0.0; C = 0.0; D = 1.0; E = 0.0; F = 0.0;
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool translate(TransformParameterList p) {
			switch ( p.size() ) {
				case 0: 	return translate(0.0);
				case 1: 	return translate(p.at(0));
				case 2: 	return translate(p.at(0), p.at(1));
				default:;
			}
			return false;
		}
		
		/////////////////////////////////////////////////////////
		bool translate(double x, double y = 0.0) {
			// matrix(1 0 0 1 x y)
			A = 1.0; B = 0.0; C = 0.0; D = 1; E = x; F = y;
			
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool scale(TransformParameterList p) {
			switch ( p.size() ) {
				case 0: 	return scale(0.0);
				case 1: 	return scale(p.at(0));
				case 2: 	return scale(p.at(0), p.at(1));
				default:;
			}
			return false;
		}

		/////////////////////////////////////////////////////////
		bool scale(double x, double y = 0.0) {
			if ( y == 0.0 )
				y = x;

			//matrix(x 0 0 y 0 0)
			A = x; B = 0.0; C = 0.0; D = y; E = 0.0; F = 0.0;
			
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool rotate(TransformParameterList p) {
			switch ( p.size() ) {
				case 0: 	return rotate(0.0);
				case 1: 	return rotate(p.at(0));
				case 2: 	return rotate(p.at(0), p.at(1));
				case 3: 	return rotate(p.at(0), p.at(1), p.at(2));
				default:;
			}
			return false;
		}

		/////////////////////////////////////////////////////////
		bool rotate(double a, double x = 0.0, double y = 0.0) {
			if ( y == 0.0 )
				y = x;
			
			A = cos(a*PI/180); B = sin(a*PI/180); C = -sin(a*PI/180); D = cos(a*PI/180); E = x; F = y;

			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool skewX(TransformParameterList p) {
			switch ( p.size() ) {
				case 0: 	return skewX(0.0);
				case 1: 	return skewX(p.at(0));
				default:;
			}
			return false;
		}

		/////////////////////////////////////////////////////////
		bool skewX(double a) {
			A = 1.0; B = 0.0; C = tan(a*PI/180); D = 1.0; E = 0.0; F = 0.0;
			
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool skewY(TransformParameterList p) {
			switch ( p.size() ) {
				case 0: 	return skewY(0.0);
				case 1: 	return skewY(p.at(0));
				default:;
			}
			return false;
		}

		/////////////////////////////////////////////////////////
		bool skewY(double a) {
			A = 1.0; B = tan(a*PI/180); C = 0.0; D = 1.0; E = 0.0; F = 0.0;
			
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool matrix(TransformParameterList p) {
			switch ( p.size() ) {
				case 0: 	return matrix(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
				case 6: 	return matrix(p.at(0), p.at(1), p.at(2), p.at(3), p.at(4), p.at(5));
				default:;
			}
			return false;
		}

		/////////////////////////////////////////////////////////
		bool matrix(double a, double b, double c, double d, double e, double f) {
			//matrix(<a> <b> <c> <d> <e> <f>)
			A = a; B = b; C = c; D = d; E = e; F = f;
			
			return true;
		}

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
		
		/////////////////////////////////////////////////////////
		bool performTransformAsStringList(const wxString& ts) {
			unsigned int tokenCount 	= 0;
			unsigned int transformCount = 0;
			bool ret = false;
			wxString cmd, param;
			
			wxStringTokenizer tokenizerClips(ts, "()");
			while ( tokenizerClips.HasMoreTokens() ) {
				if ( (++tokenCount)%2 != 0 ) { 
					cmd = tokenizerClips.GetNextToken().Trim(true).Trim(false);
					if ( cmd != "" ) 
						ret = false;
				} else {
					if ( cmd != "" ) {
						param = tokenizerClips.GetNextToken().Trim(true).Trim(false);
						
						if ( param.IsEmpty() == false ) {
							++transformCount;
							
							TransformParameterList parameters;
							if ( evaluateTransformParameters(param, parameters)	> 0 ) {
								ret = performTransform(cmd, parameters);
								if ( ret == false ) {
									break;
								}
							}
						}
					}
				}
			}
			
			if ( ret == false ) { 
				std::cerr << "SVGTransformMatrix::performTransformAsStringList: Parse error"<< std::endl;
				std::cerr << "List      : " << ts.c_str() << std::endl;
				std::cerr << "Command   : " << cmd.c_str() << std::endl;
				std::cerr << "Token #   : " << tokenCount << std::endl;
				std::cerr << "Command # : " << transformCount << std::endl;
			}
			
			return ret;
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
