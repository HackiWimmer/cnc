#include <iostream>
#include "wxcrafter.h"

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
	#include <OpenGL/glut.h>
#else
    #include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/glext.h>
#endif

#include <wx/glcanvas.h>
#include <wx/dcmemory.h>
#include "CncCommon.h"
#include "CncDrawPane.h"
#include "CncDrawPaneContext.h"

/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPaneContext::CncOpenGLDrawPaneContext(wxGLCanvas *canvas)
/////////////////////////////////////////////////////////////////////
: wxGLContext(canvas)
, cncConfig(NULL)
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
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
	//glFrustum(-0.0f, 0.0f, -0.0f, 0.0f, 1.0f, 3.0f);
	
	CncOpenGLDrawPane::CheckGLError();
}
/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPaneContext::~CncOpenGLDrawPaneContext() {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::setCncConfig(CncConfig* conf) {
/////////////////////////////////////////////////////////////////////
	cncConfig = conf;
}
/////////////////////////////////////////////////////////////////////
// Creates a 4-bytes-per-pixel, RGBA array from a wxImage.
// If the image has alpha channel, it's used. If not, pixels with 'cTrans' color
// get 'cAlpha' alpha; an the rest of pixels get alpha=255 (opaque).
//
// NOTE: The returned pointer must be deleted somewhere in the app.
//
unsigned char* CncOpenGLDrawPaneContext::convImageToPixels(const wxImage& img, const wxColour& cTrans, unsigned char cAlpha) {
/////////////////////////////////////////////////////////////////////
	int w = img.GetWidth();
	int h = img.GetHeight();
	int siz = w * h;
	unsigned char *resArr = new unsigned char [siz * 4];
	unsigned char *res = resArr;
	unsigned char *sdata = img.GetData();
	unsigned char *alpha = NULL;
	
	if ( img.HasAlpha() )
		alpha = img.GetAlpha();
		
	// Pixel by pixel
	for ( int i = 0; i < siz; i++ ) {
		//copy the colour
		res[0] = sdata[0] ;
		res[1] = sdata[1] ;
		res[2] = sdata[2] ;
		
		if ( alpha != NULL ) {   
			//copy alpha
			res[3] = alpha[i] ;
		} else {
			// Colour cTrans gets cAlpha transparency
			if ( res[0] == cTrans.Red() && res[1] == cTrans.Green() && res[2] == cTrans.Blue() )
				res[3] = cAlpha;
			else
				res[3] = 255 ;
		}
		sdata += 3 ;
		res += 4 ;
	}

	return resArr;
}
/////////////////////////////////////////////////////////////////////
// Creates an array of bytes that defines the pixels of the string.
// The background color has cAlpha transparency. 0=transparent, 255=opaque
//
// NOTE: The returned pointer must be deleted somewhere in the app.
//
unsigned char* CncOpenGLDrawPaneContext::convTextToPixels(const wxString& sText, const wxFont& sFont, const wxColour& sForeColo, 
                                                          const wxColour& sBackColo, unsigned char cAlpha, int* width, int* height) {
/////////////////////////////////////////////////////////////////////
	if ( sText.IsEmpty() )
		return NULL;

	// The dc where we temporally draw
	wxMemoryDC mdc;

	mdc.SetFont(sFont);

	// Measure
	mdc.GetMultiLineTextExtent(sText, width, height);

	// Now we know dimensions, let's draw into a memory dc
	wxBitmap bmp(*width, *height, 24);
	mdc.SelectObject(bmp);
	// If we have multiline string, perhaps not all of the bmp is used
	wxBrush brush(sBackColo);
	mdc.SetBackground(brush);
	mdc.Clear(); // Make sure all of bmp is cleared
	// Colours
	mdc.SetBackgroundMode(wxPENSTYLE_SOLID);
	mdc.SetTextBackground(sBackColo);
	mdc.SetTextForeground(sForeColo);
	// We draw the string and get it as an image.
	// NOTE: OpenGL axis are bottom to up. Be aware when setting the texture coords.
	mdc.DrawText(sText, 0, 0);
	mdc.SelectObject(wxNullBitmap); // bmp must be detached from wxMemoryDC

	// debug only
	//bmp.SaveFile("c:\\temp\\pummi.bmp", wxBITMAP_TYPE_BMP );

	// Bytes from the image. Background pixels become transparent with the
	// cAlpha transparency value.
	unsigned char *res = convImageToPixels(bmp.ConvertToImage(), sBackColo, cAlpha);

	return res;
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawZeroPlane() {
/////////////////////////////////////////////////////////////////////
	if ( displayInfo.drawZeroPlane == false )
		return;
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	GLfloat c = (GLfloat)128/255;
	glColor4f(c, c, c, 0.3);

	glBegin(GL_POLYGON);
		glVertex3f( 0.0,  0.0, 0.0);
		glVertex3f(10.0,  0.0, 0.0);
		glVertex3f(10.0, 10.0, 0.0);
		glVertex3f( 0.0, 10.0, 0.0);
	glEnd();

}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::drawWorkpieceSurface() {
/////////////////////////////////////////////////////////////////////
	if ( cncConfig == NULL )
		return;
		
	double wpt = cncConfig->getWorkpieceThickness() / cncConfig->getMaxDimensionZ();
	double wpo = cncConfig->getWorkpieceOffset() / cncConfig->getMaxDimensionZ();
	
	if ( cnc::dblCompare(wpt, 0.0) == true )
		return;
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	if ( displayInfo.drawWorkpieceSurface == true ) {
		GLfloat c = (GLfloat)64/255;
		glColor4f(c, c, c, 0.3);

		glBegin(GL_POLYGON);
			glVertex3f(0.0, 0.0, wpt);
			glVertex3f(0.5, 0.0, wpt);
			glVertex3f(0.5, 0.5, wpt);
			glVertex3f(0.0, 0.5, wpt);
		glEnd();
	}
	
	if ( displayInfo.drawWorkpieceOffset == true ) {
		GLfloat c = (GLfloat)32/255;
		glColor4f(c, c, c, 0.3);

		glBegin(GL_POLYGON);
			glVertex3f(0.0, 0.0, wpt + wpo);
			glVertex3f(0.5, 0.0, wpt + wpo);
			glVertex3f(0.5, 0.5, wpt + wpo);
			glVertex3f(0.0, 0.5, wpt + wpo);
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
void CncOpenGLDrawPaneContext::displayCoordinateOrigin( DrawPaneViewType viewType) {
/////////////////////////////////////////////////////////////////////
	// x axis
	if ( viewType != DPVT_Left && viewType != DPVT_Right ) {
		glBegin(GL_LINES);

			glColor3f(axises.colours.x.Red()/255, axises.colours.x.Green()/255, axises.colours.x.Blue()/255);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(axises.length, 0.0f, 0.0f);
			
		glEnd();
		drawX();
	}
	
	// y axis
	if ( viewType != DPVT_Front && viewType != DPVT_Rear ) {
		glBegin(GL_LINES);

			glColor3f(axises.colours.y.Red()/255, axises.colours.y.Green()/255, axises.colours.y.Blue()/255);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, axises.length, 0.0f);
		
		glEnd();
		drawY();
	}

	// z axis
	if ( viewType != DPVT_Top && viewType != DPVT_Bottom ) {
		glBegin(GL_LINES);

			glColor3f(axises.colours.z.Red()/255, axises.colours.z.Green()/255, axises.colours.z.Blue()/255);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, axises.length);
		
		glEnd();
		drawZ();
	}
	
	glFlush();
}

void DrawTextFunction(double x, double y, char* str)
{

	glClear(GL_COLOR_BUFFER_BIT); 
	glLoadIdentity();
	
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glRasterPos2f(x, y);
	glColor3f(0.0, 1.0, 0.0);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
	//glDisable(GL_TEXTURE_2D);
}

void drawBitmapText(char *string,float x,float y,float z) 
{  
	char *c;
	glRasterPos3f(x, y,z);

	for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}

void drawStrokeText(char*string,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.09f,-0.08f,z);
  
	  for (c=string; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}


/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::displayDataVector(DrawPaneData& dpd, DrawPaneViewType viewType, wxSize curSize) {
/////////////////////////////////////////////////////////////////////
	// safty
	if ( cncConfig == NULL ) {
		return;
	}
	
	// for testing only
	if ( true ) {
		testGL(curSize);
		return;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_LINE_STIPPLE);
	displayCoordinateOrigin(viewType);
	
	// display given data
	if ( dpd.size() > 0 ) {
		for( DrawPaneData::iterator it = dpd.begin(); it<dpd.end(); it++ ) {
			
			DoublePoint3D p1 = (*it).getP1();
			DoublePoint3D p2 = (*it).getP2();
			wxColour col     = (*it).getDrawColour();
			int style        = (*it).getLineStyle();
			
			switch ( style ) {
				case wxTRANSPARENT:	continue;
				
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
				
				switch ( viewType ) {
					case DPVT_Top:
					case DPVT_Bottom:		glVertex3f( p1.getX() / cncConfig->getDispFactX3D(), 
														p1.getY() / cncConfig->getDispFactY3D(), 
														0.0f);
														
											glVertex3f( p2.getX() / cncConfig->getDispFactX3D(),
														p2.getY() / cncConfig->getDispFactY3D(), 
														0.0f);
											break;
					case DPVT_Front:
					case DPVT_Rear:			glVertex3f( p1.getX() / cncConfig->getDispFactX3D(), 
														0.0f, 
														p1.getZ() / cncConfig->getDispFactZ3D());
														
											glVertex3f( p2.getX() / cncConfig->getDispFactX3D(), 
														0.0f, 
														p2.getZ() / cncConfig->getDispFactZ3D());
											break;
					case DPVT_Left:
					case DPVT_Right:		glVertex3f( 0.0f, 
														p1.getY() / cncConfig->getDispFactY3D(), 
														p1.getZ() / cncConfig->getDispFactZ3D());
														
											glVertex3f( 0.0f, 
														p2.getY() / cncConfig->getDispFactY3D(), 
														p2.getZ() / cncConfig->getDispFactZ3D());
											break;
				
					default:				glVertex3f( p1.getX() / cncConfig->getDispFactX3D(), 
														p1.getY() / cncConfig->getDispFactY3D(), 
														p1.getZ() / cncConfig->getDispFactZ3D());
														
											glVertex3f( p2.getX() / cncConfig->getDispFactX3D(), 
														p2.getY() / cncConfig->getDispFactY3D(), 
														p2.getZ() / cncConfig->getDispFactZ3D());
				}
				
			glEnd();
		}
	}
	
	drawZeroPlane();
	drawWorkpieceSurface();

	glFlush();
	CncOpenGLDrawPane::CheckGLError();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPaneContext::testGL(wxSize s) {
/////////////////////////////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	wxFont font(48, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxColour bgrdColo(*wxBLACK);
	wxColour foreColo(*wxBLUE);
	int textw = 0, texth = 0;
	unsigned char * ptr = convTextToPixels("Hallo World", font, foreColo, bgrdColo, 0, &textw, &texth); 
	
	/*
	GLuint textures;
	glGenTextures(1, &textures);
	glBindTexture(GL_TEXTURE_2D, textures);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textw, texth, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
*/
/*
	double ratiox = 1.0 * textw/s.GetWidth();
	double ratioy = 1.0 * texth/s.GetHeight();

	clog << ratiox << endl;
	clog << ratioy << endl;
*/
	//glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, textures);
		wxColour col(174,74,174);
	
		
		glBegin(GL_POLYGON);
		glColor3f(col.Red()/255.0f, col.Green()/255.0f, col.Blue()/255.0f); 
		
			//glTexCoord2f(0.0, texth); 
			glVertex2f(0, 0);
			
			//glTexCoord2f(textw, texth); 
			glVertex2f(0.4, 0);
			
			//glTexCoord2f(textw, 0.0); 
			glVertex2f(0.4,0.4);
			
			//glTexCoord2f(0.0, 0.0); 
			glVertex2f(0, 0.4);
		
		glEnd();
	//glDisable(GL_TEXTURE_2D );
	
	glFlush();
	CncOpenGLDrawPane::CheckGLError();

}
/*
	double ratiox = textw/s.GetWidth();
	double ratioy = texth/s.GetHeight();
	int gl_height = 0;
	int gl_width  = 0;
	int offset_y  = 0;
	int offset_x  = 0;
	
	if (ratiox > ratioy) {
		gl_height = round(texth / ratiox);
		offset_y  = round((s.GetHeight() - gl_height) / 2);
		gl_height += offset_y * 2;
	} else {
		gl_width = round(textw / ratioy);
		offset_x = round((s.GetWidth() - gl_width) / 2);
		gl_width += offset_x * 2;
	}
	
	
	
	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(offset_x, offset_y);

			glTexCoord2f(textw, 0);
			glVertex2f(offset_x + gl_width, offset_y);

			glTexCoord2f(textw, texth);
			glVertex2f(offset_x + gl_width, offset_y + gl_height);

			glTexCoord2f(0, texth);
			glVertex2f(offset_x, offset_y + gl_height);
		glEnd();
	glDisable(GL_TEXTURE_2D );
	*/