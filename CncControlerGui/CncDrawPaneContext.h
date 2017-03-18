#ifndef CNC_OPENGL_DRAW_PANE_CONTEXT_H
#define CNC_OPENGL_DRAW_PANE_CONTEXT_H

#include <vector>
#include "CncPoint3D.h"

////////////////////////////////////////////////////////////		
struct DisplayAngels {
////////////////////////////////////////////////////////////		
    float angleX;
    float angleY;
    float angleZ;
	
	///////////////////////////////////////////////////////
	DisplayAngels () {
	///////////////////////////////////////////////////////	
		DisplayAngels (0.0, 0.0, 0.0);
	}
	
	///////////////////////////////////////////////////////
	DisplayAngels(float ax, float ay, float az) {
	///////////////////////////////////////////////////////	
		angleX = ax;
		angleY = ay;
		angleZ = az;
	}
};

typedef std::vector<DoublePointPair3D> DrawPaneData;
	
////////////////////////////////////////////////////////////	
class CncOpenGLDrawPaneContext : public wxGLContext {
////////////////////////////////////////////////////////////	

	public:
	    CncOpenGLDrawPaneContext(wxGLCanvas *canvas);
	    ~CncOpenGLDrawPaneContext();
	
	    // render the data 
	    void displayDataVector(DrawPaneData& dpd);
	                           
	
	private:
		void displayCoordinateOrigin();
		
		void drawX();
		void drawY();
		void drawZ();

};

#endif