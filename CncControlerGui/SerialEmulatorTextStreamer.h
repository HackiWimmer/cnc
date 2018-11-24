#ifndef SERIAL_EMULATOR_TXT_STREAMER_CLASS
#define SERIAL_EMULATOR_TXT_STREAMER_CLASS

#include <sstream>
#include "SerialEmulatorNull.h"

class SerialEmulatorTextStreamer : public SerialEmulatorNULL 
{
	protected:
		
		struct SetterInfo {
			unsigned char pid = PID_UNKNOWN;
			cnc::SetterValueList values;
		};
		
		struct MoveInfo {
			CncSpeedMode speedMode	= CncSpeedUserDefined;
			
			unsigned char cmd 		= CMD_INVALID;
			int32_t sdx				= 0;
			int32_t sdy				= 0;
			int32_t sdz				= 0;
			
			double mdx				= 0.0;
			double mdy				= 0.0;
			double mdz				= 0.0;
		};
		
		std::stringstream bodyStream;
		wxString fileName;
		CncSpeedMode currentSpeedMode;
		
		virtual bool writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte);
		virtual bool writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte);
		
		virtual bool writeEncodedSetterCallback(const SetterInfo& si);
		virtual bool writeEncodedMoveCallback(const MoveInfo& mi);
		
		virtual void initializeFile(const Serial::Trigger::BeginRun& tr);
		virtual void finalizeFile(const Serial::Trigger::EndRun& tr);
		
	public:
	
		explicit SerialEmulatorTextStreamer(CncControl* cnc);
		explicit SerialEmulatorTextStreamer(const char *fileName);
		virtual ~SerialEmulatorTextStreamer();
		
		virtual bool isOutputAsTemplateAvailable();
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorTextStreamer"; }
		virtual const char* getPortName()  { return fileName; }
		
		//Open file
		virtual bool connect(const char* fileName);
		//Close file
		virtual void disconnect(void);
		
		// trigger
		virtual void processTrigger(const Serial::Trigger::BeginRun& tr);
		virtual void processTrigger(const Serial::Trigger::EndRun& tr);
};

#endif