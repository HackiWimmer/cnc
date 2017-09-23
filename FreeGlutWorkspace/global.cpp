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
/////////////////////////////////////////////////////////////////