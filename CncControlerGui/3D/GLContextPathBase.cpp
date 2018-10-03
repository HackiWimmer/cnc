#include <iostream>
#include "3D/GLContextPathBase.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
	#include <OpenGL/freeglut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut.h>
#endif

#include <wx/bitmap.h>
#include "wxcrafter.h"

GLuint theTexture = 0;
/////////////////////////////////////////////////////////
GLuint LoadBMP(const wxImage& img) {
/////////////////////////////////////////////////////////
	const unsigned char *data = img.GetData();
	unsigned int width        = img.GetWidth();
	unsigned int height       = img.GetHeight();
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	return texture;
}
/////////////////////////////////////////////////////////
static void drawBox(GLfloat size, GLenum type) {
/////////////////////////////////////////////////////////
	static GLfloat n[6][3] =
	{
	{-1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{1.0, 0.0, 0.0},
	{0.0, -1.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0, 0.0, -1.0}
	};
	
	static GLint faces[6][4] =
	{
	{0, 1, 2, 3},
	{3, 2, 6, 7},
	{7, 6, 5, 4},
	{4, 5, 1, 0},
	{5, 6, 2, 1},
	{7, 4, 0, 3}
	};
	
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = +size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = +size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = +size / 2;
	
	glBindTexture(GL_TEXTURE_2D, theTexture);
	
	for (i = 5; i >= 0; i--) {
	glBegin(type);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0.0, 0.0); glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(1.0, 0.0); glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1.0, 1.0); glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(0.0, 1.0); glVertex3fv(&v[faces[i][3]][0]);
	glEnd();
	}
}

