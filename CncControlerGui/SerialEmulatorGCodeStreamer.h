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
		virtual void initializePath(const Serial::Trigger::NextPath& tr);
		
	public:
	
		explicit SerialEmulatorGCodeStreamer(CncControl* cnc);
		explicit SerialEmulatorGCodeStreamer(const char *fileName);
		virtual ~SerialEmulatorGCodeStreamer();
		
		virtual void notifySetter(const CncCommandDecoder::SetterInfo& si);
		virtual void notifyMove(int32_t dx, int32_t dy, int32_t dz, int32_t f);
		virtual void notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequence& sequence);
		virtual void notifyMoveSequenceNext(const CncCommandDecoder::MoveSequence& sequence);
		virtual void notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequence& sequence);

		virtual bool isOutputAsTemplateAvailable();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorGCodeStreamer"; }
		
};

#endif
