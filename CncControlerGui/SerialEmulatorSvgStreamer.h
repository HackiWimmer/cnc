#ifndef SERIAL_EMULATOR_SVG_STREAMER_CLASS
#define SERIAL_EMULATOR_SVG_STREAMER_CLASS

#include "SerialEmulatorTextStreamer.h"

class SerialEmulatorSvgStreamer : public SerialEmulatorTextStreamer
{
	protected:
		
		virtual bool writeEncodedSetterCallback(const SetterInfo& si);
		virtual bool writeEncodedMoveCallback(const MoveInfo& mi);
		
		virtual bool writeEncodedMoveSequenceCallback(const MoveInfo& mi);
		virtual bool writeEncodedMoveSequenceBeginCallback(const CncCommandDecoder::MoveSequenceInfo& sequence);
		virtual bool writeEncodedMoveSequenceNextCallback(const CncCommandDecoder::MoveSequenceInfo& sequence);
		virtual bool writeEncodedMoveSequenceEndCallback(const CncCommandDecoder::MoveSequenceInfo& sequence);

		virtual void initializeFile(const Serial::Trigger::BeginRun& tr);
		virtual void finalizeFile(const Serial::Trigger::EndRun& tr);
		virtual void initializePath(const Serial::Trigger::NextPath& tr);
	
	public:
	
		explicit SerialEmulatorSvgStreamer(CncControl* cnc);
		virtual ~SerialEmulatorSvgStreamer();
		

		virtual bool isOutputAsTemplateAvailable();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorSvgStreamer"; }
};

#endif
