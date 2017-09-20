#include <iostream>
#include "3D/GLContextBase.h"
#include "3D/GLContextTestCube.h"
#include "3D/GLContextCncPath.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
	#include <OpenGL/freeglut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut.h>
#endif

/////////////////////////////////////////////////////////////////
// global vars
/////////////////////////////////////////////////////////////////
OpenGLContextBase* currentContext 	= NULL;
OpenGLContextBase* context0 		= NULL;
OpenGLContextBase* context1 		= NULL;

void display();
void reshape(int w, int h);

/////////////////////////////////////////////////////////////////
void switchContext(int id) 
/////////////////////////////////////////////////////////////////
{
	switch ( id ) {
		case 0: 	currentContext = context0; break;
		case 1: 	currentContext = context1; break;
		
		default: 	currentContext = NULL;
	}
	
	if ( currentContext != NULL ) {
		std::clog << "Context switched to: " << currentContext->getContextName() << " ["<< id  << "]" << std::endl;
		
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		display();
	} else {
		std::cerr << "Context switch failt to: " << id << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void init() 
/////////////////////////////////////////////////////////////////
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
	
	context0 = new OpenGLContextTestCube(NULL);
	context1 = new OpenGLContextCncPath(NULL);
	
	context0->init();
	context1->init();
	
	switchContext(0);
}

/////////////////////////////////////////////////////////////////
void destroy() 
/////////////////////////////////////////////////////////////////
{
	if ( context0 != NULL ) delete context0;
	if ( context1 != NULL ) delete context1;
}

/////////////////////////////////////////////////////////////////
void display()
/////////////////////////////////////////////////////////////////
{
	if ( currentContext != NULL ) {
		currentContext->display();
	} else {
		std::cerr << "display(): currentContext == NULL" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void reshape(int w, int h)
/////////////////////////////////////////////////////////////////
{
	if ( currentContext != NULL ) {
		currentContext->reshape(w, h);
	} else {
		std::cerr << "reshap(): currentContext == NULL" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void keyboard(unsigned char c, int x, int y)
/////////////////////////////////////////////////////////////////
{
	std::clog << "keyboard('" << c << "' [" << (int) c << "]) " << x << ", " << y << std::endl;
	
	switch (c) {
		case 27:	exit(0);
					break;
					
		case '0':	switchContext(0);
					break;
					
		case '1':	switchContext(1);
					break;
				
		default: 	;
	}
	
	if ( currentContext != NULL )
		currentContext->keyboardHandler(c);
}

/////////////////////////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
/////////////////////////////////////////////////////////////////
{
	std::clog << "mouse(" << button << ")(" << state << "), " << x << ", " << y << std::endl;
	
	enum ButtonState { BT_DOWN=0, BT_UP=1 };
	enum ButtonType  { BT_LEFT=0, BT_MID=1, BT_RIGHT=2, BT_WHELL_DOWN=3, BT_WHEEL_UP=4 };
	
	switch ( button ) {
		;//todo
	}
}

/////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
/////////////////////////////////////////////////////////////////
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (400, 600); 
	glutInitWindowPosition (500, 100);
	glutCreateWindow (argv[0]);
	
	init ();
		glutDisplayFunc(display); 
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutMouseFunc(mouse);
		glutMainLoop();
	destroy();
	
	return 0;
}