#ifndef CNC_PATH_LIST_ENTRY_H
#define CNC_PATH_LIST_ENTRY_H

#include <ostream>
#include <vector>
#include <wx/gdicmn.h>
#include "CncCommon.h"

class CncPathListEntry{
		
	public:
		
		wxRealPoint move;
		wxRealPoint abs;
		double xyDistance;
		
		bool zAxisDown;
		bool alreadyRendered;
		
		//////////////////////////////////////////////////////////////////
		CncPathListEntry() 
		: move(0.0, 0.0)
		, abs(0.0, 0.0)
		, xyDistance(0.0)
		, zAxisDown(false)
		, alreadyRendered(false)
		{}
		
		//////////////////////////////////////////////////////////////////
		CncPathListEntry(const CncPathListEntry& cpe) 
		: move({cpe.move})
		, abs({cpe.abs})
		, xyDistance(cpe.xyDistance)
		, zAxisDown(cpe.zAxisDown)
		, alreadyRendered(cpe.alreadyRendered)
		{}
		
		//////////////////////////////////////////////////////////////////
		~CncPathListEntry() {
		}
	
		//////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPathListEntry &a) {
			ostr << "CncPathListEntry: " 	<< std::endl;
			ostr << " Z axis down:     " 	<< a.zAxisDown << std::endl;
			ostr << " Already renderd: "	<< a.alreadyRendered << std::endl; 
			ostr << " Point (abs):     "	<< a.abs << std::endl; 
			ostr << " Point (rel):     "	<< a.move << std::endl; 
			ostr << " Distance:        "	<< wxString::Format("%10.3lf", a.xyDistance);
			return ostr;
		}
		
		//////////////////////////////////////////////////////////////////i
		wxString& getPointAsString() const {
			static wxString s;
			s.assign(wxString::Format("abs: %10.3lf, %10.3lf | rel: %10.3lf, %10.3lf | len: %10.3lf", 
			                          abs.x, abs.y, move.x, move.y, xyDistance));
			return s;
		}
};

typedef std::vector<CncPathListEntry> CncPathList;

#endif