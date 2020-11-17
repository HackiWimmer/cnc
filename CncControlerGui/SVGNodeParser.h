#ifndef SVG_NODE_PARSER_H
#define SVG_NODE_PARSER_H

#include "SVGParserBase.h"

class SVGNodeParser : public SVGParserBase {
	
	protected:
		virtual bool addPathElement(char c, unsigned int count, double values[]);
		
	public:
		SVGNodeParser();
		virtual ~SVGNodeParser();
		
		bool processSvgNode(const wxString& node);
		
};

#endif