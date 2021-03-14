#ifndef GL_VIEW_PORT_H
#define GL_VIEW_PORT_H

#include <iostream>
#include "wx/string.h"

class GLViewPort {

	public:
		enum DistortType {VPT_Distorted, VPT_Undistorted};
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
		
		GLViewPort(GLViewPort::DistortType type, int factor = 8);
		~GLViewPort();
		
		void trace(std::ostream& out);
		
		int getX()					const	{ return x; }
		int getY()					const	{ return y; }
		int getW()					const	{ return w; }
		int getH()					const	{ return h; }
		
		int getScissorX()			const	{ return currScissor.GetX(); }
		int getScissorY()			const	{ return currScissor.GetX(); }
		int getScissorW()			const	{ return currScissor.GetWidth(); }
		int getScissorH()			const	{ return currScissor.GetHeight(); }
		
		int getFactor()				const	{ return factor; }
		float getDisplayFactor()			{ return 1.0/factor; }
		
		float getAspect()			const	{ return 1.0f * getNormalizedSizeW()/getNormalizedSizeH(); }
		
		int getCurrentOriginX()		const	{ return curWndOrigin.x; }
		int getCurrentOriginY()		const	{ return curWndOrigin.y; }
		int getCurrentWindowWidth()	const	{ return curWndSize.GetWidth(); }
		int getCurrentWindowHeight()const	{ return curWndSize.GetHeight(); }
		
		int getNormalizedSizeW()	const	{ return distortType == GLViewPort::VPT_Undistorted ? n : w; }
		int getNormalizedSizeH()	const	{ return distortType == GLViewPort::VPT_Undistorted ? n : h; }
		
		GLViewPort::DistortType getDistortType() const { return distortType; }
		const char* getDistortTypeAsString() const ;
		
		GLViewPort::OrigPosType getOriginPosType() const { return origPosType; }
		const char* getOriginPosTypeAsString() const;
		
		void centreViewport();
		
		const wxPoint evaluatePreDefPositions(GLViewPort::PreDefPos pdp, int wndSizeW, int wndSizeH);
		void evaluate(int wndSizeW, int wndSizeH, int custPosX, int custPosY);
		void evaluate(int wndSizeW, int wndSizeH);
		
		bool process();
		static bool processDefault(int wndSizeW, int wndSizeH);
	
	private:
		
		static const unsigned int margin = 100;
		
		int						x, y, w, h, n;
		int						factor;
		wxSize					curWndSize;
		wxPoint					curWndOrigin;
		wxRect					currScissor;
		GLViewPort::DistortType distortType;
		GLViewPort::OrigPosType origPosType;
		
		void setOrigPosType(GLViewPort::OrigPosType opt);
};

#endif