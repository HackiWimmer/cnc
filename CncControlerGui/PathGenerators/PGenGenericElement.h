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