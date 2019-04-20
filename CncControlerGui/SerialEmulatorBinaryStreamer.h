#ifndef SERIAL_EMULATOR_BIN_STREAMER_CLASS
#define SERIAL_EMULATOR_BIN_STREAMER_CLASS

#include "CncBinaryTemplateStreamer.h"
#include "SerialEmulatorNull.h"

class SerialEmulatorBinaryStreamer : public SerialEmulatorNULL 
                                   , public CncBinaryTemplateStreamer 
{
	protected:
	
		wxString fileName;
		TemplateFormat inboundFormat;
		
		virtual bool writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte);
		virtual bool writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte);
		virtual bool writeMoveSequenceRawCallback(unsigned char *buffer, unsigned int nbByte);

		
	public:
	
		explicit SerialEmulatorBinaryStreamer(CncControl* cnc);
		explicit SerialEmulatorBinaryStreamer(const char *fileName);
		virtual ~SerialEmulatorBinaryStreamer();
		
		virtual void notifySetter(const CncCommandDecoder::SetterInfo& si);
		virtual void notifyMove(int32_t dx, int32_t dy, int32_t dz, int32_t f);
		virtual void notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequence& sequence);
		virtual void notifyMoveSequenceNext(const CncCommandDecoder::MoveSequence& sequence);
		virtual void notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequence& sequence);

		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorBinaryStreamer"; }
		virtual const char* getPortName()  { return getOutboundFileName(); }
		
		//Open file
		virtual bool connect(const char* fileName);
		//Close file
		virtual void disconnect();
		
		virtual bool isOutputAsTemplateAvailable();
		
		// trigger
		virtual void processTrigger(const Serial::Trigger::BeginRun& tr);
		virtual void processTrigger(const Serial::Trigger::EndRun& tr);
		virtual void processTrigger(const Serial::Trigger::NextPath& tr);
};

#endif
