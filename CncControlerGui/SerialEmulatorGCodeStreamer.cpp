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
bool SerialEmulatorGCodeStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	wxString G("G1 ");
	if ( mi.speedMode == CncSpeedRapid )
		G.assign("\nG0 ");
		
	bodyStream << G;

	if ( cnc::dblCompareNull(mi.mdx) == false )		bodyStream << wxString::Format("X%+.3lf ", mi.mdx);
	if ( cnc::dblCompareNull(mi.mdy) == false )		bodyStream << wxString::Format("Y%+.3lf ", mi.mdy); 
	if ( cnc::dblCompareNull(mi.mdz) == false )		bodyStream << wxString::Format("Z%+.3lf ", mi.mdz); 
	
	bodyStream << std::endl;
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorGCodeStreamer::writeEncodedSetterCallback(const SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	switch ( si.pid ) {
		
		
		case PID_SPEED_MM_MIN:	bodyStream << "F" << si.values.front() << "\n";
									break;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorGCodeStreamer::initializeFile(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << "(<File start>)\n";
	
	// some init values
	bodyStream << "\n";
		bodyStream << "(<Some Setup>)\n";
		bodyStream << "G17 G40\n";
		bodyStream << "G80\n";
		bodyStream << "T01 M6\n";
		bodyStream << "S0 F100 M3\n";
		bodyStream << "G43 H07\n";
	
	// init position
	bodyStream << "\n";
		bodyStream << "(<Init absolute position>)\n";
		bodyStream << "G90\n";
		CncDoublePosition dPos;
		GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());
		bodyStream << wxString::Format("G0 X%+.3lf Y%+.3lf Z%+.3lf\n", dPos.getX(), dPos.getY(), dPos.getZ());
	
	// all the further moves are relative
	bodyStream << "\n";
		bodyStream << "(<Spooling start>)\n";
		bodyStream << "G91\n\n";
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorGCodeStreamer::finalizeFile(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << "(<Spooling end)\n";
	bodyStream << "M2\n";
	bodyStream << "(<File end>)\n";
}

