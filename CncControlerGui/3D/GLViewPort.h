#ifndef GL_VIEW_PORT_H
#define GL_VIEW_PORT_H

#include <iostream>
#include "wx/string.h"

class GLViewPort {

	public:
		enum DistortType {VPT_Distored, VPT_Undistored};
		enum OrigPosType {VPOP_Center, VPOP_Custom};
		enum PreDefPos   {VPDP_TopLeft, 
		                  VPDP_TopRight, 
						  VPDP_BottomLeft, 
						  VPDP_BottomRight, 
						  VPDP_TopMid, 
						  VPDP_BottomMid, 
						  VPDP_LeftMid, 
						  VPDP_RightMid, 
						  VPDP_Center
						};
		
		////////////////////////////////////////////////////
		GLViewPort(GLViewPort::DistortType type)
		: x(0), y(0), w(0), h(0), n(0)
		, windowWidth(0), windowHeigth(0)
		, scissorX(x), scissorY(y)
		, factor(8)
		, distortType(type)
		, origPosType(OrigPosType::VPOP_Center)
		{}

		////////////////////////////////////////////////////
		~GLViewPort() {
		}
		
		/////////////////////////////////////////////////////////
		void trace(std::ostream& out) {
			out << wxString::Format("Window Size w, h   : %d, %d", windowWidth, windowHeigth) << std::endl;
			out << wxString::Format("Viewport:") << std::endl;
			out << wxString::Format(" x, y, w, h, n     : %d, %d, %d, %d, %d", x, y, w, h, n) << std::endl;
			out << wxString::Format(" Destort Type      : %s", getDistortTypeAsString()) << std::endl;
			out << wxString::Format(" Origin Pos        : %s", getOriginPosTypeAsString()) << std::endl;
			out << wxString::Format(" Aspect            : %f", getAspect()) << std::endl;
			out << wxString::Format(" Factor            : %d", getFactor()) << std::endl;
			out << wxString::Format(" Scissor x, y, w, h: %d, %d, %d, %d", getScissorX(), getScissorY(), getScissorW(), getScissorH()) << std::endl;
		}

		////////////////////////////////////////////////////
		int getX() { return x; }
		int getY() { return y; }
		int getW() { return w; }
		int getH() { return h; }
		
		////////////////////////////////////////////////////
		int getScissorX() { return scissorX; }
		int getScissorY() { return scissorY; }
		int getScissorW() { return windowWidth; }
		int getScissorH() { return windowHeigth; }
		
		////////////////////////////////////////////////////
		int getFactor() { return factor; }
		float getDisplayFactor() { return 1.0/factor; }
		
		////////////////////////////////////////////////////
		float getAspect() { return 1.0f * getNormalizedSizeW()/getNormalizedSizeH(); }

		////////////////////////////////////////////////////
		int getCurrentWindowWidth() { return windowWidth; }
		int getCurrentWindowHeigth() { return windowHeigth; }
		
		////////////////////////////////////////////////////
		int getNormalizedSizeW() {
			if ( distortType == GLViewPort::VPT_Undistored )
				return n;
			return w;
		}

		////////////////////////////////////////////////////
		int getNormalizedSizeH() {
			if ( distortType == GLViewPort::VPT_Undistored )
				return n;
			return h;
		}

		////////////////////////////////////////////////////
		GLViewPort::DistortType getDistortType() { return distortType; }
		const char* getDistortTypeAsString() {
			switch ( distortType ) {
				case VPT_Distored: 		return "Distored";
				case VPT_Undistored: 	return "Undistored";
			}
			return "???";
		}
		void centerViewport() { origPosType = VPOP_Center; }

		////////////////////////////////////////////////////
		GLViewPort::OrigPosType getOriginPosType() { return origPosType; }
		const char* getOriginPosTypeAsString() {
			switch ( origPosType ) {
				case VPOP_Custom: 	return "Custom";
				case VPOP_Center: 	return "Center";
			}
			return "???";
		}
		
		void resetCustomOrigPosType() { origPosType = VPOP_Center; }
		
		////////////////////////////////////////////////////
		void getPreDefCoordinatesXY(GLViewPort::PreDefPos pdp,
									int wndSizeW,
									int wndSizeH,
									int& x, 
									int& y);
		
		////////////////////////////////////////////////////
		void evaluate(int wndSizeW,
					  int wndSizeH,
					  int custPosX=0,
					  int custPosY=0);

	private:

		static const unsigned int margin = 10;

		int x, y, w, h, n;
		int windowWidth, windowHeigth;
		int scissorX, scissorY;
		int factor;
		GLViewPort::DistortType distortType;
		GLViewPort::OrigPosType origPosType;
		
		void setOrigPosType(GLViewPort::OrigPosType opt) { origPosType = opt; }

};

#endif