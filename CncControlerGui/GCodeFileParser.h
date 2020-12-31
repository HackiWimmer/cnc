#ifndef GCODE_FILE_PARSER_H
#define GCODE_FILE_PARSER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <wx/defs.h>
#include <wx/string.h>
#include "GCodePathHandlerCnc.h"
#include "FileParser.h"
#include "GCodeCommands.h"
#include "GCodeBlock.h"
#include "CncControl.h"

class wxXmlNode;
class wxXmlAttribute;
class CncGCodePreview;

typedef std::vector<GCodeBlock> GCodeSequence;

/////////////////////////////////////////////////////////////////////
class GCodeFileParser : public FileParser {
	
	protected:
		GCodePathHandlerBase* pathHandler;
		GCodeSequence gCodeSequence;
		bool programEnd;
		bool displayWarnings;
		bool resumeOnError;
		
		// error message handling
		inline bool displayMessage(const wxString& msg, int type);
		bool displayMessage(std::stringstream& ss, int type);
		
		inline bool displayError(const wxString& msg) { std::stringstream ss; ss << msg.c_str(); return displayMessage(ss, wxICON_ERROR); }
		inline bool displayWarnig(const wxString& msg) { std::stringstream ss; ss << msg.c_str(); return displayMessage(ss, wxICON_WARNING); }
		inline bool displayInfo(const wxString& msg) { std::stringstream ss; ss << msg.c_str(); return displayMessage(ss, wxICON_INFORMATION); }
		
		inline bool displayUnsupportedBlockCommand(const GCodeField& gcb, const char* additionalInfo = NULL);
		inline bool displayUnhandledBlockCommand(GCodeBlock& gcb, const char* additionalInfo = NULL);
		inline bool displayUnhandledParameter(const GCodeField& field, const char* additionalInfo = NULL);
		
		double getDefaultWidth()  { return 1000.0; }
		double getDefaultHeight() { return 1000.0; }
		const char* getDefaultUnit() { return "mm"; }
		
		void setDefaultParameters();
		bool processBlock(wxString& block, GCodeBlock& gcb);
		bool processField(const GCodeField& field, GCodeBlock& gcb);
		bool prepareBlock(GCodeBlock&gcb);
		bool performBlock(GCodeBlock&gcb);
		
		bool processG(GCodeBlock& gcb);
		bool processM(GCodeBlock& gcb);
		//....
		
		virtual bool preprocess();
		virtual bool spool();
		virtual bool postprocess();
		virtual void initNextClientId(long id);
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		
		virtual bool shouldAToolChangeProcessed() { return (pathHandler != NULL ? pathHandler->shouldAToolChangeProcessed() : false); }
		
	public:
		GCodeFileParser(const wxString& fn, GCodePathHandlerBase* ph);
		virtual ~GCodeFileParser();
		
		virtual void deligateTrigger(const Trigger::BeginRun& tr)				{ pathHandler->deligateTrigger(tr); }
		virtual void deligateTrigger(const Trigger::EndRun& tr)					{ pathHandler->deligateTrigger(tr); }
		virtual void changePathListRunnerInterface(const wxString& portName)	{ pathHandler->changePathListRunnerInterface(portName); }

		void setDisplayWarnings(bool state) { displayWarnings = state; }
};


#endif