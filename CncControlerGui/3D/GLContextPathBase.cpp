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
			
			glColor4ub(it->getColour().Red(), it->getColour().Green(), it->getColour().Blue(), alpha);
			
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
			
			glColor4ub(curr->getColour().Red(), curr->getColour().Green(), curr->getColour().Blue(), alpha);
			
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
			
			glColor4ub(it->getColour().Red(),it->getColour().Green(),it->getColour().Blue(), alpha);
			
			// determine the vertice
			glVertex3f(it->getX(), it->getY(), it->getZ());
		}
	
	glEnd();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::determineModel() {
/////////////////////////////////////////////////////////////////
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