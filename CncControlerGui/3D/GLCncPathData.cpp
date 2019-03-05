#include <iostream>
#include "GLCncPathData.h"

////////////////////////////////////////////
GLI::GLCncPath::iterator GLI::GLCncPath::vBegin() { 
////////////////////////////////////////////
	return begin(); 
}
////////////////////////////////////////////
GLI::GLCncPath::iterator GLI::GLCncPath::vEnd() { 
////////////////////////////////////////////
	if ( virtualEnd <= 0 )
		return end(); 
		
	if ( size() == 0 )
		return end(); 
		
	if ( (unsigned long)virtualEnd > size() - 1 )
		return end(); 
		
	return begin() + virtualEnd;
}
////////////////////////////////////////////
void GLI::GLCncPath::setVirtualEnd(long val) {
////////////////////////////////////////////
	virtualEnd = val;
	notifyCncPathChanged();
}
////////////////////////////////////////////
const long GLI::GLCncPath::getVirtualEndAsId() {
////////////////////////////////////////////
	if ( vEnd() == end() )
		return -1;
		
	return vEnd()->getId();
}
////////////////////////////////////////////
bool GLI::GLCncPath::hasNextVertex() const {
////////////////////////////////////////////
	return virtualEnd < std::distance(begin(), end());
}
////////////////////////////////////////////
bool GLI::GLCncPath::hasPreviousVertex() const {
////////////////////////////////////////////
	return virtualEnd > 0;
}
////////////////////////////////////////////
long GLI::GLCncPath::previewNextVertexId() {
////////////////////////////////////////////
	if ( hasNextVertex() == false )
		return -1;
		
	const auto it = vEnd() + 1;
	return it->getId();
}
////////////////////////////////////////////
long GLI::GLCncPath::previewPreviousVertexId() {
////////////////////////////////////////////
	if ( hasPreviousVertex() == false )
		return -1;
		
	const auto it = vEnd() - 1;
	return it->getId();
}
////////////////////////////////////////////
void GLI::GLCncPath::incVirtualEndById() {
////////////////////////////////////////////
	long id = getVirtualEndAsId();
	
	publishNotifications = false;
		
		do {
			incVirtualEnd();
			
			if ( virtualEnd >= std::distance(begin(), end()) - 1 )
				break;
				
		} while ( id == getVirtualEndAsId() );
		
	publishNotifications = true;
	notifyCncPathChanged();
}
////////////////////////////////////////////
void GLI::GLCncPath::decVirtualEndById() {
////////////////////////////////////////////
	long id = getVirtualEndAsId();
	
	publishNotifications = false;
		
		do {
			decVirtualEnd();
			
			if ( virtualEnd <= 1 )
				break;
				
		} while ( id == getVirtualEndAsId() );
	
	publishNotifications = true;
	notifyCncPathChanged();
}
////////////////////////////////////////////
void GLI::GLCncPath::spoolVertiesForCurrentId() {
////////////////////////////////////////////
	long id = getVirtualEndAsId();
	
	while ( id == previewNextVertexId() ) {
		incVirtualEnd();
		
		if ( virtualEnd >= std::distance(begin(), end()) - 1 )
			break;
	}
}