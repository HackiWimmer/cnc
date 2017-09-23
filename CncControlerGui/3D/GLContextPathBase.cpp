#include <iostream>
#include "3D/GLContextPathBase.h"

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
void OpenGLContextCncPathBase::determineModel() {
/////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	
	glColor3f (1.0, 1.0, 1.0);
	
	switch ( drawType ) {
	
		case DT_LINE_STRIP:
				glBegin(GL_LINE_STRIP);
				
				//todo
					for( GLI::GLCncPath::iterator it = cncPath.begin(); it < cncPath.end(); it++ ) {
						glVertex3f(it->getX() * 0.8, it->getY() * 0.8, it->getZ() * 0.8);
					}
					
				glEnd();
				break;
					
		case DT_POINTS:
				glBegin(GL_POINTS);
				
				//todo
					for( GLI::GLCncPath::iterator it = cncPath.begin(); it < cncPath.end(); it++ ) {
						glVertex3f(it->getX() * 0.8, it->getY() * 0.8, it->getZ() * 0.8);
					}
					
				glEnd();
				break;
	}
}