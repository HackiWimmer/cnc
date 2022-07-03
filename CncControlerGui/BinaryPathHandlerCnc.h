#ifndef BIN_PARSE_HANDLER_CNC_H
#define BIN_PARSE_HANDLER_CNC_H

#include "CncControl.h"
#include "CncPathListRunner.h"
#include "BinaryPathHandlerBase.h"

class BinaryPathHandlerCnc	: public BinaryPathHandlerBase 
							, public CncPathListRunner
{
	protected:
		CncControl* cncControl;
		
		virtual void initNextClientId(long id)									override;
		virtual void logMeasurementStart()										override;
		virtual void logMeasurementEnd()										override;
		
	public:
		BinaryPathHandlerCnc(CncControl* cnc);
		virtual ~BinaryPathHandlerCnc();
		
		virtual bool processCommand(const unsigned char* buffer, int nbBytes)	override;
		
		virtual void traceWorkflow(std::ostream& o)								override { CncPathListRunner::traceWorkflow(o); }
		virtual void resetWorkflow()											override { CncPathListRunner::resetWorkflow(); }
		virtual bool spoolWorkflow()											override { return CncPathListRunner::spoolWorkflow(); }

		virtual void changePathListRunnerInterface(const wxString& portName)	override { changePathListRunnerInterfaceImpl(portName); }
		
		virtual void deligateTrigger(const Trigger::BeginRun& tr)				override { processTrigger(tr); }
		virtual void deligateTrigger(const Trigger::EndRun& tr)					override { processTrigger(tr); }
};

#endif