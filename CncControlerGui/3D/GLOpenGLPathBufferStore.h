#ifndef GL_OPENGL_PATHBUFFERSTORE_H
#define GL_OPENGL_PATHBUFFERSTORE_H

#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <wx/string.h>
#include <wx/colour.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>


//-------------------------------------------------------------

class GLOpenGLPathBuffer {
	
	public:
		
		//-----------------------------------------------------
		static unsigned int checkForOpenGlErrors() {
			unsigned int ret = 0;

			GLenum errLast = GL_NO_ERROR;

			for ( ;; )
			{
				GLenum err = glGetError();
				if ( err == GL_NO_ERROR )
					return ret;

				ret++;

				// normally the error is reset by the call to glGetError() but if
				// glGetError() itself returns an error, we risk looping forever here
				// so check that we get a different error than the last time
				if ( err == errLast )
				{
					std::cerr << "OpenGLContextBase::checkGLError(): OpenGL error state couldn't be reset." << std::endl;
					return ret;
				}

				errLast = err;

				std::stringstream ss;
				ss << gluErrorString(err);
				std::cerr << "OpenGLContextBase::checkGLError(): OpenGL error [" << err << " ]: " << ss.str() << std::endl;
			}
			return ret;
		}

		//-----------------------------------------------------
		static void GLAPIENTRY
		MessageCallback( GLenum source,
		                 GLenum type,
		                 GLuint id,
		                 GLenum severity,
		                 GLsizei length,
		                 const GLchar* message,
		                 const void* userParam )
		{
		  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
		            type, severity, message );
		}


		//-----------------------------------------------------
		typedef std::map<unsigned int,bool> IndexList;
		typedef std::map<long, IndexList>   ClientIdIndex;

		typedef GLfloat vertex_type;
		typedef GLubyte colour_type;

		enum DisplayType { DT_DOTS = GL_POINTS, DT_STRIPS = GL_LINE_STRIP};
		
		static const unsigned int 	CncVertexAxisValueCount 		= 3;
		static const unsigned int 		CncVertexAxisX 				= 0;
		static const unsigned int 		CncVertexAxisY 				= 1;
		static const unsigned int 		CncVertexAxisZ 				= 2;
		
		static const unsigned int 	CncVertexColourValueCount		= 4;
		static const unsigned int 		CncVertexColourR			= 0;
		static const unsigned int 		CncVertexColourG			= 1;
		static const unsigned int 		CncVertexColourB			= 2;
		static const unsigned int 		CncVertexColourA			= 3;
		
		//-----------------------------------------------------
		struct CncVertex {
			private:
				// with respect to the openGl buffer alignment 
				// don't change the following variable order
				vertex_type vertex[CncVertexAxisValueCount];
				colour_type colour[CncVertexColourValueCount];

				// specify additional data from here on . . . 
				//clientID as wells as type isn't edible after creation!
				char type 		= '\0';
				long clientID 	= -1;

			protected:
			
				//-------------------------------------------------
				void updateColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
					colour[CncVertexColourR] = r;
					colour[CncVertexColourG] = g;
					colour[CncVertexColourB] = b;
					colour[CncVertexColourA] = a;
				}
				
				friend class GLOpenGLPathBuffer;
				
			public:

				//-------------------------------------------------
				long getClientId()	const { return clientID; }
				char getType()		const { return type; }

				//-------------------------------------------------
				CncVertex()
				: clientID(-1)
				{
					memset(vertex,  0, CncVertexAxisValueCount   * sizeof(vertex_type));
					memset(colour, 	0, CncVertexColourValueCount * sizeof(colour_type));
				}

				//-------------------------------------------------
				explicit CncVertex(char type, long clientID)
				: type(type)
				, clientID(clientID)
				{
					memset(vertex,  0, CncVertexAxisValueCount   * sizeof(vertex_type));
					memset(colour, 	0, CncVertexColourValueCount * sizeof(colour_type));
				}

				//-------------------------------------------------
				explicit CncVertex(const CncVertex& v) {
					*this = v;
				}

				//-------------------------------------------------
				CncVertex(char type, long clientID, float x, float y, float z)
				: type(type)
				, clientID(clientID)
				{
					vertex[CncVertexAxisX] = x;
					vertex[CncVertexAxisY] = y;
					vertex[CncVertexAxisZ] = z;
					memset(colour, 	0, CncVertexColourValueCount * sizeof(GLubyte));
				}

			//-------------------------------------------------
			friend std::ostream &operator<< (std::ostream &ostr, const CncVertex& a) {
				ostr << '['
					 << a.clientID
					 << ']'
					 << '['
					 << (a.type != '\0' ? a.type : '-')
					 << ']'
					 << '['
					 << a.vertex[CncVertexAxisX] << ','
					 << a.vertex[CncVertexAxisY] << ','
					 << a.vertex[CncVertexAxisZ]
					 << ']'
					 << '['
					 << (int)a.colour[CncVertexColourR] << ','
					 << (int)a.colour[CncVertexColourG] << ','
					 << (int)a.colour[CncVertexColourB] << ','
					 << (int)a.colour[CncVertexColourA]
					 << ']'
				;
				
				return ostr;
			}
		};
		
		static const GLuint 		vertexCountMax			= 4;
		static const GLsizeiptr 	vertexBufferSize		= vertexCountMax * sizeof(CncVertex);
		static const GLuint 		vertexAxisTypeSize		= sizeof(vertex_type);
		static const GLuint 		vertexColourTypeSize	= sizeof(colour_type);
		
	private:

		// --------------------------------------------------------------
		struct VertexColours {
			wxColour	rapid		= wxColour(255, 128,  64);
			wxColour	work		= wxColour(255, 255, 255);
			wxColour	max			= wxColour(255,   0,   0);
			wxColour	user		= wxColour(  0,   0, 255);
			wxColour	highlight	= wxColour(255, 255,   0);
		};

		// --------------------------------------------------------------
		unsigned int 	vertexBufferID;
		unsigned int 	vertexArrayID;
		unsigned int 	numVertices; 
		ClientIdIndex	clientIdIndex;
		VertexColours	vertexColours;
		
		// --------------------------------------------------------------
		void initBuffer() {
			glGenBuffers(1, &vertexBufferID);
			
			glBindBuffer(GL_ARRAY_BUFFER, 	vertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, 	vertexBufferSize, NULL, GL_DYNAMIC_DRAW);
			
			glGenVertexArrays(1, &vertexArrayID);
			glBindVertexArray(vertexArrayID);

			// vertex
			char *offset = (char*)NULL;
			glVertexPointer(CncVertexAxisValueCount, GL_FLOAT, sizeof(CncVertex), offset);
			glEnableClientState(GL_VERTEX_ARRAY);

			// color
			offset = (char*)NULL + CncVertexAxisValueCount * sizeof(vertex_type);
			glColorPointer(CncVertexColourValueCount, GL_UNSIGNED_BYTE, sizeof(CncVertex), offset);
			glEnableClientState(GL_COLOR_ARRAY);

			glBindVertexArray(0);

			GLOpenGLPathBuffer::checkForOpenGlErrors();
		}

		// --------------------------------------------------------------
		void updateIndex(long clientID) {
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
		
		// --------------------------------------------------------------
		void normalizeVertexColour(CncVertex& vertex) {
			switch ( vertex.getType() ) {
				case 'R':		vertex.updateColour(vertexColours.rapid.Red(), vertexColours.rapid.Green(), vertexColours.rapid.Blue(), vertexColours.rapid.Alpha()); 
								break;
				case 'W':		vertex.updateColour(vertexColours.work.Red(), vertexColours.work.Green(), vertexColours.work.Blue(), vertexColours.work.Alpha()); 
								break;
				case 'M':		vertex.updateColour(vertexColours.max.Red(), vertexColours.max.Green(), vertexColours.max.Blue(), vertexColours.max.Alpha()); 
								break;
				default:		vertex.updateColour(vertexColours.user.Red(), vertexColours.user.Green(), vertexColours.user.Blue(), vertexColours.user.Alpha()); 
								break;
			}
			
		}

		// --------------------------------------------------------------
		bool changeColourForClientID(long clientId, const char type) {
			auto cldIt = clientIdIndex.find(clientId);

			if ( cldIt == clientIdIndex.end() )
				return false;

			CncVertex vertex;
			bool ret = false;
			
			for ( auto idxIt = cldIt->second.begin(); idxIt != cldIt->second.end(); ++idxIt) {
				if ( getVertex(vertex, idxIt->first) == true ) {
				
					switch ( type ) {
						case 'H':		vertex.updateColour(vertexColours.highlight.Red(), vertexColours.highlight.Green(), vertexColours.highlight.Blue(), vertexColours.highlight.Alpha()); 
										break;
										
						default:		normalizeVertexColour(vertex);
					}
					
					updateVertex(vertex, idxIt->first);
					
					ret = true;
				}
			}
			
			return ret;
		}
		
	public:
		
		GLOpenGLPathBuffer()
		: vertexBufferID(0)
		, vertexArrayID(0)
		, numVertices(0)
		, clientIdIndex()
		, vertexColours()
		{
			initBuffer();
		}
		
		explicit GLOpenGLPathBuffer(const GLOpenGLPathBuffer& b)
		: vertexBufferID(b.getBufferID())
		, vertexArrayID(b.getArrayID())
		, numVertices(b.getNumVerties())
		, clientIdIndex(b.getClientIdIndex())
		, vertexColours()
		{
			// don't call initBuffer() here, because in the openGL context
			// the underlying buffers of the given GLOpenGLPathBuffer object
			// are already initialized
			// initBuffer();
		}

		~GLOpenGLPathBuffer()
		{}
		
		// --------------------------------------------------------------
		unsigned int			getBufferID() 				const { return vertexBufferID; }
		unsigned int			getArrayID() 				const { return vertexArrayID; }
		unsigned int			getNumVerties()				const { return numVertices; }
		unsigned int			remainingVertiesCount()		const { return vertexCountMax - numVertices; }
		
		bool         			more()						const { return remainingVertiesCount() > 0; }
		bool 					hasClientID(long clientID)	const { return clientIdIndex.find(clientID) != clientIdIndex.end(); }
		
		const ClientIdIndex& 	getClientIdIndex()			const { return clientIdIndex; }
		const VertexColours& 	getColours() 				const {	return vertexColours; }

		// --------------------------------------------------------------
		bool getVertex(CncVertex& ret, unsigned int idx) const {
			
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
		
		// --------------------------------------------------------------
		void setColours(VertexColours& colours) {
			vertexColours = colours; 
		}
		
		// --------------------------------------------------------------
		bool updateVertex(const CncVertex& vertex, unsigned int idx) {
			
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

		// --------------------------------------------------------------
		bool normalizeClientID(long clientId) {	return changeColourForClientID(clientId, '\0'); }
		bool highlightClientID(long clientId) {	return changeColourForClientID(clientId,  'H'); }

		// --------------------------------------------------------------
		bool appendVertex(CncVertex& vertex) {
			if ( remainingVertiesCount() == 0 )
				return false;
				
			// color synchronizarion
			normalizeVertexColour(vertex);
			
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
		
		// --------------------------------------------------------------
		void destroyBuffer() {
			clientIdIndex.clear();

			glDeleteVertexArrays(1, &vertexArrayID);
			glDeleteBuffers(1, &vertexBufferID);
		}
		
		// --------------------------------------------------------------
		void display(DisplayType dt, int vertices = -1) {
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

		//-------------------------------------------------
		friend std::ostream &operator<< (std::ostream &ostr, const GLOpenGLPathBuffer& b) {
			const char* indend = " ";

			ostr << indend
				 << "GLOpenGLPathBuffer: Size = [" << b.numVertices << ']' << std::endl;

			// index data
			for ( auto cldIt = b.clientIdIndex.begin(); cldIt != b.clientIdIndex.end(); ++cldIt ) {
				ostr << indend
					 << "IDX ["
					 << cldIt->first << "][";

				for ( auto idxIt = cldIt->second.begin(); idxIt != cldIt->second.end(); ++idxIt ) {
					ostr << idxIt->first;

					if ( std::distance(cldIt->second.begin(), idxIt ) < (long)cldIt->second.size() - 1 )
						ostr << ',';
				}

				ostr << ']' << std::endl;
			}

			// buffer data
			for ( unsigned int i=0; i < b.numVertices; i++  ) {
				CncVertex vertex;
				b.getVertex(vertex, i);

				ostr << indend
					 << "DAT"
					 << indend << vertex << std::endl;
			}

			ostr << std::endl;
			return ostr;
		}
};




class GLOpenGLPathBufferStore {
	
	private:

		typedef std::vector<GLOpenGLPathBuffer> 	BufferStore;
			
		BufferStore 					bufferStore;
		GLOpenGLPathBuffer::CncVertex	lastVertex;

		// --------------------------------------------------------------
		void addBuffer() { 

			GLOpenGLPathBuffer buffer;
			
			// start the new buffer with the last value of the prev buffer
			// to connect the underlying GL_LINE_STRIPS
			buffer.appendVertex(lastVertex);
			
			// add
			bufferStore.push_back(buffer);
		}
		
		// --------------------------------------------------------------
		int getBufferStoreIndex(unsigned long idx) const {
			return idx / GLOpenGLPathBuffer::vertexCountMax;
		}
		
	public:
		
		// --------------------------------------------------------------
		GLOpenGLPathBufferStore()
		: bufferStore()
		, lastVertex()

		{
			addBuffer();
		}
		
		// --------------------------------------------------------------
		~GLOpenGLPathBufferStore()
		{
			destroyBuffers();
		}
		
		// --------------------------------------------------------------
		unsigned int getBufferCount() 	const { return bufferStore.size(); }
		
		// --------------------------------------------------------------
		unsigned int getTotalSize()		const { return bufferStore.size()
				                                       * GLOpenGLPathBuffer::vertexBufferSize; }

		// --------------------------------------------------------------
		unsigned long getVertexCount() 	const { 

			if ( getBufferCount() == 0 )
				return 0L;
				
			return ( getBufferCount() - 1 ) * GLOpenGLPathBuffer::vertexCountMax
			       + bufferStore.back().getNumVerties(); 
		}
		
		// --------------------------------------------------------------
		const GLOpenGLPathBuffer::CncVertex& getLastVertex() const { return lastVertex; }

		// --------------------------------------------------------------
		bool getVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) const {
				
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
		
		// --------------------------------------------------------------
		void destroyBuffers() {
			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
				it->destroyBuffer();
				
			bufferStore.clear();
		}
		
		// --------------------------------------------------------------
		bool normalizeClientID(long clientId) {	
			bool ret = false;
			
			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
				if ( it->normalizeClientID(clientId) == true )
					ret = true;
					
			return ret;
		}
		
		// --------------------------------------------------------------
		bool updateVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) {
				
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

		// --------------------------------------------------------------
		unsigned long appendVertex(GLOpenGLPathBuffer::CncVertex& vertex) {
			if ( bufferStore.size() == 0 )
				addBuffer();
			
			if ( bufferStore.back().more() == false )
				addBuffer();
			
			lastVertex = vertex;
			return bufferStore.back().appendVertex(vertex) ? getVertexCount() : 0;
		}

		// --------------------------------------------------------------
		bool highlightClientID(long clientId) {
			bool ret = false;

			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
				if ( it->highlightClientID(clientId) == true )
					ret = true;
			}

			return ret;
		}
		
		// --------------------------------------------------------------
		void display(GLOpenGLPathBuffer::DisplayType dt, long vertices = -1) {
			const unsigned int base = vertices < 0 ? bufferStore.size() : vertices / GLOpenGLPathBuffer::vertexCountMax;
			const unsigned int rest = vertices < 0 ?                  0 : vertices % GLOpenGLPathBuffer::vertexCountMax;
			
			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
				if ( std::distance(bufferStore.begin(), it) < base )	it->display(dt);
				else													it->display(dt, rest);
			}
		}

		//-------------------------------------------------
		friend std::ostream &operator<< (std::ostream &ostr, const GLOpenGLPathBufferStore& s) {

			ostr << "GLOpenGLPathBufferStore:"
				 << " Size = [" << s.bufferStore.size() << ']'
				 << " Count =  [" << s.getVertexCount() << ']'
				 << std::endl;

			for ( auto it = s.bufferStore.begin(); it != s.bufferStore.end(); ++it ) {
				const GLOpenGLPathBuffer& b = *it;
				ostr << b;
			}

			return ostr;
		}
};

#endif