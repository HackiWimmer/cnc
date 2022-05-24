#include <iostream>
#include "CncConfig.h"
#include "CncContext.h"
#include "GLCncPathData.h"

////////////////////////////////////////////
GLI::GLCncPath::GLCncPath(const wxString& instanceName) 
: vectiesBuffer			(instanceName)
, minVecties			(FLT_MAX, FLT_MAX, FLT_MAX)
, maxVecties			(FLT_MIN, FLT_MIN, FLT_MIN)
, minObjVecties			(FLT_MAX, FLT_MAX, FLT_MAX)
, maxObjVecties			(FLT_MIN, FLT_MIN, FLT_MIN)
, virtualEnd			(-1)
, publishNotifications	(true)
, callbacks				()
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
	// reset bounding
	minVecties.set(-1L, FLT_MAX, FLT_MAX, FLT_MAX);
	maxVecties.set(-1L, FLT_MIN, FLT_MIN, FLT_MIN);
	
	minObjVecties.set(-1L, FLT_MAX, FLT_MAX, FLT_MAX);
	maxObjVecties.set(-1L, FLT_MIN, FLT_MIN, FLT_MIN);

	vectiesBuffer.resetBuffers();
	updateVirtualEnd();
}
////////////////////////////////////////////
void GLI::GLCncPath::appendPathData(const GLOpenGLPathBuffer::CncVertex& vertex) {
////////////////////////////////////////////
	minVecties.setX(std::min(vertex.getX(), minVecties.getX()));
	minVecties.setY(std::min(vertex.getY(), minVecties.getY()));
	minVecties.setZ(std::min(vertex.getZ(), minVecties.getZ()));
	
	maxVecties.setX(std::max(vertex.getX(), maxVecties.getX()));
	maxVecties.setY(std::max(vertex.getY(), maxVecties.getY()));
	maxVecties.setZ(std::max(vertex.getZ(), maxVecties.getZ()));
	
	if ( vertex.getType() == cnc::WORK_SPEED_CHAR )
	{
		minObjVecties.setX(std::min(vertex.getX(), minObjVecties.getX()));
		minObjVecties.setY(std::min(vertex.getY(), minObjVecties.getY()));
		minObjVecties.setZ(std::min(vertex.getZ(), minObjVecties.getZ()));
		
		maxObjVecties.setX(std::max(vertex.getX(), maxObjVecties.getX()));
		maxObjVecties.setY(std::max(vertex.getY(), maxObjVecties.getY()));
		maxObjVecties.setZ(std::max(vertex.getZ(), maxObjVecties.getZ()));
	}
	
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
	vectiesBuffer.getPosVertex(vertex, virtualEnd);
		
	return vertex.getClientId();
}
////////////////////////////////////////////
const long GLI::GLCncPath::getVirtualEndAsNormalizedId() {
////////////////////////////////////////////
	return ClientIds::normalize(getVirtualEndAsId());
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
	vectiesBuffer.getPosVertex(vertex, virtualEnd + 1);
	return vertex.getClientId();
}
////////////////////////////////////////////
long GLI::GLCncPath::previewPreviousVertexId() {
////////////////////////////////////////////
	if ( hasPreviousVertex() == false )
		return -1;
		
	GLOpenGLPathBuffer::CncVertex vertex;
	vectiesBuffer.getPosVertex(vertex, virtualEnd - 1);
	return vertex.getClientId();
}
////////////////////////////////////////////
long GLI::GLCncPath::previewNextVertexNormalizedId() {
////////////////////////////////////////////
	return ClientIds::normalize(previewNextVertexId());
}
////////////////////////////////////////////
long GLI::GLCncPath::previewPreviousVertexNormalizedId() {
////////////////////////////////////////////
	return ClientIds::normalize(previewPreviousVertexId());
}
////////////////////////////////////////////
void GLI::GLCncPath::incVirtualEndById() {
////////////////////////////////////////////
	long id = getVirtualEndAsNormalizedId();
	
	publishNotifications = false;
		
		do {
			incVirtualEnd();
			
			if ( virtualEnd >= (long)(vectiesBuffer.getVertexCount() - 1) )
				break;
		
		} while ( id == getVirtualEndAsNormalizedId() );
		
	publishNotifications = true;
	notifyCncPathChanged();
}
////////////////////////////////////////////
void GLI::GLCncPath::decVirtualEndById() {
////////////////////////////////////////////
	long id = getVirtualEndAsNormalizedId();
	
	publishNotifications = false;
		
		do {
			decVirtualEnd();
			
			if ( virtualEnd <= 1 )
				break;
				
		} while ( id == getVirtualEndAsNormalizedId() );
	
	publishNotifications = true;
	notifyCncPathChanged();
}
////////////////////////////////////////////
const float GLI::GLCncPath::getMinScaleFact() const {
////////////////////////////////////////////
	return 0.1;
}
////////////////////////////////////////////
const float GLI::GLCncPath::getAutoScaleFact() const {
////////////////////////////////////////////
	if ( vectiesBuffer.getVertexCount() < 1 )
		return getMinScaleFact();
		
	const float x = minVecties.getX(), X = maxVecties.getX();
	const float y = minVecties.getY(), Y = maxVecties.getY();
	const float z = minVecties.getZ(), Z = maxVecties.getZ();
	
	const float totalDistX = X - x;
	const float totalDistY = Y - y;
	const float totalDistZ = Z - z;
	
	// range: -2 >= ret <= 2
	const float ret = std::max(std::max(totalDistZ, totalDistY), totalDistX);
	
	//                                      * 1.1 => 90%
	return std::max(getMinScaleFact(), ret) * 1.1;
}
////////////////////////////////////////////
const GLI::BoundBox& GLI::GLCncPath::evaluateBoundBox(const GLCncPathVertices& min, const GLCncPathVertices& max) {
////////////////////////////////////////////
	static GLI::BoundBox bBox;
	bBox.clear();
	
	// a bound box of 3 or less points didn't make sense
	if ( vectiesBuffer.getVertexCount() < 3 )
		return bBox;
	
	const float x = min.getX(), X = max.getX();
	const float y = min.getY(), Y = max.getY();
	const float z = min.getZ(), Z = max.getZ();
	
	// bottom - push_back(BoundBoxLine)
	bBox.push_back(std::make_pair(GLCncPathVertices(x,y,z), GLCncPathVertices(X,y,z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(X,y,z), GLCncPathVertices(X,y,Z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(X,y,Z), GLCncPathVertices(x,y,Z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(x,y,Z), GLCncPathVertices(x,y,z)));
	
	// top - push_back(BoundBoxLine)
	bBox.push_back(std::make_pair(GLCncPathVertices(x,Y,z), GLCncPathVertices(X,Y,z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(X,Y,z), GLCncPathVertices(X,Y,Z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(X,Y,Z), GLCncPathVertices(x,Y,Z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(x,Y,Z), GLCncPathVertices(x,Y,z)));
	
	// perpendicular - push_back(BoundBoxLine)
	bBox.push_back(std::make_pair(GLCncPathVertices(x,y,z), GLCncPathVertices(x,Y,z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(X,y,z), GLCncPathVertices(X,Y,z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(X,y,Z), GLCncPathVertices(X,Y,Z)));
	bBox.push_back(std::make_pair(GLCncPathVertices(x,y,Z), GLCncPathVertices(x,Y,Z)));
	
	return bBox;
}
////////////////////////////////////////////
const GLI::BoundBox& GLI::GLCncPath::getBoundBox() {
////////////////////////////////////////////
	return evaluateBoundBox(minVecties, maxVecties);
}
////////////////////////////////////////////
const GLI::BoundBox& GLI::GLCncPath::getObjectBoundBox() {
////////////////////////////////////////////
	return evaluateBoundBox(minObjVecties, maxObjVecties);
}
