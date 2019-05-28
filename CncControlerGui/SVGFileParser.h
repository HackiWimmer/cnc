#ifndef SVG_FILE_PARSER_H
#define SVG_FILE_PARSER_H

#include <iostream>
#include "CncControl.h"
#include "FileParser.h"
#include "SVGNodeParser.h"
#include "SVGPathHandlerCnc.h" 
#include "SvgTransformMatrix.h"
#include "SVGUserAgent.h"

class wxString;
class wxXmlNode;
class wxStaticText;
class wxStyledTextCtrl;
class wxXmlAttribute;
class wxMenuItem;

typedef CncUnitCalculatorBase::Unit Unit;

/////////////////////////////////////////////////////////////////////////////
class SVGFileParser : public SVGNodeParser
                    , public FileParser 
{
	protected:
	
		CncControl* 		cncControl;
		SVGPathHandlerCnc* 	pathHandler;
		SVGUserAgent 		svgUserAgent;
		
		wxString 			currentNodeName;
		
		wxXmlNode* 			debugBase;
		wxXmlNode* 			debugPath;
		wxXmlNode* 			debugDetail;
		
		bool setSVGRootNode(const wxString& w, const wxString& h, const wxString& vb);
		bool determineUnit (const wxString& uw, const wxString& uh, Unit& u);
		
		virtual bool preprocess();
		virtual bool postprocess();
		virtual void initNextRunPhase(FileParserRunInfo::RunPhase p);
		virtual void initNextPath(const wxString& data);
		virtual bool evaluateProcessingCallback();
		virtual bool isInterrupted() { return cncControl->isInterrupted(); }
		virtual bool addPathElement(char c, unsigned int count, double values[]);
		virtual void initNextClientId(long id);
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		
		bool processXMLNode(wxXmlNode *child);
		bool evaluateCncParameters(wxXmlNode *child);
		void evaluateUse(wxXmlAttribute *attribute, DoubleStringMap& dsm);
		
		bool spool();
		
		void initXMLNode(wxXmlNode *child);
		void registerXMLNode(wxXmlNode *child);
		void debugXMLAttribute(wxXmlAttribute *attribute, wxString& attrString);
		
		inline bool performPath(SVGUserAgentInfo& uai);
		inline bool performPathByIds(SVGUserAgentInfo& uai);
		inline bool prepareTransformMatrix(SVGUserAgentInfo& uai);
		inline bool performUse(SVGUserAgentInfo& uai, UseDirective& ud);
		inline bool spoolPath(SVGUserAgentInfo& uai, const wxString& transform = "");
		
		virtual bool shouldAToolChangeProcessed() { return (pathHandler != NULL ? pathHandler->shouldAToolChangeProcessed() : false); }

	public:
		SVGFileParser(const wxString& fn, CncControl* cnc);
		virtual ~SVGFileParser();
		
		virtual void setPathHandler(PathHandlerBase* ph);
		SVGPathHandlerCnc* getPathHandler() { return pathHandler; }
		
		virtual void broadcastDebugState(bool state);
		virtual void clearControls();
		
		virtual void selectSourceControl(unsigned long pos);
		virtual void enableUserAgentControls(bool state);
		
		virtual void displayUserAgentDetailInfo(unsigned int pos) {
			svgUserAgent.displayDetailInfo(pos);
		}
		
};

#endif
