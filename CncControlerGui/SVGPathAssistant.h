#ifndef SVG_PATH_ASSISTANT_H
#define SVG_PATH_ASSISTANT_H

#include "SVGPathHandlerBase.h"
#include "CncConfig.h"
#include "SVGNodeParser.h"

///////////////////////////////////////////////////////////////////
class SVGPathAssistant {
	
	protected:
		
		SVGPathHandlerBase* pathHandler;
		SVGNodeParser parser;
		
		float prevCurveLibResoluton;
		
	public:
		////////////////////////////////////////////////////////////
		SVGPathAssistant()
		: pathHandler(new SVGPathHandlerBase())
		, parser(pathHandler)
		, prevCurveLibResoluton(CncConfig::getCurveLibIncrement())
		{
			pathHandler->setCurveLibResolution(CncConfig::getDefaultCurveLibResolution());
			pathHandler->prepareWork();
		}
		
		////////////////////////////////////////////////////////////
		~SVGPathAssistant() {
			CncConfig::setCurveLibIncrement(prevCurveLibResoluton);
		}
		
		void setCurveLibResolution(float res) { pathHandler->setCurveLibResolution(res); }
		
		void setPathList(const CncPathListManager& newPathList) { pathHandler->setPathList(newPathList); }
		const CncPathListManager& getPathList() { return pathHandler->getPathList(); }
		
		unsigned int getDataPointCount() const { return pathHandler->getDataPointCount(); }
		bool processSvgNode(const wxString& node) { return parser.processSvgNode(node); }

		void tracePathList(std::ostream &ostr) { pathHandler->tracePathList(ostr); }
		const char* getAsWktRepresentation() { return pathHandler->getAsWktRepresentation(); }
		const char* getAsSvgPathRepresentation(const wxString& style="stroke=\"#000000\" fill=\"none\"") { return pathHandler->getAsSvgPathRepresentation(style); }

		double getCurrentPathLength() { return pathHandler->getCurrentPathLength(); }
		double getTotalLength() { return pathHandler->getTotalLength(); }
		bool isPathClosed() { return pathHandler->isPathClosed(); }
		
		bool getCentroid(wxRealPoint& centroid) { return pathHandler->getCentroid(centroid); }
		
		const wxString& getWktTypeAsString() { return pathHandler->getWktTypeAsString(); }
		CncPathListManager::WktTypeInfo getWktType() { return pathHandler->getWktType(); }
		
		bool reversePath() { return pathHandler->reversePath(); }
		bool centerPath() { return pathHandler->centerPath(); }
	
};

#endif