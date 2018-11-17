#ifndef SERIAL_EMULATOR_STREAMER_CLASS
#define SERIAL_EMULATOR_STREAMER_CLASS

#include "CncBinaryTemplateStreamer.h"
#include "SerialEmulatorNull.h"

class SerialEmulatorStreamer : public SerialEmulatorNULL 
                             , public CncBinaryTemplateStreamer 
{
	protected:
	
		wxString fileName;
		
		virtual bool writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte);
		virtual bool writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte);
		
	public:
	
		explicit SerialEmulatorStreamer(CncControl* cnc);
		explicit SerialEmulatorStreamer(const char *fileName);
		virtual ~SerialEmulatorStreamer();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorStreamer"; }
		
		virtual const char* getPortName()  { return getOutboundFileName(); }
		
		//Open file
		virtual bool connect(const char* fileName);
		//Close file
		virtual void disconnect(void);
		
		virtual bool isOutputAsTemplateAvailable();

		// trigger
		virtual void processTrigger(const Serial::Trigger::BeginRun& tr);
		virtual void processTrigger(const Serial::Trigger::EndRun& tr);
};

#endif