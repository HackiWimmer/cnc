#ifndef SVG_PARSER_H
#define SVG_PARSER_H

#include <wx/variant.h>
#include "CncPathListEntry.h"
#include "SVGPathHandlerBase.h"

/////////////////////////////////////////////////////////////////////////////
class SVGNodeParser {
	
	protected:
	
		SVGPathHandlerBase* pathHandler;
		
		bool evaluatePath(const wxString& data);
		bool processPathCommand(const wxString& para);
		int getCommandParaCount(char c);
		
		virtual bool addPathElement(char c, unsigned int count, double values[]);
		
		// dummies to support SVGFileParser calls
		virtual void appendDebugValuePath(const char* key, wxVariant value) {}
		virtual void appendDebugValueBase(const char* key, wxVariant value) {}
		virtual void clearDebugControlPath() {}
		virtual void evaluateDebugState(bool force = false) {}
		
		virtual void initNextPath(const wxString& data) {}
		
		virtual bool shouldStop() { return false; }
		virtual long getCurrentLineNumber() { return 1; }
	
	public:
	
		////////////////////////////////////////////////////////////////////
		SVGNodeParser();
		virtual ~SVGNodeParser();
		
		////////////////////////////////////////////////////////////////////
		virtual void setPathHandler(SVGPathHandlerBase* ph) { pathHandler = ph; }
		
		////////////////////////////////////////////////////////////////////
		bool processSvgNode(const wxString& node);
};

#endif