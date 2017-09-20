#ifndef OPENGL_CONTEXTBASE_H
#define OPENGL_CONTEXTBASE_H

//#include <wx/wx.h>
#include <wx/colour.h>

#include "3D/GLViewPort.h"
#include "3D/GLCncPathData.h"

#ifdef _USE_WX_DUMMY_CANVAS 
	#include "wxDummyCanvas.h"
#else
	#include <wx/glcanvas.h>
#endif

/////////////////////////////////////////////////////////////////
class OpenGLContextBase : public wxGLContext {
	
	public:
		OpenGLContextBase(wxGLCanvas* canvas);
		virtual ~OpenGLContextBase();
		
		// public context interface 
		virtual const char* getContextName() = 0;
		virtual void keyboardHandler(unsigned char c);
		
		void init();
		void display();
		void reshape(int w, int h);
		
	protected:

		////////////////////////////////////////////////////
		class CoordOrginInfo {
			public:
				struct Colours {
					wxColour x;
					wxColour y;
					wxColour z;
					
					/////////////////////////////////////////////
					Colours() 
					: x(255, 0, 0)
					, y(0, 255, 0)
					, z(0, 0, 255)
					{}
				};
			
				Colours colours;
				float length;

				/////////////////////////////////////////////////
				CoordOrginInfo() 
				: colours()
				, length(0.25f)
				{}
		};

		bool 			initialized;
		CoordOrginInfo 	coordOriginInfo;
		GLViewPort*		viewPort;
		
		virtual void displayCoordinateOrigin();
		virtual void determineViewPort(int w, int h);
		
		// protected context interface
		virtual void initContext() = 0;
		virtual void determineProjection(int w, int h) = 0;
		virtual void displayContext() = 0;
		virtual GLViewPort* createViewPort() = 0;
};

/////////////////////////////////////////////////////////////////
class OpenGLContextCncPathBase : public OpenGLContextBase {
	
	public:
		/////////////////////////////////////////////////////////
		OpenGLContextCncPathBase(wxGLCanvas* canvas)
		: OpenGLContextBase(canvas)
		, cncPath()
		{
			cncPath.reserve(1024 * 1024);
		}
		
		/////////////////////////////////////////////////////////
		virtual ~OpenGLContextCncPathBase() {
			clearData();
		}
		
		/////////////////////////////////////////////////////////
		void clearData() {
			cncPath.clear();
		}
		
		/////////////////////////////////////////////////////////
		const GLCncPath& getData() {
			return cncPath;
		}
		
		/////////////////////////////////////////////////////////
		void traceData(std::ostream out) {
			out << "Size: " << cncPath.size() << std::endl;
			for( GLCncPath::iterator it = cncPath.begin(); it < cncPath.end(); it++ ) {
				out << ' ' << it->x << ',' << it->y << ',' << it->z << std::endl;
			}
		}
		
		/////////////////////////////////////////////////////////
		void appendData(const GLCncPathVertices& cpv) {
			cncPath.push_back(cpv);
		}
		
		/////////////////////////////////////////////////////////
		void appendData(const GLCncPath& cp) {
			if ( cp.size() > 0 )	cncPath.assign(cp.begin(), cp.end() - 1);
			else 					clearData();
		}
		
	protected:
		GLCncPath	cncPath;
};

#endif