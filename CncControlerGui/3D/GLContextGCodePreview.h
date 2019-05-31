#ifndef OPENGL_CONTEXT_GCODE_PREVIEW_H
#define OPENGL_CONTEXT_GCODE_PREVIEW_H

#include "3D/GLContextCncPathBase.h"

/////////////////////////////////////////////////////////////////
class GLContextGCodePreview : public GLContextCncPathBase {
	
	public:
		GLContextGCodePreview(wxGLCanvas* canvas, wxString name);
		virtual ~GLContextGCodePreview();
		
	protected:
		wxString previewName;
		
		virtual void initContext();
		virtual void determineProjection(int w, int h);
		virtual void determineModel();
		virtual GLViewPort* createViewPort();
};

#endif