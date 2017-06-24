#ifndef PGEN_ELLIPTICAL_RING_H
#define PGEN_ELLIPTICAL_RING_H

#include "PGenSvgElementPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenEllipticalRing : public PGenSvgElementPolygon {
	private:
		unsigned int IDX_OUTER_RADIUS_X;
		unsigned int IDX_OUTER_RADIUS_Y;
		unsigned int IDX_INNER_RADIUS_X;
		unsigned int IDX_INNER_RADIUS_Y;
		unsigned int IDX_REFPOINT;
		unsigned int IDX_INLAY;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			// outer
			SVGElementConverter::convertEllipseToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_OUTER_RADIUS_X)), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_OUTER_RADIUS_Y)), 
			                                              path);
			
			addHelpContructPath(spg, path);
			
			// inner
			path.clear();
			SVGElementConverter::convertEllipseToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_INNER_RADIUS_X)), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_INNER_RADIUS_Y)), 
			                                              path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Circle/Elliptical Ring";
			treePath 	= "Advanced Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Outer Radius X [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the outer x radius parameter of the elliptical arg.";
			IDX_OUTER_RADIUS_X = setupParameter(pi);
			
			pi.setupNum("Outer Radius Y [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the outer y radius parameter of the elliptical arg.";
			IDX_OUTER_RADIUS_Y = setupParameter(pi); 
			
			pi.setupNum("Inner Radius X [mm]", 15.0, 1.0, 400.0, 3);
			pi.help = "Determine the inner x radius parameter of the elliptical arg.";
			IDX_INNER_RADIUS_X = setupParameter(pi);
			
			pi.setupNum("Inner Radius Y [mm]", 15.0, 1.0, 400.0, 3);
			pi.help = "Determine the inner y radius parameter of the elliptical arg.";
			IDX_INNER_RADIUS_Y = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual unsigned int getElementCount() { return 2; }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return getParameterNumValue(IDX_OUTER_RADIUS_X) + 5.0; }
		virtual double getYOffset() { return getParameterNumValue(IDX_OUTER_RADIUS_Y) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString ellipse;
			ellipse.clear();
			
			switch (elementIndex) {
				// outer
				case 0:	SVGElementConverter::convertEllipseToPathData(0.0, 0.0, 
																	  getParameterNumValue(IDX_OUTER_RADIUS_X), 
																	  getParameterNumValue(IDX_OUTER_RADIUS_Y), 
																	  ellipse);
						ellipse.Prepend("<path d=\"");
						ellipse.Append("\"/>");
						break;
						
				// inner
				case 1:	SVGElementConverter::convertEllipseToPathData(0.0, 0.0, 
																	  getParameterNumValue(IDX_INNER_RADIUS_X), 
																	  getParameterNumValue(IDX_INNER_RADIUS_Y), 
																	  ellipse);
						ellipse.Prepend("<path d=\"");
						ellipse.Append("\"/>");
						break;
			}
			
			return ellipse;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenEllipticalRing() 
		: PGenSvgElementPolygon() 
		, IDX_OUTER_RADIUS_X(INT_MAX)
		, IDX_OUTER_RADIUS_Y(INT_MAX)
		, IDX_INNER_RADIUS_X(INT_MAX)
		, IDX_INNER_RADIUS_Y(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		, IDX_INLAY(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenEllipticalRing() {}
		
};

#endif