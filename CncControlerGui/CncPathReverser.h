#ifndef PATH_REVERSER_H_INCLUDED
#define PATH_REVERSER_H_INCLUDED

#include "CncPathListEntry.h"
#include <vector>

class CncPathReverser {
	
	public:
		CncPathReverser() {}
		~CncPathReverser() {}
		
		CncPathList& reversePath(CncPathList& list);
};

#endif
