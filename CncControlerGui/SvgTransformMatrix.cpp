#include "SvgTransformMatrix.h"

/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getTranslateX() const {
/////////////////////////////////////////////////////////
	return E;
}
/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getTranslateY() const {
/////////////////////////////////////////////////////////
	return F;
}
/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getScaleX() const {
/////////////////////////////////////////////////////////
	DecomposedValues ret;
	decomposeMatrix(ret);
	return ret.scaleX;
}
/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getScaleY() const {
/////////////////////////////////////////////////////////
	DecomposedValues ret;
	decomposeMatrix(ret);
	return ret.scaleY;
}
/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getRotation() const {
/////////////////////////////////////////////////////////
	DecomposedValues ret;
	decomposeMatrix(ret);
	return ret.rotate;
}
/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getSkewX() const {
/////////////////////////////////////////////////////////
	DecomposedValues ret;
	decomposeMatrix(ret);
	return ret.skewX;
}
/////////////////////////////////////////////////////////
const double SVGTransformMatrix::getSkewY() const {
/////////////////////////////////////////////////////////
	DecomposedValues ret;
	decomposeMatrix(ret);
	return ret.skewY;
}
/////////////////////////////////////////////////////////
void SVGTransformMatrix::decomposeMatrix(DecomposedValues& ret) const {
/////////////////////////////////////////////////////////
	ret.scaleX = sqrt(A * A + B * B);
	ret.scaleY = sqrt(C * C + D * D);

	// If determinant is negative, one axis was flipped.
	const double determinant = A * D - B * C;
	if ( determinant < 0 ) {
		// Flip axis with minimum unit vector dot product.
		if ( A < D )	ret.scaleX = -ret.scaleX;
		else			ret.scaleY = -ret.scaleY;
	}
	
	// Re-normalize matrix to remove scale.
	double nA = A;
	double nB = B;
	double nC = C;
	double nD = D;
	
	if ( ret.scaleX ) {
		nA *= 1 / ret.scaleX;
		nB *= 1 / ret.scaleX;
	}
	
	if ( ret.scaleY ) {
		nC *= 1 / ret.scaleY;
		nD *= 1 / ret.scaleY;
	}

	// Compute rotation and re-normalize matrix.
	ret.rotate = atan2(nB, nA) * 180.0 / PI;
	
	#warning how to caluclate this?
	ret.skewX = 42.42;
	ret.skewY = 42.42;
}
/////////////////////////////////////////////////////////
std::ostream& SVGTransformMatrix::decomposeMatrix(std::ostream& o) {
/////////////////////////////////////////////////////////
	o << wxString::Format("translation(%lf,%lf)\n",	getTranslateX(), getTranslateY());
	o << wxString::Format("scale(%lf,%lf)\n",		getScaleX(), getScaleY());
	o << wxString::Format("rotation(%lf)\n",		getRotation());
	o << wxString::Format("skewX(%lf)\n",			getSkewX());
	o << wxString::Format("skewY(%lf)\n",			getSkewY());
	return o;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::unchanged() {
/////////////////////////////////////////////////////////
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	A = 1.0; B = 0.0; C = 0.0; D = 1.0; E = 0.0; F = 0.0;
	return true;
}
/////////////////////////////////////////////////////////
void SVGTransformMatrix::multiply(const SVGTransformMatrix& m) {
/////////////////////////////////////////////////////////
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
wxRealPoint& SVGTransformMatrix::transform(double& x, double& y) {
/////////////////////////////////////////////////////////
	result.x = A * x + C * y + E;
	result.y = B * x + D * y + F;
	x = result.x;
	y = result.y;
	
	return result;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::translate(TransformParameterList p) {
/////////////////////////////////////////////////////////
	switch ( p.size() ) {
		case 0: 	return translate(0.0);
		case 1: 	return translate(p.at(0));
		case 2: 	return translate(p.at(0), p.at(1));
		default:;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::translate(double x, double y) {
/////////////////////////////////////////////////////////
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	A = 1.0; B = 0.0; C = 0.0; D = 1; E = x; F = y;
	
	return true;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::scale(TransformParameterList p) {
/////////////////////////////////////////////////////////
	switch ( p.size() ) {
		case 0: 	return scale(0.0);
		case 1: 	return scale(p.at(0));
		case 2: 	return scale(p.at(0), p.at(1));
		default:;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::scale(double x, double y) {
/////////////////////////////////////////////////////////
	if ( y == 0.0 )
		y = x;
	
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	A = x; B = 0.0; C = 0.0; D = y; E = 0.0; F = 0.0;
	
	return true;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::rotate(TransformParameterList p) {
/////////////////////////////////////////////////////////
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
bool SVGTransformMatrix::rotate(double a, double x, double y) {
/////////////////////////////////////////////////////////
	if ( y == 0.0 )
		y = x;
	
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	const float angle = a * PI / 180;
	const float COS   = cos(angle);
	const float SIN   = sin(angle);
	A =  COS; 
	B =  SIN; 
	C = -SIN; 
	D =  COS; 
	E = -x * COS + y * SIN + x; 
	F = -x * SIN - y * COS + y;
	
	return true;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::skewX(TransformParameterList p) {
/////////////////////////////////////////////////////////
	switch ( p.size() ) {
		case 0: 	return skewX(0.0);
		case 1: 	return skewX(p.at(0));
		default:;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::skewX(double a) {
/////////////////////////////////////////////////////////
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	A = 1.0; B = 0.0; C = tan(a*PI/180); D = 1.0; E = 0.0; F = 0.0;
	
	return true;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::skewY(TransformParameterList p) {
/////////////////////////////////////////////////////////
	switch ( p.size() ) {
		case 0: 	return skewY(0.0);
		case 1: 	return skewY(p.at(0));
		default:;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::skewY(double a) {
/////////////////////////////////////////////////////////
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	A = 1.0; B = tan(a*PI/180); C = 0.0; D = 1.0; E = 0.0; F = 0.0;
	
	return true;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::matrix(TransformParameterList p) {
/////////////////////////////////////////////////////////
	switch ( p.size() ) {
		case 0: 	return matrix(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		case 6: 	return matrix(p.at(0), p.at(1), p.at(2), p.at(3), p.at(4), p.at(5));
		default:;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::matrix(double a, double b, double c, double d, double e, double f) {
/////////////////////////////////////////////////////////
	// | A C E |
	// | B D F |
	// | 0 0 1 |
	A = a; B = b; C = c; D = d; E = e; F = f;
	
	return true;
}
/////////////////////////////////////////////////////////
bool SVGTransformMatrix::performTransformAsStringList(const wxString& ts) {
/////////////////////////////////////////////////////////
	unsigned int tokenCount 	= 0;
	unsigned int transformCount = 0;
	bool ret = false;
	wxString cmd, param;
	
	wxStringTokenizer tokenizerClips(ts, "()");
	while ( tokenizerClips.HasMoreTokens() ) 
	{
		if ( (++tokenCount)%2 != 0 ) 
		{ 
			cmd = tokenizerClips.GetNextToken().Trim(true).Trim(false);
			if ( cmd != "" ) 
				ret = false;
		}
		else
		{
			if ( cmd != "" )
			{
				param = tokenizerClips.GetNextToken().Trim(true).Trim(false);
				
				if ( param.IsEmpty() == false )
				{
					++transformCount;
					
					TransformParameterList parameters;
					if ( evaluateTransformParameters(param, parameters)	> 0 )
					{
						ret = performTransform(cmd, parameters);
						if ( ret == false ) {
							break;
						}
					}
				}
			}
		}
	}
	
	if ( ret == false )
	{ 
		std::cerr << "SVGTransformMatrix::performTransformAsStringList: Parse error"<< std::endl;
		std::cerr << "List      : " << ts.c_str() << std::endl;
		std::cerr << "Command   : " << cmd.c_str() << std::endl;
		std::cerr << "Token #   : " << tokenCount << std::endl;
		std::cerr << "Command # : " << transformCount << std::endl;
	}
	
	return ret;
}
