#include <iostream>
#include "3D/GLContextPathBase.h"
#include "3D/GLLabelCluster.h"
#include "CncConfig.h"

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
/////////////////////////////////////////////////////////////////
GLuint LoadBMP(const wxImage& img) {
/////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////
static void drawBox(GLfloat size, GLenum type) {
/////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////
GLContextCncPathBase::GLContextCncPathBase(wxGLCanvas* canvas)
: GLContextBase(canvas)
, cncPath()
, ruler()
, drawType(DT_LINE_STRIP)
, currentClientId(-1L)
, rulerColourX(coordOriginInfo.colours.x)
, rulerColourY(coordOriginInfo.colours.y)
, rulerColourZ(coordOriginInfo.colours.z)
/////////////////////////////////////////////////////////////////
{
	cncPath.reserve(GBL_CONFIG->getConstRerserveCapacity());
	
	wxBitmap bmp = ImageLibBig().Bitmap("BMP_CNC");
	wxImage img  = bmp.ConvertToImage();
	theTexture   = LoadBMP(img);
}
/////////////////////////////////////////////////////////////////
GLContextCncPathBase::~GLContextCncPathBase() {
/////////////////////////////////////////////////////////////////
	clearPathData();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::markCurrentPosition() {
/////////////////////////////////////////////////////////////////
	// ensure cncPath.end() - 1 is valid
	if ( cncPath.size() == 0 )
		return;
		
	// get the last/current vecties - it must be valid
	GLI::GLCncPath::iterator it = cncPath.vEnd() - 1;
	drawPosMarker(it->getX(), it->getY(), it->getZ());
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::clearPathData() {
/////////////////////////////////////////////////////////////////
	cncPath.clear();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::appendPathData(const GLI::GLCncPathVertices& cpv) {
/////////////////////////////////////////////////////////////////
	#warning
	drawPoint(cpv);
		
	glFlush();

	
	//std::clog << sizeof(GLI::GLCncPathVertices) << std::endl;
	
	
	cncPath.push_back(cpv);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::appendPathData(const GLI::GLCncPath& cp) {
/////////////////////////////////////////////////////////////////
	if ( cp.size() > 0 ) {
		for ( GLI::GLCncPath::const_iterator it = cp.begin(); it != cp.end() - 1; ++it )
			appendPathData(*it);
	} 
	else {
		clearPathData();
		determineModel();
	}
}

/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawPoint(const GLI::GLCncPathVertices& vCurr) {
/////////////////////////////////////////////////////////////////

	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	typedef GLI::GLCncPathVertices::FormatType FormatType;
	
	glBegin(GL_POINTS);
	
		int alpha = 255;
		// line stipple and colour depth
		switch ( vCurr.getFormatType() ) {
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
		
		glColor4ub(vCurr.getColour(currentClientId).Red(), vCurr.getColour(currentClientId).Green(), vCurr.getColour(currentClientId).Blue(), alpha);
		
		// determine the vertice
		glVertex3f(vCurr.getX(), vCurr.getY(), vCurr.getZ());
		
	glEnd();

}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawLine(const GLI::GLCncPathVertices& vCurr, const GLI::GLCncPathVertices& vPrev) {
	
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawLineStrip(const GLI::GLCncPathVertices& vCurr) {
	
}





/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawPoints() {
/////////////////////////////////////////////////////////////////
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	typedef GLI::GLCncPathVertices::FormatType FormatType;
	
	glBegin(GL_POINTS);
	
		int alpha = 255;
		for( GLI::GLCncPath::iterator it = cncPath.vBegin(); it != cncPath.vEnd(); it++ ) {
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
		for( GLI::GLCncPath::iterator curr = cncPath.vBegin() + 1; curr != cncPath.vEnd(); curr++ ) {
			
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
		for( GLI::GLCncPath::iterator it = cncPath.vBegin(); it != cncPath.vEnd(); it++ ) {
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
	
	drawRuler();
	
	switch ( drawType ) {
		case DT_POINTS:		drawPoints(); 		break;
		case DT_LINES:		drawLines(); 		break;
		case DT_LINE_STRIP:	drawLineStrips(); 	break;
	}
	
	if ( options.showBoundBox == true )
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
	
	glColor4ub(options.boundBoxColour.Red(), options.boundBoxColour.Green(), options.boundBoxColour.Blue(), 255);
	
	BoundBox bb = cncPath.getBoundBox();
	for ( BoundBox::iterator it=bb.begin(); it!=bb.end(); ++it ) {
		
		glBegin(GL_LINES);
			glVertex3f(std::get<0>(*it).getX(), std::get<0>(*it).getY(), std::get<0>(*it).getZ());
			glVertex3f(std::get<1>(*it).getX(), std::get<1>(*it).getY(), std::get<1>(*it).getZ());
		glEnd();
	}
	
	glDisable(GL_LINE_STIPPLE);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawRuler() {
/////////////////////////////////////////////////////////////////
	//...........................................................
	auto skipIndex = [&](int counter) {
		const float sf = this->getCurrentScaleFactor();
		
		// the mod operands below 'm u s t' be even - always
		// otherwise the value of '0' isn't reached
		if      ( sf >= 0.00 && sf < 0.05 ) 	{ if ( counter % ( 100 ) != 0 ) return true; }
		if      ( sf >= 0.05 && sf < 0.10 ) 	{ if ( counter % (  50 ) != 0 ) return true; }
		else if ( sf >= 0.10 && sf < 0.25 ) 	{ if ( counter % (  10 ) != 0 ) return true; }
		else if ( sf >= 0.25 && sf < 0.50 ) 	{ if ( counter % (   4 ) != 0 ) return true; }
		else if ( sf >= 0.50 && sf < 1.00 ) 	{ if ( counter % (   2 ) != 0 ) return true; }
		
		return false;
	};
	
	//...........................................................
	auto drawRulerAxis = [&](const wxColour& colour, const GLI::GLLineCluster& ruler) {
		
		glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 64);
		
		for ( auto it = ruler.begin(); it != ruler.end(); ++it ) {
			const GLI::GLVectiesTuple vt = *it;
			
			glBegin(GL_LINES);
				glVertex3f(vt.getX1(), vt.getY1(), vt.getZ1());
				glVertex3f(vt.getX2(), vt.getY2(), vt.getZ2());
			glEnd();
		}
	};
	
	//...........................................................
	auto drawHelpLines = [&](const wxColour& colour, const GLI::GLLineCluster& lines) {
		
		glLineStipple(2, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
		
			glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 32);
			
			int counter = 1;
			for ( auto it = lines.begin(); it != lines.end(); ++it ) {
				
				if ( skipIndex( counter++ ) == true ) 
					continue;
				
				glBegin(GL_LINES);
					const GLI::GLVectiesTuple& vt = *it;
					glVertex3f(vt.getX1(), vt.getY1(), vt.getZ1());
					glVertex3f(vt.getX2(), vt.getY2(), vt.getZ2());
				glEnd();
			}
		
		glDisable(GL_LINE_STIPPLE);
	};

	//...........................................................
	auto drawRulerLabels = [&](const wxColour& colour, const GLI::GLLabelCluster& lables) {
		
		glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 255);
		
		int counter = 0;
		for ( auto it = lables.begin(); it != lables.end(); it++ ) {
			
			if ( skipIndex( counter++ ) == true ) 
				continue;
			
			const GLI::GLLabelInfo& li = *it;
			void* font = li.font;
			if ( font == NULL )
				font = GLUT_BITMAP_8_BY_13;
			
			this->renderBitmapString(li.x, li.y, li.z, font, li.label);
		}
	};
	
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	// first deterine what's needed
	const short X  = 0, Y  = 1, Z  = 2, 
	            XY = 0, XZ = 1, YZ = 2;
				
	bool showAxis[3]; 	showAxis[X]   = false;  showAxis[Y]   = false;  showAxis[Z]   = false;
	bool showPlane[3];	showPlane[XY] = false;  showPlane[XZ] = false;  showPlane[YZ] = false;
	
	const GLContextBase::ViewMode vm = getViewMode();
	switch ( vm ) {
		case V2D_TOP:
		case V2D_BOTTOM:			showAxis[X]   = true;  showAxis[Y]   = true;  showAxis[Z]   = false;
									showPlane[XY] = true;  showPlane[XZ] = false; showPlane[YZ] = false;
									break;
		case V2D_LEFT:
		case V2D_RIGHT: 			showAxis[X]   = false; showAxis[Y]   = true;  showAxis[Z]   = true;
									showPlane[XY] = false; showPlane[XZ] = false; showPlane[YZ] = true;
									break;
		case V2D_FRONT:
		case V2D_REAR:				showAxis[X]   = true;  showAxis[Y]   = false; showAxis[Z]   = true;
									showPlane[XY] = false; showPlane[XZ] = true;  showPlane[YZ] = false;
									break;
		case V3D_ISO1:
		case V3D_ISO2: 
		case V3D_ISO3: 
		case V3D_ISO4:
		case V2D_CAM_ROT_XY_ZTOP: 	showAxis[X]   = true;  showAxis[Y]   = true;  showAxis[Z]   = true;
									showPlane[XY] = options.helpLines3D_XYPlane;  
									showPlane[XZ] = options.helpLines3D_XZPlane;
									showPlane[YZ] = options.helpLines3D_YZPlane;
									break;
	}
	
	// draw the rulers
	if ( options.showRuler == true ) {
		if ( showAxis[X] ) { drawRulerAxis(rulerColourX, ruler.xAxis.axisLines); }
		if ( showAxis[Y] ) { drawRulerAxis(rulerColourY, ruler.yAxis.axisLines); }
		if ( showAxis[Z] ) { drawRulerAxis(rulerColourZ, ruler.zAxis.axisLines); }
		
		if ( showAxis[X] ) { drawRulerLabels(rulerColourX, ruler.xAxis.axisLables); }
		if ( showAxis[Y] ) { drawRulerLabels(rulerColourY, ruler.yAxis.axisLables); }
		if ( showAxis[Z] ) { drawRulerLabels(rulerColourZ, ruler.zAxis.axisLables); }
	}
	
	if ( options.showHelpLines == true ) {
		wxColour colour(255, 255, 255);
		if ( showPlane[XY] ) { drawHelpLines(colour, ruler.helpLinesXY.helpLinesX); drawHelpLines(colour, ruler.helpLinesXY.helpLinesY); }
		if ( showPlane[XZ] ) { drawHelpLines(colour, ruler.helpLinesXZ.helpLinesX); drawHelpLines(colour, ruler.helpLinesXZ.helpLinesZ); }
		if ( showPlane[YZ] ) { drawHelpLines(colour, ruler.helpLinesYZ.helpLinesY); drawHelpLines(colour, ruler.helpLinesYZ.helpLinesZ); }
	}
}