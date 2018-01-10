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
		
		void reset();
		void setupToolId(int id);
		void addMove(const ManuallyPathHandlerCnc::MoveDefinition& md);
		
	protected:
		ManuallyPathHandlerCnc* pathHandler;
		
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