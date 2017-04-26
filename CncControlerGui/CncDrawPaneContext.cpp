#include <iostream>

#include "wx/glcanvas.h"
#include "CncCommon.h"
#include "CncDrawPane.h"
#include "CncDrawPaneContext.h"

/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPaneContext::CncOpenGLDrawPaneContext(wxGLCanvas *canvas)
/////////////////////////////////////////////////////////////////////
: wxGLContext(canvas)
, axises()
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
void CncOpenGLDrawPaneContext::drawZeroPlane() {
/////////////////////////////////////////////////////////////////////
	if ( workpieceInfo.drawZeroPlane == false )
		return;
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	GLfloat c = (GLfloat)128/255;
	glColor4f(c, c, c, 0.3);

	glBegin(GL_POLYGON);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.5, 0.0, 0.0);
		glVertex3f(0.5, 0.5, 0.0);
		glVertex3f(0.0, 0.5, 0.0);
	glEnd();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawWorkpieceSurface() {
/////////////////////////////////////////////////////////////////////
	if ( cnc::dblCompare(workpieceInfo.thickness, 0.0) == true )
		return;
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	if ( workpieceInfo.drawWorkpieceSurface == true ) {
		GLfloat c = (GLfloat)64/255;
		glColor4f(c, c, c, 0.3);

		glBegin(GL_POLYGON);
			glVertex3f(0.0, 0.0, workpieceInfo.thickness);
			glVertex3f(0.5, 0.0, workpieceInfo.thickness);
			glVertex3f(0.5, 0.5, workpieceInfo.thickness);
			glVertex3f(0.0, 0.5, workpieceInfo.thickness);
		glEnd();
	}
	
	if ( workpieceInfo.drawWorkpieceOffset == true ) {
		GLfloat c = (GLfloat)32/255;
		glColor4f(c, c, c, 0.3);

		glBegin(GL_POLYGON);
			glVertex3f(0.0, 0.0, workpieceInfo.thickness + workpieceInfo.offset);
			glVertex3f(0.5, 0.0, workpieceInfo.thickness + workpieceInfo.offset);
			glVertex3f(0.5, 0.5, workpieceInfo.thickness + workpieceInfo.offset);
			glVertex3f(0.0, 0.5, workpieceInfo.thickness + workpieceInfo.offset);
		glEnd();
	}
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawX() {
/////////////////////////////////////////////////////////////////////
	double p[4][2] = {
		{ 0.0f, 0.0f}, //0
		{ 1.0f, 1.0f}, //1
		
		{ 0.0f, 1.0f}, //2
		{ 1.0f, 0.0f}, //3
	};
	
	glBegin(GL_LINES);
	
		glColor3f(axises.colours.x.Red()/255, axises.colours.x.Green()/255, axises.colours.x.Blue()/255);
		
		glVertex3f(p[0][0] * axises.letterScale + axises.length, p[0][1] * axises.letterScale, 0.0f);
		glVertex3f(p[1][0] * axises.letterScale + axises.length, p[1][1] * axises.letterScale, 0.0f);
		
		glVertex3f(p[2][0] * axises.letterScale + axises.length, p[2][1] * axises.letterScale, 0.0f);
		glVertex3f(p[3][0] * axises.letterScale + axises.length, p[3][1] * axises.letterScale, 0.0f);
		
	glEnd();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawY() {
/////////////////////////////////////////////////////////////////////	
	double p[6][2] = {
		{ 0.0f, 0.0f}, //0
		{ 0.5f, 0.5f}, //1
		
		{ 0.5f, 0.5f}, //2
		{ 1.0f, 0.0f}, //3
		
		{ 0.5f, 0.5f}, //4
		{ 0.5f, 1.0f}, //5
	};
	
	glBegin(GL_LINES);

		glColor3f(axises.colours.y.Red()/255, axises.colours.y.Green()/255, axises.colours.y.Blue()/255);
		
		glVertex3f(p[0][0] * axises.letterScale, p[0][1] * axises.letterScale + axises.length, 0.0f);
		glVertex3f(p[1][0] * axises.letterScale, p[1][1] * axises.letterScale + axises.length, 0.0f);
		
		glVertex3f(p[2][0] * axises.letterScale, p[2][1] * axises.letterScale + axises.length, 0.0f);
		glVertex3f(p[3][0] * axises.letterScale, p[3][1] * axises.letterScale + axises.length, 0.0f);
		
		glVertex3f(p[4][0] * axises.letterScale, p[4][1] * axises.letterScale + axises.length, 0.0f);
		glVertex3f(p[5][0] * axises.letterScale, p[5][1] * axises.letterScale + axises.length, 0.0f);
		
	glEnd();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawZ() {
/////////////////////////////////////////////////////////////////////	
	double p[6][2] = {
		{ 0.0f, 0.0f}, //0
		{ 1.0f, 0.0f}, //1
		
		{ 1.0f, 0.0f}, //2
		{ 0.0f, 1.0f}, //3
		
		{ 0.0f, 1.0f}, //4
		{ 1.0f, 1.0f}, //5
	};
	
	glBegin(GL_LINES);
	
		glColor3f(axises.colours.z.Red()/255, axises.colours.z.Green()/255, axises.colours.z.Blue()/255);
		
		glVertex3f(p[0][0] * axises.letterScale, p[0][1] * axises.letterScale, axises.length);
		glVertex3f(p[1][0] * axises.letterScale, p[1][1] * axises.letterScale, axises.length);
		
		glVertex3f(p[2][0] * axises.letterScale, p[2][1] * axises.letterScale, axises.length);
		glVertex3f(p[3][0] * axises.letterScale, p[3][1] * axises.letterScale, axises.length);
		
		glVertex3f(p[4][0] * axises.letterScale, p[4][1] * axises.letterScale, axises.length);
		glVertex3f(p[5][0] * axises.letterScale, p[5][1] * axises.letterScale, axises.length);
		
	glEnd();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::displayCoordinateOrigin() {
/////////////////////////////////////////////////////////////////////
	
	glBegin(GL_LINES);

		glColor3f(axises.colours.x.Red()/255, axises.colours.x.Green()/255, axises.colours.x.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(axises.length, 0.0f, 0.0f);
		
	glEnd();
	drawX();

	glBegin(GL_LINES);

		glColor3f(axises.colours.y.Red()/255, axises.colours.y.Green()/255, axises.colours.y.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, axises.length, 0.0f);
	
	glEnd();
	drawY();

	glBegin(GL_LINES);

		glColor3f(axises.colours.z.Red()/255, axises.colours.z.Green()/255, axises.colours.z.Blue()/255);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, axises.length);
	
	glEnd();
	drawZ();
	
	glFlush();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::displayDataVector(DrawPaneData& dpd) {
/////////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LINE_STIPPLE);
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
	
	drawZeroPlane();
	drawWorkpieceSurface();

    glFlush();
    CncOpenGLDrawPane::CheckGLError();
}