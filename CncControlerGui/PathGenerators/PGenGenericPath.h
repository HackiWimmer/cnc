#ifndef PGEN_GENERIC_PATH_H
#define PGEN_GENERIC_PATH_H

#include "PGenSvgElementPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenGenericPath : public PGenSvgElementPolygon {
	
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
			name 		= "SVG Path";
			treePath 	= "Generic [px]";
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupString("Path d [px]", "M348.000,257.000 L 364.000,148.000 362.000,148.000 326.000,241.000 295.000,219.000 258.000,88.000 440.000,129.000 370.000,196.000 372.000,275.000 z", true);
			IDX_DATA = setupParameter(pi); 
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initPreDefinedParameterSetups() {
			PreDefParameterMap map;
			map[IDX_DATA] 			=  wxVariant("M100,200 C100,100 250,100 250,200 S400,300 400,200");
			addPreDefinedParameterSetup("Cubic Bezier", map);
			
			map.clear();
			map[IDX_DATA] 			= wxVariant("M400,400 Q400,50 600,300 T100,300");
			addPreDefinedParameterSetup("Quadratic Bezier", map);
		}
		
		///////////////////////////////////////////////////////////////////
		//virtual double getXOffset() { return + 5.0; }
		//virtual double getYOffset() { return + 5.0; }
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
		
			path.Prepend("<path d=\"");
			path.Append("\"/>");
			
			return path;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenGenericPath() 
		: PGenSvgElementPolygon()
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{
			setInputUnit(px);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenGenericPath() {}
		
};

#endif