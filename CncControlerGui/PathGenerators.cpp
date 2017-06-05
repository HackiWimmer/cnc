#include "SVGElementConverter.h"
#include "PathGenerators.h"

///////////////////////////////////////////////////////////////////
void PGenPolygon::getInternalInformation(wxTextCtrl* ctl) {
///////////////////////////////////////////////////////////////////
	wxASSERT(ctl);
	
	ctl->Clear();
	ctl->SetDefaultStyle(wxTextAttr(*wxWHITE));
	
	// data points
	std::stringstream ss;
	ss << polygonData << std::endl;
	
	// data points wkt style
	ctl->AppendText(ss.str().c_str());

	// final line feed
	ctl->AppendText("\n");
}
///////////////////////////////////////////////////////////////////
const char* PGenPolygon::getDataPointsAsWktString(wxString& ret) {
///////////////////////////////////////////////////////////////////
	ret.assign(polygonData.getDataPointsAsWktString(ret));
	return ret;
}
///////////////////////////////////////////////////////////////////
bool PGenPolygon::centerPolygon() {
///////////////////////////////////////////////////////////////////
	wxRealPoint cp;
	determineCentroid(cp);
	
	if ( cnc::dblCompareNull(cp.x) == true && cnc::dblCompareNull(cp.x) == true )
		return true;
		
	for (auto it = polygonData.begin(); it != polygonData.end(); ++it) {
		wxRealPoint op(CncPolygonPoints::convertToRealPoint(*it));
		wxRealPoint np = op - cp;
		
		*it = CncPolygonPoints::convertToIntPoint(np);
	}
	
	polygonData.evaluateAdditionalValues();
	
	return true;
}
///////////////////////////////////////////////////////////////////
const wxRealPoint& PGenPolygon::getPolygonDataPoint(unsigned index) {
///////////////////////////////////////////////////////////////////
	if ( index > polygonData.size() -1 ) {
		addErrorInfo(wxString::Format("getPolygonDataPoint(): Invalid index: %d, current count: %d", index, polygonData.size()));
		static wxRealPoint p;
		return p;
	}
	
	return CncPolygonPoints::convertToRealPoint(polygonData.at(index));
}
///////////////////////////////////////////////////////////////////
int PGenPolygon::fillPolygonData(const wxString& data) {
///////////////////////////////////////////////////////////////////
	polygonData.clear();
	
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
			polygonData.append(p);
		}
	}
	
	return polygonData.size();
}
///////////////////////////////////////////////////////////////////
const wxRealPoint& PGenPolygon::determineCentroid(wxRealPoint& cp) {
///////////////////////////////////////////////////////////////////
	try {
		typedef boost::geometry::model::d2::point_xy<double> point_type;
		typedef boost::geometry::model::polygon<point_type> polygon_type;

		polygon_type poly;
		wxString wkt;
		boost::geometry::read_wkt(getDataPointsAsWktString(wkt), poly);

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
void PGenPolygon::addPolyLine(SvgPathGroup& spg) {
///////////////////////////////////////////////////////////////////
	double offset = 0.0;
	
	switch ( commonValues.getCorrectionType() ) {
		case CncCT_Inner:	offset = -commonValues.toolDiameter/2; break;
		case CncCT_Outer:	offset = +commonValues.toolDiameter/2; break;
		default:			offset = 0.0; break;
	}
	
	CncClipperWrapper cw;
	cw.correctEndPoints(polygonData, offset);
	spoolPolygon(spg, polygonData);
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::addPolygon(SvgPathGroup& spg, bool inlay) {
///////////////////////////////////////////////////////////////////
	double offset = 0.0;
	
	switch ( commonValues.getCorrectionType() ) {
		case CncCT_Inner:	offset = -commonValues.toolDiameter/2; break;
		case CncCT_Outer:	offset = +commonValues.toolDiameter/2; break;
		default:			offset = 0.0; break;
	}
	
	CncClipperWrapper cw;
	CncPolygonPoints polygonToSpool;
 
	// first perform tool correction offset
	CncPolygons results;
	cw.offsetPath(polygonData, results, offset, commonValues.getCornerType()); 
	results.getPolygonPoints(0, polygonToSpool);
	
	polygonToSpool.closePolygon();
	
	// perform inlay
	if ( inlay == true ) {

		CncPolygonPoints reference = polygonToSpool;
		
		unsigned int cnt = 1;
		while ( true ) {
			// offset
			CncPolygons results;
			if ( cw.offsetPath(reference, results, -commonValues.toolDiameter * cnt, commonValues.getCornerType()) == false) { 
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