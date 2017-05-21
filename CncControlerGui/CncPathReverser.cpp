#include "CncPosition.h"
#include "CncPathReverser.h"

//////////////////////////////////////////////////////////////////
CncPathList& CncPathReverser::reversePath(CncPathList& list) {
//////////////////////////////////////////////////////////////////
	if ( list.size() < 2 )
		return list;
	
	CncPathListEntry first = *(list.begin());
	
	// reverse relativ move steps
	for (CncPathList::iterator it = list.begin() + 1; it != list.end(); ++it) {
		(*it).move.x *= -1;
		(*it).move.y *= -1;
	}
	
	// update first entry and append it, after reverse it will be first again
	first.move.x = list.back().abs.x;
	first.move.y = list.back().abs.y;
	first.alreadyRendered = true;
	first.zAxisDown = false;
	list.push_back(first);
	
	// remove old first entry
	list.erase(list.begin());
	
	// reverse list
	std::reverse(list.begin(), list.end());
	
	return list;
}