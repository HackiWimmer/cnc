#ifndef GCODE_FILE_PARSER_H
#define GCODE_FILE_PARSER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <wx/defs.h>
#include <wx/string.h>
#include "CncPosition.h"
#include "SVGFileParser.h"
#include "GCodeCommands.h"
#include "GCodeBlock.h"
#include "GCodePath.h"
#include "CncControl.h"

class wxXmlNode;
class wxXmlAttribute;

/////////////////////////////////////////////////////////////////////
struct GCodeFileParserMsgInfo {
	int type		= wxICON_INFORMATION;
	wxString msg	= wxT("");
};

/////////////////////////////////////////////////////////////////////
typedef std::vector<GCodeFileParserMsgInfo> UserMessages;

/////////////////////////////////////////////////////////////////////
class GCodeFileParser : public SVGFileParser {
	
	private:
		wxXmlNode* currentBlockNode;
		bool programEnd;
		bool previewMode;
		bool resumeOnError;
		CncDoublePosition curPxPos;
		GCodePath svgPath;
		
		std::fstream preview;
		UserMessages userMessages;
		
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
		
		// ...
		void createXmlBlockNode();
		template<typename T> inline void appendBlockAttribute(const char* key, T value, const char* format);
		wxXmlAttribute* getAttributes(GCodeBlock& gcb);
		
		inline void processSvgPath();
		inline void updateCurrentPxPosition(GCodeBlock& gcb);
		
		void startPreview();
		void streamErrorInfo();
		void endPreview();
		
		double getDefaultWidth()  { return 1000.0; }
		double getDefaultHeight() { return 1000.0; }
		const char* getDefaultUnit() { return "mm"; }
		
	protected:
		void setDefaultParameters();
		bool processBlock(wxString& block, GCodeBlock& gcb);
		bool processField(const GCodeField& field, GCodeBlock& gcb);
		bool performBlock(GCodeBlock&gcb);
		
		bool processRapidLinearMove(GCodeBlock& gcb);
		bool processLinearMove(GCodeBlock& gcb);
		bool processArcMove(GCodeBlock& gcb, bool sweep);
		
		bool processG(GCodeBlock& gcb);
		bool processM(GCodeBlock& gcb);
		//....
		
		// overridden from SVGFileParser
		virtual bool preprocess();

	public:
		GCodeFileParser(const char* fn, CncControl* cnc);
		virtual ~GCodeFileParser();
		
		// overridden from SVGFileParser
		virtual bool createPreview(const wxString& resultingFileName, bool withErrorInfo);
		
		
		virtual bool processRelease();
		virtual bool processDebug();
};

#endif