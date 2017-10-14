#ifndef GCODE_FILE_PREVIEW_H
#define GCODE_FILE_PREVIEW_H

#include <fstream>
#include <sstream>
#include <vector>
#include <wx/defs.h>
#include <wx/string.h>
#include "GCodePathHandlerCnc.h"
#include "FileParser.h"
#include "GCodeCommands.h"
#include "GCodeBlock.h"
#include "GCodePath.h"
#include "CncControl.h"

class wxXmlNode;
class wxXmlAttribute;
class CncGCodePreview;

/////////////////////////////////////////////////////////////////////
class GCodeFileParser : public FileParser {
	
	protected:
		GCodePathHandlerBase* pathHandler;
		bool programEnd;
		bool displayWarnings;
		bool resumeOnError;
		
		// error message handling
		inline bool displayMessage(const wxString& msg, int type);
		bool displayMessage(std::stringstream& ss, int type);
		
		inline bool displayError(const wxString& msg) { std::stringstream ss; ss << msg.c_str(); return displayMessage(ss, wxICON_ERROR); }
		inline bool displayWarnig(const wxString& msg) { std::stringstream ss; ss << msg.c_str(); return displayMessage(ss, wxICON_WARNING); }
		inline bool displayInfo(const wxString& msg) { std::stringstream ss; ss << msg.c_str(); return displayMessage(ss, wxICON_INFORMATION); }
		
		inline bool displayUnsupportedBlockCommand(const GCodeField& gcb);
		inline bool displayUnhandledBlockCommand(GCodeBlock& gcb);
		inline bool displayUnhandledParameter(const GCodeField& field);
		inline bool displayToolChangeDetected(const GCodeField& field);
		
		double getDefaultWidth()  { return 1000.0; }
		double getDefaultHeight() { return 1000.0; }
		const char* getDefaultUnit() { return "mm"; }
		
		void setDefaultParameters();
		bool processBlock(wxString& block, GCodeBlock& gcb);
		bool processField(const GCodeField& field, GCodeBlock& gcb);
		bool performBlock(GCodeBlock&gcb);
		
		bool processG(GCodeBlock& gcb);
		bool processM(GCodeBlock& gcb);
		//....
		
		virtual bool preprocess();
		virtual bool spool();
		virtual void initNextClientId(long id);

	public:
		GCodeFileParser(const wxString& fn, GCodePathHandlerBase* ph);
		virtual ~GCodeFileParser();
		
		void setDisplayWarnings(bool state) { displayWarnings = state; }
};


#endif