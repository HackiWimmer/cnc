#include "3D/GLViewPort.h"


/////////////////////////////////////////////////////////////////
void GLViewPort::getPreDefCoordinatesXY(GLViewPort::PreDefPos pdp,
										int wndSizeW,
										int wndSizeH,
										int& x, 
										int& y) {
/////////////////////////////////////////////////////////////////

	// x as well as y is returned in open opengl logic
	// 0,0 = left/bottom
	
	switch ( pdp ) {
		case VPDP_TopLeft:		x = margin;
								y = wndSizeH - margin;
								break;
								
		case VPDP_TopMid:		x = wndSizeW/2;
								y = wndSizeH - margin;
								break;

		case VPDP_TopRight:		x = wndSizeW - margin;
								y = wndSizeH - margin;
								break;
								
		case VPDP_BottomLeft:	x = margin;
								y = margin;
								break;
								
		case VPDP_BottomMid:	x = wndSizeW/2;
								y = margin;
								break;

		case VPDP_BottomRight:	x = wndSizeW - margin;
								y = margin;
								break;
								
		case VPDP_LeftMid:		x = margin;
								y = wndSizeH/2;
								break;
								
		case VPDP_RightMid:		x = wndSizeW - margin;
								y = wndSizeH/2;
								break;
								
		case VPDP_Center:		
		default:				x = 0;
								y = 0; 
								break;
	}		
}
/////////////////////////////////////////////////////////////////
void GLViewPort::evaluate(int wndSizeW,
						  int wndSizeH,
						  int custPosX,
						  int custPosY) {
/////////////////////////////////////////////////////////////////

	// first check the cust* parameters and set der orig position type
	bool restoreCustomMode = false;
	if ( custPosX != 0 || custPosY != 0) {
		// switch to custom mode
		setOrigPosType(GLViewPort::VPOP_Custom);
	} else {
		if ( origPosType == GLViewPort::VPOP_Custom ) {
			// restore the last values
			restoreCustomMode = true;
		} else {
			// switch to the center mode
			setOrigPosType(GLViewPort::VPOP_Center);
		}
	}
	
	// keep previous values
	int prevW = w;
	int prevH = h;
	int prevN = n;
	int prevX = x;
	int prevY = y;
	
	// store real window size
	windowWidth  = wndSizeW;
	windowHeigth = wndSizeH;
	
	// calc new values
	w = wndSizeW * factor;
	h = wndSizeH * factor;

	// determine the normalized size
	w > h ? n = h : n = w;
	
	switch ( origPosType ) {
		case VPOP_Custom: 		if ( restoreCustomMode == false ) {
									if ( distortType == DistortType::VPT_Undistored) {
										x = custPosX - n / 2;
										y = custPosY - n / 2;
									} else {
										x = custPosX - w / 2;
										y = custPosY - h / 2;
									}
								} else {
									if ( distortType == DistortType::VPT_Undistored) {
										x = prevX - ( n - prevN ) / 2;
										y = prevY - ( n - prevN ) / 2;
									} else {
										x = prevX - ( w - prevW ) / 2;
										y = prevY - ( h - prevH ) / 2;
									}
								}
								
								break;
		case VPOP_Center:
		default: 				// normalize the new center
								if ( distortType == DistortType::VPT_Undistored) {
									x = ( w - n ) / 2;
									y = ( h - n ) / 2;
								} else {
									x = 0;
									y = 0;
								}
								
								// determine the new center (factor correction)
								x -= ( w / 2 - wndSizeW / 2 );
								y -= ( h / 2 - wndSizeH / 2 );
								
								break;
	}
}