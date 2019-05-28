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

#include "3D/GLInclude.h"
#include "3D/GLCommon.h"

//-------------------------------------------------------------
class GLOpenGLPathBuffer {
	
	public:
		
		//-----------------------------------------------------
		typedef std::map<unsigned int, bool> IndexList;
		typedef std::map<long, IndexList>    ClientIdIndex;

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
				
				float getX()		const { return vertex[CncVertexAxisX]; }
				float getY()		const { return vertex[CncVertexAxisY]; }
				float getZ()		const { return vertex[CncVertexAxisZ]; }

				float getR()		const { return colour[CncVertexColourR]; }
				float getG()		const { return colour[CncVertexColourG]; }
				float getB()		const { return colour[CncVertexColourB]; }
				float getA()		const { return colour[CncVertexColourA]; }
				
				//-------------------------------------------------
				CncVertex()
				: type('R')
				, clientID(-1)
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
				const CncVertex& set(char type, long clientID, float x, float y, float z)
				{
					this->type 				= type;
					this->clientID			= clientID;
					vertex[CncVertexAxisX] 	= x;
					vertex[CncVertexAxisY] 	= y;
					vertex[CncVertexAxisZ] 	= z;
					
					//memset(colour, 	0, CncVertexColourValueCount * sizeof(GLubyte));
					
					updateColour(255,0,0,255);
					
					
					return *this;
				}
			//-------------------------------------------------
			friend std::ostream& operator<< (std::ostream &ostr, const CncVertex& a) {
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
		
		// --------------------------------------------------------------
		struct VertexColours {
			wxColour	rapid		= wxColour(255, 128,  64);
			wxColour	work		= wxColour(255, 255, 255);
			wxColour	max			= wxColour(255,   0,   0);
			wxColour	user		= wxColour(  0,   0, 255);
			wxColour	highlight	= wxColour(255, 255,   0);
		};

		static const GLuint 		vertexCountMax			= 1024 * 16;
		static const GLsizeiptr 	vertexBufferSize		= vertexCountMax * sizeof(CncVertex);
		static const GLuint 		vertexAxisTypeSize		= sizeof(vertex_type);
		static const GLuint 		vertexColourTypeSize	= sizeof(colour_type);
		
	private:

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
			
			GLCommon::checkGLError("GLOpenGLPathBuffer::initBuffer() Type=A");

			glGenVertexArrays(1, &vertexArrayID);
			glBindVertexArray(vertexArrayID);

			GLCommon::checkGLError("GLOpenGLPathBuffer::initBuffer() Type=B");

			// vertex
			char *offset = (char*)NULL;
			glVertexPointer(CncVertexAxisValueCount, GL_FLOAT, sizeof(CncVertex), offset);
			glEnableClientState(GL_VERTEX_ARRAY);

			GLCommon::checkGLError("GLOpenGLPathBuffer::initBuffer() Type=C");

			// color
			offset = (char*)NULL + CncVertexAxisValueCount * sizeof(vertex_type);
			glColorPointer(CncVertexColourValueCount, GL_UNSIGNED_BYTE, sizeof(CncVertex), offset);
			glEnableClientState(GL_COLOR_ARRAY);

			GLCommon::checkGLError("GLOpenGLPathBuffer::initBuffer() Type=D");
			
			glBindVertexArray(0);

			GLCommon::checkGLError("GLOpenGLPathBuffer::initBuffer() Type=E");
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
		
		GLOpenGLPathBuffer();
		explicit GLOpenGLPathBuffer(const GLOpenGLPathBuffer& b);
		~GLOpenGLPathBuffer();
		
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
		bool normalizeClientID(long clientId) 					{ return changeColourForClientID(clientId, '\0'); }
		bool highlightClientID(long clientId) 					{ return changeColourForClientID(clientId,  'H'); }
		
		long getFirstEntryForClientId(long clientId) const;

		// --------------------------------------------------------------
		const wxString& getIndexForClientIdAsString(long clientId, wxString& ret);
		bool getVertex(CncVertex& ret, unsigned int idx) const;
		void setColours(const VertexColours& colours);
		bool updateVertex(const CncVertex& vertex, unsigned int idx);
		bool appendVertex(const CncVertex& vertex);
		void destroyBuffer();
		void display(DisplayType dt, int vertices = -1);

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
		
		bool								initialized;
		BufferStore 						bufferStore;
		GLOpenGLPathBuffer::VertexColours	colourStore;
		GLOpenGLPathBuffer::CncVertex		lastVertex;

		// --------------------------------------------------------------
		void addBuffer();
		int getBufferStoreIndex(unsigned long idx) const;
		
	public:
		
		// --------------------------------------------------------------
		GLOpenGLPathBufferStore();
		~GLOpenGLPathBufferStore();
		
		void init();
		bool checkInitialized();
		
		unsigned int getBufferCount() 	const;
		unsigned int getTotalSize()		const;
		unsigned long getVertexCount() 	const;
		const GLOpenGLPathBuffer::CncVertex& getLastVertex() const;
		bool getVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) const;
		
		void destroyBuffers();
		bool normalizeClientID(long clientId);
		bool updateVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx);
		unsigned long appendVertex(const GLOpenGLPathBuffer::CncVertex& vertex);
		bool highlightClientID(long clientId);
		void display(GLOpenGLPathBuffer::DisplayType dt, long vertices = -1);
		long findFirstEntryForClientId(long cliendId);
		GLOpenGLPathBuffer::VertexColours& getColoursAsReference();
		void setColours(const GLOpenGLPathBuffer::VertexColours& colours);
		
		const wxString& getIndexForClientIdAsString(long clientId, wxString& ret);
		
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