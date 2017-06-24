#ifndef PGEN_REGULAR_DIAMETER_POLYGON_H
#define PGEN_REGULAR_DIAMETER_POLYGON_H

#include "PGenRegularRadiusPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenRegularDiameterPolygon : public PGenRegularRadiusPolygon {
	private:
		unsigned int IDX_SECTIONS;
		unsigned int IDX_DIAMETER;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual double getRadius() {
			double diameter = getParameterNumValue(IDX_DIAMETER);
			int sections 	= getParameterNumValue(IDX_SECTIONS);
			double alpha  	= 360.0 / sections / 2;
			double b      	= tan(alpha*PI/180) * diameter / 2;
			
			return sqrt(pow(diameter/2, 2) + pow(b, 2));
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Regular - Points based on Diameter";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Sections", 6, 1, 100, 0);
			pi.help = "Count of data points to connect on radius. Only values even values > 3 are valid. Exception value == 3";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Diameter [mm]", 40.0, 1.0, 500.0, 3);
			pi.help = "Diameter between two opposite sides.";
			IDX_DIAMETER = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRegularDiameterPolygon() 
		: PGenRegularRadiusPolygon() 
		, IDX_SECTIONS(INT_MAX)
		, IDX_DIAMETER(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenRegularDiameterPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			int sections 	= getParameterNumValue(IDX_SECTIONS);
			double steps  	= 360.0 / sections;
			double radius   = getRadius();
		
			for (double i=0; i<360; i+=steps ) {
				double x = cos(i*PI/180) * radius;
				double y = sin(i*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
						
				getPolygonData(0).append(x, y);
			}
			
			addPolygon(0, spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
			setTranslateX(getParameterNumValue(IDX_DIAMETER) / 2 + 5.0);
			setTranslateY(getParameterNumValue(IDX_DIAMETER) / 2 + 5.0);
			
			return spg.isOK();
		}
};

#endif