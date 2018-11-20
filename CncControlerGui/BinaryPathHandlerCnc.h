#ifndef BIN_PARSE_HANDLER_CNC_H
#define BIN_PARSE_HANDLER_CNC_H

#include "CncControl.h"
#include "BinaryPathHandlerBase.h"

class BinaryPathHandlerCnc : public BinaryPathHandlerBase {
	
	protected:
		CncControl* cncControl;
		
		void initNextClientId(long id);
		void logMeasurementStart();
		void logMeasurementEnd();
			
	public:
		BinaryPathHandlerCnc(CncControl* cnc);
		virtual ~BinaryPathHandlerCnc();
		
		virtual bool processCommand(const unsigned char* buffer, int nbBytes);
};

#endif