#ifndef SVG_PARSER_BASE_H
#define SVG_PARSER_BASE_H

#include <wx/variant.h>
#include "CncPathListEntry.h"
#include "SVGPathHandlerBase.h"

class SVGParserBase {
	
	protected:
		
		SVGPathHandlerBase* pathHandler;
		
		explicit SVGParserBase(SVGPathHandlerBase* ph);
		virtual ~SVGParserBase();

		bool evaluatePath(const wxString& data);
		bool processPathCommand(const wxString& para);
		int  getCommandParaCount(char c);
		
		virtual void initNextPath(const wxString& data) = 0;
		virtual bool addPathElement(char c, unsigned int count, double values[]) = 0;
		
		virtual bool evaluateProcessingCallback() { return true; }
};

#endif
