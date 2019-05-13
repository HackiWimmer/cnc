
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

/////////////////////////////////////////////////////////////////
namespace OGL_DEBUG {
	/////////////////////////////////////////////////////////////////
	void debugInfo(const char* context, const char* function, const char* message) {
	/////////////////////////////////////////////////////////////////
		context == NULL ? 	std::clog << "???" : std::clog << context;
		std::clog << "::";
		
		function == NULL ? 	std::clog << "???" : std::clog << function;
		std::clog << ": ";
		
		message == NULL ? 	std::clog << "???" : std::clog << message;
		std::clog << std::endl;
	}
	
	/////////////////////////////////////////////////////////////////
	void debugError(const char* context, const char* function, const char* message) {
	/////////////////////////////////////////////////////////////////
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);

		context == NULL ? 	std::clog << "???" : std::clog << context;
		std::clog << "::";
		
		function == NULL ? 	std::clog << "???" : std::clog << function;
		std::clog << ": ";
		
		message == NULL ? 	std::clog << "???" : std::clog << message;
		std::clog << std::endl;
		
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
	}
}

float w, h, tip = 0, turn = 0;

float ORG[3] = {0,0,0};

float XP[3] = {1,0,0}, XN[3] = {-1,0,0},
YP[3] = {0,1,0}, YN[3] = {0,-1,0},
ZP[3] = {0,0,1}, ZN[3] = {0,0,-1};

void reshape (int nw, int nh)
{
w = nw;
h = nh;
}

void Turn (int key, int x, int y)
{
switch (key) {
case GLUT_KEY_RIGHT: turn += 5; break;
case GLUT_KEY_LEFT : turn -= 5; break;
case GLUT_KEY_UP : tip -= 5; break;
case GLUT_KEY_DOWN : tip += 5; break;
}
}

void Draw_Axes (void)
{
	glPushMatrix ();

	glTranslatef (-2.4, -1.5, -5);
	glRotatef (tip , 1,0,0);
	glRotatef (turn, 0,1,0);
	glScalef (0.25, 0.25, 0.25);

	glLineWidth (2.0);

	glBegin (GL_LINES);
	glColor3f (1,0,0); // X axis is red.
	glVertex3fv (ORG);
	glVertex3fv (XP );
	glColor3f (0,1,0); // Y axis is green.
	glVertex3fv (ORG);
	glVertex3fv (YP );
	glColor3f (0,0,1); // z axis is blue.
	glVertex3fv (ORG);
	glVertex3fv (ZP );
	glEnd();
	
	glPopMatrix ();
}

void Draw_Teapot (void)
{
	glPushMatrix ();

	glTranslatef (0, 0, -5);
	glRotatef (tip , 1,0,0);
	glRotatef (turn, 0,1,0);

	glColor3f (1.0, 0.5, 0.1);
	glutSolidTeapot (1.0);

	glLineWidth (2.0);
	glColor3f (0.0, 0.2, 0.9);
	glutWireTeapot (1.01);

	glPopMatrix ();
}

//------------------------------------------------------

class CncOpenGlPathBuffer {
	
	private:
		unsigned int 	vertexBufferID;
		unsigned int 	vertexArrayID;
		unsigned int 	numVertices; 
		GLenum 			displayType;
		
		// --------------------------------------------------------------
		void initBuffer() {
			glGenVertexArrays(1, &vertexArrayID);
			glGenBuffers(1, &vertexBufferID);
			
			glBindBuffer(GL_ARRAY_BUFFER, 	vertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, 	vertexBufferSize, NULL, GL_STATIC_DRAW);
		}
		
	public:
	
		enum DisplayType { DT_DOTS, DT_STRIPS };
		
		static const unsigned int 	CncVertexAxisValueCount 		= 3;
		static const unsigned int 	CncVertexColourValueCount		= 4;
		
		struct CncVertex {
			// don't change the following variables order
			GLfloat vertex[CncVertexAxisValueCount];
			GLfloat color[CncVertexColourValueCount];
			
			// specify additional data from here on . . . 
			long clientID;
		};
		
		static const GLuint 		vertexCountMax					= 20;
		static const GLsizeiptr 	vertexBufferSize				= vertexCountMax * sizeof(CncVertex);
		static const GLenum 		VertexAxisType					= GL_FLOAT;
		static const GLenum 		VertexColourType				= GL_FLOAT;
		
		
		CncOpenGlPathBuffer()
		: vertexBufferID(0)
		, vertexArrayID(0)
		, numVertices(0)
		, displayType(GL_LINE_STRIP)
		{
			initBuffer();
		}
		
