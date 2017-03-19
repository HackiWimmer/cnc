#include <iostream>

#include "wx/glcanvas.h"
#include "CncDrawPane.h"
#include "CncDrawPaneContext.h"

/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPaneContext::CncOpenGLDrawPaneContext(wxGLCanvas *canvas)
/////////////////////////////////////////////////////////////////////	
: wxGLContext(canvas)
{
	SetCurrent(*canvas);

	// set up the parameters we want to use
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	// add slightly more light, the default lighting is rather dark
	GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.5 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// set viewing projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

	CncOpenGLDrawPane::CheckGLError();
}
/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPaneContext::~CncOpenGLDrawPaneContext() {
/////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawX() {
/////////////////////////////////////////////////////////////////////
	return;
	//todo
	
	double p[4][2] = {
		{ 0.0f, 0.0f}, //0
		{ 1.0f, 1.0f}, //1
		{ 0.0f, 1.0f}, //2
		{ 1.0f, 0.0f}, //3
	};
	
	double scale = 0.05;
	
    glBegin(GL_LINES);
    	glColor3f(0.0, 0.0, 1.0); 
    	
        glVertex2f(p[0][0] * scale, p[0][1] * scale);
        glVertex2f(p[1][0] * scale, p[1][1] * scale);

        glVertex2f(p[2][0] * scale, p[2][1] * scale);
        glVertex2f(p[3][0] * scale, p[3][1] * scale);
        
    glEnd();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawY() {
/////////////////////////////////////////////////////////////////////	
	//todo
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawZ() {
/////////////////////////////////////////////////////////////////////	
	//todo
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::displayCoordinateOrigin() {
/////////////////////////////////////////////////////////////////////
	glDisable(GL_LINE_STIPPLE);
	
    glBegin(GL_LINES);
    	glColor3f(1.0, 0.0, 0.0); // red
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.5f, 0.0f, 0.0f);
    glEnd();
    drawX();

    glBegin(GL_LINES);			// green
    	glColor3f(0.0, 1.0, 0.0); 
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();
    drawY();

    glBegin(GL_LINES);			// blue
    	glColor3f(0.0, 0.0, 1.0); 
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.5f);
    glEnd();
	drawZ();

    glFlush();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::displayDataVector(DrawPaneData& dpd) {
/////////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	displayCoordinateOrigin();

	// display given data
	if ( dpd.size() > 0 ) {
			for( DrawPaneData::iterator it = dpd.begin(); it<dpd.end(); it++ ) {
				
				DoublePoint3D p1 = (*it).getP1();
				DoublePoint3D p2 = (*it).getP2();
				wxColour col     = (*it).getDrawColour();
				int style        = (*it).getLineStyle();
				
				switch ( style ) {
					case wxDOT: 		glLineStipple(2, 0xAAAA); 
										glEnable(GL_LINE_STIPPLE);
										break;
								
					case wxDOT_DASH:	glLineStipple(1, 0x0C0F); 
										glEnable(GL_LINE_STIPPLE);
										break;
								
					default: 			glDisable(GL_LINE_STIPPLE);
				}

				glBegin(GL_LINES);
					glColor3f(col.Red()/255.0f, col.Green()/255.0f, col.Blue()/255.0f); 
					
					//glNormal3f( 0.0f, 0.0f, 1.0f);
					glVertex3f(p1.getX(), p1.getY(), p1.getZ());
					glVertex3f(p2.getX(), p2.getY(), p2.getZ());
				glEnd();
			}
	}

    glFlush();
    CncOpenGLDrawPane::CheckGLError();
}

