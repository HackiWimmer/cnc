#ifndef GL_OPENGL_PATHBUFFERSTORE_H
#define GL_OPENGL_PATHBUFFERSTORE_H

#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cinttypes>

#include <wx/string.h>
#include <wx/colour.h>

#include "3D/GLInclude.h"
#include "3D/GLCommon.h"

//-------------------------------------------------------------
class GLOpenGLPathBufferStore;

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
		
		// --------------------------------------------------------------
		struct VertexColours {
			unsigned int rapidAlpha	= 80;
			
			wxColour	rapid		= wxColour(255, 128,  64, rapidAlpha);
			wxColour	work		= wxColour(255, 255, 255, 255);
			wxColour	max			= wxColour(255,   0,   0, 255);
			wxColour	user		= wxColour(  0,   0, 255, 255);
			wxColour	highlight	= wxColour(255, 255,   0, 255);
			
			// ----------------------------------------------------------
			void showRapidPathes(bool state) {
				rapid.Set(rapid.Red(), rapid.Green(), rapid.Blue(), state == true ? rapid.Alpha() : 0);
			}
		};
		
		// --------------------------------------------------------------
		struct ReconstructOptions {
			bool showRapidPathes = true;
		};

		static const VertexColours& getColours() { return GLOpenGLPathBuffer::vertexColours; }
		static void setColours(const VertexColours& colours);
		
		//-----------------------------------------------------
		struct CncVertex {
			private:
				// with respect to the openGl buffer alignment 
				// don't change the following variable order
				vertex_type vertex[CncVertexAxisValueCount];
				colour_type colour[CncVertexColourValueCount];

				// specify additional data from here on . . . 
				//clientID as wells as type isn't edible after creation!
				char type 		= 'R';
				long clientID 	= -1;

			protected:
			
				//-------------------------------------------------
				inline void updateColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
					colour[CncVertexColourR] = r;
					colour[CncVertexColourG] = g;
					colour[CncVertexColourB] = b;
					colour[CncVertexColourA] = a;
				}
				
				// --------------------------------------------------------------
				inline void normalizeVertexColour() {
					const VertexColours& vc = GLOpenGLPathBuffer::vertexColours;
					switch ( getType() ) {
						case 'R':		updateColour(vc.rapid.Red(), vc.rapid.Green(),	vc.rapid.Blue(), vc.rapid.Alpha()); 
										break;
						case 'W':		updateColour(vc.work.Red(), vc.work.Green(),	vc.work.Blue(),  vc.work.Alpha()); 
										break;
						case 'M':		updateColour(vc.max.Red(), 	vc.max.Green(),		vc.max.Blue(), 	 vc.max.Alpha()); 
										break;
						default:		updateColour(vc.user.Red(), vc.user.Green(),	vc.user.Blue(),  vc.user.Alpha()); 
										break;
					}
				}
				
				friend GLOpenGLPathBuffer;
				
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
					normalizeVertexColour();
				}

				//-------------------------------------------------
				explicit CncVertex(char type, long clientID)
				: type(type)
				, clientID(clientID)
				{
					memset(vertex,  0, CncVertexAxisValueCount   * sizeof(vertex_type));
					normalizeVertexColour();
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
					
					normalizeVertexColour();
				}
				
				//-------------------------------------------------
				const CncVertex& set(char type, long clientID, float x, float y, float z)
				{
					this->type 				= type;
					this->clientID			= clientID;
					vertex[CncVertexAxisX] 	= x;
					vertex[CncVertexAxisY] 	= y;
					vertex[CncVertexAxisZ] 	= z;
					
					normalizeVertexColour();
					
					return *this;
				}
				
				//-------------------------------------------------
				void reconstruct() {
					normalizeVertexColour();
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
		
		static const GLuint 		vertexCountMax			= 1024 * 16;
		static const GLsizeiptr 	vertexBufferSize		= vertexCountMax * sizeof(CncVertex);
		static const GLuint 		vertexAxisTypeSize		= sizeof(vertex_type);
		static const GLuint 		vertexColourTypeSize	= sizeof(colour_type);
		
	private:
		
		static VertexColours vertexColours;
		
		// --------------------------------------------------------------
		GLOpenGLPathBufferStore* store;
		unsigned int 		vertexBufferID;
		unsigned int 		vertexArrayID;
		unsigned int 		numVertices; 
		ClientIdIndex		clientIdIndex;
		
		// --------------------------------------------------------------
		void generateBuffers();
		void initBuffers();

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
										
						default:		vertex.normalizeVertexColour();
					}
					
					updateVertex(vertex, idxIt->first);
					
					ret = true;
				}
			}
			
			return ret;
		}
		
	public:
		
		explicit GLOpenGLPathBuffer(GLOpenGLPathBufferStore* store, GLOpenGLPathBuffer::CncVertex* firstVertex);
		explicit GLOpenGLPathBuffer(const GLOpenGLPathBuffer& b);
		~GLOpenGLPathBuffer();
		
		// --------------------------------------------------------------
		const wxString&			getStoreInstanceName()			const;
		const wxString&			getInstanceName()				const;
		GLOpenGLPathBufferStore*getBufferStore()				const { return store; }
		uint64_t 				getInstance() 					const { return (uint64_t)this; }
		unsigned int			getBufferID() 					const { return vertexBufferID; }
		unsigned int			getArrayID() 					const { return vertexArrayID; }
		unsigned int			getNumVerties()					const { return numVertices; }
		unsigned int			remainingVertiesCount()			const { return vertexCountMax - numVertices; }
		
		bool         			more()							const { return remainingVertiesCount() > 0; }
		bool 					hasClientID(long clientID)		const { return clientIdIndex.find(clientID) != clientIdIndex.end(); }
		
		const ClientIdIndex& 	getClientIdIndex()				const { return clientIdIndex; }
		unsigned int 			getClientIdCount()				const { return clientIdIndex.size(); }
		
		// --------------------------------------------------------------
		bool normalizeClientID(long clientId) 					{ return changeColourForClientID(clientId, '\0'); }
		bool highlightClientID(long clientId) 					{ return changeColourForClientID(clientId,  'H'); }
		
		void traceParameters(std::ostream& o, bool indent = true, bool oneLine=true) const;
		void traceIdentifier(std::ostream& o, const char* prepend = NULL) const;
		void traceIdentifierEndl(std::ostream& o, const char* prepend = NULL) const ;
		
		long getFirstEntryForClientId(long clientId) const;
		long getFirstIndexForClientId(long clientId) const;

		// --------------------------------------------------------------
		const wxString& getIndexForClientIdAsString(long clientId, wxString& ret, bool summerize);
		bool getVertex(CncVertex& ret, unsigned int idx) const;
		bool updateVertex(const CncVertex& vertex, unsigned int idx);
		bool appendVertex(const CncVertex& vertex);
		void resetBuffer();
		void destroyBuffer();
		void display(DisplayType dt, int vertices = -1);
		void reconstruct(const ReconstructOptions& opt);
		
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
			if ( false ) {
				for ( unsigned int i = 0; i < b.numVertices; i++ ) {
					CncVertex vertex;
					b.getVertex(vertex, i);

					ostr << indend
						 << "DAT"
						 << indend << vertex << std::endl;
				}
			}
			
			ostr << std::endl;
			return ostr;
		}
};


