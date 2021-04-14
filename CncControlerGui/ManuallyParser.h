#ifndef MANUALLY_FILE_PREVIEW_H
#define MANUALLY_FILE_PREVIEW_H

#include <ostream>
#include <vector>
#include "CncCommon.h"
#include "ManuallyPathHandlerCnc.h"
#include "FileParser.h"
#include "CncControl.h"

/////////////////////////////////////////////////////////////////////
class ManuallyParser : public FileParser {
	
	public:
		ManuallyParser(ManuallyPathHandlerCnc* ph);
		virtual ~ManuallyParser();
		
		virtual void deligateTrigger(const Trigger::BeginRun& tr)				{ wxASSERT(pathHandler); pathHandler->deligateTrigger(tr); }
		virtual void deligateTrigger(const Trigger::EndRun& tr)					{ wxASSERT(pathHandler); pathHandler->deligateTrigger(tr); }
		virtual void changePathListRunnerInterface(const wxString& portName)	{ wxASSERT(pathHandler); pathHandler->changePathListRunnerInterface(portName); }
		virtual void initCurrentPos(const CncDoublePosition& pos)				{ wxASSERT(pathHandler); pathHandler->initCurrentPos(pos); } 
			
		void reset();
		void reset(const CncDoublePosition& pos);
		void setupToolId(int id);
		void addMove(const ManuallyPathHandlerCnc::MoveDefinition& md);
		
	protected:
		ManuallyPathHandlerCnc* pathHandler;
		
		virtual void initNextClientId(long id);
		virtual bool preprocess();
		virtual bool spool();
		virtual bool postprocess();
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		
		virtual bool shouldAToolChangeProcessed() { return (pathHandler != NULL ? pathHandler->shouldAToolChangeProcessed() : false); }
		
	private:
		typedef std::vector<ManuallyPathHandlerCnc::MoveDefinition> Moves;
		
		Moves moves;
		int toolId;
};


#endif