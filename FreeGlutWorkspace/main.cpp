#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Sample2.h"


/////////////////////////////////////////////////////////////////
// global vars
/////////////////////////////////////////////////////////////////
OpenGLContext* currentContext 	= NULL;
OpenGLContext* context0 		= NULL;
OpenGLContext* context1 		= NULL;

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
	
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	display();
}

/////////////////////////////////////////////////////////////////
void init() 
/////////////////////////////////////////////////////////////////
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
	
	context0 = new OpenGLDrawPaneContext0();
	context1 = new OpenGLDrawPaneContext1();
	
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
int main(int argc, char** argv)
/////////////////////////////////////////////////////////////////
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	
	init ();
		glutDisplayFunc(display); 
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutMainLoop();
	destroy();
	
	return 0;
}