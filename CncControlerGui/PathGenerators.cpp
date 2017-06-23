#include "SVGElementConverter.h"
#include "PathGenerators.h"

///////////////////////////////////////////////////////////////////
CncPolygonPoints& PGenPolygon::getPolygonData(unsigned int polygonIndex) {
///////////////////////////////////////////////////////////////////
	// should not occur - only safty
	if ( polygonDataList.size() == 0 )
		appendNextPolygonPoints();
	
	// generate polygon points up to the given index
	if ( polygonIndex > polygonDataList.size() - 1 ) {
		unsigned int currentSize = polygonDataList.size();
		for ( unsigned int i=0; i<polygonIndex + 1 - currentSize; i++ ) {
			appendNextPolygonPoints();
		}
	}
	
	return polygonDataList.at(polygonIndex);
}

///////////////////////////////////////////////////////////////////
void PGenPolygon::getInternalInformation(wxTextCtrl* ctl) {
///////////////////////////////////////////////////////////////////
	wxASSERT(ctl);
	
	ctl->Clear();
	ctl->SetDefaultStyle(wxTextAttr(*wxWHITE));
	
	for ( auto it=polygonDataList.begin(); it!=polygonDataList.end(); ++it) {
		// data points
		std::stringstream ss;
		ss << *it << std::endl;
		
		// data points wkt style
		ctl->AppendText(ss.str().c_str());

		// final line feed
		ctl->AppendText("\n");
	}
}
///////////////////////////////////////////////////////////////////
const char* PGenPolygon::getDataPointsAsWktString(wxString& ret, unsigned int polygonIndex) {
///////////////////////////////////////////////////////////////////
	CncPolygonPoints cpp(getPolygonData(polygonIndex));
	ret.assign(cpp.getDataPointsAsWktString(ret));
	return ret;
}
///////////////////////////////////////////////////////////////////
bool PGenPolygon::centerPolygon(unsigned int polygonIndex) {
///////////////////////////////////////////////////////////////////
	wxRealPoint cp;
	determineCentroid(cp);
	
	if ( cnc::dblCompareNull(cp.x) == true && cnc::dblCompareNull(cp.x) == true )
		return true;
	
	CncPolygonPoints cpp(getPolygonData(polygonIndex));
	for (auto it = cpp.begin(); it != cpp.end(); ++it) {
		wxRealPoint op(CncPolygonPoints::convertToRealPoint(*it));
		wxRealPoint np = op - cp;
		
		*it = CncPolygonPoints::convertToIntPoint(np);
	}
	
	cpp.evaluateAdditionalValues();
	
	return true;
}
///////////////////////////////////////////////////////////////////
const wxRealPoint& PGenPolygon::getPolygonDataPoint(unsigned int polygonIndex, unsigned pointIndex) {
///////////////////////////////////////////////////////////////////
	CncPolygonPoints cpp(getPolygonData(polygonIndex));
	if ( pointIndex > cpp.size() -1 ) {
		addErrorInfo(wxString::Format("getPolygonDataPoint(): Invalid index: %d, current count: %d", pointIndex, getPolygonData().size()));
		static wxRealPoint p;
		return p;
	}
	
	return CncPolygonPoints::convertToRealPoint(getPolygonData().at(pointIndex));
}

