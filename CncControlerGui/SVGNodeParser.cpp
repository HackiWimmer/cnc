#include "SVGPathHandlerBase.h"
#include "SVGElementConverter.h"
#include "SVGNodeParser.h"

//////////////////////////////////////////////////////////////////
SVGNodeParser::SVGNodeParser()
: SVGParserBase		(new SVGPathHandlerBase())
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SVGNodeParser::~SVGNodeParser() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
bool SVGNodeParser::addPathElement(char c, unsigned int count, double values[]) {
//////////////////////////////////////////////////////////////////
	if ( pathHandler == NULL ) {
		std::cerr << CNC_LOG_FUNCT << ": Failed: Member pathHandler is NULL " << std::endl;
		return false;
	}
	
	// todo return pathHandler-> ....
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGNodeParser::processSvgNode(const wxString& node) {
//////////////////////////////////////////////////////////////////
	if ( pathHandler == NULL ) {
		std::cerr << CNC_LOG_FUNCT << ": Failed: Member pathHandler is NULL " << std::endl;
		return false;
	}

	pathHandler->initNextPath();

	static wxString path;
	path.clear();
	
	SvgNodeAttributeMap aMap;
	if ( SVGElementConverter::convertNodeToPathData(node, path, aMap) ) {

		// reset transform
		SVGTransformMatrix& tm = pathHandler->getSvgTransformMatrix();
		tm.unchanged();
		
		// perform transform
		SvgNodeAttributeMap::iterator it = aMap.find("transform");
		if ( it != aMap.end() ) {
			if ( tm.performTransformAsStringList(it->second) == false )
				return false;
		}

		// perform path
		bool ret = evaluatePath(path);

		pathHandler->finishCurrentPath();
		return ret;
	}

	std::cerr << CNC_LOG_FUNCT << ": Failed: "			<< std::endl;
	std::cerr << SVGElementConverter::getErrorInfo() 	<< std::endl;
	std::cerr << "Node: " << node 						<< std::endl;
	SVGElementConverter::resetErrorInfo();
	
	return false;
}
