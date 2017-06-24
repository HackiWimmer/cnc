#ifndef PGEN_RECTANGLE_POLYGON_H
#define PGEN_RECTANGLE_POLYGON_H

#include "PGenSvgElementPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenRectanglePolygon : public PGenSvgElementPolygon {
	private:
		unsigned int IDX_WIDTH;
		unsigned int IDX_HEIGHT;
		unsigned int IDX_CORNER_STYLE;
		unsigned int IDX_REFPOINT;
		unsigned int IDX_INLAY;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			
			// square corners
			double rx = 0.0, ry = 0.0;
			if ( getParameterEnumValue(IDX_CORNER_STYLE) == 0 ) {
				//round corners
				rx = ry = commonValues.toolDiameter/2;
			}
			
			SVGElementConverter::convertRectToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                           SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                           SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_WIDTH)), 
			                                           SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_HEIGHT)), 
			                                           rx,
			                                           ry,
			                                           path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Rectangle";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Width [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine rect width (x axis).";
			IDX_WIDTH = setupParameter(pi);
			
			pi.setupNum("Height [mm]", 10.0, 1.0, 400.0, 3);
			pi.help = "Determine rect height (y axis).";
			IDX_HEIGHT = setupParameter(pi);
			
			pi.setupEnum("Corners Style", "round;square", 0);
			pi.help = "Determine corner style.";
			IDX_CORNER_STYLE = setupParameter(pi);
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupReferencePoint(pi, "center/center;left/center;left/top;left/bottom", 1);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			switch ( getParameterEnumValue(IDX_REFPOINT) ) {
				// center/center
				case 0: 	determineReferencePoint(spg, -getParameterNumValue(IDX_WIDTH)/2, -getParameterNumValue(IDX_HEIGHT)/2);
							break;
				// left/center
				case 1: 	determineReferencePoint(spg, 0.0, -getParameterNumValue(IDX_HEIGHT)/2);
							break;
				// left/top
				case 2:		determineReferencePoint(spg, 0.0, 0.0);
							break;
				// left bootom
				case 3:		determineReferencePoint(spg, 0.0, -getParameterNumValue(IDX_HEIGHT));
							break;
							
				default:	addErrorInfo(wxString::Format("setReferencePoint: invalid enum: %d", getParameterEnumValue(IDX_REFPOINT)));
							return false;
			}
			
			return true;
		}

		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return getParameterNumValue(IDX_WIDTH) + 5.0; }
		virtual double getYOffset() { return getParameterNumValue(IDX_HEIGHT) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString rect;
			rect.clear();
			
			// square corners
			double rx = 0.0, ry = 0.0;
			if ( getParameterEnumValue(IDX_CORNER_STYLE) == 0 ) {
				//round corners
				rx = ry = commonValues.toolDiameter/2;
			}
			
			// ref point
			double x = 0.0, y = 0.0;
			if ( isReferencePointDefined() ) {
				x = getReferencePoint().x;
				y = getReferencePoint().y;
			}
			
			SVGElementConverter::convertRectToPathData(x, 
			                                           y, 
			                                           getParameterNumValue(IDX_WIDTH), 
			                                           getParameterNumValue(IDX_HEIGHT), 
			                                           rx,
			                                           ry,
			                                           rect);
			rect.Prepend("<path d=\"");
			rect.Append("\"/>");
			
			return rect;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRectanglePolygon() 
		: PGenSvgElementPolygon() 
		, IDX_WIDTH(INT_MAX)
		, IDX_HEIGHT(INT_MAX)
		, IDX_CORNER_STYLE(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		, IDX_INLAY(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenRectanglePolygon() {}
		
};

#endif