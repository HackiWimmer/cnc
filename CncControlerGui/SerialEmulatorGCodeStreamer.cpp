#include <wx/file.h>
#include "CncConfig.h"
#include "SerialEmulatorGCodeStreamer.h"

///////////////////////////////////////////////////////////////////
SerialEmulatorGCodeStreamer::SerialEmulatorGCodeStreamer(CncControl* cnc)
: SerialEmulatorTextStreamer(cnc)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
//Initialize Serial communication with the given COM port
SerialEmulatorGCodeStreamer::SerialEmulatorGCodeStreamer(const char* fileName) 
: SerialEmulatorTextStreamer(fileName)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialEmulatorGCodeStreamer::~SerialEmulatorGCodeStreamer() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::isOutputAsTemplateAvailable() {
///////////////////////////////////////////////////////////////////
	if ( wxFile::Exists(getPortName()) == true )
		return true;
	
	return false; 
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedMoveSequenceCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	return writeEncodedMoveCallback(mi);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedMoveSequenceBeginCallback(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedMoveSequenceNextCallback(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedMoveSequenceEndCallback(const CncCommandDecoder::MoveSequenceInfo& sequence) {
///////////////////////////////////////////////////////////////////
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	wxString G("G1 ");
	if ( mi.speedMode == CncSpeedRapid )
		G.assign("G0 ");
		
	bodyStream << G;

	if ( cnc::dblCompareNull(mi.mdx) == false )			bodyStream << wxString::Format("X%+.3lf ", mi.mdx);
	if ( cnc::dblCompareNull(mi.mdy) == false )			bodyStream << wxString::Format("Y%+.3lf ", mi.mdy); 
	if ( cnc::dblCompareNull(mi.mdz) == false )			bodyStream << wxString::Format("Z%+.3lf ", mi.mdz); 
	if ( cnc::dblCompareNull(mi.speedValue) == false )	bodyStream << wxString::Format("F%+.3lf ", mi.speedValue); 
	
	bodyStream << std::endl;
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedSetterCallback(const SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	// nothing to do
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorGCodeStreamer::initializeFile(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	headerStream << "(<File start>)\n";
	
	// some init values
	headerStream << "\n";
		headerStream << "(<Some Setup>)\n";
		headerStream << "G17 G40\n";
		headerStream << "G80\n";
		headerStream << "T01 M6\n";
		headerStream << "S0 F100 M3\n";
		headerStream << "G43 H07\n";
	
	// init position
	headerStream << "\n";
		headerStream << "(<Init absolute position>)\n";
		headerStream << "G90\n";
		CncDoublePosition dPos;
		GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());
		headerStream << wxString::Format("G0 X%+.3lf Y%+.3lf Z%+.3lf\n", dPos.getX(), dPos.getY(), dPos.getZ());
	
	// all the further moves are relative
	headerStream << "\n";
		headerStream << "(<Spooling start>)\n";
		headerStream << "G91\n\n";
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorGCodeStreamer::finalizeFile(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	footerStream << "(<Spooling end)\n";
	footerStream << "M2\n";
	footerStream << "(<File end>)\n";
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorGCodeStreamer::initializePath(const Serial::Trigger::NextPath& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << "\n(<Next Path>)\n";
}

