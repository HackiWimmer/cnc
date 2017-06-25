#include <list>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "PathGenerators/PGenPolygon.h"

///////////////////////////////////////////////////////////////////
const CncPolygonPoints& PGenPolygon::getPolygonDataConst(unsigned int polygonIndex) {
///////////////////////////////////////////////////////////////////
	return (const CncPolygonPoints&)getPolygonData(polygonIndex);
}
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
	ret.assign(getPolygonData(polygonIndex).getDataPointsAsWktString(ret));
	return ret;
}
///////////////////////////////////////////////////////////////////
bool PGenPolygon::centerPolygon(unsigned int polygonIndex) {
///////////////////////////////////////////////////////////////////
	wxRealPoint cp;
	determineCentroid(cp);
	
	if ( cnc::dblCompareNull(cp.x) == true && cnc::dblCompareNull(cp.x) == true )
		return true;
	
	for (auto it = getPolygonData(polygonIndex).begin(); it != getPolygonData(polygonIndex).end(); ++it) {
		wxRealPoint op(CncPolygonPoints::convertToRealPoint(*it));
		wxRealPoint np = op - cp;
		
		*it = CncPolygonPoints::convertToIntPoint(np);
	}
	
	getPolygonData(polygonIndex).evaluateAdditionalValues();
	
	return true;
}
///////////////////////////////////////////////////////////////////
const wxRealPoint& PGenPolygon::getPolygonDataPoint(unsigned int polygonIndex, unsigned pointIndex) {
///////////////////////////////////////////////////////////////////
	if ( pointIndex > getPolygonData(polygonIndex).size() -1 ) {
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
int PGenPolygon::appendHoles(PolygonList& pl) {
///////////////////////////////////////////////////////////////////
	if ( pl.size() == 0 )
		return 0;
				
	// determine the outer direction
	CncDirection outerDir = pl[0].getOrientation();
	
	int ret = 0;
	for ( unsigned int i=0; i<getPolygonCount(); i++) {
		if ( isHole(i) ) {
			CncPolygons results;
			performToolCorrection(i, results);
			
			// over all results
			for ( auto it=results.begin(); it !=results.end(); ++it ) {
				CncPolygonPoints pp = *it;
			
				// check and perform orientation, always reverse due to the outer dir 
				if ( pp.getOrientation() == outerDir )
					pp.reverseOrientation();
					
				// finaly close the polygon - on demand
				pp.closePolygon();
				pl.push_back(pp);
			}
			
			// return the amount of added holes
			ret  += results.size(); 
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
double PGenPolygon::getToolCorrectionOffset(unsigned int polygonIndex) { 
///////////////////////////////////////////////////////////////////
	double fact = ( isHole(polygonIndex) ? -1.0 : 1.0 );
	return fact * commonValues.toolDiameter/2; 
}
///////////////////////////////////////////////////////////////////
int PGenPolygon::performToolCorrection(unsigned int polygonIndex, CncPolygons& results) {
///////////////////////////////////////////////////////////////////
	double offset = 0.0;
	
	// determine offset
	switch ( commonValues.getCorrectionType() ) {
		case CncCT_Inner:	offset = -getToolCorrectionOffset(polygonIndex); break;
		case CncCT_Outer:	offset = +getToolCorrectionOffset(polygonIndex); break;
		default:			offset = 0.0; break;
	}
	
	// consider input unit
	if ( inputUnit != mm )
		offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
		
	// perform tool correction offset
	CncPolygonPoints pp(getPolygonData(polygonIndex));
	if ( pp.isPolygonClosed() ) {
		
		if ( cnc::dblCompareNull(offset) == false ) {
			CncClipperWrapper cw;
			cw.offsetPath(pp, results, offset, commonValues.getCornerType()); 
		} else {
			results.push_back(pp);
		}
		
	} else {
		CncClipperWrapper cw;
		if ( cnc::dblCompareNull(offset) == false )
			cw.correctEndPoints(pp, offset);
			
		results.push_back(pp);
	}
	
	return results.size();
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::drawPolyLine(unsigned int polylineIndex, SvgPathGroup& spg) {
///////////////////////////////////////////////////////////////////
	// first perform tool correction offset
	CncPolygons results;
	performToolCorrection(polylineIndex, results);

	// over all results
	for ( auto it=results.begin(); it !=results.end(); ++it )
		spoolPolygon(spg, *it);
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::drawPolygon(unsigned int polygonIndex, SvgPathGroup& spg, bool inlay) {
///////////////////////////////////////////////////////////////////
	// first perform tool correction offset
	CncPolygons results;
	performToolCorrection(polygonIndex, results);
	
	// over all results
	for ( auto it=results.begin(); it !=results.end(); ++it ) {
		CncPolygonPoints polygon = *it;
		polygon.closePolygon();

		// perform inlay
		if ( inlay == true && isHole(polygonIndex) == false ) {
			CncPolygonPoints polygonToSpool;
			if ( inlayPolygonB(spg, polygon, polygonToSpool, 1) == false ) {
				addErrorInfo("PGenPolygon::drawPolygon: inlayPolygon failed");
			}
		} else {
			spoolPolygon(spg, polygon);
		}
	}
}
///////////////////////////////////////////////////////////////////
bool PGenPolygon::inlayPolygonA(SvgPathGroup& spg, 
							    CncPolygonPoints& polygonInData, 
							    CncPolygonPoints& polygonToSpool, 
							    unsigned int callDepth) {
///////////////////////////////////////////////////////////////////
	static const int maxCallDepth = 32;
	
	CncClipperWrapper cw;
	
	// first spool the given polygon
	polygonInData.closePolygon();
	clog << "D:"<< wxString(polygonInData.getAsSvgPathRepresentation(mm, false)).SubString(0,40) << endl;
	spoolPolygon(spg, polygonInData);
	
	// prepare the path input incl. the corresponding holes
	PolygonList pl;
	pl.push_back(polygonInData);
	appendHoles(pl);
		
	// determine offset
	double offset = -commonValues.toolDiameter;
	// consider input unit
	if ( inputUnit != mm )
		offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
		
	// perform the offseting
	CncPolygons results;
	if ( cw.offsetPath(pl, results, offset, commonValues.getCornerType()) == false) { 
		addErrorInfo("PGenPolygon::inlayPolygon: offset path failed");
		return false;
	}
	
	clog << callDepth << ":" << endl;
	results.trace(clog);
	
	// generate the output 
	if ( results.size() == 0 ) {
		clog << "0:"<< wxString(polygonToSpool.getAsSvgPathRepresentation(mm, false)).SubString(0,40) << endl;
		spoolPolygon(spg, polygonToSpool);
		return true;
	}
	
	
	
	// append the result and inlay again (recusive call)
	if ( results.size() == 1 ) {
		// append
		CncPolygonPoints pp(results[0]);
		pp.closePolygon();
		polygonToSpool.append(pp);
		
		// check
		if ( ++callDepth > maxCallDepth ) {
			addErrorInfo("PGenPolygon::inlayPolygon: Max call depth reached (type B)");
			return false;
		}
		
		// recursive call
		bool ret = inlayPolygonA(spg, pp, polygonToSpool, callDepth);
		callDepth--;
		
		if  ( ret == false ) {
			addErrorInfo("PGenPolygon::inlayPolygon: Recursive call type A of inlayPolygon failed");
			return false;
		}
		
		return true;
	// start a new inlay procedure for each new polygon
	} else {
		//results.removeHoles();
		
		for ( auto it=results.begin(); it !=results.end(); ++it ) {
			CncPolygonPoints pp = *it;
			
			/*
			if ( pp.getOrientationAsBool() == false )
				continue;
			*/
			
			
			pp.closePolygon();
			
			CncPolygonPoints newPolygonToSpool;
			
			// check
			if ( ++callDepth > maxCallDepth ) {
				addErrorInfo("PGenPolygon::inlayPolygon: Max call depth reached (type B)");
				return false;
			}

			// recursive call
			bool ret = inlayPolygonA(spg, pp, newPolygonToSpool, callDepth);
			--callDepth;
			
			if  ( ret == false ) {
				addErrorInfo("PGenPolygon::inlayPolygon: Recursive call type B of inlayPolygon failed");
				return false;
			}
		}
		
		return true;
	}
	
	return true;
}


///////////////////////////////////////////////////////////////////
bool PGenPolygon::inlayPolygonB(SvgPathGroup& spg, 
							    CncPolygonPoints& polygonInData, 
							    CncPolygonPoints& polygonToSpool, 
							    unsigned int callDepth) {
///////////////////////////////////////////////////////////////////
	static const int maxCallDepth = 32;
	
	CncClipperWrapper cw;
	
	// first spool the given polygon
	polygonInData.closePolygon();
	spoolPolygon(spg, polygonInData);
	
	// prepare the path input incl. the corresponding holes
	PolygonList pl;
	pl.push_back(polygonInData);
	appendHoles(pl);
		
	// until the polygon is filled with paths
	unsigned int cnt = 1;
	while ( true ) {
		// offset
		double offset = -commonValues.toolDiameter;
		// consider input unit
		if ( inputUnit != mm )
			offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
			
		//perform the offseting
		CncPolygons results;
		if ( cw.offsetPath(pl, results, offset * cnt, commonValues.getCornerType()) == false) { 
			addErrorInfo("PGenPolygon::inlayPolygon: failed");
			return false;
		}
		
		wxString xxx(' ', callDepth);
		clog << xxx << callDepth << " duration count: " << cnt << "; result size: "<< results.size() << endl;
		
		MessageBoxA(0, wxString::Format("%d", results.getOuterCount()),"",0);
		
		// nothing more to do
		if ( results.getOuterCount() == 0 )
			break;
		
		if ( results.getOuterCount() == 1 ) {
			// append
			CncPolygonPoints pp(results[0]);
			pp.closePolygon();
			polygonToSpool.append(pp);
			
		} else {
			
			/*
			
			// recursive call 
			for ( auto it=results.begin(); it !=results.end(); ++it ) {
				CncPolygonPoints pp = *it;
				pp.closePolygon();
				++callDepth;
				
				//safty 
				if ( callDepth > maxCallDepth ) {
					std::cerr << "PGenPolygon::inlayPolygon: failed: Max call depth reached!" << std::endl;
					break;
				}
	
				inlayPolygonOld(spg, pp, callDepth);
				--callDepth;
			}
			
			break;
			 */
		}
		
		cnt++;
		
	}
	
	spoolPolygon(spg, polygonToSpool);
	
	return true;
}






///////////////////////////////////////////////////////////////////
void PGenPolygon::spoolPolygon(SvgPathGroup& spg, const CncPolygonPoints& dataPoints) {
///////////////////////////////////////////////////////////////////
	spg.pGen().add(dataPoints.getAsSvgPathRepresentation(getInputUnit(), false, ""));
	spg.add(spg.pGen().get());
}









///////////////////////////////////////////////////////////////////
void PGenPolygon::inlayPolygonOld(SvgPathGroup& spg, CncPolygonPoints& polygon, unsigned int callDepth) {
///////////////////////////////////////////////////////////////////
	static const int maxCallDepth = 32;
	
	CncClipperWrapper cw;
	
	// first spool the given polygon
	polygon.closePolygon();
	spoolPolygon(spg, polygon);
	
	// prepare the path input incl. the corresponding holes
	PolygonList pl;
	pl.push_back(polygon);
	appendHoles(pl);
		
	// create ....
	CncPolygonPoints polygonToSpool;
		
	// until the polygon is filled with pathes
	unsigned int cnt = 1;
	while ( true ) {
		// offset
		double offset = -commonValues.toolDiameter;
		// consider input unit
		if ( inputUnit != mm )
			offset = SvgUnitCalculator::convertUnit2Unit(mm, inputUnit, offset);
			
		//perform the offseting
		CncPolygons results;
		if ( cw.offsetPath(pl, results, offset * cnt, commonValues.getCornerType()) == false) { 
			std::cerr << "PGenPolygon::inlayPolygon: failed" << std::endl;
			return;
		}
		
		wxString xxx(' ', callDepth);
		clog << xxx << callDepth << " duration count: " << cnt << "; result size: "<< results.size() << endl;
		
		// nothing more to do
		if ( results.size() == 0 )
			break;
		
		if ( results.size() == 1 ) {
			// append
			CncPolygonPoints pp(results[0]);
			pp.closePolygon();
			polygonToSpool.append(pp);
			
		} else {
			// recursive call 
			for ( auto it=results.begin(); it !=results.end(); ++it ) {
				CncPolygonPoints pp = *it;
				pp.closePolygon();
				++callDepth;
				
				//safty 
				if ( callDepth > maxCallDepth ) {
					std::cerr << "PGenPolygon::inlayPolygon: failed: Max call depth reached!" << std::endl;
					break;
				}
	
				inlayPolygonOld(spg, pp, callDepth);
				--callDepth;
			}
			
			break;
		}
		
		cnt++;
		
	}
	
	spoolPolygon(spg, polygonToSpool);
}
