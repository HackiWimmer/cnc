#ifndef PGEN_REGULAR_RADIUS_POLYGON_H
#define PGEN_REGULAR_RADIUS_POLYGON_H

#include "PGenPolygon.h"
#include "SVGElementConverter.h"

///////////////////////////////////////////////////////////////////////////
class PGenRegularRadiusPolygon : public PGenPolygon {
	private:
		unsigned int IDX_SECTIONS;
		unsigned int IDX_RADIUS;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual double getRadius() {
			return getParameterNumValue(IDX_RADIUS);
		}
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			double radius = getRadius();
			
			wxString path;
			SVGElementConverter::convertCircleToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                             SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                             SvgUnitCalculator::convertMM2ReferenceUnit(radius), 
			                                             path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Regular - Points based on Radius";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Sections", 6, 1, 100, 0);
			pi.help = "Count of data points to connect on radius. Only values even values > 3 are valid. Exception value == 3";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Radius [mm]", 40.0, 1.0, 500.0, 3);
			pi.help = "Radius on which the data points take place.";
			IDX_RADIUS = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRegularRadiusPolygon() 
		: PGenPolygon() 
		, IDX_SECTIONS(INT_MAX)
		, IDX_RADIUS(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		virtual ~PGenRegularRadiusPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanging(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { 
			if ( cc == PathGeneratorBase::ChangeCategory::CC_TPL_CAT && paraIdxInCat == (int)IDX_SECTIONS ) {
				
				if ( value.GetInteger() < 3 )
					return false;
					
				if ( value.GetInteger() == 3 )
					return true;
				
				if ( value.GetInteger()%2 != 0 )
					return false;
			}
			
			return true; 
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			// calculate polygon data points
			double steps  = 360.0 / getParameterNumValue(IDX_SECTIONS);
			double radius = getParameterNumValue(IDX_RADIUS);

			for (double i=0; i<360; i+=steps ) {
				double x = cos(i*PI/180) * radius;
				double y = sin(i*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
						
				getPolygonData(0).append(x, y);
			}
			
			drawPolygon(0, spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
			setTranslateX(getParameterNumValue(IDX_RADIUS) + 5.0);
			setTranslateY(getParameterNumValue(IDX_RADIUS) + 5.0);
			
			return spg.isOK();
		}
};

#endif