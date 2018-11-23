#ifndef SERIAL_EMULATOR_FILE_CLASS
#define SERIAL_EMULATOR_FILE_CLASS

#include <fstream> 
#include "SerialEmulatorNull.h"

class SerialEmulatorFile : public SerialEmulatorNULL
{
	protected:
		std::fstream fileStream;
		std::ios_base::openmode fileFlags;
		
		virtual bool writeMoveCmdRendered(int32_t x , int32_t y , int32_t z);
		
	public:
	
		//Initialize Serial communication without an acitiv connection 
		SerialEmulatorFile(CncControl* cnc)
		: SerialEmulatorNULL(cnc) {
			fileFlags = std::ios::out | std::ios::trunc | std::ios::binary;
		}
		//Initialize Serial communication with the given COM port
		SerialEmulatorFile(const char *fileName)
		: SerialEmulatorNULL(fileName) {
			fileFlags = std::ios::out | std::ios::trunc | std::ios::binary;
		}
		virtual ~SerialEmulatorFile() {}
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorFile"; }
		//Open file
		virtual bool connect(const char* fileName);
		//Close file
		virtual void disconnect(void);
};

#endif