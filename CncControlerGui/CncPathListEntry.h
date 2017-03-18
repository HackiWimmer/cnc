#ifndef CNC_PATH_LIST_ENTRY_H
#define CNC_PATH_LIST_ENTRY_H

#include <ostream>
#include <vector>
#include <wx/gdicmn.h>

class CncPathListEntry{
	private:
		wxString result;
		
	public:
		
		wxRealPoint move;
		bool zAxisDown;
		bool alreadyRendered;
		
		//////////////////////////////////////////////////////////////////
		CncPathListEntry() 
		: move(0.0, 0.0)
		, zAxisDown(false)
		, alreadyRendered(false)
		{}
		
		//////////////////////////////////////////////////////////////////
		CncPathListEntry(const CncPathListEntry& cpe) 
		: move({cpe.move})
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
			ostr << " Point:           "	<< a.move.x << "," << a.move.y << std::endl; 
			return ostr;
		}
		
		//////////////////////////////////////////////////////////////////i
		wxString& getPointAsString() {
			result.clear();
			result << move.x;
			result << ",";
			result << move.y;
			
			return result;
		}
};

typedef std::vector<CncPathListEntry> CncPathList;

#endif