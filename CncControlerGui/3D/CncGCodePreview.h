#ifndef CNC_GCODE_PREVIEW_H
#define CNC_GCODE_PREVIEW_H

#include <wx/timer.h>

#include "3D/CncGLCanvas.h"
#include "CncPosition.h"
#include "CncCommon.h"
#include "3D/VerticeData.h"
#include "3D/GLContextGCodePreview.h"

////////////////////////////////////////////////////////////
class CncGCodePreview : public CncGlCanvas {

	public:

		// constructor
		CncGCodePreview(wxWindow *parent, wxString name, int *attribList = NULL);
		virtual ~CncGCodePreview();
		
		// is used from global key down hook, that's the reason why it is public
		void onKeyDown(wxKeyEvent& event);
		
		// interface
		void setMaxDimension(double maxDim); 
		void appendVertice(const GLI::VerticeDoubleData& vd);
		void clear();
		
		void pushProcessMode();
		void popProcessMode();

		void onPaint(wxPaintEvent& event);
		void onMouse(wxMouseEvent& event);
		void onSize(wxSizeEvent& event);
		void onEraseBackground(wxEraseEvent& event);
		
		bool isAlreadyShown() { return isShown; }
		
	protected:
		
		wxString previewName;
		GLContextGCodePreview* preview;
		double maxDimension;
		bool isShown;
		
	private:
		void appendVertice(float x, float y, float z, GLI::GLCncPathVertices::CncMode cm);
		
		wxDECLARE_NO_COPY_CLASS(CncGCodePreview);
		wxDECLARE_EVENT_TABLE();
};

#endif