/////////////////////////////////////////////////////////
GLContextCncPathBase::GLContextCncPathBase(wxGLCanvas* canvas)
: GLContextBase(canvas)
, cncPath()
, drawType(DT_LINE_STRIP)
, currentClientId(-1L)
, boundBox(true)
, boundBoxColour(185, 127, 87)
/////////////////////////////////////////////////////////
{
	cncPath.reserve(1024 * 1024);
	
	wxBitmap bmp = ImageLibBig().Bitmap("BMP_CNC");
	wxImage img  = bmp.ConvertToImage();
	theTexture   = LoadBMP(img);
}
/////////////////////////////////////////////////////////
GLContextCncPathBase::~GLContextCncPathBase() {
/////////////////////////////////////////////////////////
	clearPathData();
}
/////////////////////////////////////////////////////////
void GLContextCncPathBase::markCurrentPosition() {
/////////////////////////////////////////////////////////
	// ensure cncPath.end() - 1 is valid
	if ( cncPath.size() == 0 )
		return;
		
	// get the last/current vecties - it must be valid
	GLI::GLCncPath::iterator it = cncPath.end() - 1;
	drawPosMarker(it->getX(), it->getY(), it->getZ());
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawPoints() {
/////////////////////////////////////////////////////////////////
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	typedef GLI::GLCncPathVertices::FormatType FormatType;
	
	glBegin(GL_POINTS);
	
		int alpha = 255;
		for( GLI::GLCncPath::iterator it = cncPath.begin(); it != cncPath.end(); it++ ) {
			// line stipple and colour depth
			switch ( it->getFormatType() ) {
				case FormatType::FT_SOLID: {
						alpha = 255;
						break;
				}
				case FormatType::FT_TRANSPARENT:
				case FormatType::FT_DOT:
				case FormatType::FT_LONG_DASH:
				case FormatType::FT_SHORT_DASH:
				case FormatType::FT_DOT_DASH: {
						alpha = 0;
						break;
				}
			}
			
			glColor4ub(it->getColour(currentClientId).Red(), it->getColour(currentClientId).Green(), it->getColour(currentClientId).Blue(), alpha);
			
			// determine the vertice
			glVertex3f(it->getX(), it->getY(), it->getZ());
		}
		
	glEnd();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawLines() {
/////////////////////////////////////////////////////////////////
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	typedef GLI::GLCncPathVertices::FormatType FormatType;
	
	if ( cncPath.size() > 1 ) {
		GLI::GLCncPath::iterator prev = cncPath.begin();
		
		int alpha = 255;
		for( GLI::GLCncPath::iterator curr = cncPath.begin() + 1; curr != cncPath.end(); curr++ ) {
			
			// line stipple and colour depth
			switch ( curr->getFormatType() ) {
				case FormatType::FT_TRANSPARENT: {
						alpha = 0;
						break;
				}
				case FormatType::FT_SOLID: {
						alpha = 255;
						break;
				}
				case FormatType::FT_DOT: {
						alpha = 255;
						glLineStipple(1, 0xAAAA);
						glEnable(GL_LINE_STIPPLE);
						break;
				}
				case FormatType::FT_LONG_DASH: {
						alpha = 255;
						glLineStipple(2, 0x00FF);
						glEnable(GL_LINE_STIPPLE);
						break;
				}
				case FormatType::FT_SHORT_DASH: {
						alpha = 255;
						glLineStipple(3, 0xAAAA);
						glEnable(GL_LINE_STIPPLE);
						break;
				}
				case FormatType::FT_DOT_DASH: {
						alpha = 255;
						glLineStipple(1, 0x0C0F);
						glEnable(GL_LINE_STIPPLE);
						break;
				}
			}
			
			glColor4ub(curr->getColour(currentClientId).Red(), curr->getColour(currentClientId).Green(), curr->getColour(currentClientId).Blue(), alpha);
			
			glBegin(GL_LINES);
				glVertex3f(prev->getX(), prev->getY(), prev->getZ());
				glVertex3f(curr->getX(), curr->getY(), curr->getZ());
			glEnd();
			
			if ( glIsEnabled(GL_LINE_STIPPLE) )
				glDisable(GL_LINE_STIPPLE);
			
			prev = curr;
		}
	} else {
		drawPoints();
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawLineStrips() {
/////////////////////////////////////////////////////////////////
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	typedef GLI::GLCncPathVertices::FormatType FormatType;
	
	glBegin(GL_LINE_STRIP);
	
		int alpha = 255;
		for( GLI::GLCncPath::iterator it = cncPath.begin(); it != cncPath.end(); it++ ) {
			// line stipple and colour depth
			switch ( it->getFormatType() ) {
				case FormatType::FT_TRANSPARENT: {
						alpha = 0;
						break;
				}
				case FormatType::FT_SOLID: {
						alpha = 255;
						break;
				}
				case FormatType::FT_DOT:
				case FormatType::FT_LONG_DASH:
				case FormatType::FT_SHORT_DASH:
				case FormatType::FT_DOT_DASH: {
						alpha = 100;
						break;
				}
			}
			
			glColor4ub(it->getColour(currentClientId).Red(),it->getColour(currentClientId).Green(),it->getColour(currentClientId).Blue(), alpha);
			
			// determine the vertice
			glVertex3f(it->getX(), it->getY(), it->getZ());
		}
	
	glEnd();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::determineModel() {
/////////////////////////////////////////////////////////////////
	if ( isEnabled() == false ) {
		glPushMatrix();
			glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_2D, theTexture);
				drawBox(0.05, GL_QUADS);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		return;
	}
	
	switch ( drawType ) {
		case DT_POINTS:		drawPoints(); 		break;
		case DT_LINES:		drawLines(); 		break;
		case DT_LINE_STRIP:	drawLineStrips(); 	break;
	}
	
	if ( boundBox == true )
		drawBoundBox();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawBoundBox() {
/////////////////////////////////////////////////////////////////
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	typedef GLI::BoundBox BoundBox;
	
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	
	glColor4ub(boundBoxColour.Red(), boundBoxColour.Green(), boundBoxColour.Blue(), 255);
	
	BoundBox bb = cncPath.getBoundBox();
	for ( BoundBox::iterator it=bb.begin(); it!=bb.end(); ++it ) {
		
		glBegin(GL_LINES);
			glVertex3f(std::get<0>(*it).getX(), std::get<0>(*it).getY(), std::get<0>(*it).getZ());
			glVertex3f(std::get<1>(*it).getX(), std::get<1>(*it).getY(), std::get<1>(*it).getZ());
		glEnd();
	}
	
	glDisable(GL_LINE_STIPPLE);
}