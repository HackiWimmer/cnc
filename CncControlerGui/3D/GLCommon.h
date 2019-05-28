#ifndef CNC_GL_COMMON_H
#define CNC_GL_COMMON_H

#include <iostream>
#include <sstream>
#include "GlInclude.h"

struct GLCommon {
	
	static bool glewInitializedGlobalFlag;

	/////////////////////////////////////////////////////////////////////
	static bool isGlewAvailable() { return glewInitializedGlobalFlag; }
	
	/////////////////////////////////////////////////////////////////////
	static void GLAPIENTRY MessageCallback(	 GLenum source,
											 GLenum type,
											 GLuint id,
											 GLenum severity,
											 GLsizei length,
											 const GLchar* message,
											 const void* userParam )
	{
		std::cerr << wxString::Format("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
										(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
										type, 
										severity, 
										message 
									 );
	}
	
	/////////////////////////////////////////////////////////////////////
	static void checkGLError(const char* cont) {

		GLenum errLast = GL_NO_ERROR;

		wxString context("");
		if ( cont != NULL )
			context.assign(cont);

		for ( ;; )
		{
			GLenum err = glGetError();
			if ( err == GL_NO_ERROR )
				return;

			// normally the error is reset by the call to glGetError() but if
			// glGetError() itself returns an error, we risk looping forever here
			// so check that we get a different error than the last time
			if ( err == errLast )
			{
				std::cerr << "OpenGLContextBase::checkGLError(" << context << "): OpenGL error state couldn't be reset." << std::endl;
				return;
			}

			errLast = err;
	 
			std::stringstream ss;
			ss << gluErrorString(err);
			std::cerr << "OpenGLContextBase::checkGLError(" << context << "): " << wxString::Format("OpenGL error [ %d ]: %s", err, ss.str()) << std::endl;
		}
	}
};


#endif