#ifndef SERIAL_EMULATOR_GCODE_STREAMER_CLASS
#define SERIAL_EMULATOR_GCODE_STREAMER_CLASS

#include "SerialEmulatorTextStreamer.h"

class SerialEmulatorGCodeStreamer : public SerialEmulatorTextStreamer
{
	protected:
	
		virtual bool writeEncodedSetterCallback(const SetterInfo& si);
		virtual bool writeEncodedMoveCallback(const MoveInfo& mi);
		
		virtual void initializeFile(const Serial::Trigger::BeginRun& tr);
		virtual void finalizeFile(const Serial::Trigger::EndRun& tr);
		
	public:
	
		explicit SerialEmulatorGCodeStreamer(CncControl* cnc);
		explicit SerialEmulatorGCodeStreamer(const char *fileName);
		virtual ~SerialEmulatorGCodeStreamer();
		
		virtual bool isOutputAsTemplateAvailable();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorGCodeStreamer"; }
		
};

#endif