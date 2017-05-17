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
	ss << "Polyon::Data::Points: generic format\n";
	ss << "Original (x,y)        | Last Transformed (x,y)\n";
	
	for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
		ss << *it << "\n";
	}
	
	// data points wkt style
	ctl->AppendText(ss.str().c_str());
	ctl->AppendText("\nPolyon::Data::Points wkt format:\n");
	wxString wkt;
	ctl->AppendText(getDataPointsAsWktString(wkt));
	ctl->AppendText("\n\n");
	
	// min max positons
	ctl->AppendText(wxString::Format("Polyon::Data::Min (x,y): %10.3lf, %10.3lf\n", getMinPosition().x, getMinPosition().y));
	ctl->AppendText(wxString::Format("Polyon::Data::Max (x,y): %10.3lf, %10.3lf\n", getMaxPosition().x, getMaxPosition().y));
	
	// final line feed
	ctl->AppendText("\n");
}
///////////////////////////////////////////////////////////////////
const char* PGenPolygon::getDataPointsAsWktString(wxString& ret) {
///////////////////////////////////////////////////////////////////
	ret.assign("POLYGON((");
	
	unsigned int cnt = 0;
	PathFragmentPolygonData::iterator it;
	for ( it = polygonData.begin(); it != polygonData.end(); ++it ) {
		if ( cnt == 0 )	ret.Append(wxString::Format("%.3lf %.3lf", it->getTransformedPoint().x, it->getTransformedPoint().y));
		else			ret.Append(wxString::Format(",%.3lf %.3lf", it->getTransformedPoint().x, it->getTransformedPoint().y));
		cnt++;
	}
	
	// append first point at the end too
	it = polygonData.begin();
	ret.Append(wxString::Format(",%.3lf %.3lf", it->getTransformedPoint().x, it->getTransformedPoint().y));
	
	ret.Append("))");
	return ret;
}
///////////////////////////////////////////////////////////////////
bool PGenPolygon::centerPolygon() {
///////////////////////////////////////////////////////////////////
	wxRealPoint cp;
	determineCentroid(cp);
	
	if ( cnc::dblCompareNull(cp.x) == true && cnc::dblCompareNull(cp.x) == true )
		return true;
		
	for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
		wxRealPoint op(it->getOriginalPoint());
		wxRealPoint np = op - cp;
		
		changeDataPoint(it, np);
	}
	
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
	
	return polygonData.at(index).getTransformedPoint();
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
			appendPolygonPoint(p);
		}
	}
	
	return polygonData.size();
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::appendPolygonPoint(const wxRealPoint& p) {
///////////////////////////////////////////////////////////////////
	if ( polygonData.size() == 0 ) {
		pMin = p;
		pMax = p;
	} else {
		if ( p.x < pMin.x ) pMin.x = p.x;
		if ( p.y < pMin.y ) pMin.y = p.y;
		if ( p.x > pMax.x ) pMax.x = p.x;
		if ( p.y > pMax.y ) pMax.y = p.y;
	}
	
	polygonData.push_back(p);
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::changeDataPoint(PathFragmentPolygonData::iterator it, const wxRealPoint& p) {
///////////////////////////////////////////////////////////////////
	if ( p.x < pMin.x ) pMin.x = p.x;
	if ( p.y < pMin.y ) pMin.y = p.y;
	if ( p.x > pMax.x ) pMax.x = p.x;
	if ( p.y > pMax.y ) pMax.y = p.y;
				
	it->setOriginalPoint(p);
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::transformPolygon(SVGTransformer& t) {
///////////////////////////////////////////////////////////////////
	for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
		it->transform(t);
	}
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::transformPolygonAgain(SVGTransformer& t) {
///////////////////////////////////////////////////////////////////
	for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
		it->transformAgain(t);
	}
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
void PGenPolygon::addPolygon(SvgPathGroup& spg, bool inlay) {
///////////////////////////////////////////////////////////////////
	PathFragmentPolygonData result;
	
	// perform first correction on demand
	if ( commonValues.toolCorrection == true ) {
		correctPolygon(spg, commonValues.toolDiameter/2, true, result);
		
	} else {
		// copy the current polygon data points to the result vector
		for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
			result.push_back(it->getTransformedPoint());
		}
	}
	
	// perform inlay
	if ( inlay == true ) {
		correctPolygon(spg, commonValues.toolDiameter, false, result);
	}
	
	spoolPolygon(spg, result);
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::correctPolygon(SvgPathGroup& spg, double value, bool once, PathFragmentPolygonData& result) {
///////////////////////////////////////////////////////////////////
	wxRealPoint cp;
	determineCentroid(cp);
	
	bool anythingTransformed = false;
	do {
		anythingTransformed = false;
		// over all data points
		for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
			wxRealPoint p(it->getTransformedPoint());
			wxRealPoint np(p);
			
			double dx = cp.x - p.x;
			double dy = cp.y - p.y;
			double fx = (p.x < 0.0 ? -1.0 : 1.0);
			double fy = (p.y < 0.0 ? -1.0 : 1.0);
			
			// nothing to do
			if ( cnc::dblCompareNull(dx)  && cnc::dblCompareNull(dy) ) {
				continue;
			} 
			
			// determine vector length
			double l = DBL_MAX;
			if ( cnc::dblCompareNull(dx) ) 			l = abs(dy);
			else if ( cnc::dblCompareNull(dy) ) 	l = abs(dx);
			else 									l = sqrt(pow(dx, 2) + pow(dy, 2));
			
			// check current distance and update data point
			double nl = l - value;
			if ( nl > value/2 ) {
				np.x = (nl/l) * abs(dx) * fx;
				np.y = (nl/l) * abs(dy) * fy;
				
				anythingTransformed = true;
				it->setTransformedPoint(np);
				result.push_back(np);
			} else {
				it->setTransformedPoint(cp);
				result.push_back(cp);
			}
			
			// debug only
			if ( false ) {
				clog << "cp: "<< cp << " | p:" << p;
				clog << wxString::Format(" | dx: %10.3lf", dx)   << wxString::Format(", dy: %10.3lf", dy);
				clog << wxString::Format(" | l: %10.3lf", l)     << wxString::Format(", nl: %10.3lf", nl);
				clog << " | np: "<< np;
				clog << "\n";
			}
		}
		
		// add the first polygon data points again to close the polygon
		result.push_back(getFirstPolygonDataPoint());
		
		// break if only one correction is requested
		if ( once == true )
			break;
		
	// check if min one point is transformed
	} while ( anythingTransformed == true );
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::spoolPolygon(SvgPathGroup& spg, const PathFragmentPolygonData& dataPoints) {
///////////////////////////////////////////////////////////////////
	bool asPath = (commonValues.outputType == 0);

	// generate svg polygon
	if ( asPath == false ) {
		spg.add(spg.fGen().addPolygon(dataPoints));
		return;
	}
	
	// generate svg path
	wxString pd, path;
	for (PathFragmentPolygonData::const_iterator it = dataPoints.begin(); it != dataPoints.end(); ++it) {
		if ( cnc::dblCompareNull(it->getTransformedPoint().x) && cnc::dblCompareNull(it->getTransformedPoint().y) )
			continue;
			
		pd.append(wxString::Format(" %.3lf,%.3lf", SvgPathFragment::convertToDouble(mm, it->getTransformedPoint().x), 
		                                           SvgPathFragment::convertToDouble(mm, it->getTransformedPoint().y)));
	}
	
	// convert and add
	SVGElementConverter::convertPolygonToPathData(pd, path);
	spg.pGen().add(path);
	spg.add(spg.pGen().get());
}