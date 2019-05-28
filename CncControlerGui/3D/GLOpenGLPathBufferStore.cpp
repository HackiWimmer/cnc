#include "3D/GLOpenGLPathBufferStore.h"


/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::GLOpenGLPathBuffer()
: vertexBufferID(0)
, vertexArrayID(0)
, numVertices(0)
, clientIdIndex()
, vertexColours()
/////////////////////////////////////////////////////////////
{
	initBuffer();
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::GLOpenGLPathBuffer(const GLOpenGLPathBuffer& b)
: vertexBufferID(b.getBufferID())
, vertexArrayID(b.getArrayID())
, numVertices(b.getNumVerties())
, clientIdIndex(b.getClientIdIndex())
, vertexColours()
/////////////////////////////////////////////////////////////
{
	// don't call initBuffer() here, because in the openGL context
	// the underlying buffers of the given GLOpenGLPathBuffer object
	// are already initialized
	// initBuffer();
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::~GLOpenGLPathBuffer() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::getVertex(CncVertex& ret, unsigned int idx) const {
/////////////////////////////////////////////////////////////
	
	if ( idx < numVertices ) {
		
		glBindVertexArray(vertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	
			const GLintptr vertexOffset = (idx) * sizeof(CncVertex);
			glGetBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &ret);
		
		glBindVertexArray(0);
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::setColours(const VertexColours& colours) {
/////////////////////////////////////////////////////////////
	vertexColours = colours; 
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::updateVertex(const CncVertex& vertex, unsigned int idx) {
/////////////////////////////////////////////////////////////
	
	if ( idx < numVertices ) {
		
		glBindVertexArray(vertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	
			const GLintptr vertexOffset = (idx) * sizeof(CncVertex);
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
		
		glBindVertexArray(0);
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::appendVertex(const CncVertex& vertex) {
/////////////////////////////////////////////////////////////
	if ( remainingVertiesCount() == 0 )
		return false;
		
	// color synchronizarion
	#warning !!!
	//normalizeVertexColour(vertex);
	
	// buffer management
	glBindVertexArray(vertexArrayID);
	
		const GLintptr vertexOffset = (numVertices) * sizeof(CncVertex);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
	
	glBindVertexArray(0);
	
	++numVertices;
	updateIndex(vertex.getClientId());

	return true;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::destroyBuffer() {
/////////////////////////////////////////////////////////////
	clientIdIndex.clear();

	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::display(DisplayType dt, int vertices) {
/////////////////////////////////////////////////////////////
	const unsigned int displayCount = vertices >= 0 ? std::min((unsigned int)vertices, numVertices) : numVertices;
	//std::cout << "ID[" << vertexBufferID<< "][" << vertexArrayID << "]: " << "displayCount: " << displayCount << std::endl;
	
	// select buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBindVertexArray(vertexArrayID);
	
		// render data
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		const unsigned int displayOffset = 0;
		glDrawArrays(dt, displayOffset, displayCount);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	
	glBindVertexArray(0);
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBuffer::getFirstEntryForClientId(long clientId) const {
/////////////////////////////////////////////////////////////
	long ret = -1;
	
	auto cldIt = clientIdIndex.find(clientId);
	if ( cldIt != clientIdIndex.end() ) {
		const IndexList& index = cldIt->second;
		
		for (auto it = index.begin(); it != index.end(); ++it) {
			if ( ret < 0 )	ret = (long)it->first;
			else 			ret = std::min( (long)it->first, ret);
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBuffer::getIndexForClientIdAsString(long clientId, wxString& ret) {
/////////////////////////////////////////////////////////////
	ret.clear();
	
	auto cldIt = clientIdIndex.find(clientId);
	if ( cldIt != clientIdIndex.end() ) {
		const IndexList& index = cldIt->second;
		
		for (auto it = index.begin(); it != index.end(); ++it) {
			ret.append(wxString::Format("%u", it->first));
			
			if ( std::distance(index.begin(), it) < (long)(index.size() - 1) )
				ret.append(",");
		}
	}
	
	return ret;
}





/////////////////////////////////////////////////////////////
GLOpenGLPathBufferStore::GLOpenGLPathBufferStore()
: initialized(false)
, bufferStore()
, colourStore()
, lastVertex()
/////////////////////////////////////////////////////////////
{
	init();
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBufferStore::~GLOpenGLPathBufferStore() {
/////////////////////////////////////////////////////////////
	if ( initialized == true )
		destroyBuffers();
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::addBuffer() {
///////////////////////////////////////////////////////////// 
	GLOpenGLPathBuffer buffer;
	
	// start the new buffer with the last value of the prev buffer
	// to connect the underlying GL_LINE_STRIPS
	buffer.appendVertex(lastVertex);
	
	// add
	bufferStore.push_back(buffer);
}
/////////////////////////////////////////////////////////////
int GLOpenGLPathBufferStore::getBufferStoreIndex(unsigned long idx) const {
/////////////////////////////////////////////////////////////
	return idx / GLOpenGLPathBuffer::vertexCountMax;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::init() {
/////////////////////////////////////////////////////////////
	if ( initialized == false ) {
		if ( GLCommon::isGlewAvailable() ) {
			addBuffer();
			initialized = true;
		}
	}
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::checkInitialized() {
/////////////////////////////////////////////////////////////
	if ( initialized == false ) {
		
		// try to init
		init();
		if ( initialized == false ) {
			std::cerr << "GLOpenGLPathBufferStore::checkInitialized() == false " << std::endl;
		}
	}
	
	return initialized;
}
/////////////////////////////////////////////////////////////
unsigned int GLOpenGLPathBufferStore::getBufferCount() const { 
/////////////////////////////////////////////////////////////
	return bufferStore.size(); 
}
/////////////////////////////////////////////////////////////
unsigned int GLOpenGLPathBufferStore::getTotalSize() const { 
/////////////////////////////////////////////////////////////
	return bufferStore.size() * GLOpenGLPathBuffer::vertexBufferSize; 
}
/////////////////////////////////////////////////////////////
unsigned long GLOpenGLPathBufferStore::getVertexCount() 	const { 
/////////////////////////////////////////////////////////////
	if ( getBufferCount() == 0 )
		return 0L;
		
	return ( getBufferCount() - 1 ) * GLOpenGLPathBuffer::vertexCountMax
		   + bufferStore.back().getNumVerties(); 
}
/////////////////////////////////////////////////////////////
const GLOpenGLPathBuffer::CncVertex& GLOpenGLPathBufferStore::getLastVertex() const { 
/////////////////////////////////////////////////////////////
	return lastVertex; 
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::getVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) const {
/////////////////////////////////////////////////////////////
	if ( idx < getVertexCount() ) {
		const unsigned int bufferIdx  = getBufferStoreIndex(idx);
		const unsigned long offset 	  = bufferIdx * GLOpenGLPathBuffer::vertexCountMax;
		
		if ( offset <= idx ) {
			const unsigned int rest	  = idx - offset;
			
			bufferStore[bufferIdx].getVertex(ret, rest);
			return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::destroyBuffers() {
/////////////////////////////////////////////////////////////
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
		it->destroyBuffer();
		
	bufferStore.clear();
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::normalizeClientID(long clientId) {
/////////////////////////////////////////////////////////////
	bool ret = false;
	
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
		if ( it->normalizeClientID(clientId) == true )
			ret = true;
			
	return ret;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::updateVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) {
/////////////////////////////////////////////////////////////
	if ( idx < getVertexCount() ) {
		const unsigned int bufferIdx  = getBufferStoreIndex(idx);
		const unsigned long offset 	  = bufferIdx * GLOpenGLPathBuffer::vertexCountMax;
		
		if ( offset <= idx ) {
			const unsigned int rest	  = idx - offset;
			
			bufferStore[bufferIdx].updateVertex(ret, rest);
			return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
unsigned long GLOpenGLPathBufferStore::appendVertex(const GLOpenGLPathBuffer::CncVertex& vertex) {
/////////////////////////////////////////////////////////////
	if ( checkInitialized() == false )
		return false;

	if ( bufferStore.size() == 0 )
		addBuffer();
	
	if ( bufferStore.back().more() == false )
		addBuffer();
	
	lastVertex = vertex;
	return bufferStore.back().appendVertex(vertex) ? getVertexCount() : 0;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::highlightClientID(long clientId) {
/////////////////////////////////////////////////////////////
	bool ret = false;

	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( it->highlightClientID(clientId) == true )
			ret = true;
	}

	return ret;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::display(GLOpenGLPathBuffer::DisplayType dt, long vertices) {
/////////////////////////////////////////////////////////////
	const unsigned int base = vertices < 0 ? bufferStore.size() : vertices / GLOpenGLPathBuffer::vertexCountMax;
	const unsigned int rest = vertices < 0 ?                  0 : vertices % GLOpenGLPathBuffer::vertexCountMax;
	
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( std::distance(bufferStore.begin(), it) < base )	it->display(dt);
		else													it->display(dt, rest);
	}
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBufferStore::findFirstEntryForClientId(long cliendId) {
/////////////////////////////////////////////////////////////
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( it->hasClientID(cliendId) == true ) {
			return it->getFirstEntryForClientId(cliendId);
		}
	}
	
	return -1;
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::VertexColours& GLOpenGLPathBufferStore::getColoursAsReference() {
/////////////////////////////////////////////////////////////
	return colourStore;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::setColours(const GLOpenGLPathBuffer::VertexColours& colours) {
/////////////////////////////////////////////////////////////
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) 
		it->setColours(colours);
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBufferStore::getIndexForClientIdAsString(long clientId, wxString& ret) {
/////////////////////////////////////////////////////////////
	ret.clear();
	
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( it->hasClientID(clientId) ) {
			ret.append(wxString::Format("[%u]: ", std::distance(bufferStore.begin(), it) + 1));
			
			wxString idx;
			ret.append(getIndexForClientIdAsString(clientId, idx));
		}
	}
	
	return ret;
}
