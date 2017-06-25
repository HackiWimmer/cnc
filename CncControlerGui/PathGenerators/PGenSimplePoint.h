#ifndef PGEN_SIMPLE_POINT_H
#define PGEN_SIMPLE_POINT_H

#include "PGenPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenSimplePoint : public PGenPolygon {
	private:
		unsigned int IDX_X;
		unsigned int IDX_Y;
		unsigned int IDX_REFPOINT;
	
	protected:
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Simple Point";

			commonValues.canJointType = false;
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupNum("X [mm]", 10.0, 0.0, 500.0, 3);
			pi.help = "Position X";
			IDX_X = setupParameter(pi); 
			
			pi.setupNum("Y [mm]", 15.0, 0.0, 500.0, 3);
			pi.help = "Position Y";
			IDX_Y = setupParameter(pi); 
			
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenSimplePoint() 
		: PGenPolygon() 
		, IDX_X(INT_MAX)
		, IDX_Y(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSimplePoint() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			getPolygonData().append(getParameterNumValue(IDX_X), getParameterNumValue(IDX_Y));
			getPolygonData().append(getParameterNumValue(IDX_X), getParameterNumValue(IDX_Y));
			drawPolyLine(0, spg);
			
			setTranslateX(getParameterNumValue(IDX_X) + 5.0);
			setTranslateY(getParameterNumValue(IDX_Y) + 5.0);
			
			return spg.isOK();
		}
};

#endif