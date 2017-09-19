#ifndef CNC_OPENGL_DRAW_PANE_CONTEXT_H
#define CNC_OPENGL_DRAW_PANE_CONTEXT_H

#include <vector>
#include "CncPoint3D.h"
#include "CncConfig.h"

enum DrawPaneViewType 	{ DPVT_Front, DPVT_Rear, DPVT_Top, DPVT_Bottom, DPVT_Left, DPVT_Right, DPVT_3D_ISO1, DPVT_3D_ISO2, DPVT_3D_ISO3, DPVT_3D_ISO4 };
enum DrawPaneOrigin 	{ DPO_TOP_LEFT, DPO_TOP_RIGHT, DPO_BOTTOM_LEFT, DPO_BOTTOM_RIGHT, DPO_CENTER, DPO_CUSTOM};
enum DrawPaneSelect 	{ DPS_XY, DPS_YZ, DPS_ZX };

////////////////////////////////////////////////////////////
struct DisplayAngels {
////////////////////////////////////////////////////////////

	private:
		
		int angleX;
		int angleY;
		int angleZ;
		
		int defaultAngleX;
		int defaultAngleY;
		int defaultAngleZ;
		
	public:
		
		///////////////////////////////////////////////////////
		DisplayAngels () 
		///////////////////////////////////////////////////////	
		: angleX(0)
		, angleY(0)
		, angleZ(0)
		, defaultAngleX(0)
		, defaultAngleY(0)
		, defaultAngleZ(0)
		{
		}
		
		///////////////////////////////////////////////////////
		DisplayAngels(int ax, int ay, int az, int dx, int dy, int dz) 
		///////////////////////////////////////////////////////
		: angleX(ax)
		, angleY(ay)
		, angleZ(az)
		, defaultAngleX(dx)
		, defaultAngleY(dy)
		, defaultAngleZ(dz)
		{
		}
		
		///////////////////////////////////////////////////////
		const float getDefaultX() const { return defaultAngleX; }
		const float getDefaultY() const { return defaultAngleY; }
		const float getDefaultZ() const { return defaultAngleZ; }
		
		///////////////////////////////////////////////////////
		const float getX() const { return angleX; }
		const float getY() const { return angleY; }
		const float getZ() const { return angleZ; }
		
		///////////////////////////////////////////////////////
		const DisplayAngels& setX(int v) { angleX = v%360; return *this; }
		const DisplayAngels& setY(int v) { angleY = v%360; return *this; }
		const DisplayAngels& setZ(int v) { angleZ = v%360; return *this; }
		
		///////////////////////////////////////////////////////
		const DisplayAngels& incX(int v) { angleX += v; angleX %= 360; return *this; }
		const DisplayAngels& incY(int v) { angleY += v; angleY %= 360; return *this; }
		const DisplayAngels& incZ(int v) { angleZ += v; angleZ %= 360; return *this; }
		
		///////////////////////////////////////////////////////
		const DisplayAngels& setDefaultX(int v) { defaultAngleX = v%360; return *this; }
		const DisplayAngels& setDefaultY(int v) { defaultAngleY = v%360; return *this; }
		const DisplayAngels& setDefaultZ(int v) { defaultAngleZ = v%360; return *this; }
		
		///////////////////////////////////////////////////////
		void setToDefault() {
		///////////////////////////////////////////////////////
			angleX = defaultAngleX;
			angleY = defaultAngleY;
			angleZ = defaultAngleZ;
		}
		
		///////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const DisplayAngels &a) {
		///////////////////////////////////////////////////////
			ostr << a.angleX << ',' << a.angleY << ',' << a.angleZ;
			return ostr;
		}
};

////////////////////////////////////////////////////////////
class CncOpenGLDrawPaneContext : public wxGLContext {
////////////////////////////////////////////////////////////

	public:
	    CncOpenGLDrawPaneContext(wxGLCanvas *canvas);
	    ~CncOpenGLDrawPaneContext();
		
		struct DisplayOptions3D {
			bool drawZeroPlane			= true;
			bool drawWorkpieceSurface	= true;
			bool drawWorkpieceOffset	= true;
		};
	
		// render the data 
		void displayDataVector(DrawPaneData& dpd, DrawPaneViewType viewType, wxSize curSize);
		
		// setting configuration
		void setCncConfig(CncConfig* conf);
		
		// setting display options
		void setDisplayInfo(const DisplayOptions3D& di) { displayInfo = di; }
		
		///////////////////////////////////////////////////////
		static unsigned char* convImageToPixels(const wxImage& img, const wxColour& cTrans, unsigned char cAlpha);
		static unsigned char* convTextToPixels(const wxString& sText, const wxFont& sFont, const wxColour& sForeColo, 
                                               const wxColour& sBackColo, unsigned char cAlpha, int* width, int* height); 
	
	private:
		DisplayOptions3D displayInfo;
		CncConfig* cncConfig;
		
		struct Axises {

			struct Colours {
				wxColour x;
				wxColour y;
				wxColour z;
				
				/////////////////////////////////////////////
				Colours() 
				: x(255, 0, 0)
				, y(0, 255, 0)
				, z(0, 0, 255)
				{
				}
			};

			Colours colours;
			float length;
			float letterScale;

			/////////////////////////////////////////////////
			Axises() 
			: colours()
			, length(0.25f)
			, letterScale(0.01f)
			{
			}
		};

		Axises axises;
		
		void displayCoordinateOrigin(DrawPaneViewType viewType);
		
		void drawX();
		void drawY();
		void drawZ();
		
		void drawZeroPlane();
		void drawWorkpieceSurface();
		
		void testGL(wxSize s);
};

#endif