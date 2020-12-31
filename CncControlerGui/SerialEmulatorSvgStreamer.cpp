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
bool SerialEmulatorSvgStreamer::writeEncodedMoveSequenceCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	CncDoublePosition dPos;
	THE_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());

	bodyStream << "\t\t<path d=\"M ";
		bodyStream << dPos.getX() << " ";
		bodyStream << dPos.getY();
		
		bodyStream << " ";
		
		dPos.incX(mi.mdx);
		dPos.incY(mi.mdy);
		
		bodyStream << dPos.getX() << " ";
		bodyStream << dPos.getY();
		
		
	if ( mi.speedMode == CncSpeedRapid )	bodyStream << "\"  stroke=\"yellow\" fill=\"none\" stroke-width=\"0.5\" stroke-dasharray=\"5,10,5\" />\n";
	else									bodyStream << "\"  stroke=\"black\"  fill=\"none\" stroke-width=\"0.5\" />\n";
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedMoveSequenceBeginCallback(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedMoveSequenceNextCallback(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedMoveSequenceEndCallback(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	CncDoublePosition dPos;
	THE_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());

	if ( cnc::dblCompareNull(mi.speedValue) == false ) {
		bodyStream << wxString::Format("<!-- cnc speed change F=%+.3lf -->\n", mi.speedValue); 
	}
	
	return writeEncodedMoveSequenceCallback(mi);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSvgStreamer::writeEncodedSetterCallback(const SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSvgStreamer::initializeFile(const Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSvgStreamer::finalizeFile(const Trigger::EndRun& tr) {
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
///////////////////////////////////////////////////////////////////
void SerialEmulatorSvgStreamer::initializePath(const Trigger::NextPath& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << "<!-- Next Path-->\n";
}
