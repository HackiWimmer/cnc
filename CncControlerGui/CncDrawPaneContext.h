#ifndef CNC_OPENGL_DRAW_PANE_CONTEXT_H
#define CNC_OPENGL_DRAW_PANE_CONTEXT_H

#include <vector>
#include "CncPoint3D.h"

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

typedef std::vector<DoublePointPair3D> DrawPaneData;


////////////////////////////////////////////////////////////
class CncOpenGLDrawPaneContext : public wxGLContext {
////////////////////////////////////////////////////////////

	public:
	    CncOpenGLDrawPaneContext(wxGLCanvas *canvas);
	    ~CncOpenGLDrawPaneContext();
		
		struct WorkpieceInfo {
			bool drawZeroPlane			= true;
			bool drawWorkpieceSurface	= true;
			bool drawWorkpieceOffset	= true;
			
			double thickness 			= 0.0;
			double offset 				= 0.0;
		};
	
		// render the data 
		void displayDataVector(DrawPaneData& dpd);
		
		///////////////////////////////////////////////////////
		void setWorkpieceInfo(const WorkpieceInfo& wi) { workpieceInfo = wi; }
	
	private:
		WorkpieceInfo workpieceInfo;
		
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

		void displayCoordinateOrigin();
		
		void drawX();
		void drawY();
		void drawZ();
		
		void drawZeroPlane();
		void drawWorkpieceSurface();
};

#endif