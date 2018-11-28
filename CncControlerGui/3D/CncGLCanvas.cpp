#include "CncConfig.h"
#include "CncGLCanvas.h"

///////////////////////////////////////////////////
CncMetricRulerSetup::CncMetricRulerSetup() 
: majorScanning(10.0)
, minorScanning( 5.0)
, origin({-10.0, -10.0, -10.0})
, factorX( GBL_CONFIG->getCalculationFactX() / GBL_CONFIG->getDispFactX3D() )
, factorY( GBL_CONFIG->getCalculationFactY() / GBL_CONFIG->getDispFactY3D() )
, factorZ( GBL_CONFIG->getCalculationFactZ() / GBL_CONFIG->getDispFactZ3D() )
, minX(0.0)
, maxX(0.0)
, minY(0.0)
, maxY(0.0)
, minZ(0.0)
, maxZ(0.0)
///////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////
CncMetricRulerSetup::~CncMetricRulerSetup() {
///////////////////////////////////////////////////
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::setupOrigin(const CncDoublePosition& o) {
///////////////////////////////////////////////////
	origin.set(o);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::setupSize(double xMin, double xMax, 
                                    double yMin, double yMax, 
								    double zMin, double zMax) {
///////////////////////////////////////////////////
	// define min size
	minX = fabs(xMin) > majorScanning ? xMin : +majorScanning; 
	maxX = fabs(xMax) > majorScanning ? xMax : -majorScanning;
	minY = fabs(yMin) > majorScanning ? yMin : +majorScanning;
	maxY = fabs(yMax) > majorScanning ? yMax : -majorScanning;
	minZ = fabs(zMin) > majorScanning ? zMin : +majorScanning;
	maxZ = fabs(zMax) > majorScanning ? zMax : -majorScanning;
	
	// strech
	unsigned int fact = 2;
	minX += -( fact * majorScanning );
	maxX += +( fact * majorScanning );
	minY += -( fact * majorScanning );
	maxY += +( fact * majorScanning );
	minZ += -( fact * majorScanning );
	maxZ += +( fact * majorScanning );
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::addLine(GLI::GLLineCluster& lines, double x1, double x2, 
															 double y1, double y2, 
															 double z1, double z2) {
///////////////////////////////////////////////////
	static GLI::GLVectiesTuple vt;
	
	vt.setData(cnvX(x1), cnvX(y1), 
	           cnvZ(z1), cnvX(x2), 
			   cnvX(y2), cnvZ(z2));
			   
	lines.push_back(vt);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::addLabel(GLI::GLLabelCluster& labels, double x, 
															    double y, 
															    double z, 
															    void* font, 
															    const wxString label) {
///////////////////////////////////////////////////
	static GLI::GLLabelInfo li;
	
	li.x 		= cnvX(x);
	li.y 		= cnvY(y);
	li.z 		= cnvZ(z);
	li.font 	= font;
	li.label 	= label;
	
	labels.push_back(li);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createRulerOrigin(GLI::GLLabelCluster& o) {
///////////////////////////////////////////////////
	addLabel(o, 
			 origin.getX(), 
			 origin.getY(), 
			 origin.getZ(), 
			 GLUT_BITMAP_8_BY_13, 
			 wxString::Format("%d", 0));
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createHelpLinesXY(GLI::GLLineCluster& xyPlane) {
///////////////////////////////////////////////////
	// init with origin offset
	const double z = origin.getZ(); 
	
	// help lines - x axis
	for (double x = minX; x <= maxX; x += majorScanning )
		addLine(xyPlane, x, x, minY, maxY, z, z);
	
	// help lines - y axis
	for (double y = minY; y <= maxY; y += majorScanning )
		addLine(xyPlane, minX, maxX, y, y, z, z);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createHelpLinesXZ(GLI::GLLineCluster& xzPlane) {
///////////////////////////////////////////////////
	// init with origin offset
	const double y = origin.getY(); 
	
	// help lines - x axis
	for (double x = minX; x <= maxX; x += majorScanning )
		addLine(xzPlane, x, x, y, y, minZ, maxZ);
	
	// help lines - y axis
	for (double z = minX; z <= maxX; z += majorScanning )
		addLine(xzPlane, minX, maxX, y, y, z, z);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createHelpLinesYZ(GLI::GLLineCluster& yzPlane) {
///////////////////////////////////////////////////
	// init with origin offset
	const double x = origin.getX(); 
	
	// help lines - x axis
	for (double y = minY; y <= maxY; y += majorScanning )
		addLine(yzPlane, x, x, y, y, minZ, maxZ);
	
	// help lines - y axis
	for (double z = minX; z <= maxX; z += majorScanning )
		addLine(yzPlane, x, x, minY, maxY, z, z);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createRulerX(GLI::GLAxisRuler& ruler) {
///////////////////////////////////////////////////
	// init with origin offset
	const double y = origin.getY(), z = origin.getZ(); 
	//reset
	ruler.clear();
	
	// axis
	addLine(ruler.axisLines, minX, maxX, y, y, z, z);
	
	// scanning
	for (double ix = minX; ix <= maxX; ix += 1.0 ) {
		// transform origin
		const double x = ix + origin.getX();
		
		if ( cnc::dblCompareNull(fmod(ix, majorScanning)) == true ) {
			// add label
			const int label = majorScanning * ( x / majorScanning );
			addLabel(ruler.axisLables, x, y, z, GLUT_BITMAP_8_BY_13, wxString::Format("%+d", label));
			
			// add major scanning line
			addLine(ruler.axisLines, x, x, y - oMajor, y + oMajor, z - oMajor, z + oMajor);
			
		} else if ( cnc::dblCompareNull(fmod(x, minorScanning)) == true )  {
			// add minor scanning line
			addLine(ruler.axisLines, x, x, y - oMinor, y + oMinor, z -oMinor, z + oMinor);
			
		} else {
			// add normal scanning line
			addLine(ruler.axisLines, x, x, y - oScann, y + oScann, z -oScann, z + oScann);
		}
	}
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createRulerY(GLI::GLAxisRuler& ruler) {
///////////////////////////////////////////////////
	// init with origin offset
	const double x = origin.getX(), z = origin.getZ(); 
	//reset
	ruler.clear();
	
	// axis
	addLine(ruler.axisLines, x, x, minY, maxY, z, z);
	
	// scanning
	for (double iy = minY; iy <= maxY; iy += 1.0 ) {
		// transform origin
		const double y = iy + origin.getY();
		
		if ( cnc::dblCompareNull(fmod(iy, majorScanning)) == true ) {
			// add label
			const int label = majorScanning * ( y / majorScanning );
			addLabel(ruler.axisLables, x, y, z, GLUT_BITMAP_8_BY_13, wxString::Format("%+d", label));
			
			// add major scanning line
			addLine(ruler.axisLines, x - oMajor, x + oMajor, y, y, z - oMajor, z + oMajor);
			
		} else if ( cnc::dblCompareNull(fmod(iy, minorScanning)) == true )  {
			// add minor scanning line
			addLine(ruler.axisLines, x - oMinor, x + oMinor, y, y, z - oMinor, z + oMinor);
			
		} else {
			// add normal scanning line
			addLine(ruler.axisLines, x - oScann, x + oScann, y, y, z - oScann, z + oScann);
		}
	}
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createRulerZ(GLI::GLAxisRuler& ruler) {
///////////////////////////////////////////////////
	// init with origin offset
	const double x = origin.getX(), y = origin.getY(); 
	//reset
	ruler.clear();
	
	// axis
	addLine(ruler.axisLines, x, x, y, y, minZ, maxZ);
	
	// scanning
	for (double iz = minZ; iz <= maxZ; iz += 1.0 ) {
		// transform origin
		const double z = iz + origin.getZ();
		
		if ( cnc::dblCompareNull(fmod(iz, majorScanning)) == true ) {
			// add label
			const int label = majorScanning * ( z / majorScanning );
			addLabel(ruler.axisLables, x, y, z, GLUT_BITMAP_8_BY_13, wxString::Format("%+d", label));
			
			// add major scanning line
			addLine(ruler.axisLines, x - oMajor, x + oMajor, y, y, z - oMajor, z + oMajor);
			
		} else if ( cnc::dblCompareNull(fmod(iz, minorScanning)) == true )  {
			// add minor scanning line
			addLine(ruler.axisLines, x - oMinor, x + oMinor, y, y, z - oMinor, z + oMinor);
			
		} else {
			// add normal scanning line
			addLine(ruler.axisLines, x - oScann, x + oScann, y, y, z - oScann, z + oScann);
		}
	}
}



//////////////////////////////////////////////////
CncGlCanvas::CncGlCanvas(wxWindow *parent, int *attribList) 
//////////////////////////////////////////////////
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
, context(NULL)
, mouseMoveMode(false)
{
}
//////////////////////////////////////////////////
CncGlCanvas::~CncGlCanvas() {
//////////////////////////////////////////////////
}
//////////////////////////////////////////////////
void CncGlCanvas::view(GLContextBase::ViewMode fm) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->setViewMode(fm);
	
	const wxSize cs = GetClientSize();
	context->reshapeViewMode(cs.GetWidth(), cs.GetHeight());
	display();
}
//////////////////////////////////////////////////
void CncGlCanvas::onMouse(wxMouseEvent& event) {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;

	const wxSize cs = GetClientSize();

	// activate the keyboard focus for this frame
	this->SetFocusFromKbd();
	
	// wheel
	const int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		if (rot < 0 ) 	context->getModelScale().decScale();
		else 			context->getModelScale().incScale();
		display();
	}
	
	// move origin
	if ( event.ControlDown() == false ) {
		static int lx = 0, ly = 0;
		static int mx = 0, my = 0;
		
		if ( event.LeftDown() == true && mouseMoveMode == false ) {
			lx = context->getLastReshapeX();
			ly = cs.GetHeight() - context->getLastReshapeY();
			
			mx = event.GetX();
			my = event.GetY();
			
			mouseMoveMode = true;
		}
		
		// calculate new origin
		if ( mouseMoveMode == true ) {
			const int dx = event.GetX() - mx;
			const int dy = event.GetY() - my;
			
			const int nx = lx + dx;
			const int ny = cs.GetHeight() - ly - dy;
			
			context->reshape(cs.GetWidth(), cs.GetHeight(), nx, ny);
			display();
		}
		
		// reset move mode
		if ( event.LeftUp() == true ) {
			mouseMoveMode = false;
		}

	// set origin
	} else {
		
		// left button
		if ( event.LeftIsDown() == true ) {
			// reverse y because the opengl viewport origin (0,0) is at left/bottom
			const int x = event.GetX();
			const int y = cs.GetHeight() - event.GetY();
			
			context->reshape(cs.GetWidth(), cs.GetHeight(), x, y);
			display();
		}
	}
}
