#ifndef PGEN_ELLIPTICALARC_POLYGON_H
#define PGEN_ELLIPTICALARC_POLYGON_H

#include "SVGPathAssistant.h"
#include "PGenSvgElementPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenEllipticalArcPolygon : public PGenSvgElementPolygon {
	private:
		unsigned int IDX_RADIUS_X;
		unsigned int IDX_RADIUS_Y;
		unsigned int IDX_REFPOINT;
		unsigned int IDX_INLAY;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertEllipseToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_RADIUS_X)), 
														  SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_RADIUS_Y)), 
			                                              path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Circle/Elliptical ARC";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Radius X [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the x radius parameter of the elliptical arg.";
			IDX_RADIUS_X = setupParameter(pi);
			
			pi.setupNum("Radius Y [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the y radius parameter of the elliptical arg.";
			IDX_RADIUS_Y = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return getParameterNumValue(IDX_RADIUS_X) + 5.0; }
		virtual double getYOffset() { return getParameterNumValue(IDX_RADIUS_Y) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString ellipse;
			ellipse.clear();
			SVGElementConverter::convertEllipseToPathData(0.0, 0.0, 
			                                              getParameterNumValue(IDX_RADIUS_X), 
			                                              getParameterNumValue(IDX_RADIUS_Y), 
			                                              ellipse);
			ellipse.Prepend("<path d=\"");
			ellipse.Append("\"/>");
			
			return ellipse;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenEllipticalArcPolygon() 
		: PGenSvgElementPolygon() 
		, IDX_RADIUS_X(INT_MAX)
		, IDX_RADIUS_Y(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		, IDX_INLAY(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenEllipticalArcPolygon() {}
		
};

#endif