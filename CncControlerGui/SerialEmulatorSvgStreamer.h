#ifndef SERIAL_EMULATOR_SVG_STREAMER_CLASS
#define SERIAL_EMULATOR_SVG_STREAMER_CLASS

#include "SerialEmulatorTextStreamer.h"

class SerialEmulatorSvgStreamer : public SerialEmulatorTextStreamer
{
	protected:
		
		virtual bool writeEncodedSetterCallback(const SetterInfo& si);
		virtual bool writeEncodedMoveCallback(const MoveInfo& mi);
		
		virtual void initializeFile(const Serial::Trigger::BeginRun& tr);
		virtual void finalizeFile(const Serial::Trigger::EndRun& tr);
	
	public:
	
		explicit SerialEmulatorSvgStreamer(CncControl* cnc);
		explicit SerialEmulatorSvgStreamer(const char *fileName);
		virtual ~SerialEmulatorSvgStreamer();
		
		virtual bool isOutputAsTemplateAvailable();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorSvgStreamer"; }
};

#endif