#include <iostream>
#include <sstream>
#include "CncConfig.h"
#include "CncContext.h"
#include "3D/GLContextCncPathBase.h"
#include "GLCommon.h"

int  GLCommon::glTraceLevel				 = 1;
bool GLCommon::glInitializedGlobalFlag   = false;
bool GLCommon::glewInitializedGlobalFlag = false;

/////////////////////////////////////////////////////////////////
void GLCommon::traceOpenGLVersionInfo() {
/////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << ( glGetString(GL_VERSION)  ? (const char*)glGetString(GL_VERSION)  : "?" )	<< "; "
	   << ( glGetString(GL_VENDOR)   ? (const char*)glGetString(GL_VENDOR)   : "?" )	<< "; "
	   << ( glGetString(GL_RENDERER) ? (const char*)glGetString(GL_RENDERER) : "?" );
	   
	GBL_CONTEXT->versionInfoMap["opengl"] = ss.str().c_str();
	
	ss.str("");
	ss << (GLCommon::isGlewAvailable() ? glewGetString(GLEW_VERSION) : (const unsigned char*)"Glew isn't initialized!" );
		
	GBL_CONTEXT->versionInfoMap["glew"] = ss.str().c_str();
}
/////////////////////////////////////////////////////////////////
void GLCommon::traceOpenGLExtentionInfo() {
/////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Extention list:" << std::endl;
	wxString ext(glGetString(GL_EXTENSIONS));
	wxStringTokenizer extentions(ext, " ");
	
	while ( extentions.HasMoreTokens() ) {
		wxString token = extentions.GetNextToken();
		ss << " " << token << std::endl;
	}
}
/////////////////////////////////////////////////////////////////
bool GLCommon::initOpenGL() {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::initGlew() == false )
		return GLCommon::glInitializedGlobalFlag;

	if ( GLCommon::glInitializedGlobalFlag == true )
		return GLCommon::glInitializedGlobalFlag;
		
	//glDebugMessageCallback(GLCommon::MessageCallback, 0);
	
	GLCommon::glInitializedGlobalFlag = true;
	
	if ( GLCommon::glInitializedGlobalFlag == true ) {
		GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("OpenGL interface is ready to use . . . "));
	} else {
		if ( GL_ERROR_TRACE_LEVEL > 0 ) 
			GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("OpenGL State = %d", GLCommon::glInitializedGlobalFlag));
	}

	return GLCommon::glInitializedGlobalFlag;
}
/////////////////////////////////////////////////////////////////
bool GLCommon::initGlew() {
/////////////////////////////////////////////////////////////////
	if ( GLCommon::glewInitializedGlobalFlag == true )
		return GLCommon::glewInitializedGlobalFlag;
		
	GLenum err = glewInit();
	if ( err != GLEW_OK ) {
		
		GLCommon::glewInitializedGlobalFlag 	= false;
	}
	else {
		
		GLCommon::glewInitializedGlobalFlag 	= true;
		GLCommon::traceOpenGLVersionInfo();
	}

	if ( GLCommon::glewInitializedGlobalFlag == true ) {
		GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("OpenGL GLEW interface is ready to use . . . "));
	} else {
		if ( GL_ERROR_TRACE_LEVEL > 0 ) 
			GL_CTX_OBS->appendMessage('I', CNC_LOG_FUNCT, wxString::Format("GLEW state = %d", GLCommon::glewInitializedGlobalFlag));
	}
	return GLCommon::glewInitializedGlobalFlag;
}
/////////////////////////////////////////////////////////////////////
void GLAPIENTRY GLCommon::MessageCallback(	GLenum source,
											GLenum type,
											GLuint id,
											GLenum severity,
											GLsizei length,
											const GLchar* message,
											const void* userParam )
/////////////////////////////////////////////////////////////////////
{
	std::cerr << wxString::Format("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
									(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
									type, 
									severity, 
									message 
								 );
}
/////////////////////////////////////////////////////////////////////
int GLCommon::checkGLError(const char* file, unsigned int line, const char* funct) {
/////////////////////////////////////////////////////////////////////
	// --------------------------------------------------------------
	auto trace = [&](GLenum err, const std::stringstream& ss) {

		std::cerr	<< wxString::Format("GLCommon::checkGLError(): OpenGL error [ %d ]: '%s'", err, ss.str()) 
					<< std::endl;
					
		if ( file != NULL ) 	std::cerr	<< " [ " << file << ":" << line << " ]\n [ " << funct << " ]"; 
		else 					std::cerr	<< " [ " << funct << ":" << line << " ]"; 
		
		std::cerr << std::endl;
	};

	// --------------------------------------------------------------
	GLenum errLast = GL_NO_ERROR;

	int ret = 0;
	for ( ;; )
	{
		GLenum err = glGetError();
		if ( err == GL_NO_ERROR )
			return ret;

		// normally the error is reset by the call to glGetError() but if
		// glGetError() itself returns an error, we risk looping forever here
		// so check that we get a different error than the last time
		if ( err == errLast )
		{
			std::stringstream ss;
			ss << "OpenGL error state couldn't be reset.";
			
			trace (err, ss);
			return ret;
		}

		errLast = err;
		++ret;
 
		std::stringstream ss;
		ss << gluErrorString(err);
		
		trace (err, ss);
	}
	
	return ret;
}
