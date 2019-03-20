#include <list>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "CncPathListManager.h"

//////////////////////////////////////////////////////////////////
bool CncPathListManager::overAllBoostWktEntriesSample() {
//////////////////////////////////////////////////////////////////
	try {
		typedef boost::geometry::model::d2::point_xy<double> 	point_type;
		typedef boost::geometry::model::polygon<point_type> 	polygon_type;
		typedef boost::geometry::model::linestring<point_type> 	linestring_type;
		
		polygon_type 	polygonType;
		linestring_type	linestringType;
		
		using boost::geometry::get;
		
		switch ( getWktType() ) {
			case CncPathListManager::WKT_EMPTY: 
			case CncPathListManager::WKT_POINT:
									// nothing should happen
									return true;
									
			case CncPathListManager::WKT_POLYGON:
									boost::geometry::read_wkt(getAsWktRepresentation(), polygonType);
									boost::geometry::reverse(polygonType);
									
									for(auto it = boost::begin(boost::geometry::exterior_ring(polygonType)); it != boost::end(boost::geometry::exterior_ring(polygonType)); ++it)
										std::clog << get<0>(*it) << ", " <<  get<1>(*it) << std::endl;
										
									break;
									
			case CncPathListManager::WKT_LINESTRING:
									boost::geometry::read_wkt(getAsWktRepresentation(), linestringType);
									boost::geometry::reverse(linestringType);
									
									for(auto it = boost::begin(linestringType); it != boost::end(linestringType); ++it)
										std::clog << get<0>(*it) << ", " <<  get<1>(*it) << std::endl;
										
									break;
									
			default:				std::cerr << "reversePath(): Unknown wkt type: " << getWktTypeAsString() << std::endl;
									return false;
			
		}
	}
	catch (boost::geometry::centroid_exception& e) {
		std::cerr << "reversePath(): Error while reverse path\n";
		std::cerr << e.what();
		std::cerr << std::endl;
		return false;
	}
	catch (...) {
		std::cerr << "reversePath(): Unknown Error while reverse path\n";
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
const wxRealPoint& CncPathListManager::getStartPos() const {
//////////////////////////////////////////////////////////////////
	static wxRealPoint p(0.0, 0.0);
	if ( list.size() > 0 ) {
		return const_begin()->abs;
	}
	
	return p; 
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::isPathClosed() {
//////////////////////////////////////////////////////////////////
	if ( getPathListSize() > 0 ) {
		CncPathList::iterator itFirst = begin(); 
		CncPathList::iterator itLast  = end() - 1;
		
		return ( cnc::dblCompare((*itFirst).abs.x, (*itLast).abs.x) && 
				 cnc::dblCompare((*itFirst).abs.y, (*itLast).abs.y)
			   ); 
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
CncPathListManager::WktTypeInfo CncPathListManager::getWktType() {
//////////////////////////////////////////////////////////////////
	switch ( getPathListSize() ) {
		case 0:			return WKT_EMPTY;
		case 1:			return WKT_POINT;
		default:
						if ( isPathClosed() )	return WKT_POLYGON;
						else 					return WKT_LINESTRING;
	}
	
	// should not appear
	wxASSERT(NULL);
	return WKT_UNKNOWN;
}
//////////////////////////////////////////////////////////////////
const wxString& CncPathListManager::getWktTypeAsString() {
//////////////////////////////////////////////////////////////////
	static wxString s;
	switch ( getWktType() ) {
		case WKT_EMPTY:			s.assign("EMPTY"); 		break;
		case WKT_POINT:			s.assign("POINT"); 		break;
		case WKT_POLYGON:		s.assign("POLYGON"); 	break;
		case WKT_LINESTRING:	s.assign("LINESTRING"); break;
		default:				s.assign("UNKNOWN"); 	break;
	}
	
	return s;
}
//////////////////////////////////////////////////////////////////
const char* CncPathListManager::getAsWktRepresentation() {
//////////////////////////////////////////////////////////////////
	static wxString s;
	
	if ( getPathListSize() == 0 ) {
		s.assign("");
		
	} else if ( getPathListSize() == 1 ) {
		CncPathList::iterator itFirst = begin();
		s.assign(wxString::Format("POINT(%.3lf %.3lf)", (*itFirst).abs.x, (*itFirst).abs.y));
		
	} else {
		if ( isPathClosed() )	s.assign("POLYGON((");
		else 					s.assign("LINESTRING(");
		
		unsigned int cnt = 0;
		for (CncPathList::iterator it = begin(); it != end(); ++it) {
			if ( cnt == 0 ) s.append(wxString::Format("%.3lf %.3lf", (*it).abs.x, (*it).abs.y));
			else			s.append(wxString::Format(",%.3lf %.3lf", (*it).abs.x, (*it).abs.y));
			cnt++;
		}
		
		if ( isPathClosed() )	s.append("))");
		else					s.append(")");
	}
	
	return s.c_str();
}
//////////////////////////////////////////////////////////////////
const char* CncPathListManager::getAsSvgPathRepresentation(const wxString& style) {
//////////////////////////////////////////////////////////////////
	static wxString s;
	if ( getPathListSize() == 0 ) {
		s.assign("");
		return s.c_str();
	}
	
	s.assign("<path d=\"M");
	wxString x((getPathListSize() > 1 ? " L" : ""));
		
	unsigned int cnt = 0;
	for (CncPathList::iterator it = begin(); it != end(); ++it) {
		if ( cnt == 0 ) s.append(wxString::Format("%.3lf,%.3lf%s", (*it).abs.x, (*it).abs.y, x));
		else			s.append(wxString::Format(" %.3lf,%.3lf",  (*it).abs.x, (*it).abs.y));
		cnt++;
	}
	
	s.append("\" ");
	
	if ( style.IsEmpty() == false ) 
		s.append(style);
	
	s.append("/>");
	return s.c_str();
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::getCentroid(wxRealPoint& centroid) {
//////////////////////////////////////////////////////////////////
	try {
		typedef boost::geometry::model::d2::point_xy<double> 	point_type;
		typedef boost::geometry::model::polygon<point_type> 	polygon_type;
		typedef boost::geometry::model::linestring<point_type> 	linestring_type;
		
		point_type p(0.0, 0.0);
		
		point_type		pointType;
		polygon_type 	polygonType;
		linestring_type	linestringType;
		
		switch ( getWktType() ) {
			case CncPathListManager::WKT_POINT:
									boost::geometry::read_wkt(getAsWktRepresentation(), pointType);
									boost::geometry::centroid(pointType, p);
									break;
									
			case CncPathListManager::WKT_POLYGON:
									boost::geometry::read_wkt(getAsWktRepresentation(), polygonType);
									boost::geometry::centroid(polygonType, p);
									break;
									
			case CncPathListManager::WKT_LINESTRING:
									boost::geometry::read_wkt(getAsWktRepresentation(), linestringType);
									boost::geometry::centroid(linestringType, p);
									break;
									
			default:				std::cerr << "determineCentroid(): Unknown wkt type: " << getWktTypeAsString() << std::endl;
									return false;
			
		}
		
		centroid.x = (cnc::dblCompare(p.x(), 0.0, 0.001) == true ? 0.0 : p.x());
		centroid.y = (cnc::dblCompare(p.y(), 0.0, 0.001) == true ? 0.0 : p.y());
	}
	catch (boost::geometry::centroid_exception& e) {
		std::cerr << "determineCentroid(): Error while determine centroid\n";
		std::cerr << e.what();
		std::cerr << std::endl;
		return false;
	}
	catch (...) {
		std::cerr << "determineCentroid(): Unknown Error while determine centroid\n";
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::shiftPathStart() {
//////////////////////////////////////////////////////////////////
	//todo
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::centerPath() {
//////////////////////////////////////////////////////////////////
	// empty or single point, nothing should happen
	// the size check is also very importent for the further implementation
	if ( getPathListSize() < 2 )
		return true;

	wxRealPoint cp; getCentroid(cp);
	std::clog << "RefPoint: " << getReferencePos() << std::endl;
	std::clog << "Centroid: " << cp << std::endl;
	
	//todo
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::reversePath() {
//////////////////////////////////////////////////////////////////
	// empty or single point, nothing should happen
	// the size check is also very importent for the further implementation
	if ( getPathListSize() < 2 )
		return true;

	// reverse list
	std::reverse(list.begin(), list.end());
	
	// correct new fientry
	begin()->move 				= begin()->abs - referencePos;
	begin()->alreadyRendered 	= true;
	begin()->zAxisDown 			= false;
	
	// correct last entry;
	last()->alreadyRendered 	= false; // not well known, false is always secure
	last()->zAxisDown 			= true;
	
	// reset length - will be recalculate in next loop
	xyLength = 0.0;
	
	// over all entries 
	unsigned int cnt = 0;
	for (CncPathList::iterator it = list.begin(); it != list.end(); ++it) {
		if ( cnt > 0 ) {
			// reverse relative move steps
			(*it).move = it->abs - (it - 1)->abs;
			
			// recalculate distance
			xyLength += sqrt(pow(it->move.x, 2) + pow(it->move.y, 2));
			it->xyDistance = xyLength;
			
		} else {
			it->xyDistance = 0.0;
		}
		
		cnt++;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::appendEntry(CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	// additionally calculate length and distance
	if ( list.size() > 0 ) {
		xyLength += sqrt(pow(cpe.move.x, 2) + pow(cpe.move.y, 2));
		cpe.xyDistance = xyLength;
	}
	
	// addionally determine fences
	minPosX = std::min(minPosX, cpe.abs.x);
	minPosY = std::min(minPosY, cpe.abs.y);
	maxPosX = std::max(maxPosX, cpe.abs.x);
	maxPosY = std::max(maxPosY, cpe.abs.y);
	
	// store
	list.push_back(cpe);
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::calculateAndAddEntry(double newAbsPosX, 
																 double newAbsPosY,
																 bool alreadyRendered, 
																 bool zAxisDown) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;
	cpe.zAxisDown		= zAxisDown;
	cpe.alreadyRendered	= alreadyRendered;
	cpe.abs.x 			= newAbsPosX;
	cpe.abs.y 			= newAbsPosY;

	// calculate
	if ( list.size() == 0 ) {
		cpe.move.x = newAbsPosX;
		cpe.move.y = newAbsPosY;
		
	} else {
		cpe.move.x = newAbsPosX - list.back().abs.x;
		cpe.move.y = newAbsPosY - list.back().abs.y;
	}
	
	// append
	appendEntry(cpe);
	return list.back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::calculateAndAddEntry(const wxRealPoint& newAbsPoint, 
																 bool alreadyRendered, 
																 bool zAxisDown) {
//////////////////////////////////////////////////////////////////
	
	return calculateAndAddEntry(newAbsPoint.x, newAbsPoint.y, alreadyRendered, zAxisDown);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase) {
//////////////////////////////////////////////////////////////////
	if ( itToErase < begin() )
		return false;

	if ( itToErase >= end() )
		return false;
		
	// store entry 
	CncPathListEntry entry = *itToErase;
	
	// check first position
	bool first = false;
	if ( itToErase == begin() )
		first = true;
	
	// remove entry
	if ( list.erase(itToErase) == end() )
		return false;
		
	// nothing further to do if the list is now empty
	if ( list.size() == 0 )
		return true;
	
	// redetermine additional values
	if ( first == true ) {
		begin()->move 				= begin()->abs - referencePos;
		begin()->alreadyRendered 	= true;
		begin()->zAxisDown 			= false;
	}
	
	// set recalculateMinMax flag
	bool recalculateMinMax = false;
	if ( cnc::dblCompare(entry.abs.x, minPosX) || cnc::dblCompare(entry.abs.y, minPosY) || 
		 cnc::dblCompare(entry.abs.x, maxPosX) || cnc::dblCompare(entry.abs.y, maxPosY) ) {
		
		recalculateMinMax = true;
		resetMinMax();
	}

	// recalculate length and min or max on demand
	xyLength = 0.0;
	unsigned int cnt = 0;
	for ( auto it=begin(); it !=end(); ++it ) {
		
		if ( cnt > 0 ) {
			xyLength += sqrt(pow(it->move.x, 2) + pow(it->move.y, 2));
			it->xyDistance = xyLength;
		} else {
			it->xyDistance = 0.0;
		}
		
		if ( recalculateMinMax == true ) {
			minPosX = std::min(minPosX, it->abs.x);
			minPosY = std::min(minPosY, it->abs.y);
			maxPosX = std::max(maxPosX, it->abs.x);
			maxPosY = std::max(maxPosY, it->abs.y);
		}
		
		cnt++;
	}
	
	return true;
}