		explicit CncOpenGlPathBuffer(const CncOpenGlPathBuffer& b)
		: vertexBufferID(b.getBufferID())
		, vertexArrayID(b.getArrayID())
		, numVertices(b.getNumVerties())
		, displayType(b.getDisplayType())
		{
			initBuffer();
		}

		~CncOpenGlPathBuffer()
		{}
		
		unsigned int getBufferID() 				const { return vertexBufferID; }
		unsigned int getArrayID() 				const { return vertexArrayID; }
		unsigned int getNumVerties()			const { return numVertices; }
		unsigned int remainingVertiesCount()	const { return vertexCountMax - numVertices; }
		bool         more()						const { return remainingVertiesCount() > 0; } 
		
		// --------------------------------------------------------------
		DisplayType getDisplayType() const { 
			if ( displayType == GL_LINE_STRIP)
				return DT_STRIPS; 
			
			return DT_DOTS;
		}
		
		// --------------------------------------------------------------
		void setDisplayType(const DisplayType dt) {
			switch (dt){
				case DT_DOTS:	 	displayType = GL_POINTS; 		break;
				case DT_STRIPS:		displayType = GL_LINE_STRIP; 	break;
			}
		}
		
		// --------------------------------------------------------------
		void destroyBuffer() {
			glDeleteVertexArrays(1, &vertexArrayID);
			glDeleteBuffers(1, &vertexBufferID);
		}
		
		// --------------------------------------------------------------
		bool appendVertex(const CncVertex& vertex) {
			if ( remainingVertiesCount() == 0 )
				return false;
			
			GLintptr vertexOffset = (numVertices) * sizeof(CncVertex);
			
			glBindVertexArray(vertexArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeof(CncVertex), &vertex);
			
			++numVertices;
			return true;
		}
		
		// --------------------------------------------------------------
		void display(int vertices = -1) {
			const unsigned int displayCount = vertices >= 0 ? std::min((unsigned int)vertices, numVertices) : numVertices;
			
			std::cout << "ID[" << vertexBufferID<< "]: " << "displayCount: " << displayCount << std::endl;
			
			// select buffer
			glBindVertexArray(vertexArrayID);
			//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			
			// vertex
			char *offset = (char*)NULL;
			glVertexPointer(CncVertexAxisValueCount, GL_FLOAT, sizeof(CncVertex), offset);
			
			// color
			offset = (char*)NULL + CncVertexAxisValueCount * sizeof(GLfloat);
			glColorPointer(CncVertexColourValueCount, GL_FLOAT, sizeof(CncVertex), offset);
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			// render data
			const unsigned int displayOffset = 0;
			glDrawArrays(displayType, displayOffset, displayCount);
		}

};

class CncOpenGlPathBufferStore {
	
	private:
		typedef std::vector<CncOpenGlPathBuffer> BufferStore;
		BufferStore bufferStore;
		
		void addBuffer() { 
			bufferStore.push_back(CncOpenGlPathBuffer()); 
			bufferStore.back().setDisplayType(bufferStore.front().getDisplayType());
		}
		
	public:
		
		// --------------------------------------------------------------
		CncOpenGlPathBufferStore()
		: bufferStore()
		{
			addBuffer();
		}
		
		// --------------------------------------------------------------
		~CncOpenGlPathBufferStore()
		{
			destroyBuffers();
		}
		
		// --------------------------------------------------------------
		unsigned int getBufferCount() 	const { return bufferStore.size(); }
		
		// --------------------------------------------------------------
		unsigned int getVertexCount() 	const { 
			if ( getBufferCount() == 0 )
				return 0;
				
			return ( getBufferCount() - 1 ) * CncOpenGlPathBuffer::vertexCountMax 
			       + bufferStore.back().getNumVerties(); 
		}
		
