#include <wx/file.h>
#include "CncConfig.h"
#include "SerialEmulatorSvgStreamer.h"

///////////////////////////////////////////////////////////////////
SerialEmulatorSvgStreamer::SerialEmulatorSvgStreamer(CncControl* cnc)
: SerialEmulatorTextStreamer(cnc)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
//Initialize Serial communication with the given COM port
SerialEmulatorSvgStreamer::SerialEmulatorSvgStreamer(const char* fileName) 
: SerialEmulatorTextStreamer(fileName)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialEmulatorSvgStreamer::~SerialEmulatorSvgStreamer() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	CncDoublePosition dPos;
	GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());

	svgStream << "\t\t<path d=\"M ";
		svgStream << dPos.getX() << " ";
		svgStream << dPos.getY();
		
		svgStream << " ";
		
		dPos.incX(mi.mdx);
		dPos.incY(mi.mdy);
		
		svgStream << dPos.getX() << " ";
		svgStream << dPos.getY();
		
	svgStream << "\"  stroke=\"black\" fill=\"none\" stroke-width=\"0.5\" />\n";
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedSetterCallback(const SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSvgStreamer::initializeFile(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	svgStream.str("");
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSvgStreamer::finalizeFile(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////

	CncDoublePosition dPos;
	GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());

	CncLongPosition::Watermarks wm;
	getCurrentEmulatorPosition().getWatermarks(wm);
	
	const double xMin = GBL_CONFIG->convertStepsToMetricX(wm.xMin);
	const double xMax = GBL_CONFIG->convertStepsToMetricX(wm.xMax);
	const double yMin = GBL_CONFIG->convertStepsToMetricY(wm.yMin);
	const double yMax = GBL_CONFIG->convertStepsToMetricY(wm.yMax);
	//const double zMin = GBL_CONFIG->convertStepsToMetricZ(wm.zMin);
	//const double zMax = GBL_CONFIG->convertStepsToMetricZ(wm.zMax);
	
	const double width  = abs(xMax - xMin);
	const double height = abs(yMax - yMin);
	const double vbX	= xMin;
	const double vbY	= yMin;
	

#warning
/*
	std::cout << xMin << std::endl;
	std::cout << xMax << std::endl;
	std::cout << yMin << std::endl;
	std::cout << yMax << std::endl;
*/

	// don't strem something bofore the next line
	bodyStream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n";
	
	bodyStream << "<svg xmlns=\"http://www.w3.org/2000/svg\"\n";
	
		bodyStream << wxString::Format("\twidth=\"%.3lfmm\" \n",  width);
		bodyStream << wxString::Format("\theight=\"%.3lfmm\" \n", height);
		bodyStream << wxString::Format("\tviewBox=\"%.3lf %.3lf %.3lf %.3lf\" \n", vbX, vbY, width, height);
		bodyStream <<                  "\txmlns:xlink=\"http://www.w3.org/1999/xlink\">\n\n";
		
	bodyStream << "\t<title>CNC Woodworking streaming template</title>\n";
	bodyStream << "\t<desc>....</desc>\n";
	
		bodyStream << svgStream.str();
	
	bodyStream << "</svg>\n";
}

