#ifndef PGEN_SIMPLELINE_H
#define PGEN_SIMPLELINE_H

#include "PGenPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenSimpleLine : public PGenPolygon {
	private:
		unsigned int IDX_X1;
		unsigned int IDX_Y1;
		unsigned int IDX_X2;
		unsigned int IDX_Y2;
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
			name 		= "Simple Line";
			treePath 	= "Polylines [mm]";
			
			commonValues.canJointType = false;
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupNum("X1 [mm]", 0.0, 0.0, 500.0, 3);
			pi.help = "Start position X";
			IDX_X1 = setupParameter(pi); 
			
			pi.setupNum("Y1 [mm]", 0.0, 0.0, 500.0, 3);
			pi.help = "Start position Y";
			IDX_Y1 = setupParameter(pi); 

			pi.setupNum("X2 [mm]", 60.0, 0.0, 500.0, 3);
			pi.help = "End position X";
			IDX_X2 = setupParameter(pi); 
			
			pi.setupNum("Y2 [mm]", 70.0, 0.0, 500.0, 3);
			pi.help = "End position Y";
			IDX_Y2 = setupParameter(pi); 
			
			IDX_REFPOINT = setupReferencePoint(pi,"start");
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenSimpleLine() 
		: PGenPolygon() 
		, IDX_X1(INT_MAX)
		, IDX_Y1(INT_MAX)
		, IDX_X2(INT_MAX)
		, IDX_Y2(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSimpleLine() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			getPolygonData().append(getParameterNumValue(IDX_X1), getParameterNumValue(IDX_Y1));
			getPolygonData().append(getParameterNumValue(IDX_X2), getParameterNumValue(IDX_Y2));
			origPolygonData = getPolygonData();
			
			drawPolyLine(0, spg);
			
			setTranslateX(abs(getMinPosition().x) + 5.0);
			setTranslateY(abs(getMinPosition().y) + 5.0);
			
			return spg.isOK();
		}
};

#endif