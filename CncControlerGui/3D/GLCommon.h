#ifndef CNC_GL_COMMON_H
#define CNC_GL_COMMON_H

#include <iostream>
#include <sstream>
#include "GLInclude.h"

struct GLCommon {
	
	private:
		static int  glTraceLevel;
		static bool glInitializedGlobalFlag;
		static bool glewInitializedGlobalFlag;
	
		static bool initGlew();
		
		static void traceOpenGLVersionInfo();
		static void traceOpenGLExtentionInfo();
		
	public:

		static void setTraceLevel(int level)	{ glTraceLevel = level; }
		static int  getTraceLevel()				{ return glTraceLevel; }

		static bool isGlAvailable() 			{ return glInitializedGlobalFlag; }
		static bool isGlewAvailable() 			{ return glewInitializedGlobalFlag; }
		static bool isReady() 					{ return isGlAvailable() && isGlewAvailable(); }
		
		static bool initOpenGL();
		static void GLAPIENTRY MessageCallback(	 GLenum source,
												 GLenum type,
												 GLuint id,
												 GLenum severity,
												 GLsizei length,
												 const GLchar* message,
												 const void* userParam);
		
		static int checkGLError(const char* file, unsigned int line, const char* funct);
};

// *****************************************************************
// Logging start
// *****************************************************************
	// usage:
	// GL_COMMON_CHECK_ERROR;
	// 
	// if ( GL_COMMON_CHECK_ERROR > 0 )
	//	;
	
	#define GL_ERROR_TRACE_LEVEL 2
	#if ( GL_ERROR_TRACE_LEVEL > 0 )
		
		#if ( GL_ERROR_TRACE_LEVEL > 1 )
		#define GL_COMMON_CHECK_ERROR \
				GLCommon::checkGLError(__FILE__, __LINE__, __PRETTY_FUNCTION__)
		#else
			#define GL_COMMON_CHECK_ERROR \
				GLCommon::checkGLError(NULL, __LINE__, __PRETTY_FUNCTION__)
		#endif
		
	#else

		#define GL_COMMON_CHECK_ERROR 		0
		
	#endif
	
// *****************************************************************
// Logging end
// *****************************************************************

#endif
