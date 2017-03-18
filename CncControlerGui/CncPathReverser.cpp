#include "CncPosition.h"
#include "CncPathReverser.h"

//////////////////////////////////////////////////////////////////
std::vector<CncPathListEntry>& CncPathReverser::reversePath(std::vector<CncPathListEntry>& list) {
//////////////////////////////////////////////////////////////////
	if ( list.size() < 2 )
		return list;
	
	CncPathListEntry ple = *(list.begin());
	CncDoublePosition sp;
	sp.setX(ple.move.x);
	sp.setY(ple.move.y);
	
	// find abs path end location
	for (CncPathList::iterator it = list.begin() + 1; it != list.end(); ++it) {
		sp.incX((*it).move.x);
		sp.incY((*it).move.y);
		
		// reverse relativ move steps
		(*it).move.x *= -1;
		(*it).move.y *= -1;
	}
	
	// insert new first path entry
	ple.move.x = sp.getX();
	ple.move.y = sp.getY();
	ple.alreadyRendered = true;
	ple.zAxisDown = false;
	list.push_back(ple);
	
	// remove old first entry
	list.erase(list.begin());
	
	// reverse list
	std::reverse(list.begin(), list.end());
	
	return list;
}