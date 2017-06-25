#ifndef PGEN_GENERIC_ELEMENT_H
#define PGEN_GENERIC_ELEMENT_H

#include "PGenSvgElementPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenGenericElement : public PGenSvgElementPolygon {
	
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
	
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolylineToPathData(getOriginalPolygon(), px, path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "SVG Element";
			treePath 	= "Generic [px]";
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupString("<Element ...> [px]", "<circle cx=\"0\" cy=\"0\" r=\"47\"/>", true);
			IDX_DATA = setupParameter(pi); 
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initPreDefinedParameterSetups() {
			PreDefParameterMap map;
			map[IDX_DATA] 			=  wxVariant("<line x1=\"100\" y1=\"0\" x2=\"200\" y2=\"0\"/>");
			addPreDefinedParameterSetup("Line", map);
			
			map.clear();
			map[IDX_DATA] 			= wxVariant("<rect x=\"50\" y=\"50\" width=\"100\" height=\"100\"/>");
			addPreDefinedParameterSetup("Regtangle", map);
			
			map.clear();
			map[IDX_DATA] 			= wxVariant("<circle cx=\"50\" cy=\"40\" r=\"20\" />");
			addPreDefinedParameterSetup("Circle", map);
			
			map.clear();
			map[IDX_DATA] 			= wxVariant("<ellipse cx=\"50\" cy=\"50\" rx=\"40\" ry=\"30\" />");
			addPreDefinedParameterSetup("Ellipse", map);
			
			map.clear();
			map[IDX_DATA] 			= wxVariant("<polyline points=\"100,100 150,150 200,100 250,150\"/>");
			addPreDefinedParameterSetup("Polyline", map);
			
			map.clear();
			map[IDX_DATA] 			= wxVariant("<polygon points=\"100 100 150 150 200 100\"/>");
			addPreDefinedParameterSetup("Polygon", map);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(getPolygonData().getMinRealPos().x)) + 5.0; }
		virtual double getYOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(getPolygonData().getMinRealPos().y)) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString path;
			path.assign(getParameterStringValue(IDX_DATA));
			return path;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenGenericElement() 
		: PGenSvgElementPolygon()
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{
			setInputUnit(px);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenGenericElement() {}
		
};

#endif