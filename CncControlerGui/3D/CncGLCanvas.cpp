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
void CncMetricRulerSetup::trace(GLContextCncPathBase* monitor, std::ostream& out) {
///////////////////////////////////////////////////
	out << "\nCncMetricRulerSetup::trace:" << std::endl;
	
	out << " Origin         : " << origin.getX() << ", " << origin.getY() << ", " << origin.getZ() << std::endl;
	
	out << " X axis         : " << minX << " <-> " << maxX << std::endl;
	out << " Y axis         : " << minY << " <-> " << maxY << std::endl;
	out << " Z axis         : " << minZ << " <-> " << maxZ << std::endl;
	
	out << " X major label #: " << monitor->getRulerX().axisLables.size() << std::endl;
	out << " Y major label #: " << monitor->getRulerY().axisLables.size() << std::endl;
	out << " Z major label #: " << monitor->getRulerZ().axisLables.size() << std::endl;
}
///////////////////////////////////////////////////
bool CncMetricRulerSetup::check(GLContextCncPathBase* monitor, std::ostream& out) {
///////////////////////////////////////////////////
	bool ret = true;
	
	auto checkSizes = [&](int enr, int s1, int s2, const char* m1, const char* m2) {
		if ( s1 != s2 ) {
			out << "CncMetricRulerSetup::checkSizes(" << enr << "): The following sizes aren't equal!" << std::endl;
			out << " " << m1 << " : "  << s1 << std::endl;
			out << " " << m2 << " : "  << s2 << std::endl;
			
			ret = false;
		}
	};
	
	/*
	auto checkEven = [&](int enr, int val, const char* m) {
		if ( val % 2 != 0 ) {
			out << "CncMetricRulerSetup::checkEven(" << enr << "): The value '" << m << "' isn't even!" << std::endl;
			
			ret = false;
		}
	};
	*/
	auto checkOdd = [&](int enr, int val, const char* m) {
		if ( val % 2 == 0 ) {
			out << "CncMetricRulerSetup::checkOdd(" << enr << "): The value '" << m << "' isn't odd!" << std::endl;
			
			ret = false;
		}
	};
	
	auto checkTrue = [&](int enr, bool val, const char* m) {
		if ( val != true ) {
			out << "CncMetricRulerSetup::checkTrue(" << enr << "): The value '" << m << "' isn't true!" << std::endl;
			
			ret = false;
		}
	};

	// helplines and labels are always aligned
	checkSizes( 1000, monitor->getXYPlane().helpLinesX.size(), monitor->getRulerX().axisLables.size(), "helpLinesX.size()", "axisLables.size()");
	checkSizes( 1001, monitor->getXYPlane().helpLinesY.size(), monitor->getRulerY().axisLables.size(), "helpLinesY.size()", "axisLables.size()");
	
	checkSizes( 1010, monitor->getXZPlane().helpLinesX.size(), monitor->getRulerX().axisLables.size(), "helpLinesX.size()", "axisLables.size()");
	checkSizes( 1011, monitor->getXZPlane().helpLinesZ.size(), monitor->getRulerZ().axisLables.size(), "helpLinesZ.size()", "axisLables.size()");
	
	checkSizes( 1020, monitor->getYZPlane().helpLinesY.size(), monitor->getRulerY().axisLables.size(), "helpLinesY.size()", "axisLables.size()");
	checkSizes( 1021, monitor->getYZPlane().helpLinesZ.size(), monitor->getRulerZ().axisLables.size(), "helpLinesZ.size()", "axisLables.size()");
	
	// to reach the null point always
	checkOdd ( 3000, (int)(minX / majorScanning), "minX / majorScanning"); 
	checkOdd ( 3001, (int)(minY / majorScanning), "minY / majorScanning");
	checkOdd ( 3002, (int)(minZ / majorScanning), "minZ / majorScanning");
	
	// to reach the null point always
	checkTrue( 4000, cnc::dblCompareNull(fmod(origin.getX(), majorScanning)), "cnc::dblCompareNull(fmod(origin.getX(), majorScanning))");
	checkTrue( 4001, cnc::dblCompareNull(fmod(origin.getY(), majorScanning)), "cnc::dblCompareNull(fmod(origin.getY(), majorScanning))");
	checkTrue( 4002, cnc::dblCompareNull(fmod(origin.getZ(), majorScanning)), "cnc::dblCompareNull(fmod(origin.getZ(), majorScanning))");
	
	return ret;
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::setupSize(double xDim, double yDim, double zDim) {
///////////////////////////////////////////////////
	double xMin = -fabs(xDim);
	double xMax = +fabs(xDim);
	double yMin = -fabs(yDim);
	double yMax = +fabs(yDim);
	double zMin = -fabs(zDim);
	double zMax = +fabs(zDim);

	// define min size
	minX = fabs(xMin) > majorScanning ? xMin : -majorScanning; 
	maxX = fabs(xMax) > majorScanning ? xMax : +majorScanning;
	minY = fabs(yMin) > majorScanning ? yMin : -majorScanning;
	maxY = fabs(yMax) > majorScanning ? yMax : +majorScanning;
	minZ = fabs(zMin) > majorScanning ? zMin : -majorScanning;
	maxZ = fabs(zMax) > majorScanning ? zMax : +majorScanning;
	
	wxASSERT(minX < maxX);
	wxASSERT(minY < maxY);
	wxASSERT(minZ < maxZ);
	
	// normalize (round) - now we have many times of majorScanning
	minX = minX - fmod(minX, majorScanning);
	maxX = maxX - fmod(maxX, majorScanning);
	minY = minY - fmod(minY, majorScanning);
	maxY = maxY - fmod(maxY, majorScanning);
	minZ = minZ - fmod(minZ, majorScanning);
	maxZ = maxZ - fmod(maxZ, majorScanning);
	
	// strech
	auto strechMin = [&](double min) {
		unsigned int fact = 2;
		min += -( fact * majorScanning );
		
		// the additional add is necessary to reach the '0' 
		// always with the following code:
		// for (int i = min; i <= max; i += majorScanning )
		if ( (int)(min / majorScanning) % 2 == 0 )
			min -= majorScanning;
			
		return min;
	};
	
	auto strechMax = [&](double max) {
		unsigned int fact = 2;
		max += +( fact * majorScanning );
		
		return max;
	};

	minX = strechMin(minX);
	maxX = strechMax(maxX);
	minY = strechMin(minY);
	maxY = strechMax(maxY);
	minZ = strechMin(minZ);
	maxZ = strechMax(maxZ);
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
void CncMetricRulerSetup::createHelpLinesXY(GLI::GLLineCluster& xLines, GLI::GLLineCluster& yLines) {
///////////////////////////////////////////////////
	// init with origin offset
	const double z = origin.getZ(); 
	
	// help lines - x axis
	for (double x = minX; x <= maxX; x += majorScanning )
		addLine(xLines, x, x, minY, maxY, z, z);
	
	// help lines - y axis
	for (double y = minY; y <= maxY; y += majorScanning )
		addLine(yLines, minX, maxX, y, y, z, z);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createHelpLinesXZ(GLI::GLLineCluster& xLines, GLI::GLLineCluster& zLines) {
///////////////////////////////////////////////////
	// init with origin offset
	const double y = origin.getY(); 
	
	// help lines - x axis
	for (double x = minX; x <= maxX; x += majorScanning )
		addLine(xLines, x, x, y, y, minZ, maxZ);
	
	// help lines - y axis
	for (double z = minZ; z <= maxZ; z += majorScanning )
		addLine(zLines, minX, maxX, y, y, z, z);
}
///////////////////////////////////////////////////
void CncMetricRulerSetup::createHelpLinesYZ(GLI::GLLineCluster& yLines, GLI::GLLineCluster& zLines) {
///////////////////////////////////////////////////
	// init with origin offset
	const double x = origin.getX(); 
	
	// help lines - x axis
	for (double y = minY; y <= maxY; y += majorScanning )
		addLine(yLines, x, x, y, y, minZ, maxZ);
	
	// help lines - y axis
	for (double z = minZ; z <= maxZ; z += majorScanning )
		addLine(zLines, x, x, minY, maxY, z, z);
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
			addLine(ruler.axisLines, x - oMinor, x + oMinor, y - oMinor, y + oMinor, z, z);
			
		} else {
			// add normal scanning line
			addLine(ruler.axisLines, x - oScann, x + oScann, y - oScann, y + oScann, z, z);
		}
	}
}



//////////////////////////////////////////////////
CncGlCanvas::CncGlCanvas(wxWindow *parent, int *attribList) 
//////////////////////////////////////////////////
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
, context(NULL)
, lastReshape()
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
void CncGlCanvas::incScale() {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().decScale();
}
//////////////////////////////////////////////////
void CncGlCanvas::decScale() {
//////////////////////////////////////////////////
	if ( context == NULL )
		return;
		
	context->getModelScale().incScale();
}
//////////////////////////////////////////////////
void CncGlCanvas::reshapeRelative(int dx, int dy) {
//////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int nx 	= lastReshape.x + dx;
	const int ny 	= cs.GetHeight() - lastReshape.y - dy;
	
	context->reshape(cs.GetWidth(), cs.GetHeight(), nx, ny);
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
		if (rot < 0 ) 	decScale();
		else 			incScale();
		display();
	}
	
	// move origin
	if ( event.ControlDown() == false ) {
		static int mx = 0, my = 0;
		
		if ( event.LeftDown() == true && mouseMoveMode == false ) {
			lastReshape.x = context->getLastReshapeX();
			lastReshape.y = cs.GetHeight() - context->getLastReshapeY();
			
			mx = event.GetX();
			my = event.GetY();
			
			mouseMoveMode = true;
		}
		
		// calculate new origin
		if ( mouseMoveMode == true ) {
			const int dx = event.GetX() - mx;
			const int dy = event.GetY() - my;
			
			reshapeRelative(dx, dy);
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
