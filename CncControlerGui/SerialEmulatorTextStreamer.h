#ifndef SERIAL_EMULATOR_TXT_STREAMER_CLASS
#define SERIAL_EMULATOR_TXT_STREAMER_CLASS

#include <sstream>
#include "SerialEmulatorNull.h"

class SerialEmulatorTextStreamer : public SerialEmulatorNULL 
{
	protected:
		
		struct MetricBoundbox {
			double minX				= 0.0;
			double maxX				= 0.0;
			double minY				= 0.0;
			double maxY				= 0.0;
			double minZ				= 0.0;
			double maxZ				= 0.0;
			
			double distanceX		= 0.0;
			double distanceY		= 0.0;
			double distanceZ		= 0.0;
		};
		
		struct SetterInfo {
			unsigned char pid = PID_UNKNOWN;
			cnc::SetterValueList values;
		};
		
		struct MoveInfo {
			CncSpeedMode speedMode	= CncSpeedUserDefined;
			double speedValue		= 0.0;
			
			unsigned char cmd 		= CMD_INVALID;
			int32_t sdx				= 0;
			int32_t sdy				= 0;
			int32_t sdz				= 0;
			
			double mdx				= 0.0;
			double mdy				= 0.0;
			double mdz				= 0.0;
		};
		
		std::stringstream 	headerStream;
		std::stringstream 	bodyStream;
		std::stringstream 	footerStream;
		
		wxString 			fileName;
		CncDoublePosition	startPos;
		CncSpeedMode 		currentSpeedMode;
		double				currentSpeedValue;
		MetricBoundbox 		metricBoundbox;
		
		virtual bool writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte);
		virtual bool writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte);
		
		virtual bool writeEncodedSetterCallback(const SetterInfo& si);
		virtual bool writeEncodedMoveCallback(const MoveInfo& mi);
		
		virtual void initializeFile(const Serial::Trigger::BeginRun& tr);
		virtual void finalizeFile(const Serial::Trigger::EndRun& tr);
		virtual void initializePath(const Serial::Trigger::NextPath& tr);
		
		virtual const wxString& formatPosition(const int32_t value)          const;
		virtual const wxString& formatPosition(const double value)           const;
		virtual const wxString& formatPosition(const CncLongPosition& pos)   const;
		virtual const wxString& formatPosition(const CncDoublePosition& pos) const;
		
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
		virtual void processTrigger(const Serial::Trigger::NextPath& tr);
};

#endif