///////////////////////////////////////////////////////////////////
int PGenPolygon::fillPolygonData(CncPolygonPoints& toFill, const wxString& data) {
///////////////////////////////////////////////////////////////////
	toFill.clear();
	
	wxString d(data);
	d.Replace("\\n", ";", true);
	d.Replace("\\t", "",  true);
	
	wxStringTokenizer lineTokenizer(d, ";");
	while ( lineTokenizer.HasMoreTokens() ) {
		wxString token = lineTokenizer.GetNextToken();
		
		if ( token.Matches("*,*") == true ) {
			wxStringTokenizer pointTokenizer(token, ",");
			wxString xv = pointTokenizer.GetNextToken();
			wxString yv = pointTokenizer.GetNextToken();
			
			wxRealPoint p;
			xv.ToDouble(&p.x);
			yv.ToDouble(&p.y);
			toFill.append(p);
		}
	}
	
	return toFill.size();
}
///////////////////////////////////////////////////////////////////
int PGenPolygon::fillPolygonData(unsigned int polygonIndex, const wxString& data) {
///////////////////////////////////////////////////////////////////
	return fillPolygonData(getPolygonData(polygonIndex), data);
}
///////////////////////////////////////////////////////////////////
const wxRealPoint& PGenPolygon::determineCentroid(wxRealPoint& cp, unsigned int polygonIndex) {
///////////////////////////////////////////////////////////////////
	try {
		typedef boost::geometry::model::d2::point_xy<double> point_type;
		typedef boost::geometry::model::polygon<point_type> polygon_type;

		polygon_type poly;
		wxString wkt;
		boost::geometry::read_wkt(getDataPointsAsWktString(wkt, polygonIndex), poly);

		point_type p(0.0, 0.0);
		boost::geometry::centroid(poly, p);
		
		cp.x = (cnc::dblCompare(p.x(), 0.0, 0.001) == true ? 0.0 : p.x());
		cp.y = (cnc::dblCompare(p.y(), 0.0, 0.001) == true ? 0.0 : p.y());
	}
	catch (boost::geometry::centroid_exception e) {
		addErrorInfo("determineCentroid(): Error while determine centroid");
		addErrorInfo(e.what());
	}
	catch (...) {
		addErrorInfo("determineCentroid(): Error while determine centroid");
	}
	return cp;
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::addPolyLine(unsigned int polylineIndex, SvgPathGroup& spg) {
///////////////////////////////////////////////////////////////////
	double offset = 0.0;
	
	// determine offset
	switch ( commonValues.getCorrectionType() ) {
		case CncCT_Inner:	offset = -commonValues.toolDiameter/2; break;
		case CncCT_Outer:	offset = +commonValues.toolDiameter/2; break;
		default:			offset = 0.0; break;
	}
	
	// consider input unit
	if ( inputUnit != mm )
		offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
	
	// perform offset
	CncClipperWrapper cw;
	CncPolygonPoints cpp(getPolygonData(polylineIndex));
	cw.correctEndPoints(cpp, offset);
	spoolPolygon(spg, cpp);
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::addPolygon(unsigned int polygonIndex, SvgPathGroup& spg, bool inlay) {
///////////////////////////////////////////////////////////////////
	double offset = 0.0;
	
	// determine offset
	switch ( commonValues.getCorrectionType() ) {
		case CncCT_Inner:	offset = -commonValues.toolDiameter/2; break;
		case CncCT_Outer:	offset = +commonValues.toolDiameter/2; break;
		default:			offset = 0.0; break;
	}
	
	// consider input unit
	if ( inputUnit != mm )
		offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
		
	CncClipperWrapper cw;
	CncPolygonPoints polygonToSpool;
 
	// first perform tool correction offset
	CncPolygons results;
	CncPolygonPoints cpp(getPolygonData(polygonIndex));
	cw.offsetPath(cpp, results, offset, commonValues.getCornerType()); 
	results.getPolygonPoints(0, polygonToSpool);
	
	polygonToSpool.closePolygon();
	
	// perform inlay
	if ( inlay == true ) {

		CncPolygonPoints reference = polygonToSpool;
		
		unsigned int cnt = 1;
		while ( true ) {
			// offset
			CncPolygons results;
			
			offset = -commonValues.toolDiameter;
			// consider input unit
			if ( inputUnit != mm )
				offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
				
			if ( cw.offsetPath(reference, results, offset * cnt, commonValues.getCornerType()) == false) { 
				std::cerr << "PGenPolygon::addPolygon: failed" << std::endl;
				return;
			}
			
			// nothing more to do
			if ( results.size() == 0 )
				break;
			
			// append
			polygonToSpool.append(results[0]);
			cnt++;
		}
	}
	
	spoolPolygon(spg, polygonToSpool);
}

///////////////////////////////////////////////////////////////////
void PGenPolygon::spoolPolygon(SvgPathGroup& spg, const CncPolygonPoints& dataPoints) {
///////////////////////////////////////////////////////////////////
	spg.pGen().add(dataPoints.getAsSvgPathRepresentation(getInputUnit(), false, ""));
	spg.add(spg.pGen().get());
}