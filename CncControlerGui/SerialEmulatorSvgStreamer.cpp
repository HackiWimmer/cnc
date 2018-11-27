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
bool SerialEmulatorSvgStreamer::isOutputAsTemplateAvailable() {
///////////////////////////////////////////////////////////////////
	if ( wxFile::Exists(getPortName()) == true )
		return true;
	
	return false; 
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	CncDoublePosition dPos;
	GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());

	if ( cnc::dblCompareNull(mi.speedValue) == false ) {
		bodyStream << wxString::Format("<!-- cnc speed change F=%+.3lf -->\n", mi.speedValue); 
	}
	
	bodyStream << "\t\t<path d=\"M ";
		bodyStream << dPos.getX() << " ";
		bodyStream << dPos.getY();
		
		bodyStream << " ";
		
		dPos.incX(mi.mdx);
		dPos.incY(mi.mdy);
		
		bodyStream << dPos.getX() << " ";
		bodyStream << dPos.getY();
		
	bodyStream << "\"  stroke=\"black\" fill=\"none\" stroke-width=\"0.5\" />\n";
	
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
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSvgStreamer::finalizeFile(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	const double width  = metricBoundbox.distanceX;
	const double height = metricBoundbox.distanceY;
	const double vbX	= metricBoundbox.minX;
	const double vbY	= metricBoundbox.minY;
	
	// don't stream something bofore the next line
	headerStream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n";
	headerStream << "<svg xmlns=\"http://www.w3.org/2000/svg\"\n";
	
		headerStream << wxString::Format("\twidth=\"%.3lfmm\" \n",  width);
		headerStream << wxString::Format("\theight=\"%.3lfmm\" \n", height);
		headerStream << wxString::Format("\tviewBox=\"%.3lf %.3lf %.3lf %.3lf\" \n", vbX, vbY, width, height);
		headerStream <<                  "\txmlns:xlink=\"http://www.w3.org/1999/xlink\">\n\n";
		
	headerStream << "\t<title>CNC Woodworking streaming template</title>\n";
	headerStream << "\t<desc>....</desc>\n";
	
	footerStream << "</svg>\n";
}

