#ifndef SERIAL_EMULATOR_GCODE_STREAMER_CLASS
#define SERIAL_EMULATOR_GCODE_STREAMER_CLASS

#include "SerialEmulatorTextStreamer.h"

class SerialEmulatorGCodeStreamer : public SerialEmulatorTextStreamer
{
	protected:
	
		virtual bool writeEncodedSetterCallback(const SetterInfo& si);
		virtual bool writeEncodedMoveCallback(const MoveInfo& mi);
		
		virtual bool writeEncodedMoveSequenceCallback(const MoveInfo& mi);
		virtual bool writeEncodedMoveSequenceBeginCallback(const CncCommandDecoder::MoveSequenceInfo& sequence);
		virtual bool writeEncodedMoveSequenceNextCallback(const CncCommandDecoder::MoveSequenceInfo& sequence);
		virtual bool writeEncodedMoveSequenceEndCallback(const CncCommandDecoder::MoveSequenceInfo& sequence);
		
		virtual void initializeFile(const Trigger::BeginRun& tr);
		virtual void initializePath(const Trigger::NextPath& tr);
		virtual void finalizeFile(const Trigger::EndRun& tr);
		
	public:
	
		explicit SerialEmulatorGCodeStreamer(CncControl* cnc);
		virtual ~SerialEmulatorGCodeStreamer();
		
		virtual bool isOutputAsTemplateAvailable();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorGCodeStreamer"; }
		
};

#endif
