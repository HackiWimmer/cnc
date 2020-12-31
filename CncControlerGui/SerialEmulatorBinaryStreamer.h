#ifndef SERIAL_EMULATOR_BIN_STREAMER_CLASS
#define SERIAL_EMULATOR_BIN_STREAMER_CLASS

#include "CncBinaryTemplateStreamer.h"
#include "SerialEmulatorNull.h"

class SerialEmulatorBinaryStreamer : public SerialEmulatorNULL 
                                   , public CncBinaryTemplateStreamer 
{
	protected:
	
		wxString fileName;
		CncTemplateFormat inboundFormat;
		
		virtual bool writeMoveSequenceRawCallback(unsigned char* buffer, unsigned int nbByte);
		virtual bool writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte);
		virtual bool writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte);
		
	public:
	
		explicit SerialEmulatorBinaryStreamer(CncControl* cnc);
		virtual ~SerialEmulatorBinaryStreamer();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorBinaryStreamer"; }
		virtual const char* getPortName()  { return getOutboundFileName(); }
		
		//Open file
		virtual bool connect(const char* fileName);
		//Close file
		virtual void disconnect();
		
		virtual bool isOutputAsTemplateAvailable();
		
		// trigger
		virtual void processTrigger(const Trigger::BeginRun& tr);
		virtual void processTrigger(const Trigger::EndRun& tr);
		virtual void processTrigger(const Trigger::NextPath& tr);
		virtual void processTrigger(const Trigger::GuidePath& tr);
};

#endif