class GLOpenGLPathBufferStore {
	
	private:

		typedef std::vector<GLOpenGLPathBuffer> 	BufferStore;
		
		bool								initialized;
		wxString							instanceName;
		unsigned int						currentBufferIndex;
		BufferStore 						bufferStore;
		GLOpenGLPathBuffer::CncVertex		lastVertex;

		// --------------------------------------------------------------
		void addBuffer();
		int getBufferStoreIndex(unsigned long idx) const;
		
	public:
		
		static GLOpenGLPathBuffer::CncVertex defaultVertex;
		// --------------------------------------------------------------
		explicit GLOpenGLPathBufferStore(const wxString& instanceName);
		~GLOpenGLPathBufferStore();
		
		void init();
		void init(const GLOpenGLPathBuffer::CncVertex& firstVertex);
		bool checkInitialized();
		
		uint64_t getInstance()													const { return (uint64_t)this; }
		const wxString& getInstanceName()										const { return instanceName; }
		const wxString& getInstanceFullName()									const;
		unsigned int getBufferCount() 											const;
		unsigned int getTotalSize()												const;
		unsigned long getVertexCount() 											const;
		
		const GLOpenGLPathBuffer::CncVertex& getLastVertex() 					const;
		bool getVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx) 	const;
		long getBufferId(unsigned long idx) 									const;
		
		unsigned long getClientIdCount() 										const;
		unsigned int getBufferIdForIdx(unsigned long idx) 						const;
		long getClientIdForIdx(unsigned long idx) 								const;

		void reconstruct(const GLOpenGLPathBuffer::ReconstructOptions& opt);
		void resetBuffers();
		void destroyBuffers();
		bool normalizeClientID(long clientId);
		bool updateVertex(GLOpenGLPathBuffer::CncVertex& ret, unsigned long idx);
		unsigned long appendVertex(const GLOpenGLPathBuffer::CncVertex& vertex);
		bool highlightClientID(long clientId);
		void display(GLOpenGLPathBuffer::DisplayType dt, long vertices = -1);
		long findFirstEntryForClientId(long cliendId);
		long findFirstIndexForClientId(long cliendId);
		void setColours(const GLOpenGLPathBuffer::VertexColours& colours);
		
		const wxString& getIndexForClientIdAsString(long clientId, wxString& ret, bool summerize);
		const wxString& getIndexForIdxAsString(unsigned long idx, wxString& ret, bool summerize);
		
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
