#ifndef PGEN_FREEHAND_POLYLINE_H
#define PGEN_FREEHAND_POLYLINE_H

#include "PGenPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenFreehandPolyline : public PGenPolygon {
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_REFPOINT;
		CncPolygonPoints origPolygonData;
		
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolylineToPathData(origPolygonData, mm, path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name		= "Freehand";
			treePath 	= "Polylines [mm]";
			
			commonValues.canJointType = false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupString("Data [mm]", "0.0,0.0\\n10.0,100.0\\n55.0,55.0\\n95.0,95.0", true);
			IDX_DATA= setupParameter(pi);
			
			IDX_REFPOINT = setupReferencePoint(pi,"start");
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
	
		///////////////////////////////////////////////////////////////////
		PGenFreehandPolyline() 
		: PGenPolygon() 
		, IDX_DATA(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenFreehandPolyline() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			const unsigned int polygonIndex = 0;
			fillPolygonData(polygonIndex, getParameterStringValue(IDX_DATA));
			origPolygonData = getPolygonData(polygonIndex);
			addPolyLine(polygonIndex, spg);

			setTranslateX(abs(getMinPosition().x) + 5.0);
			setTranslateY(abs(getMinPosition().y) + 5.0);
			return spg.isOK();
		}
};

#endif