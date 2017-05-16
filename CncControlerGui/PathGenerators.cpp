#include "PathGenerators.h"


///////////////////////////////////////////////////////////////////
void PGenPolygon::addPolygon(SvgPathGroup& spg, bool inlay) {
///////////////////////////////////////////////////////////////////
	// perform first correction on demand
	if ( commonValues.toolCorrection == true )
		correctPolygon(spg, commonValues.toolDiameter/2, true);
	else
		spg.add(spg.fGen().addPolygon(polygonData));
	
	// perform inlay
	if ( inlay == true ) {
		correctPolygon(spg, commonValues.toolDiameter, false);
	}
}
///////////////////////////////////////////////////////////////////
void PGenPolygon::correctPolygon(SvgPathGroup& spg, double value, bool once) {
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
			} else {
				it->setTransformedPoint(cp);
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
		
		// add cuurent polygon data points
		spg.add(spg.fGen().addPolygon(polygonData));
		
		// break if only one correction is requested
		if ( once == true )
			break;
		
	// check if min one point is transformed
	} while ( anythingTransformed == true );
}