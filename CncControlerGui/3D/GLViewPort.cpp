#include "CncCommon.h"
#include "3D/GLCommon.h"
#include "3D/GLViewPort.h"

/////////////////////////////////////////////////////////////////
GLViewPort::GLViewPort(GLViewPort::DistortType type, int f)
: x					(0)
, y					(0)
, w					(0)
, h					(0)
, n					(0)
, factor			(f >= 1 ? f : 1)
, curWndSize		(0, 0)
, curWndOrigin		(0, 0)
, currScissor		(x, y, 0, 0)
, distortType		(type)
, origPosType		(OrigPosType::VPOP_Center)
/////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////
GLViewPort::~GLViewPort() {
/////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////
void GLViewPort::trace(std::ostream& out) {
/////////////////////////////////////////////////////////////////
	out << wxString::Format("Window Size w, h   : %d, %d\n", 				getCurrentWindowWidth(), getCurrentWindowHeight());
	out << wxString::Format("Window Origin x, y : %d, %d\n", 				getCurrentOriginX(), getCurrentOriginY());
	
	out << wxString::Format("Viewport:\n");
	out << wxString::Format(" x, y, w, h, n     : %d, %d, %d, %d, %d\n",	x, y, w, h, n);
	out << wxString::Format(" Distort Type      : %s\n",					getDistortTypeAsString());
	out << wxString::Format(" Origin Pos        : %s\n",					getOriginPosTypeAsString());
	out << wxString::Format(" Aspect            : %f\n",					getAspect());
	out << wxString::Format(" Normalized w, h   : %d, %d\n",				getNormalizedSizeW(), getNormalizedSizeH());
	out << wxString::Format(" Factor            : %d\n",					getFactor());
	out << wxString::Format(" Scissor x, y, w, h: %d, %d, %d, %d\n",		getScissorX(), getScissorY(), getScissorW(), getScissorH());
}
/////////////////////////////////////////////////////////////////
const char* GLViewPort::getDistortTypeAsString() const {
/////////////////////////////////////////////////////////////////
	switch ( distortType ) {
		case VPT_Distorted:		return "Distorted";
		case VPT_Undistorted:	return "Undistorted";
	}
	
	return "???";
}
/////////////////////////////////////////////////////////////////
const char* GLViewPort::getOriginPosTypeAsString() const {
/////////////////////////////////////////////////////////////////
	switch ( origPosType ) {
		case VPOP_Custom:	return "Custom";
		case VPOP_Center:	return "Centre";
	}
	
	return "???";
}
/////////////////////////////////////////////////////////////////
void GLViewPort::setOrigPosType(GLViewPort::OrigPosType opt) { 
/////////////////////////////////////////////////////////////////
	origPosType = opt;
}
/////////////////////////////////////////////////////////////////
const wxPoint GLViewPort::evaluatePreDefPositions(GLViewPort::PreDefPos pdp, int wndSizeW, int wndSizeH) {
/////////////////////////////////////////////////////////////////
	// x as well as y is prepared in opengl logic ( 0,0 = left/bottom )
	// (INT_MAX, INT_MAX) is a alias for the window centre
	wxPoint ret(INT_MAX, INT_MAX);

	switch ( pdp )
	{
		case VPDP_TopLeft:		ret.x = margin;
								ret.y = wndSizeH - margin;
								break;
								
		case VPDP_TopMid:		ret.x = wndSizeW/2;
								ret.y = wndSizeH - margin;
								break;

		case VPDP_TopRight:		ret.x = wndSizeW - margin;
								ret.y = wndSizeH - margin;
								break;
								
		case VPDP_BottomLeft:	ret.x = margin;
								ret.y = margin;
								break;
								
		case VPDP_BottomMid:	ret.x = wndSizeW/2;
								ret.y = margin;
								break;

		case VPDP_BottomRight:	ret.x = wndSizeW - margin;
								ret.y = margin;
								break;
								
		case VPDP_LeftMid:		ret.x = margin;
								ret.y = wndSizeH/2;
								break;
								
		case VPDP_RightMid:		ret.x = wndSizeW - margin;
								ret.y = wndSizeH/2;
								break;
								
		case VPDP_Center:		
		default:				ret.x = INT_MAX;
								ret.y = INT_MAX; 
								break;
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////////
void GLViewPort::centreViewport() { 
/////////////////////////////////////////////////////////////////
	//std::cout << CNC_LOG_FUNCT_A("\n");
	
	origPosType = GLViewPort::VPOP_Center;
	evaluate(curWndSize.GetWidth(), curWndSize.GetHeight());
}
/////////////////////////////////////////////////////////////////
void GLViewPort::evaluate(int wndSizeW, int wndSizeH) {
/////////////////////////////////////////////////////////////////
	//std::cout << CNC_LOG_FUNCT_A("\n");
	
	const int px = origPosType == GLViewPort::VPOP_Center ? INT_MAX : curWndOrigin.x;
	const int py = origPosType == GLViewPort::VPOP_Center ? INT_MAX : curWndOrigin.y;
	
	evaluate(wndSizeW, wndSizeH, px, py);
}
/////////////////////////////////////////////////////////////////
void GLViewPort::evaluate(int wndSizeW, int wndSizeH, int custPosX, int custPosY) 
// wndSizeW: real window width
// wndSizeH: real window height
// custPosX: new centre pos.x - relative to (0, 0)
// custPosY: new centre pos.y - relative to (0, 0)
//
//  ---------------------------
//  |                         |
//  |       ----------        |
//  |       |        |        |
//  |       |        | n      | h
//  |       |(0,0)   |        |
//  |       ----------        |
//  |(x,y)      n             |
//  ---------------------------
//             w
// 
/////////////////////////////////////////////////////////////////
{
	// validation/correction to get every time a result
	wndSizeW = std::max(2, wndSizeW);
	wndSizeH = std::max(2, wndSizeH);
	
	// process centre mode
	if ( custPosX == INT_MAX || custPosY == INT_MAX ) {
		custPosX = wndSizeW / 2;
		custPosY = wndSizeH / 2;
		
		setOrigPosType(GLViewPort::VPOP_Center);
	}
	else {
		
		setOrigPosType(GLViewPort::VPOP_Custom);
	}
	
	// debug only
	//std::cout << CNC_LOG_FUNCT_A("given(%d,%d,%d,%d) [%s]\n", custPosX, custPosY, wndSizeW, wndSizeH, getOriginPosTypeAsString());
	
	// store given values
	curWndOrigin.x = custPosX;
	curWndOrigin.y = custPosY;
	curWndSize.Set(wndSizeW, wndSizeH);
	
	// determine w, h & n
	w = wndSizeW;
	h = wndSizeH;
	
	// determine the normalized size - for a quadratic view port
	n = w > h ? h : w;
	
	// stretch the values
	w *= factor;
	h *= factor;
	n *= factor;

	// move the virtual quadratic windows (centre based)
	const wxPoint wndCenter( n / 2, n / 2 );
	x = custPosX - wndCenter.x;
	y = custPosY - wndCenter.y;
	
	// calculate  the scissor values as sub rectangle from teh underlying window
	currScissor.SetX     ( std::max(0, std::min(x, wndSizeW)) );
	currScissor.SetY     ( std::max(0, std::min(y, wndSizeH)) );
	currScissor.SetWidth ( wndSizeW );
	currScissor.SetHeight( wndSizeH );
}
/////////////////////////////////////////////////////////////////
bool GLViewPort::process() {
/////////////////////////////////////////////////////////////////
	// debug only
	//trace(std::clog);
	
	glViewport(	getX(), 
				getY(),
				(GLsizei)getNormalizedSizeW(), 
				(GLsizei)getNormalizedSizeH());
	
	if ( GL_COMMON_CHECK_ERROR > 0 )
		std::cerr << CNC_LOG_FUNCT_A(": glViewport(%d,%d,%d,%d) failed\n", 
										getX(), getY(), getNormalizedSizeW(), getNormalizedSizeH());
	
	// 
	//glScissor(getScissorX(), getScissorY(), getScissorW(), getScissorH());
	
	return GL_COMMON_CHECK_ERROR == 0;
}
/////////////////////////////////////////////////////////////////
bool GLViewPort::processDefault(int wndSizeW, int wndSizeH) {
/////////////////////////////////////////////////////////////////
	std::cerr << CNC_LOG_FUNCT_A("No viewport defined, using default view port\n");
	glViewport (0, 0, (GLsizei)wndSizeW, (GLsizei)wndSizeH);
	
	if ( GL_COMMON_CHECK_ERROR > 0 )
		std::cerr << CNC_LOG_FUNCT_A(": glViewport(%d,%d,%d,%d) failed\n", 
										0, 0, wndSizeW, wndSizeH);
	
	return GL_COMMON_CHECK_ERROR == 0;
}

