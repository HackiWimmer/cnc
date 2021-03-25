#ifndef GL_INCLUDE_H
#define GL_INCLUDE_H

	#include <GL/glew.h>

	#ifdef _USE_WX_DUMMY_CANVAS 
	
		#include "wxDummyCanvas.h"
		
	#else
	
		#include <wx/glcanvas.h>
		
		#if !wxUSE_GLCANVAS
			#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
		#endif
		
	#endif
	
	
	#define GLEW_STATIC 
	#define FREEGLUT_STATIC
	
	//#include <GL/gl.h>
	//#include <GL/glext.h>
	#include <GL/glut.h>
	
#endif

