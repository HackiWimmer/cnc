#ifndef PGEN_POLYGON_H
#define PGEN_POLYGON_H

#include "PathGeneratorBase.h"

typedef std::vector<CncPolygonPoints> PolygonList;
///////////////////////////////////////////////////////////////////////////
class PGenPolygon : public PathGeneratorBase {
	private:
		PolygonList polygonDataList;
		
	protected:
		
		///////////////////////////////////////////////////////////////////
		PGenPolygon() 
		: PathGeneratorBase()
		{
			// initialize default polygon points - index = 0
			appendNextPolygonPoints();
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenPolygon() {
			for ( auto it=polygonDataList.begin(); it!=polygonDataList.end(); ++it)
				it->clear();
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int appendNextPolygonPoints() {
			CncPolygonPoints defaultPolygonPoints;
			polygonDataList.push_back(defaultPolygonPoints);
			
			return polygonDataList.size() - 1;
		}
		
		CncPolygonPoints& getPolygonData(unsigned int polygonIndex=0);
		const CncPolygonPoints& getPolygonDataConst(unsigned int polygonIndex=0);
		
		unsigned int getDataPointCount(unsigned int polygonIndex=0) { return getPolygonData(polygonIndex).size(); }
		virtual void getInternalInformation(wxTextCtrl* ctl);
		const char* getDataPointsAsWktString(wxString& ret, unsigned int polygonIndex=0);
		const wxRealPoint& getMinPosition(unsigned int polygonIndex=0) { return getPolygonData(polygonIndex).getMinRealPos(); }
		const wxRealPoint& getMaxPosition(unsigned int polygonIndex=0) { return getPolygonData(polygonIndex).getMaxRealPos(); }
		
		bool centerPolygon(unsigned int polygonIndex=0);
		const wxRealPoint& determineCentroid(wxRealPoint& cp, unsigned int polygonIndex=0);
		
		const wxRealPoint& getPolygonDataPoint(unsigned int polygonIndex, unsigned pointIndex);
		const wxRealPoint& getFirstPolygonDataPoint(unsigned int polygonIndex=0) { return getPolygonDataPoint(polygonIndex, 0); }
		const wxRealPoint& getSecondPolygonDataPoint(unsigned int polygonIndex=0) { return getPolygonDataPoint(polygonIndex, 1); }
		const wxRealPoint& getLastPolygonDataPoint(unsigned int polygonIndex=0) { return getPolygonDataPoint(polygonIndex, getPolygonData(polygonIndex).size() - 1); }
		
		void clearPolygonData(unsigned int polygonIndex=0) { getPolygonData(polygonIndex).clear(); }
		int fillPolygonData(CncPolygonPoints& toFill, const wxString& data);
		int fillPolygonData(unsigned int polygonIndex, const wxString& data);
		void addPolygon(unsigned int polygonIndex, SvgPathGroup& spg, bool inlay=false);
		void addPolyLine(unsigned int polylineIndex, SvgPathGroup& spg);
		
		void spoolPolygon(SvgPathGroup& spg, const CncPolygonPoints& dataPoints);
};

#endif