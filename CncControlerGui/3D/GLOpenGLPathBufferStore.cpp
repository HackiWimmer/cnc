#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#include "CncCommon.h"
#include "GLCommon.h"
#include "GlobalFunctions.h"
#include "3D/GLContextCncPathBase.h"
#include "3D/GLOpenGLPathBufferStore.h"

/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::VertexColours GLOpenGLPathBuffer::vertexColours;


/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::VertexColours::VertexColours()
: rapidAlpha	(defRapidAlpha)
, rapid			(wxColour(255, 128,  64, rapidAlpha))
, work			(wxColour(255, 255, 255, GLOpenGLPathBuffer::defWorkAlpha))
, max			(wxColour(255,   0,   0, GLOpenGLPathBuffer::defMaxAlpha))
, user			(wxColour(  0,   0, 255, GLOpenGLPathBuffer::defUserAlpha))
, highlight		(wxColour(255, 255,   0, GLOpenGLPathBuffer::defHighlightAlpha))
/////////////////////////////////////////////////////////////
{
	if ( false ) {
		std::cout	<< "VertexColours():"	<< std::endl
					<< (*this)				<< std::endl
		;
	}
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::VertexColours::changeLightness(float lightness) {
/////////////////////////////////////////////////////////////
	if ( lightness < 0.0 || lightness > 2.0 )
		lightness = 1.0;
		
	// ChangeLightness() is a utility function that simply darkens or lightens a color, 
	// based on the specified percentage ialpha of 
	// -   0 would be completely black, 
	// - 200 completely white and ialpha of 
	// - 100 returns the colour. 
	
	rapid.	Set(rapid.Red(), rapid.Green(), rapid.Blue(), (int)(lightness * rapidAlpha));
	work.	Set(work.Red(),  work.Green(),  work.Blue(),  (int)(lightness * GLOpenGLPathBuffer::defWorkAlpha));
	max.	Set(max.Red(),   max.Green(),   max.Blue(),   (int)(lightness * GLOpenGLPathBuffer::defMaxAlpha));
	user.	Set(user.Red(),  user.Green(),  user.Blue(),  (int)(lightness * GLOpenGLPathBuffer::defUserAlpha));
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::VertexColours::showRapidPathes(bool state) {
/////////////////////////////////////////////////////////////
	rapid.Set(rapid.Red(), rapid.Green(), rapid.Blue(), state == true ? rapid.Alpha() : 0);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::VertexColours::restoreLightness() {
/////////////////////////////////////////////////////////////
	changeLightness(GLOpenGLPathBuffer::dimUpFact);
}



/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::CncVertex::updateColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
/////////////////////////////////////////////////////////////
	colour[CncVertexColourR] = r;
	colour[CncVertexColourG] = g;
	colour[CncVertexColourB] = b;
	colour[CncVertexColourA] = a;
}

/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::CncVertex::changeLightness(unsigned char a) {
/////////////////////////////////////////////////////////////
	colour[CncVertexColourA] = a;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::CncVertex::changeLightness(float lightness) {
/////////////////////////////////////////////////////////////
	if ( lightness < 0.0 || lightness > 2.0 )
		lightness = 1.0;
	
	const VertexColours& vc = GLOpenGLPathBuffer::vertexColours;
	unsigned char alpha = 0;
	
	switch ( getType() ) {
		case cnc::RAPID_SPEED_CHAR:			alpha =  vc.rapidAlpha;						break;
		case cnc::WORK_SPEED_CHAR:			alpha =  GLOpenGLPathBuffer::defWorkAlpha;	break;
		case cnc::MAX_SPEED_CHAR:			alpha =  GLOpenGLPathBuffer::defMaxAlpha;	break;
		case cnc::USER_DEFIND_SPEED_CHAR:	alpha =  GLOpenGLPathBuffer::defUserAlpha;	break;
	}
	
	colour[CncVertexColourA] = (unsigned char)(alpha * lightness);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::CncVertex::initializeVertexColour() {
/////////////////////////////////////////////////////////////
	const VertexColours& vc = GLOpenGLPathBuffer::vertexColours;
	
	switch ( getType() ) {
		case cnc::RAPID_SPEED_CHAR:	updateColour(vc.rapid.Red(),vc.rapid.Green(),	vc.rapid.Blue(),	vc.rapidAlpha);						break;
		case cnc::WORK_SPEED_CHAR:	updateColour(vc.work.Red(),	vc.work.Green(),	vc.work.Blue(),		GLOpenGLPathBuffer::defWorkAlpha);	break;
		case cnc::MAX_SPEED_CHAR:	updateColour(vc.max.Red(),	vc.max.Green(),		vc.max.Blue(),		GLOpenGLPathBuffer::defMaxAlpha);	break;
		default:					updateColour(vc.user.Red(),	vc.user.Green(),	vc.user.Blue(),		GLOpenGLPathBuffer::defUserAlpha); 
	}
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::CncVertex::normalizeVertexColour() {
/////////////////////////////////////////////////////////////
	const VertexColours& vc = GLOpenGLPathBuffer::vertexColours;
	
	switch ( getType() ) {
		case cnc::RAPID_SPEED_CHAR:	updateColour(vc.rapid.Red(),vc.rapid.Green(),	vc.rapid.Blue(),	vc.rapid.Alpha());	break;
		case cnc::WORK_SPEED_CHAR:	updateColour(vc.work.Red(),	vc.work.Green(),	vc.work.Blue(),		vc.work.Alpha());	break;
		case cnc::MAX_SPEED_CHAR:	updateColour(vc.max.Red(),	vc.max.Green(),		vc.max.Blue(),		vc.max.Alpha());	break;
		default:					updateColour(vc.user.Red(),	vc.user.Green(),	vc.user.Blue(),		vc.user.Alpha()); 
	}
}




/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::GLOpenGLPathBuffer(GLOpenGLPathBufferStore* s, GLOpenGLPathBuffer::CncVertex* firstVertex)
: store(s)
, vertexBufferID(0)
, vertexArrayID(0)
, numVertices(0)
, clientIdIndex()
/////////////////////////////////////////////////////////////
{
	generateBuffer();
	
	if ( vertexBufferID != 0 ) {
		if ( firstVertex != NULL )
			appendVertex(*firstVertex);
	} 
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::GLOpenGLPathBuffer(const GLOpenGLPathBuffer& b)
: store(b.getBufferStore())
, vertexBufferID(b.getBufferID())
, vertexArrayID(b.getArrayID())
, numVertices(b.getNumVerties())
, clientIdIndex(b.getClientIdIndex())
/////////////////////////////////////////////////////////////
{
	// don't call generateBuffers() here, because in the openGL context
	// the underlying buffers of the given GLOpenGLPathBuffer object
	// are already initialized
	// generateBuffers();
	
	if ( GL_ERROR_TRACE_LEVEL > 0 ) {
		std::stringstream ss;
		ss << "Create new GLOpenGLPathBuffer: ";
		traceParameters(ss);
		GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, ss.str());
	}
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBuffer::~GLOpenGLPathBuffer() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::traceParameters(std::ostream& os, bool indent, bool oneLine) const {
/////////////////////////////////////////////////////////////
	const char* delm 		= oneLine ? ", " : "\n";
	const char* storeName 	= store ? store->getInstanceName() : _("Unknown Store"); 
	const uint64_t storeID	= store ? store->getInstance() : 0; 
	
	if ( indent == true )
		os << "\t"; 
	
	os << "GLOpenGLPathBuffer Parameters" 				<< delm;
	os << "Store Name = "  << storeName					<< delm;
	os << "Buffer ID = "   << vertexBufferID 			<< delm;
	os << "Vertex ID = "   << vertexArrayID 			<< delm;
	os << "Vertex Num = "  << numVertices				<< delm;
	os << "Store PTR = "   << storeID				 	<< delm;
	os << "Buffer PTR = "  << this->getInstance()		<< delm;
	 
	os << std::endl;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::setColours(const VertexColours& colours) {
/////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::vertexColours = colours; 
	
	// overrule alpha
	GLOpenGLPathBuffer::vertexColours.rapid = wxColour(colours.rapid.Red(), 
													   colours.rapid.Green(), 
													   colours.rapid.Blue(), 
													   GLOpenGLPathBuffer::defRapidAlpha);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::setLightness(float lightness) {
/////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::vertexColours.changeLightness(lightness);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::dimDownColours() {
/////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::vertexColours.changeLightness(GLOpenGLPathBuffer::dimDownFact);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::dimUpColours() {
/////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::vertexColours.restoreLightness();
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBuffer::getStoreInstanceName() const {
/////////////////////////////////////////////////////////////
	return store ? store->getInstanceName() : _("???");
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBuffer::getInstanceName() const {
/////////////////////////////////////////////////////////////
	return _(wxString::Format("%s[ptr=%" PRIu64 "]", getStoreInstanceName(), getInstance()));
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::traceIdentifier(std::ostream& os, const char* prepend) const {
/////////////////////////////////////////////////////////////
	if ( prepend != NULL )
		os << prepend << ": ";
	
	os << wxString::Format("%s(vaid = %2u, buid = %2u)", getInstanceName(), getArrayID(), getBufferID());
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::traceIdentifierEndl(std::ostream& os, const char* prepend) const {
/////////////////////////////////////////////////////////////
	traceIdentifier(os, prepend);
	os << std::endl;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::generateBuffer() {
/////////////////////////////////////////////////////////////
	glGenBuffers(1, &vertexBufferID);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't create a new buffer VBO!"));
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_DYNAMIC_DRAW);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't define buffer VBO!"));
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::resetBuffer() {
/////////////////////////////////////////////////////////////
	clientIdIndex.clear();
	numVertices 	= 0;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::destroyBuffer() {
/////////////////////////////////////////////////////////////
	clientIdIndex.clear();
	
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	
	numVertices 	= 0;
	vertexArrayID	= 0;
	vertexBufferID	= 0;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::createVertexArray() {
/////////////////////////////////////////////////////////////
	//destroyVertexArray();
		
	glGenVertexArrays(1, &vertexArrayID);
	if ( GL_COMMON_CHECK_ERROR> 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't assign vertex array!"));

	glBindVertexArray(vertexArrayID);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't bind vertex array VAO!"));
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't bind buffer VBO!"));
	
	// vertex
	char *offset = (char*)NULL;
	glVertexPointer(CncVertexAxisValueCount, GL_FLOAT, sizeof(CncVertex), offset);
	glEnableClientState(GL_VERTEX_ARRAY);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't define vertex pointer!"));
	
	// color
	offset = (char*)NULL + CncVertexAxisValueCount * sizeof(vertex_type);
	glColorPointer(CncVertexColourValueCount, GL_UNSIGNED_BYTE, sizeof(CncVertex), offset);
	glEnableClientState(GL_COLOR_ARRAY);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't define colour pointer!"));
	
	glBindVertexArray(0);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't reset vertex array VAO!"));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if ( GL_COMMON_CHECK_ERROR > 0 ) traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Can't reset buffer VBO!"));
	
	if ( GL_ERROR_TRACE_LEVEL > 0 )
		GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("VAO initialized. VAO=%u", vertexArrayID));
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::destroyVertexArray() {
/////////////////////////////////////////////////////////////
	if ( vertexArrayID != 0 ) {
		glDeleteVertexArrays(1, &vertexArrayID);
		vertexArrayID = 0;
		
		if ( GL_ERROR_TRACE_LEVEL > 0 )
			GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("VAO=%u destroyed.", vertexArrayID));
	}
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::getPosVertex(CncVertex& ret, unsigned int idx) const {
/////////////////////////////////////////////////////////////
	if ( idx < numVertices ) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	
			const GLintptr vertexOffset = (idx) * sizeof(CncVertex);
			glGetBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &ret);
			
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::updateVertex(const CncVertex& vertex, unsigned int idx) {
/////////////////////////////////////////////////////////////
	if ( idx < numVertices ) {
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		
			const GLintptr vertexOffset = (idx) * sizeof(CncVertex);
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
			
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::appendVertex(const CncVertex& vertex) {
/////////////////////////////////////////////////////////////
	if ( remainingVertiesCount() == 0 )
		return false;
	
	// buffer management
	const GLintptr vertexOffset = (numVertices) * sizeof(CncVertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		
		glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	++numVertices;
	updateIndex(vertex.getClientId());

	return true;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::display(DisplayType dt, int vertices) {
/////////////////////////////////////////////////////////////
	/*
	if ( numVertices < 2 )
		return;
	
	if ( GL_COMMON_CHECK_ERROR > 0 ) {
		traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Previous Error"));
		return;
	}
	
	if ( vertexArrayID == 0 || vertexBufferID == 0 ) {
		traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT_A("Invalid IDs"));
		return;
	}
	*/
	
	if ( vertexArrayID == 0 )
		createVertexArray();
	
	glBindVertexArray(vertexArrayID);
	
		if ( GL_COMMON_CHECK_ERROR > 0 ) {
			traceIdentifierEndl(std::cerr, CNC_LOG_FUNCT);
			traceParameters(std::cerr);
			return;
		}
		
		const unsigned int displayCount = vertices >= 0 ? std::min((unsigned int)vertices, numVertices) : numVertices;
		const unsigned int displayOffset = 0;
		
		if ( GL_ERROR_TRACE_LEVEL > 0 ) 
			GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("glDrawArrays(%d, %u, %u) - VAO=%u, VBO=%u", dt, displayOffset, displayCount, vertexArrayID, vertexBufferID));

		//#warning
		glDrawArrays(dt, displayOffset, displayCount);
		
	glBindVertexArray(0);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::reconstruct(const ReconstructOptions& opt) {
/////////////////////////////////////////////////////////////
	// perform options
	GLOpenGLPathBuffer::vertexColours.showRapidPathes(opt.showRapidPathes);
	
	// over all verties
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	
		CncVertex vertex;
		for ( unsigned int i = 0; i < numVertices; i++ ) {
			const GLintptr vertexOffset = i * sizeof(CncVertex);
			
			glGetBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
			vertex.reconstruct();
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
		}
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GL_COMMON_CHECK_ERROR;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::hasClientID(long clientId, bool exact) const { 
/////////////////////////////////////////////////////////////
	const bool ret = clientIdIndex.find(clientId) != clientIdIndex.end(); 
	if ( ret == true )
		return ret;
	
	if ( exact == true)
		return ret;
		
	// use upper bound instead of find here to get all sub ids also.
	// 180 --> 18 * 10 = 180
	// 182 --> 18 * 10 = 180
	const long firstClientId = ( (clientId / CLIENT_ID.TPL_FACTOR) ) * CLIENT_ID.TPL_FACTOR;
	return clientIdIndex.upper_bound(firstClientId) != clientIdIndex.end(); 
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBuffer::getFirstIndexForClientId(long clientId) const {
/////////////////////////////////////////////////////////////
	long ret = CLIENT_ID.INVALID;
	
	auto cldIt = clientIdIndex.find(clientId);
	if ( cldIt != clientIdIndex.end() )
		return (long)std::distance(clientIdIndex.begin(), cldIt);
	
	return ret;
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBuffer::getFirstEntryForClientId(long clientId) const {
/////////////////////////////////////////////////////////////
	long ret   = CLIENT_ID.INVALID;
	auto cldIt = clientIdIndex.find(clientId);
	
	if ( cldIt == clientIdIndex.end() ) {
		// auto cldIt = clientIdIndex.find(clientId);
		// use upper bound instead of find here to get all sub ids also.
		// 180 --> 18 * 10 = 180
		// 182 --> 18 * 10 = 180
		const long firstClientId = ( (clientId / CLIENT_ID.TPL_FACTOR) ) * CLIENT_ID.TPL_FACTOR;
		cldIt = clientIdIndex.upper_bound(firstClientId);
	}
	
	if ( cldIt != clientIdIndex.end() ) {
		const IndexList& index = cldIt->second;
		
		for (auto it = index.begin(); it != index.end(); ++it) {
			if ( ret < 0 )	ret = (long)it->first;
			else 			ret = std::min((long)it->first, ret);
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBuffer::getLastEntryForClientId(long clientId) const {
/////////////////////////////////////////////////////////////
	long ret   = CLIENT_ID.INVALID;
	auto cldIt = clientIdIndex.find(clientId);
	
	if ( cldIt == clientIdIndex.end() ) {
		// auto cldIt = clientIdIndex.find(clientId);
		// use lower bound instead of find here to get all sub ids also.
		// 180 --> 18 + 1 = 19 * 10 = 190
		// 182 --> 18 + 1 = 19 * 10 = 190
		const long nextClientId = ( (clientId / CLIENT_ID.TPL_FACTOR) + 1 ) * CLIENT_ID.TPL_FACTOR;
		cldIt = clientIdIndex.lower_bound(nextClientId);
	}
	
	if ( cldIt != clientIdIndex.end() ) {
		const IndexList& index = cldIt->second;
		
		for (auto it = index.begin(); it != index.end(); ++it) {
			if ( ret < 0 )	ret = (long)it->first;
			else 			ret = std::max((long)it->first, ret);
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBuffer::getIndexForClientIdAsString(long clientId, wxString& ret, bool summerize) {
/////////////////////////////////////////////////////////////
	ret.clear();
	
	auto cldIt = clientIdIndex.find(clientId);
	if ( cldIt != clientIdIndex.end() ) {
		const IndexList& index = cldIt->second;
		
		unsigned int lastIdx = 0;
		for (auto it = index.begin(); it != index.end(); ++it) {
			
			if ( summerize == false ) {
				ret.append(wxString::Format("%u", it->first));
				
				if ( std::distance(index.begin(), it) < (long)(index.size() - 1) )
					ret.append(",");
			}
			else {
				
				// first ... 
				if ( std::distance(index.begin(), it) == 0 ) {
					ret.append(wxString::Format("%u", it->first));
					lastIdx = it->first;
				}
				else {
					// last ... 
					if ( std::distance(index.begin(), it) == (long)(index.size() - 1) ) {
						if ( it->first == lastIdx + 1 ) ret.append(wxString::Format("-%u", it->first));
						else							ret.append(wxString::Format(",%u", it->first));
					}
					else {
						if ( it->first == lastIdx + 1 ) lastIdx = it->first;
						else							ret.append(wxString::Format("-%u,%u", lastIdx, it->first));
					}
				}
				
				
			}
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBuffer::updateIndex(long clientID) {
/////////////////////////////////////////////////////////////
	const int idx = numVertices - 1;
	if ( idx < 0 )
		return;

	auto it = clientIdIndex.find(clientID);
	if ( it != clientIdIndex.end() ) {
		IndexList& index = it->second;
		index[idx] = true;
		clientIdIndex[clientID] = index;
	}
	else {
		IndexList index;
		index[idx] = true;
		clientIdIndex[clientID] = index;
	}
}
///////////////////////////////////////////////////////////
bool GLOpenGLPathBuffer::changeColourForClientID(long clientId, const char type) {
/////////////////////////////////////////////////////////////
	auto cldIt = clientIdIndex.find(clientId);

	if ( cldIt == clientIdIndex.end() )
		return false;

	CncVertex vertex;
	bool ret = false;
	
	for ( auto idxIt = cldIt->second.begin(); idxIt != cldIt->second.end(); ++idxIt) {
		if ( getPosVertex(vertex, idxIt->first) == true ) {
		
			switch ( type ) {
				case 'H':		vertex.updateColour(vertexColours.highlight.Red(), vertexColours.highlight.Green(), vertexColours.highlight.Blue(), vertexColours.highlight.Alpha()); 
								break;
								
				case 'D':		vertex.normalizeVertexColour(); 
								vertex.changeLightness(GLOpenGLPathBuffer::dimDownFact);
								break;
								
				case 'U':		vertex.normalizeVertexColour();
								vertex.changeLightness(GLOpenGLPathBuffer::dimUpFact);
								break;
								
				default:		vertex.normalizeVertexColour();
			}
			
			updateVertex(vertex, idxIt->first);
			
			ret = true;
		}
	}
	
	return ret;
}




GLOpenGLPathBuffer::CncVertex GLOpenGLPathBufferStore::defaultVertex('R', -1, 0.0, 0.0, 0.0);

/////////////////////////////////////////////////////////////
GLOpenGLPathBufferStore::GLOpenGLPathBufferStore(const wxString& instanceName)
: initialized(false)
, instanceName(instanceName)
, currentBufferIndex(0)
, bufferStore()
, lastVertex(defaultVertex)
/////////////////////////////////////////////////////////////
{
	init(lastVertex);
}
/////////////////////////////////////////////////////////////
GLOpenGLPathBufferStore::~GLOpenGLPathBufferStore() {
/////////////////////////////////////////////////////////////
	if ( initialized == true )
		destroyBuffers();
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::createVertexArray() {
/////////////////////////////////////////////////////////////
	if ( initialized == true ) {
		for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it )
			it->createVertexArray();
			
		if ( GL_ERROR_TRACE_LEVEL > 0 )	{
			if ( bufferStore.size() == 0 )
				GL_CTX_OBS->appendMessage('W', CNC_LOG_FUNCT, wxString::Format("Buffer count = 0!; Initialized = %d", initialized));
		}
	}
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::destroyVertexArray() {
/////////////////////////////////////////////////////////////
	if ( initialized == true ) {
		for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it )
			it->destroyVertexArray();
			
		if ( GL_ERROR_TRACE_LEVEL > 0 )	{
			if ( bufferStore.size() == 0 )
				GL_CTX_OBS->appendMessage('W', CNC_LOG_FUNCT, wxString::Format("Buffer count = 0!; Initialized = %d", initialized));
		}
	}
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBufferStore::getInstanceFullName() const {
/////////////////////////////////////////////////////////////
	return _(wxString::Format("%s(PTR=%" PRIu64 ")", instanceName, getInstance()));
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::addBuffer() {
///////////////////////////////////////////////////////////// 
	const GLOpenGLPathBuffer buffer(this, &lastVertex);
	
	// add, please not this creates a copy of buffer
	bufferStore.push_back(buffer);
	currentBufferIndex = bufferStore.size() - 1;
}
/////////////////////////////////////////////////////////////
int GLOpenGLPathBufferStore::getBufferStoreIndex(unsigned long idx) const {
/////////////////////////////////////////////////////////////
	if ( getVertexCount() == 0 || idx > getVertexCount() - 1 )
		return -1;
	
	return idx / GLOpenGLPathBuffer::vertexCountMax;
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBufferStore::getBufferId(unsigned long idx) const {
/////////////////////////////////////////////////////////////
	const int bufferStoreIdx = getBufferStoreIndex(idx);
	if ( bufferStoreIdx < 0 ) 
		return -1L;
		
	return bufferStore.at(bufferStoreIdx).getBufferID();
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::init() {
/////////////////////////////////////////////////////////////
	init(defaultVertex);
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::init(const GLOpenGLPathBuffer::CncVertex& firstVertex) {
/////////////////////////////////////////////////////////////
	if ( initialized == false ) {
		if ( GLCommon::isGlewAvailable() ) {
			lastVertex = firstVertex;
			
			addBuffer();
			initialized = true;
		}
	}
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::resetBuffers() {
/////////////////////////////////////////////////////////////
	if ( initialized == true  ) {
		for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
			it->resetBuffer();
			
		lastVertex 			= defaultVertex;
		currentBufferIndex 	= 0;
	}
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::destroyBuffers() {
/////////////////////////////////////////////////////////////
	if ( initialized == true  ) {
		for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
			it->destroyBuffer();
			
		bufferStore.clear();
		initialized = false;
		
		lastVertex 			= defaultVertex;
		currentBufferIndex 	= 0;
	}
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::initialize() {
/////////////////////////////////////////////////////////////
	if ( initialized == false ) {
		// try to init
		init();
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
unsigned long GLOpenGLPathBufferStore::getVertexCount() const { 
/////////////////////////////////////////////////////////////
	unsigned long count = 0L;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it )
		count += it->getNumVerties();
	
	return count;
}
/////////////////////////////////////////////////////////////
const GLOpenGLPathBuffer::CncVertex& GLOpenGLPathBufferStore::getLastVertex() const { 
/////////////////////////////////////////////////////////////
	return lastVertex; 
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::getDirVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) const {
/////////////////////////////////////////////////////////////
	if ( initialized == false )
		return false;
	
	GLOpenGLPathBuffer::CncVertex v0('R', CLIENT_ID.INVALID, 0.0, 0.0, 0.0);
	
	if ( idx > 0 && idx < getVertexCount() ) {
		
		if ( idx > 1 ) {
			if ( getPosVertex(v0, idx - 1 ) == false )
				return false;
		}
		
		GLOpenGLPathBuffer::CncVertex v1;
		if ( getPosVertex(v1, idx ) == false )
			return false;
			
		// ret = v1 - v0;
		ret.set(v1.getType(), v1.getClientId(), (v1.getX() - v0.getX()), (v1.getY() - v0.getY()), (v1.getZ() - v0.getZ()) );
		
	}
	else {
		ret.set(v0);
	}
	
	return true;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::getPosVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) const {
/////////////////////////////////////////////////////////////
	if ( initialized == false )
		return false;
		
	if ( idx < getVertexCount() ) {
		const unsigned int bufferIdx  = getBufferStoreIndex(idx);
		const unsigned long offset 	  = bufferIdx * GLOpenGLPathBuffer::vertexCountMax;
		
		if ( offset <= idx ) {
			const unsigned int rest	  = idx - offset;
			
			bufferStore[bufferIdx].getPosVertex(ret, rest);
			return true;
		}
	}
	
	return false;
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
	if ( initialize() == false )
		return 0;
		
	if ( bufferStore.size() == 0 )
		addBuffer();
		
	if ( bufferStore[currentBufferIndex].more() == false ) {
		if ( currentBufferIndex + 1 >= bufferStore.size() )		addBuffer();
		else 													++currentBufferIndex;
	}
	
	lastVertex = vertex;
	return bufferStore[currentBufferIndex].appendVertex(vertex) ? getVertexCount() : 0;
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
bool GLOpenGLPathBufferStore::dimDownClientID(long clientId) {
/////////////////////////////////////////////////////////////
	bool ret = false;
	
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( it->dimDownClientID(clientId) == true )
			ret = true;
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
bool GLOpenGLPathBufferStore::dimUpClientID(long clientId) {
/////////////////////////////////////////////////////////////
	bool ret = false;
	
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( it->dimUpClientID(clientId) == true )
			ret = true;
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::display(GLOpenGLPathBuffer::DisplayType dt, long vertices) {
/////////////////////////////////////////////////////////////
	if ( initialized == false )
		return;
	
	if ( vertices <= 0 )
		return;
		
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		
		count += it->getNumVerties();

		if ( (unsigned long )vertices < count ) {
			
			const unsigned int pos = vertices - ( count - it->getNumVerties() );
			it->display(dt, pos);
			break;
		}
		
		it->display(dt); 
	}
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBufferStore::findFirstEntryForClientId(long cliendId) {
/////////////////////////////////////////////////////////////
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {

		if ( it->hasClientID(cliendId) == true )
			return count + it->getFirstEntryForClientId(cliendId);
		
		count += it->getNumVerties();
	}
	
	return CLIENT_ID.INVALID;
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBufferStore::findFirstIndexForClientId(long cliendId) {
/////////////////////////////////////////////////////////////
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		
		const long firstEntry = it->getFirstIndexForClientId(cliendId);
		if ( firstEntry >= 0 )
			return count + firstEntry;
			
		count += it->getClientIdCount();
	}
	
	return CLIENT_ID.INVALID;;
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBufferStore::findLastEntryForClientId(long cliendId) {
/////////////////////////////////////////////////////////////
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		
		const long lastEntry = it->getLastEntryForClientId(cliendId);
		if ( lastEntry >= 0 )
			return count + lastEntry;
		
		count += it->getNumVerties();
	}
	
	return CLIENT_ID.INVALID;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::setColours(const GLOpenGLPathBuffer::VertexColours& colours) {
/////////////////////////////////////////////////////////////
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) 
		it->setColours(colours);
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBufferStore::getVaoAndVboSummary() {
/////////////////////////////////////////////////////////////
	static wxString ret;
	ret.clear();
	
	unsigned int i = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it, i++ ) {
		ret.append(wxString::Format("{ IDX=%u, VBO=%u, VAO=%u }", i + 1, it->getBufferID(), it->getArrayID() ));
		
		if ( i < bufferStore.size() - 1 )
			ret.append(", ");
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBufferStore::getIndexForIdxAsString(unsigned long idx, wxString& ret, bool summerize) {
/////////////////////////////////////////////////////////////
	ret.clear();
	
	const long clientId = getClientIdForIdx(idx);
	
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		count += it->getClientIdCount();
		
		if ( idx < count ) {
			wxString s;
			ret.append(it->getIndexForClientIdAsString(clientId, s, summerize));
			break;
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
const wxString& GLOpenGLPathBufferStore::getIndexForClientIdAsString(long clientId, wxString& ret, bool summerize) {
/////////////////////////////////////////////////////////////
	ret.clear();
	
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		if ( it->hasClientID(clientId) ) {
			const unsigned int d = std::distance(bufferStore.begin(), it) + 1;
			ret.append(wxString::Format("[%u]: ", d));
			
			wxString idx;
			ret.append(it->getIndexForClientIdAsString(clientId, idx, summerize));
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
void GLOpenGLPathBufferStore::reconstruct(const GLOpenGLPathBuffer::ReconstructOptions& opt) {
/////////////////////////////////////////////////////////////
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it )
		it->reconstruct(opt);
}
/////////////////////////////////////////////////////////////
unsigned long GLOpenGLPathBufferStore::getClientIdCount() const {
/////////////////////////////////////////////////////////////
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it )
		count += it->getClientIdCount();
		
	return count;
}
/////////////////////////////////////////////////////////////
unsigned int  GLOpenGLPathBufferStore::getBufferIdForIdx(unsigned long idx) const {
/////////////////////////////////////////////////////////////
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		count += it->getClientIdCount();
		
		if ( idx < count )
			return it->getBufferID();
	}
	
	return 0;
}
/////////////////////////////////////////////////////////////
long GLOpenGLPathBufferStore::getClientIdForIdx(unsigned long idx) const {
/////////////////////////////////////////////////////////////
	if ( idx > getClientIdCount() - 1 ) {
		std::cerr << "GLOpenGLPathBufferStore::getClientIdFor() Invalid index: " << idx << "; Count: " << getClientIdCount() << std::endl;
		return CLIENT_ID.INVALID;
	}
	
	unsigned long count = 0;
	for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
		count += it->getClientIdCount();
		
		if ( idx < count) {
			const GLOpenGLPathBuffer::ClientIdIndex& cldIdx = it->getClientIdIndex();
			const unsigned int pos = count - 1 - idx;
			
			if ( pos <= cldIdx.size() ) {
				auto itt = cldIdx.end();
				
				for (unsigned int i = 0; i <= pos; i++)
					itt--;
					
				return itt->first;
			}
			else {
				std::cerr << "GLOpenGLPathBufferStore::getClientIdFor() !!!!! " << std::endl;
				std::cerr << " pos           : " << pos << std::endl;
				std::cerr << " cldIdx.size() : " << cldIdx.size() << std::endl;
				
				return -77;
			}
		}
	}
	
	return CLIENT_ID.INVALID;
}
