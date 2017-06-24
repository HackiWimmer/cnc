#ifndef PGEN_FREEHANDPOLYGON_H
#define PGEN_FREEHANDPOLYGON_H

#include "PGenPolygon.h"
#include "SVGElementConverter.h"

///////////////////////////////////////////////////////////////////////////
class PGenFreehandPolygon : public PGenPolygon {
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		CncPolygonPoints origPolygonData;
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolygonToPathData(origPolygonData, mm, path);
			addHelpContructPath(spg, path);
		}
		
	protected:
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name		= "Freehand";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupString("Data [mm]", "10.0,10.0\\n10.0,100.0\\n55.0,55.0", true);
			IDX_DATA = setupParameter(pi);

			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
	
		///////////////////////////////////////////////////////////////////
		PGenFreehandPolygon() 
		: PGenPolygon() 
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenFreehandPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			fillPolygonData(0, getParameterStringValue(IDX_DATA));
			if ( getDataPointCount() < 3 ) {
				addErrorInfo(wxString::Format("generate(): Not enough data points available: Current count: %d", getDataPointCount()));
				return false;
			}
			
			centerPolygon();
			origPolygonData = getPolygonData(0);
			addPolygon(0, spg, (getParameterEnumValue(IDX_INLAY) == 1));

			setTranslateX(abs(getMinPosition().x) + 5.0);
			setTranslateY(abs(getMinPosition().y) + 5.0);
			return spg.isOK();
		}
};

#endif