		// --------------------------------------------------------------
		void setDisplayType(const CncOpenGlPathBuffer::DisplayType dt) {
			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it) {
				
				std::cout << "Buffer iD: " << it->getBufferID() << std::endl;
				it->setDisplayType(dt);
			}
		}
		
		// --------------------------------------------------------------
		void destroyBuffers() {
			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it)
				it->destroyBuffer();
				
			bufferStore.clear();
		}
		
		// --------------------------------------------------------------
		bool appendVertex(const CncOpenGlPathBuffer::CncVertex& vertex) {
			if ( bufferStore.size() == 0 )
				addBuffer();
			
			if ( bufferStore.back().more() == false )
				addBuffer();
			
			std::cout << "Buffer count: " << bufferStore.size() << std::endl;
			return bufferStore.back().appendVertex(vertex);
		}
		
		// --------------------------------------------------------------
		void display(long vertices = -1) {
			
			std::cout << "Total verties count: " << getVertexCount() << std::endl;
			
			//bufferStore[0].display();
			//bufferStore[1].display();
			//bufferStore[2].display();
			
			//bufferStore.front().display();
			//bufferStore.back().display();
			
			//glDisableClientState(GL_VERTEX_ARRAY);
			//glDisableClientState(GL_COLOR_ARRAY);
			
			//return;
			
			
			const unsigned int base = vertices < 0 ? bufferStore.size() : vertices / CncOpenGlPathBuffer::vertexCountMax;
			const unsigned int rest = vertices < 0 ?                  0 : vertices % CncOpenGlPathBuffer::vertexCountMax;
			
			for ( auto it = bufferStore.begin(); it != bufferStore.end(); ++it ) {
				if ( std::distance(bufferStore.begin(), it) < base )	it->display();
				else													it->display(/*rest*/);
			}
		}
};

CncOpenGlPathBufferStore* pbs;

void Draw_Test (void)
{
	glPushMatrix ();

	glTranslatef (-1.4, -0.5, -5);
	glRotatef (tip , 1,0,0);
	glRotatef (turn, 0,1,0);
	glScalef (0.25, 0.25, 0.25);

	pbs->display();
		
	glPopMatrix ();
}

// -------------------------------------------------------------

void display (void)
{
	glViewport (0, 0, w, h);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw_Teapot ();
	//Draw_Axes ();
	Draw_Test();

	glutSwapBuffers();
	
	glutReportErrors();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (600, 400);
	glutInitWindowPosition (400, 300);
	glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("GLEW Test");
	GLenum err = glewInit();
	if ( GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	glutDisplayFunc (display);
	//glutIdleFunc (display);
	glutReshapeFunc (reshape);
	glutSpecialFunc (Turn);

	glClearColor (0.1, 0.2, 0.1, 1.0);
	glEnable (GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
	gluPerspective (40.0, 1.5, 1.0, 10.0);
	glMatrixMode (GL_MODELVIEW);
	
	pbs = new CncOpenGlPathBufferStore();
	
	{ CncOpenGlPathBuffer::CncVertex v; v.vertex[0] = +0.0f; v.vertex[1] = +0.0f; v.vertex[2] = +0.0f; v.color[0] = 1.0f; v.color[1] = 0.0f; v.color[2] = 0.0f; v.color[3] = 0.0f; pbs->appendVertex(v); }
	{ CncOpenGlPathBuffer::CncVertex v; v.vertex[0] = +2.0f; v.vertex[1] = +2.0f; v.vertex[2] = +2.0f; v.color[0] = 1.0f; v.color[1] = 0.0f; v.color[2] = 0.0f; v.color[3] = 0.0f; pbs->appendVertex(v); }

	{ CncOpenGlPathBuffer::CncVertex v; v.vertex[0] = +3.0f; v.vertex[1] = -1.0f; v.vertex[2] = -1.0f; v.color[0] = 1.0f; v.color[1] = 1.0f; v.color[2] = 0.0f; v.color[3] = 0.0f; pbs->appendVertex(v); }
	{ CncOpenGlPathBuffer::CncVertex v; v.vertex[0] = +2.0f; v.vertex[1] = +2.0f; v.vertex[2] = -1.0f; v.color[0] = 1.0f; v.color[1] = 0.0f; v.color[2] = 1.0f; v.color[3] = 0.0f; pbs->appendVertex(v); }

	{ CncOpenGlPathBuffer::CncVertex v; v.vertex[0] = -1.0f; v.vertex[1] = +2.0f; v.vertex[2] = -1.0f; v.color[0] = 1.0f; v.color[1] = 0.0f; v.color[2] = 1.0f; v.color[3] = 0.0f; pbs->appendVertex(v); }
	{ CncOpenGlPathBuffer::CncVertex v; v.vertex[0] = +1.0f; v.vertex[1] = -2.0f; v.vertex[2] = -1.0f; v.color[0] = 1.0f; v.color[1] = 0.0f; v.color[2] = 1.0f; v.color[3] = 0.0f; pbs->appendVertex(v); }

	pbs->setDisplayType(CncOpenGlPathBuffer::DT_DOTS);
	pbs->setDisplayType(CncOpenGlPathBuffer::DT_STRIPS);
	
	glutMainLoop ();
	return 0;
} 
