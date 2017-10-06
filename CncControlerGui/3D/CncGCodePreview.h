#ifndef CNC_GCODE_PREVIEW_H
#define CNC_GCODE_PREVIEW_H

#include <wx/glcanvas.h>
#include <wx/timer.h>

#include "CncPosition.h"
#include "CncCommon.h"
#include "CncConfig.h"
#include "3D/GLContextGCodePreview.h"

////////////////////////////////////////////////////////////
class CncGCodePreview : public wxGLCanvas {

	public:

		class VerticeData {

			public:
				
				//////////////////////////////////////////////
				VerticeData()
				: _x(0)
				, _y(0)
				, _z(0)
				, mode(GLI::GLCncPathVertices::CncMode::CM_FLY)
				{}

				//////////////////////////////////////////////
				virtual ~VerticeData() {
				}

				//////////////////////////////////////////////
				double getX() const { return _x; }
				double getY() const { return _y; }
				double getZ() const { return _z; }
				
				const GLI::GLCncPathVertices::CncMode getMode() const { return mode; }

				//////////////////////////////////////////////
				void setVertice(CncSpeed speedType, const CncDoublePosition& cp) {
					switch ( speedType ) {
						case CncSpeedWork: 	setWorkVertice(cp);
											break;
											
						case CncSpeedRapid: setRapidVertice(cp);
											break;
					}
				}

			protected:

				//CncLongPosition pos;
				double _x;
				double _y;
				double _z;
				GLI::GLCncPathVertices::CncMode mode;
				
				//////////////////////////////////////////////
				void setRapidVertice(const CncDoublePosition& cp) {
					_x = cp.getX();
					_y = cp.getY();
					_z = cp.getZ();
					
					mode = GLI::GLCncPathVertices::CncMode::CM_FLY;
				}

				//////////////////////////////////////////////
				void setWorkVertice(const CncDoublePosition& cp) {
					_x = cp.getX();
					_y = cp.getY();
					_z = cp.getZ();
					
					mode = GLI::GLCncPathVertices::CncMode::CM_WORK;
				}
		};

		// constructor
		CncGCodePreview(wxWindow *parent, int *attribList = NULL);
		virtual ~CncGCodePreview();
		
		// is used from global key down hook, that's the reason why it is public
		void onKeyDown(wxKeyEvent& event);

		// interface
		void appendVertice(const CncGCodePreview::VerticeData& vd);
		void clear();
		
		void pushProcessMode();
		void popProcessMode();

		void onPaint(wxPaintEvent& event);
		void onMouse(wxMouseEvent& event);
		void onSize(wxSizeEvent& event);
		void onEraseBackground(wxEraseEvent& event);
/*
		void view(GLContextBase::ViewMode fm);
*/

		
	protected:
		GLContextCncPathBase* preview;
		
	private:
		void appendVertice(float x, float y, float z, GLI::GLCncPathVertices::CncMode cm);
		
		wxDECLARE_NO_COPY_CLASS(CncGCodePreview);
		wxDECLARE_EVENT_TABLE();
};

#endif