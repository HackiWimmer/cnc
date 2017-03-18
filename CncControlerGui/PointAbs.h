#ifndef POINT_ABS_H
#define POINT_ABS_H

#include <iostream>
#include <wx/gdicmn.h>

class PointAbs {
	
	protected:
		wxRealPoint ret;
		wxRealPoint reference;
		
	public:
		PointAbs() {
			reInit(0,0);
		}

		PointAbs(const PointAbs& pa) {
			reInit(pa);
		}

		PointAbs(const wxRealPoint& ref) {
			reInit(ref);
		}
		
		~PointAbs() {
			
		}
		
		const wxRealPoint getRef() const { return reference; }
		const double getRefX() const { return reference.x; }
		const double getRefY() const { return reference.y; }
		
		void reInit(const PointAbs& pa) {
			reference.x = pa.getRefX();
			reference.y = pa.getRefY();
		}
		
		void reInit(const wxRealPoint& ref) {
			reference = ref;
		}

		void reInit(double xRef, double yRef) {
			reference.x = xRef;
			reference.y = yRef;
		}

		wxRealPoint& makeAbs(const wxRealPoint& relativPoint) {
			return makeAbs(relativPoint.x, relativPoint.y);
		}
			
		wxRealPoint& makeAbs(double dx, double dy) {
			ret.x = reference.x + dx;
			ret.y = reference.y + dy;
			
			return ret;
		}

		wxRealPoint& makeRel(const wxRealPoint& absolutePoint) {
			ret.x = absolutePoint.x - reference.x;
			ret.y = absolutePoint.y - reference.y;
			
			return ret;
		}



};

#endif