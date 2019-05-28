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

//#include "../CncControlerGui/3D/GLOpenGLPathBufferStore.h"

/////////////////////////////////////////////////////////////////
namespace OGL_DEBUG {
	/////////////////////////////////////////////////////////////////
/*
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
*/
}

float w, h, tip = 0, turn = 0;

float ORG[3] = {0,0,0};

float XP[3] = {1,0,0}, XN[3] = {-1,0,0},
YP[3] = {0,1,0}, YN[3] = {0,-1,0},
ZP[3] = {0,0,1}, ZN[3] = {0,0,-1};

// -------------------------------------------------------------
void reshape (int nw, int nh)
{
	w = nw;
	h = nh;
}

// -------------------------------------------------------------
void Turn (int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_RIGHT: turn += 5; break;
		case GLUT_KEY_LEFT : turn -= 5; break;
		case GLUT_KEY_UP : tip -= 5; break;
		case GLUT_KEY_DOWN : tip += 5; break;
	}
}

// -------------------------------------------------------------
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

// -------------------------------------------------------------
void Draw_Teapot (void)
{
	glPushMatrix ();

	glTranslatef (0, 0, -5);
	glRotatef (tip , 1,0,0);
	glRotatef (turn, 0,1,0);

	glColor3f (1.0, 0.5, 0.1);
	//glutSolidTeapot (1.0);

	glLineWidth (2.0);
	glColor3f (0.0, 0.2, 0.9);
	glutWireTeapot (1.01);

	glPopMatrix ();
}

// -------------------------------------------------------------
//GLOpenGLPathBufferStore* pbs;
void Draw_Test (void)
{
	glPushMatrix ();

	glTranslatef (-1.4, -0.5, -5);
	glRotatef (tip , 1,0,0);
	glRotatef (turn, 0,1,0);
	glScalef (0.25, 0.25, 0.25);

	//DT_DOTS
	//pbs->display(GLOpenGLPathBuffer::DT_STRIPS);
		
	glPopMatrix ();
}

// -------------------------------------------------------------
void display (void)
{
	glViewport (0, 0, w, h);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Draw_Teapot ();
	//Draw_Axes ();
	//Draw_Test();

	glutSwapBuffers();
	
	glutReportErrors();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
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

	//glEnable              ( GL_DEBUG_OUTPUT );
	//glDebugMessageCallback( GLOpenGLPathBuffer::MessageCallback, 0 );

	glClearColor (0.1, 0.2, 0.1, 1.0);
	glEnable (GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
	gluPerspective (40.0, 1.5, 1.0, 10.0);
	glMatrixMode (GL_MODELVIEW);
	
	/*
	pbs = new GLOpenGLPathBufferStore();
	
	auto appendVertex = [&](GLOpenGLPathBuffer::CncVertex& v) {
		if ( true ) {
			pbs->appendVertex(v);
		}
		else {
			std::cout 	<< pbs->getLastVertex()
						<< " add at: " << pbs->appendVertex(v)
						<< std::endl;
		}
	};
	
	{ GLOpenGLPathBuffer::CncVertex v('R', -1, +0.0f, +0.0f, +0.0f); appendVertex(v); }
	{ GLOpenGLPathBuffer::CncVertex v('R', -1, +2.0f, +2.0f, +2.0f); appendVertex(v); }

	{ GLOpenGLPathBuffer::CncVertex v('W', +1, +3.0f, -1.0f, -1.0f); appendVertex(v); }
	{ GLOpenGLPathBuffer::CncVertex v('W', +1, +2.0f, +2.0f, -1.0f); appendVertex(v); }

	{ GLOpenGLPathBuffer::CncVertex v('W', +2, -1.0f, +2.0f, -1.0f); appendVertex(v); }
	{ GLOpenGLPathBuffer::CncVertex v('W', +2, +1.0f, -2.0f, -1.0f); appendVertex(v); }
	{ GLOpenGLPathBuffer::CncVertex v('R', +2, +2.0f, -2.0f, -1.0f); appendVertex(v); }
	{ GLOpenGLPathBuffer::CncVertex v('R', +2, +1.0f, -2.0f, -1.0f); appendVertex(v); }
	
	std::cout << "highlight: " << pbs->highlightClientID(1) << std::endl;

	GLOpenGLPathBuffer::CncVertex v;
	if ( pbs->getVertex(v, 8) ) {
		std::cout << v << std::endl;
		
		// update by reference
		//v.vertex[0] = 3;
			
		// update by function
		//pbs->updateVertex(v, 8);
		std::cout << v << std::endl;
	}

	std::cout << "GLfloat size     : " << sizeof(GLfloat) << std::endl;
	std::cout << "GLubyte size     : " << sizeof(GLubyte) << std::endl;
	std::cout << "long size        : " << sizeof(long) 	  << std::endl;
	std::cout << "CncVertex size   : " << sizeof(GLOpenGLPathBuffer::CncVertex) << std::endl;
	std::cout << "Buffer size      : " << GLOpenGLPathBuffer::vertexBufferSize  << std::endl;
	std::cout << "Buffer size total: " << pbs->getTotalSize() << std::endl;

	std::cout << *pbs;
*/
	glutMainLoop ();
	return 0;
} 
