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
		
		static const unsigned int	CncVertexAxisValueCount 		= 3;
		static const unsigned int		CncVertexAxisX 				= 0;
		static const unsigned int		CncVertexAxisY 				= 1;
		static const unsigned int		CncVertexAxisZ 				= 2;
		
		static const unsigned int	CncVertexColourValueCount		= 4;
		static const unsigned int		CncVertexColourR			= 0;
		static const unsigned int		CncVertexColourG			= 1;
		static const unsigned int		CncVertexColourB			= 2;
		static const unsigned int		CncVertexColourA			= 3;
		
		static const unsigned int		defRapidAlpha				=  80;
		static const unsigned int		defWorkAlpha				= 100;
		static const unsigned int		defMaxAlpha					= 100;
		static const unsigned int		defUserAlpha				= 100;
		static const unsigned int		defHighlightAlpha			= 100;
		
		static constexpr  float			dimDownFact					= 0.2;
		static constexpr  float			dimUpFact					= 1.0;
		
		// --------------------------------------------------------------
		struct VertexColours {
			
			unsigned int rapidAlpha	=  defRapidAlpha;
			
			wxColour	rapid		= wxColour(255, 128,  64, rapidAlpha);
			wxColour	work		= wxColour(255, 255, 255, GLOpenGLPathBuffer::defWorkAlpha);
			wxColour	max			= wxColour(255,   0,   0, GLOpenGLPathBuffer::defMaxAlpha);
			wxColour	user		= wxColour(  0,   0, 255, GLOpenGLPathBuffer::defUserAlpha);
			
			wxColour	highlight	= wxColour(255, 255,   0, GLOpenGLPathBuffer::defHighlightAlpha);
			
			void showRapidPathes(bool state);
			void restoreLightness();
			void changeLightness(float lightness = GLOpenGLPathBuffer::dimUpFact);
		};
		
		// --------------------------------------------------------------
		struct ReconstructOptions {
			bool showRapidPathes = true;
		};

		static const VertexColours& getColours()					{ return GLOpenGLPathBuffer::vertexColours; }
		static void resetColoursLightness()							{ setColoursLightness(GLOpenGLPathBuffer::dimUpFact); }
		static void setColours(const VertexColours& colours);
		static void setColoursLightness(float lightness = GLOpenGLPathBuffer::dimUpFact);
		static void dimDownColours();
		static void dimUpColours();
		
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
				
				void updateColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				void changeLightness(unsigned char a);
				void changeLightness(float lightness);
				void normalizeVertexColour();
				
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
		
		void generateBuffer();
		void updateIndex(long clientID);
		bool changeColourForClientID(long clientId, const char type);
		
	public:
		
		explicit GLOpenGLPathBuffer(GLOpenGLPathBufferStore* store, GLOpenGLPathBuffer::CncVertex* firstVertex);
		explicit GLOpenGLPathBuffer(const GLOpenGLPathBuffer& b);
		~GLOpenGLPathBuffer();
		
		void createVertexArray();
		void destroyVertexArray();
		
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
		bool dimDownClientID(long clientId)						{ return changeColourForClientID(clientId,  'D'); }
		bool dimUpClientID(long clientId)						{ return changeColourForClientID(clientId,  'U'); }
		
		void traceParameters(std::ostream& o, bool indent = true, bool oneLine=true) const;
		void traceIdentifier(std::ostream& o, const char* prepend = NULL) const;
		void traceIdentifierEndl(std::ostream& o, const char* prepend = NULL) const ;
		
		long getFirstEntryForClientId(long clientId) const;
		long getFirstIndexForClientId(long clientId) const;
		
		long getLastEntryForClientId(long clientId) const;

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
		
		void createVertexArray();
		void destroyVertexArray();
		
		void init();
		void init(const GLOpenGLPathBuffer::CncVertex& firstVertex);
		bool isInitialized() 													const { return initialized; }
		bool initialize();
		
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
		bool dimDownClientID(long clientId);
		bool dimUpClientID(long clientId);
		
		void display(GLOpenGLPathBuffer::DisplayType dt, long vertices = -1);
		long findFirstEntryForClientId(long cliendId);
		long findFirstIndexForClientId(long cliendId);
		long findLastEntryForClientId(long cliendId);
		void setColours(const GLOpenGLPathBuffer::VertexColours& colours);
		
		const wxString& getIndexForClientIdAsString(long clientId, wxString& ret, bool summerize);
		const wxString& getIndexForIdxAsString(unsigned long idx, wxString& ret, bool summerize);
		
		const wxString& getVaoAndVboSummary();
		
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
