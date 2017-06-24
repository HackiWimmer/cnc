#ifndef PGEN_SVGELEMENT_POLYGON_H
#define PGEN_SVGELEMENT_POLYGON_H

#include "PGenPolygon.h"

///////////////////////////////////////////////////////////////////////////
class PGenSvgElementPolygon : public PGenPolygon {
	private:
		unsigned int elementCount;
		PolygonList origPoligonList;
		
	protected:
		wxRealPoint centroid;
		
		///////////////////////////////////////////////////////////////////
		PGenSvgElementPolygon() 
		: PGenPolygon()
		, elementCount(1)
		, centroid(0.0, 0.0)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSvgElementPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) = 0;
		virtual double getXOffset() = 0;
		virtual double getYOffset() = 0;
		virtual bool getInlayMode() = 0;
		virtual unsigned int getElementCount() { return 1; }
		
		///////////////////////////////////////////////////////////////////
		const CncPolygonPoints& getOriginalPolygon(unsigned int idx=0) {
			static CncPolygonPoints defaultPolygon;
			
			if ( idx > origPoligonList.size() -1 ) 
				return defaultPolygon;
				
			return origPoligonList.at(idx);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			origPoligonList.clear();
			
			for ( unsigned int i=0; i<getElementCount(); i++) {
				clearPolygonData(i);
				
				SVGPathAssistant pa;
				pa.setCurveLibResolution(commonValues.curveLibResolution);
				
				if ( pa.processSvgNode(getElementAsSvgPath(i)) == false ) {
					addErrorInfo("processSvgNode failed");
					return false;
				}
				
				CncPathListManager plm = pa.getPathList();
				for ( auto it=plm.begin(); it!=plm.end(); ++it )
					getPolygonData(i).append(CncPolygonPoints::convertToIntPoint(it->abs));
					
				plm.getCentroid(centroid);
				
				origPoligonList.push_back(getPolygonData(i));
				addPolygon(i, spg, getInlayMode());
			}
			
			setTranslateX(getXOffset());
			setTranslateY(getYOffset());
			
			return spg.isOK();
		}
};

#endif