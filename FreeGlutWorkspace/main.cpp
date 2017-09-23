#include <iostream>
#include "3D/GLContextPathBase.h"
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
OpenGLContextCncPathBase* currentContext 	= NULL;
OpenGLContextCncPathBase* context0 			= NULL;
OpenGLContextCncPathBase* context1 			= NULL;

void display();
void reshape(int w, int h);

/////////////////////////////////////////////////////////////////
void headline() {
/////////////////////////////////////////////////////////////////
	std::clog << "#############################################################" << std::endl;
}

/////////////////////////////////////////////////////////////////
void fillDataVector() {
/////////////////////////////////////////////////////////////////
	if ( currentContext == 0 )
		return;
		
	std::clog << "main::fillDataVector()" << std::endl;
		
	currentContext->clearPathData();
	
	GLI::GLCncPathVertices vertice;
	currentContext->appendPathData(vertice.set(0, 0, 0));
	currentContext->appendPathData(vertice.set(1, 0, 0));
	currentContext->appendPathData(vertice.set(1, 1, 0));
	currentContext->appendPathData(vertice.set(0, 1, 0));
	currentContext->appendPathData(vertice.set(0, 0, 0));
}

/////////////////////////////////////////////////////////////////
void switchContext(int id) 
/////////////////////////////////////////////////////////////////
{
	switch ( id ) {
		case 0: 	currentContext = context0; 
					break;
					
		case 1: 	currentContext = context1; 
					fillDataVector();
					break;
		
		default: 	currentContext = NULL;
	}
	
	if ( currentContext != NULL ) {
		std::clog << "main::Context switched to: " << currentContext->getContextName() << " ["<< id  << "]" << std::endl;
		
		currentContext->setViewMode(OpenGLContextBase::V2D_TOP);
		currentContext->reshapeViewMode(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		display();
	} else {
		std::cerr << "main::Context switch failt to: " << id << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void setViewMode(unsigned char c) {
/////////////////////////////////////////////////////////////////
	if ( currentContext == NULL )
		return;
	
	std::clog << "main::switch view mode to: ";
	switch ( c ) {
		case '1': currentContext->setViewMode(OpenGLContextBase::V2D_TOP); 		std::clog << c; break;
		case '2': currentContext->setViewMode(OpenGLContextBase::V2D_BOTTOM); 	std::clog << c; break;
		case '3': currentContext->setViewMode(OpenGLContextBase::V2D_LEFT); 	std::clog << c; break;
		case '4': currentContext->setViewMode(OpenGLContextBase::V2D_RIGHT); 	std::clog << c; break;
		case '5': currentContext->setViewMode(OpenGLContextBase::V2D_FRONT); 	std::clog << c; break;
		case '6': currentContext->setViewMode(OpenGLContextBase::V2D_REAR); 	std::clog << c; break;
		case '7': currentContext->setViewMode(OpenGLContextBase::V3D_ISO1); 	std::clog << c; break;
		case '8': currentContext->setViewMode(OpenGLContextBase::V3D_ISO2); 	std::clog << c; break;
		case '9': currentContext->setViewMode(OpenGLContextBase::V3D_ISO3); 	std::clog << c; break;
		case '0': currentContext->setViewMode(OpenGLContextBase::V3D_ISO4); 	std::clog << c; break;
		default: 	std::clog << '?' << " It still stay unchanged";
	}
	std::clog << std::endl;
	
	currentContext->reshapeViewMode(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	display();
}

/////////////////////////////////////////////////////////////////
void centerViewport() {
/////////////////////////////////////////////////////////////////
	if ( currentContext == NULL )
		return;
	
	std::clog << "main::centerViewport()" << std::endl;
		
	currentContext->centerViewport();
	
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	display();
}

/////////////////////////////////////////////////////////////////
void resetRotation() {
/////////////////////////////////////////////////////////////////
	if ( currentContext == NULL )
		return;
		
	currentContext->resetAngles();
	display();
}
/////////////////////////////////////////////////////////////////
void switchDrawType() {
/////////////////////////////////////////////////////////////////
	if ( currentContext == NULL )
		return;
		
	if ( currentContext->getDrawType() == OpenGLContextCncPathBase::DT_POINTS )
		currentContext->setDrawType(OpenGLContextCncPathBase::DT_LINE_STRIP);
	else 
		currentContext->setDrawType(OpenGLContextCncPathBase::DT_POINTS);
		
	display();
}

/////////////////////////////////////////////////////////////////
void init() 
/////////////////////////////////////////////////////////////////
{
	std::clog << "main::init()" << std::endl;
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
	
	context0 = new OpenGLContextTestCube(NULL);
	context1 = new OpenGLContextCncPath(NULL);
	
	context0->init();
	context1->init();
	
	switchContext(1);
}

/////////////////////////////////////////////////////////////////
void destroy() 
/////////////////////////////////////////////////////////////////
{
	std::clog << "main::destroy()" << std::endl;
	
	if ( context0 != NULL ) delete context0;
	if ( context1 != NULL ) delete context1;
}

/////////////////////////////////////////////////////////////////
void display()
/////////////////////////////////////////////////////////////////
{
	std::cerr << "main::display()" << std::endl;
	if ( currentContext != NULL ) {
		currentContext->display();
	} else {
		std::cerr << "main::display(): currentContext == NULL" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void reshape(int w, int h, int x, int y)
/////////////////////////////////////////////////////////////////
{
	std::clog << "main::reshap(" << w << "," << h << "," << x << "," << y << ")" << std::endl;
	if ( currentContext != NULL ) {
		currentContext->reshape(w, h, x, y);
	} else {
		std::cerr << "reshap(): currentContext == NULL" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void reshape(int w, int h)
/////////////////////////////////////////////////////////////////
{
	std::clog << "main::reshap(" << w << "," << h << ")" << std::endl;
	if ( currentContext != NULL ) {
		currentContext->reshape(w, h);
	} else {
		std::cerr << "main::reshap(): currentContext == NULL" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
void keyboard(unsigned char c, int x, int y)
/////////////////////////////////////////////////////////////////
{
	bool shift = false, ctrl = false, alt = false;
	switch ( glutGetModifiers() ) {
		case GLUT_ACTIVE_SHIFT:		shift = true; break;
		case GLUT_ACTIVE_CTRL:		ctrl  = true; break;
		case GLUT_ACTIVE_ALT:		alt   = true; break;
	}
	
	if ( false ) {
		std::clog << "main::keyboard('" << c << "' [" << (int) c << "]) " << x << ", " << y
										<< ", shift: " << shift
										<< ", ctlr: " << ctrl 
										<< ", alt: " << alt << std::endl;
	}
	
	switch (c) {
		case 27:	exit(0);
					break;
					
		case 't':	switchContext(0);
					break;
					
		case 'p':	switchContext(1);
					break;
					
		case 'n':	centerViewport();
					break;
					
		case 'r':	resetRotation();
					break;
					
		case 'd':	switchDrawType();
					break;
					
		case ' ':	headline();
					break;
					
		default: 	if ( c >= '0' && c <= '9')
						setViewMode(c);;
	}
	
	if ( currentContext != NULL )
		currentContext->keyboardHandler(c);
}

/////////////////////////////////////////////////////////////////
void special(int key, int x, int y) {
/////////////////////////////////////////////////////////////////
	if ( currentContext == NULL )
		return;

	bool shift = false, ctrl = false, alt = false;
	switch ( glutGetModifiers() ) {
		case GLUT_ACTIVE_SHIFT:		shift = true; break;
		case GLUT_ACTIVE_CTRL:		ctrl  = true; break;
		case GLUT_ACTIVE_ALT:		alt   = true; break;
	}
	
	if ( false ) {
		std::clog << "main::special('"  << key << ") " << x << ", " << y
										<< ", shift: " << shift
										<< ", ctlr: " << ctrl 
										<< ", alt: " << alt << std::endl;
	}
	
	switch ( key ) {
		case GLUT_KEY_LEFT: 	currentContext->incAngle(); break;
		case GLUT_KEY_UP:		currentContext->incAngle(); break;
		case GLUT_KEY_RIGHT:	currentContext->decAngle(); break;
		case GLUT_KEY_DOWN:		currentContext->decAngle(); break;
	}
}

/////////////////////////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
/////////////////////////////////////////////////////////////////
{
	if ( currentContext == NULL )
		return;
		
	// for debug only
	//std::clog << "main::mouse(" << button << ")(" << state << "), " << x << ", " << y << std::endl;
	
	enum ButtonState { BT_DOWN=0, BT_UP=1 };
	enum ButtonType  { BT_LEFT=0, BT_MID=1, BT_RIGHT=2, BT_WHEEL_UP=3, BT_WHELL_DOWN=4 };
	
	switch ( button ) {
		case BT_WHELL_DOWN: 	if ( state == BT_DOWN )
									currentContext->decScale();
								break;
								
		case BT_WHEEL_UP: 		if ( state == BT_DOWN )	
									currentContext->incScale();
								break;
								
		case BT_LEFT:			if ( state == BT_DOWN ) {
									// reverse y because the opengl viewport origin (0,0) is at left/bottom
									y = glutGet(GLUT_WINDOW_HEIGHT) - y;
									reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), x, y);
									display();
								}
								break;
	}
}

/////////////////////////////////////////////////////////////////
void motion(int x, int y) {
/////////////////////////////////////////////////////////////////
	if ( currentContext == NULL )
		return;
		
	// for debug only
	//std::clog << "main::motion: " << x << ", " << y << std::endl;
	
	// reverse y because the opengl viewport origin (0,0) is at left/bottom
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), x, y);
	display();
}

/////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
/////////////////////////////////////////////////////////////////
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (400, 600); 
	glutInitWindowPosition (500, 100);
	glutCreateWindow (argv[0]);
	
	init ();
		glutDisplayFunc(display); 
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutSpecialFunc(special);
		glutMouseFunc(mouse);
		glutMotionFunc(motion);
		glutMainLoop();
	destroy();
	
	return 0;
}