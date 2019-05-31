#include <iostream>
#include "GLCncPathData.h"

////////////////////////////////////////////
GLI::GLCncPath::GLCncPath(const wxString& instanceName) 
: vectiesBuffer(instanceName)
, minVecties(FLT_MAX, FLT_MAX, FLT_MAX)
, maxVecties(FLT_MIN, FLT_MIN, FLT_MIN)
, virtualEnd(-1)
, publishNotifications(true)
, callbacks()
////////////////////////////////////////////
{
}
////////////////////////////////////////////
GLI::GLCncPath::~GLCncPath() {
////////////////////////////////////////////
}
////////////////////////////////////////////
void GLI::GLCncPath::clear() {
////////////////////////////////////////////
	if ( GLCommon::getTraceLevel() > 0 )
		std::cout << CNC_LOG_FUNCT << std::endl;

	// reset boundings
	minVecties.set(-1L, FLT_MAX, FLT_MAX, FLT_MAX);
	maxVecties.set(-1L, FLT_MIN, FLT_MIN, FLT_MIN);
	
	vectiesBuffer.resetBuffers();
	updateVirtualEnd();
}
////////////////////////////////////////////
void GLI::GLCncPath::appendPathData(const GLOpenGLPathBuffer::CncVertex& vertex) {
////////////////////////////////////////////
	if ( GLCommon::getTraceLevel() > 0 ) {
		if ( vectiesBuffer.getVertexCount() < 2 )
			std::cout << CNC_LOG_FUNCT << std::endl;
	}

	minVecties.setX(std::min(vertex.getX(), minVecties.getX()));
	minVecties.setY(std::min(vertex.getY(), minVecties.getY()));
	minVecties.setZ(std::min(vertex.getZ(), minVecties.getZ()));
	
	maxVecties.setX(std::max(vertex.getX(), maxVecties.getX()));
	maxVecties.setY(std::max(vertex.getY(), maxVecties.getY()));
	maxVecties.setZ(std::max(vertex.getZ(), maxVecties.getZ()));
	
	vectiesBuffer.appendVertex(vertex);
	updateVirtualEnd();
}
////////////////////////////////////////////
void GLI::GLCncPath::display(GLOpenGLPathBuffer::DisplayType dt) {
////////////////////////////////////////////
	vectiesBuffer.display(dt, virtualEnd);
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
	if ( virtualEnd >= (long)(vectiesBuffer.getVertexCount() - 1) )
		return -1;
		
	GLOpenGLPathBuffer::CncVertex vertex;
	vectiesBuffer.getVertex(vertex, virtualEnd);
		
	return vertex.getClientId();
}
////////////////////////////////////////////
bool GLI::GLCncPath::hasNextVertex() const {
////////////////////////////////////////////
	return virtualEnd < (long)(vectiesBuffer.getVertexCount() - 1);
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
		
	GLOpenGLPathBuffer::CncVertex vertex;
	vectiesBuffer.getVertex(vertex, virtualEnd + 1);
	return vertex.getClientId();
}
////////////////////////////////////////////
long GLI::GLCncPath::previewPreviousVertexId() {
////////////////////////////////////////////
	if ( hasPreviousVertex() == false )
		return -1;
		
	GLOpenGLPathBuffer::CncVertex vertex;
	vectiesBuffer.getVertex(vertex, virtualEnd - 1);
	return vertex.getClientId();
}
////////////////////////////////////////////
void GLI::GLCncPath::incVirtualEndById() {
////////////////////////////////////////////
	long id = getVirtualEndAsId();
	
	publishNotifications = false;
		
		do {
			incVirtualEnd();
			
			if ( virtualEnd >= (long)(vectiesBuffer.getVertexCount() - 1) )
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
		
		if ( virtualEnd >= (long)(vectiesBuffer.getVertexCount() - 1) )
			break;
	}
}
