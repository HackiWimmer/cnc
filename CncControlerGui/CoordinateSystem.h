#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

enum CoordinateSytemType { CST_NULL_Y_IS_TOP, CST_NULL_Y_IS_BOTTOM};

class CoordinateSystem {
	private:
		CoordinateSytemType type;
		unsigned int nullPointX;
		unsigned int nullPointY;
		double factX;
		double factY;
		
	public:
		//////////////////////////////////////////////////////////
		CoordinateSystem() 
		: type(CST_NULL_Y_IS_TOP)
		, nullPointX(0)
		, nullPointY(0)
		, factX(1.0)
		, factY(1.0)
		{
		}
		~CoordinateSystem() {
			
		}
		//////////////////////////////////////////////////////////
		void setup(CoordinateSytemType t, unsigned int nx, unsigned int ny, double fx = 1.0, double fy = 1.0) {
			type 		= t;
			nullPointX	= nx;
			nullPointY	= ny;
			factX 		= fx;
			factY		= fy;
		}
		//////////////////////////////////////////////////////////
		void setType(CoordinateSytemType t) {
			type 		= t;
		}
		//////////////////////////////////////////////////////////
		void setFactorXY(double fx = 1.0, double fy = 1.0) {
			factX 		= fx;
			factY		= fy;
		}
		//////////////////////////////////////////////////////////
		void setFactorX(double fx = 1.0) {
			factX 		= fx;
		}
		//////////////////////////////////////////////////////////
		void setFactorY(double fy = 1.0) {
			factY		= fy;
		}
		//////////////////////////////////////////////////////////
		CoordinateSytemType getType() 	{ return type; }
		unsigned int getNpX() 			{ return nullPointX; }
		unsigned int getNpY() 			{ return nullPointY; }
		double getFactorX()				{ return factX; }
		double getFactorY()				{ return factY; }
		//////////////////////////////////////////////////////////
		int getCorrectionY() {
			if ( type == CST_NULL_Y_IS_BOTTOM )
				return -1;
				
			return 1;
		}
};


#endif