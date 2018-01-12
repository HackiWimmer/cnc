#include <vector>
#include <cmath>
#include <iostream>
#include <wx/gdicmn.h>
#include <wx/string.h>

#include "LinearFunc.h"
#include "PointAbs.h"
#include "CncToolCorrection.h"

///////////////////////////////////////////////////////
CncToolCorrection::CncToolCorrection(double r, CncToolCorretionType t) {
///////////////////////////////////////////////////////
	radius 	= r;
	type 	= t;
}
///////////////////////////////////////////////////////
CncToolCorrection::~CncToolCorrection() {
///////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////
void CncToolCorrection::debugList(const CncPathList& list) {
///////////////////////////////////////////////////////
	PointAbs pa;
	unsigned int counter = 0;
	for (CncPathList::const_iterator it = list.begin(); it != list.end(); ++it) {
		CncPathListEntry cpe = *it;
		
		std::cout << "(" << cpe.move.x << "," << cpe.move.y << ")" ;
		
		if ( counter == 0 ) {
			std::cout << " abs: (" <<  cpe.move.x << "," << cpe.move.y << ")" << std::endl;
			pa.reInit(cpe.move);
		} else {
			wxRealPoint p = pa.makeAbs(cpe.move);
			std::cout << " abs: (" <<  p.x << "," << p.y << ")" << std::endl;
			pa.reInit(p);
		}
		
		counter++;
	}
}
//////////////////////////////////////////////////////////////////
inline CncDirection CncToolCorrection::getCncDirection(SubPathDirection d1, SubPathDirection d2) {
//////////////////////////////////////////////////////////////////
	CncDirection ret = CncUndefDir;

	switch ( d2 ) {
		case FC_South: 	
						switch ( d1 ) {
							case FC_North: 		ret = CncUndefDir; 		break;
							case FC_South:
							case FC_East: 
							case FC_NorthEast:
							case FC_SouthEast: 	ret = CncAnticlockwise; break;
							case FC_West:
							case FC_NortWest:
							case FC_SouthWest: 	ret = CncClockwise; 	break;
							default:;
						}
						break;
		case FC_North: 	
						switch ( d1 ) {
							case FC_South: 		ret = CncUndefDir; 		break;
							case FC_North:
							case FC_NorthEast:
							case FC_East:
							case FC_SouthEast: 	ret = CncClockwise; 	break;
							case FC_West:
							case FC_NortWest:
							case FC_SouthWest: 	ret = CncClockwise; 	break;
							
							default:;
						}
						break;
		case FC_West: 		
						switch ( d1 ) {
							case FC_East: 		ret = CncUndefDir; 		break;
							case FC_West:
							case FC_SouthWest:
							case FC_South:
							case FC_SouthEast: 	ret = CncAnticlockwise; break;
							case FC_North:
							case FC_NortWest:
							case FC_NorthEast: 	ret = CncClockwise;		break;
							
							default:;
						}
						break;
		case FC_East: 		
						switch ( d1 ) {
							case FC_West: 		ret = CncUndefDir; 		break;
							case FC_East:
							case FC_SouthEast:
							case FC_South:
							case FC_SouthWest: 	ret = CncAnticlockwise;	break;
							case FC_North:
							case FC_NortWest:
							case FC_NorthEast: 	ret = CncClockwise; 	break;
							
							
							default:;
						}
						break;
		case FC_NortWest: 	
						switch ( d1 ) {
							case FC_SouthEast: 	ret = CncUndefDir; 		break;
							case FC_NortWest:
							case FC_North:
							case FC_NorthEast:
							case FC_East: 		ret = CncClockwise; 	break;
							case FC_South:
							case FC_West:
							case FC_SouthWest: 	ret = CncAnticlockwise;	break;
							
							default:;
						}
						break;
		case FC_NorthEast: 	
						switch ( d1 ) {
							case FC_SouthWest: 	ret = CncUndefDir; 		break;
							case FC_NorthEast:
							case FC_East:
							case FC_SouthEast:
							case FC_South: 		ret = CncClockwise; 	break;
							case FC_North:
							case FC_West:
							case FC_NortWest: 	ret = CncAnticlockwise;	break;
							default:;
						}
						break;
		case FC_SouthWest: 	
						switch ( d1 ) {
							case FC_NorthEast: 	ret = CncUndefDir; 		break;
							case FC_SouthWest:
							case FC_South:
							case FC_SouthEast:
							case FC_East: 		ret = CncAnticlockwise; break;
							case FC_North:
							case FC_West:
							case FC_NortWest: 	ret = CncClockwise; 	break;
							default:;
						}
						break;
		case FC_SouthEast: 	
						switch ( d1 ) {
							case FC_NortWest: 	ret = CncUndefDir; 	break;
							case FC_SouthEast:
							case FC_South:
							case FC_SouthWest:
							case FC_West: 		ret = CncClockwise; 	break;
							case FC_North:
							case FC_East:
							case FC_NorthEast: 	ret = CncAnticlockwise; break;
							
							default:;
						}
						break;
		default:;
	}
	
	return ret;
}
///////////////////////////////////////////////////////
inline double CncToolCorrection::getToolDiameterCorrectionValue(SubPathDirection pd) {
///////////////////////////////////////////////////////
	int dir = 1; //--> 1 == clockwise; -1 = anticlockwise todo ????
	
	int io = 0;
	switch( type ) {
		case CncCT_None:
		case CncCT_Center: 	io =  0; break;
		case CncCT_Inner: 	io = -1; break;
		case CncCT_Outer: 	io = +1; break;
	}

	switch ( pd ) {
		case FC_South: 		return (-1) * radius * io * dir;
		case FC_North: 		return (+1) * radius * io * dir;
		case FC_West: 		return (+1) * radius * io * dir;
		case FC_East: 		return (-1) * radius * io * dir;
		case FC_NortWest: 	return (+1) * radius * io * dir;
		case FC_NorthEast: 	return (-1) * radius * io * dir;
		case FC_SouthWest: 	return (+1) * radius * io * dir;
		case FC_SouthEast: 	return (-1) * radius * io * dir;
		default:
		;
	}

	return 0;
}
///////////////////////////////////////////////////////
SubPathDirection CncToolCorrection::determinePathDirection(wxRealPoint P1, wxRealPoint P2) {
///////////////////////////////////////////////////////
	double dx = P2.x - P1.x;
	double dy = P2.y - P1.y;
	
	if 		( dx == 0 && dy > 0 ) return FC_North;
	else if ( dx == 0 && dy < 0 ) return FC_South;
	
	else if ( dy == 0 && dx > 0 ) return FC_East;
	else if ( dy == 0 && dx < 0 ) return FC_West;
	
	else if ( dx > 0 && dy > 0 )  return FC_NorthEast;
	else if ( dx > 0 && dy < 0 )  return FC_SouthEast;

	else if ( dx < 0 && dy > 0 )  return FC_NortWest;
	else if ( dx < 0 && dy < 0 )  return FC_SouthWest;
	
	std::cerr << "FC_Unknown error: (dx, dy)" << dx << "," << dy << std::endl;
	std::cerr << " Point 1: " << P1.x << "," << P1.y << std::endl;
	std::cerr << " Point 2: " << P2.x << "," << P2.y << std::endl;
	return FC_Unknown;
}
///////////////////////////////////////////////////////
bool CncToolCorrection::process(CncPathList& oList) {
///////////////////////////////////////////////////////
	if ( type == CncCT_None )
		return true;

	iList.swap(oList);
	oList.clear();
	
	// first of all the path has to be compacted. This step is mandatory reqirement
	// for the following corecction algorythem.
	evaluatePath();
	compactPath();
		
	switch ( iList.size() ) {
		case 0:		std::cerr << "CncToolCorrection::process: Invalid size: "<< iList.size() << std::endl;
					return false;
		// borehole
		case 1:		return processBorehole(oList);
		// slot
		case 2:		return processSimpleSlot(oList);
		// path
		default:	return processPath(oList);
	}

	return false;
}
///////////////////////////////////////////////////////
void CncToolCorrection::debugPoints(const char* userPerspectivePrefix, CncPathListEntry P1, CncPathListEntry P2, CncPathListEntry P3) {
///////////////////////////////////////////////////////
	std::cout << userPerspectivePrefix << ": (" << P1.move.x << "," << P1.move.y << ")" 
	                     << " (" << P2.move.x << "," << P2.move.y << ")" 
						 << " (" << P3.move.x << "," << P3.move.y << ")" 
						 << std::endl;
}
///////////////////////////////////////////////////////
bool CncToolCorrection::processBorehole(CncPathList& olist) {
///////////////////////////////////////////////////////
	// one point path
	wxASSERT(iList.size() == 1);
	wxASSERT(olist.size() == 0);
	
	type = CncCT_Center;
	
	//simple swap back
	olist.swap(iList);
	return true;
}
///////////////////////////////////////////////////////
bool CncToolCorrection::processSimpleSlot(CncPathList& olist) {
///////////////////////////////////////////////////////
	// two point path
	wxASSERT(iList.size() == 2);
	wxASSERT(olist.size() == 0);

	type = CncCT_Center;
	
	// always absolute coordinates
	CncPathList::iterator it = iList.begin();
	CncPathListEntry cpeStart, cpeEnd;
	wxRealPoint sp = cpeStart.move; 
	
	// ep always relative coordinates
	it = iList.end() - 1;
	wxRealPoint ep = cpeEnd.move; 
	
	// determine pitch
	double dx =	ep.x; 
	double dy =	ep.y;
	
	FunctionValue p;
	if ( dx != 0 ) {
		p.valid = true;
		p.value = dy/dx;
	}
	
	// make ep absolute
	ep.x = sp.x + dx;
	ep.y = sp.y + dy;
	
	// determine hypotenuse (path lenght)
	double h = 0;
	if ( p.valid == true )  h = sqrt(dx*dx + dy*dy);
	else 					h = abs(dy);
	
	if ( h < 2*radius ) {
		std::cerr << "Path lenght to short: "<< std::endl;
		std::cerr << " Path length:     " << h << std::endl;
		std::cerr << " 2 * tool radius: " << (2 * radius) << std::endl;
		return false;
	}

	wxRealPoint p1;
	wxRealPoint p2;
	if ( p.valid == true ) {
		// pure horizontal line
		if ( p.value == 0 ) {
			p1 = {sp.x + radius, sp.y};
			p2 = {ep.x - radius, sp.y};
		} else {
			double xc = cos(p.value) * radius;
			double yc = sin(p.value) * radius;
			p1 = {sp.x + xc, sp.y - yc};
			p2 = {ep.x - xc, ep.y + yc};
		}
	} else {
		// pure vertical line
		p1 = {sp.x, sp.y + radius};
		p2 = {sp.x, ep.y - radius};
	}
	
	// convert p2 finally back to relative coordinates
	p2.x -= p1.x;
	p2.y -= p1.y;
	
	cpeStart.move 	= p1;
	cpeEnd.move 	= p2;
	olist.push_back(cpeStart);
	olist.push_back(cpeEnd);

	return true;
}
///////////////////////////////////////////////////////
bool CncToolCorrection::processPath(CncPathList& olist) {
///////////////////////////////////////////////////////
	if ( pathParameter.isClosed() == false ) {
		std::clog << "CncToolCorrection::processPath" << std::endl;
		std::clog << " Only closed paths will be tool corrected." << std::endl;
		std::clog << " Path will be processed unmodified." << std::endl;
		
		olist.swap(iList);
		return true;
	}
	
	wxASSERT(iList.size() >= 3);
	wxASSERT(olist.size() == 0);
	
	CncPathListEntry p1, p2, p3, P1, P2, P3, Pl;
	CncPathList::iterator it = iList.begin();
	p1 = *it++;
	p2 = *it++;
	p3 = *it++;
	
	PointAbs pa(p1.move);
	P2 = p2;
	P2.move = pa.makeAbs(p2.move);
	
	Pl = P2;
	pa.reInit(P2.move);
	
	P3 = p3;
	P3.move = pa.makeAbs(p3.move);
	
	// init points
	if ( pathBegin(p1, P2, P3, olist) == false ) {
		std::cerr << "CncToolCorrection::processPath: pathBegin(...) failed:" << std::endl;
		return false;
	}
	
	for ( ; it != iList.end(); ++it) {
		// shift points
		P1 = P2;
		P2 = P3;
		pa.reInit(P2.move);
		P3.move = pa.makeAbs((*it).move);
		
		if ( pathNext(P1, P2, P3, olist) == false ) {
			std::cerr << "CncToolCorrection::processPath: pathNext(...) failed:" << std::endl;
			return false;
		}
	}

	if ( pathClose(P2, P3, Pl, olist) == false ) {
		std::cerr << "CncToolCorrection::processPath: pathClose(...) failed:" << std::endl;
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////
bool CncToolCorrection::pathBegin(CncPathListEntry P1, 
								  CncPathListEntry P2, 
								  CncPathListEntry P3, 
								  CncPathList& olist) {
///////////////////////////////////////////////////////
	//debugPoints("pathBegin", P1, P2, P3);
	return pathNext(P1, P2, P3, olist);
}
///////////////////////////////////////////////////////
bool CncToolCorrection::pathNext(CncPathListEntry P1, 
								 CncPathListEntry P2, 
								 CncPathListEntry P3, 
								 CncPathList& olist) {
///////////////////////////////////////////////////////
	//debugPoints("pathNext ", P1, P2, P3);

	if ( P1.move == P2.move || P2.move == P3.move )
		return true;
	//std::cout << P1.zAxisDown << std::endl;	
	f1.reInit(P1.move, P2.move);
	f2.reInit(P2.move, P3.move);
	
	double r1 = getToolDiameterCorrectionValue(determinePathDirection(P1.move, P2.move));
	double r2 = getToolDiameterCorrectionValue(determinePathDirection(P2.move, P3.move));
	
	/* only for debugging
	std::cout << "pd: " << determinePathDirection(P1.move, P2.move) << ", " << determinePathDirection(P2.move, P3.move) << " :::: "
			  << r1 << ", " << r2 << std::endl;
	*/
	
	f1.parallelShift(r1);
	f2.parallelShift(r2);
		
	IntersectionValue iv = f1.intersection(f2);
	//std::cout << iv << std::endl;
	if ( iv.valid ) {
		
		//std::cout << P1.zAxisDown << std::endl;
		
		CncPathListEntry cpe;
		cpe.move 			= iv.p;
		cpe.zAxisDown 		= P2.zAxisDown;
		cpe.alreadyRendered = P2.alreadyRendered;
		
		olist.push_back(cpe);
		
	} else {
		
		std::cerr << "CncToolCorrection::pathNext:"<< std::endl;
		std::cerr << " Invalid intersection, "; 
		debugPoints("points: ", P1, P2, P3);
		std::cerr << " Linear functions: " << std::endl; 
		std::cerr << " f1:" << f1;
		std::cerr << " f2:" << f1;
		
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////
bool CncToolCorrection::pathClose(CncPathListEntry P1, 
								  CncPathListEntry P2, 
								  CncPathListEntry P3, 
								  CncPathList& olist) {
///////////////////////////////////////////////////////
	//debugPoints("pathClose", P1, P2, P3);
	bool ret = pathNext(P1, P2, P3, olist);
	
	if ( ret == true) {
		if ( olist.size() > 0 ) {
			CncPathList::iterator it = olist.begin();
			CncPathListEntry back = olist.back();
			// The first point of a path has never zAxisDown = true;
			// So, if we reconstruct the firt point we have to consider that
			back.zAxisDown = false;
			olist.insert(it, back);
			
			wxRealPoint Pa = (*olist.begin()).move;
			wxRealPoint Pb;
			for ( it = olist.begin() + 1; it != olist.end(); ++it ) {
				CncPathListEntry cpe = *it;
				Pb = cpe.move;
				
				cpe.move = {Pb.x - Pa.x, Pb.y - Pa.y};
				*it = cpe;
				
				Pa = Pb;
			}
		} else {
			std::cerr << "CncToolCorrection::pathClose:" << std::endl;
			std::cerr << " Cant close path, size=" << olist.size() << std::endl;
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////
void CncToolCorrection::compactPath() {
///////////////////////////////////////////////////////
	if ( iList.size() < 2 )
		return;
	
	// this function accumulates  chained path elements with the same linear function
	//
	//e. g. *---->*----->*----->*  ==> *------------------->*
	bool debug = false;
	
	CncPathList tmpList;
	tmpList.swap(iList);
	iList.clear();
	
	LinearFunc fc;
	LinearFunc fp;

	CncPathList::iterator it = tmpList.begin();
	wxRealPoint p2, P1, P2;
	
	CncPathListEntry cpe = *it;
	P1 = cpe.move; it++;
	iList.push_back(cpe);

	PointAbs pa(P1);
	while ( it != tmpList.end() ) {
		cpe = *it;
		
		P2 = pa.makeAbs(cpe.move);
		fc.reInit(P1, P2);
		
		if ( debug )
			std::clog << "compact::current functions: " << std::endl << fp << fc;
		
		if ( fc != fp ) {
			if ( debug )
				std::clog << "compact::push back: " << pa.makeRel(P2).x << "," << pa.makeRel(P2).y << std::endl;
				
			cpe.move = pa.makeRel(P2);
			iList.push_back(cpe);
		}
		else {
			CncPathListEntry back = iList.back();
			wxRealPoint l = back.move;
			wxRealPoint p = pa.makeRel(P2);
			
			l.x += p.x; 
			l.y += p.y;
			
			if ( debug )
				std::clog << "compact::change back: " << l.x << "," << l.y << std::endl;

			back.move = l;
			iList.pop_back();
			iList.push_back(back);
		}
		
		P1 = P2;
		pa.reInit(P1);
		fp.reInit(fc);
		
		it++;
	}
	
	// only for debuging
	if ( debug ) {
		for (CncPathList::iterator it = iList.begin(); it != iList.end(); ++it) {
			CncPathListEntry cpe = *it;
			std::clog << "compact::list: "<< cpe.move.x << "," << cpe.move.y << ", Z, rendered: " << cpe.zAxisDown << "," << cpe.alreadyRendered << std::endl;
		}
	}
}
///////////////////////////////////////////////////////
void CncToolCorrection::evaluatePath() {
///////////////////////////////////////////////////////
	double absX = 0, absY = 0;
	double refX = 0, refY = 0;
	bool first = true;
	bool start = false;
	bool end   = false;
	
	pathParameter.reset();
	
	CncPathList::iterator it = iList.begin();
	for ( ; it != iList.end(); ++it) {
		if ( first == true ) {
			absX = (*it).move.x;
			absY = (*it).move.y;
			
			refX = absX;
			refY = absY;
			
			first = false;
		} else {
			absX += (*it).move.x;
			absY += (*it).move.y;
		}
		
		if ( (*it).zAxisDown == true && start == false ) {
			start = true;
		}

		if ( start == false )
			pathParameter.setWorkStarPoint(absX, absY);
		
		if ( (*it).zAxisDown == false && start == true && end == false ) {
			pathParameter.setWorkEndPoint(absX, absY);
			end = true;
		}
		
		pathParameter.setXMin(absX);
		pathParameter.setXMax(absX);
		pathParameter.setYMin(absY);
		pathParameter.setYMax(absY);
	}
	
	// check if path is closed
	double diffX = absX - refX;
	double diffY = absY - refY;
	double deviation = 0.001;
	
	if ( diffX < 0 ) diffX *=-1;
	if ( diffY < 0 ) diffY *=-1;
	
	if ( diffX <= deviation && diffY <= deviation )
		pathParameter.setClosed();

	//debug
	if ( true ) {
		std::clog << pathParameter;
		std::clog << " absX: " << absX << ", refX: " << refX << ", diff: " << absX - refX << std::endl
		          << " absY: " << absY << ", refY: " << refY << ", diff: " << absY - refY << std::endl;
	}
}
