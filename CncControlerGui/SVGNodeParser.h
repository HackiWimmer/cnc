#ifndef SVG_PARSER_H
#define SVG_PARSER_H

#include <wx/variant.h>
#include "CncPathListEntry.h"
#include "SVGPathHandlerBase.h"

/////////////////////////////////////////////////////////////////////////////
class SVGNodeParser {
	
	private:
		// The path handler has to be compellingly declared private,
		// to hide it for all derived classes
		SVGPathHandlerBase* pathHandler;

	protected:
		
		bool evaluatePath(const wxString& data);
		bool processPathCommand(const wxString& para);
		int getCommandParaCount(char c);
		
		virtual bool addPathElement(char c, unsigned int count, double values[]);
		virtual void initNextPath(const wxString& data) {}
		virtual bool evaluateProcessingCallback() { return true; }
	
	public:
	
		////////////////////////////////////////////////////////////////////
		SVGNodeParser();
		explicit SVGNodeParser(SVGPathHandlerBase* ph);
		virtual ~SVGNodeParser();
		
		////////////////////////////////////////////////////////////////////
		bool processSvgNode(const wxString& node);
};

#endif
