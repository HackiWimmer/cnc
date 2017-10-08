#ifndef SVG_FILE_PARSER_H
#define SVG_FILE_PARSER_H

#include <iostream>
#include "FileParser.h"
#include "SVGNodeParser.h"
#include "SVGPathHandlerCnc.h" 
#include "SVGTransformMatrix.h"
#include "SVGUserAgent.h"

class wxString;
class wxXmlNode;
class wxStaticText;
class wxStyledTextCtrl;
class wxWebView;
class wxXmlAttribute;
class wxMenuItem;

/////////////////////////////////////////////////////////////////////////////
class SVGFileParser : public SVGNodeParser, public FileParser {
	
	protected:
	
		CncControl* cncControl;
		SVGPathHandlerCnc* pathHandler;
		SVGUserAgent svgUserAgent;
		bool cncNodeBreak;
		
		wxString currentNodeName;
		
		wxXmlNode* debugBase;
		wxXmlNode* debugPath;
		wxXmlNode* debugDetail;
		
		bool setSVGWH(wxString w, wxString h);
		bool setSVGViewBox(wxString vb);
		
		SVGUnit determineUnit (wxString uw, wxString uh);
		
		virtual bool preprocess();
		virtual void initNextRunPhase(FileParserRunInfo::RunPhase p);
		virtual void initNextPath(const wxString& data);
		virtual bool evaluateProcessingCallback();
		virtual bool isInterrupted() { return cncControl->isInterrupted(); }
		virtual void selectSourceControl(unsigned long pos);
		virtual bool addPathElement(char c, unsigned int count, double values[]);
		
		bool processXMLNode(wxXmlNode *child);
		bool evaluateCncParameters(wxXmlNode *child);
		void evaluateUse(wxXmlAttribute *attribute, DoubleStringMap& dsm);
		
		bool spool();
		
		void initXMLNode(wxXmlNode *child);
		void debugXMLNode(wxXmlNode *child);
		void debugXMLAttribute(wxXmlAttribute *attribute, wxString& attrString);
		
		inline bool performPath(SVGUserAgentInfo& uai);
		inline bool performPathByIds(SVGUserAgentInfo& uai);
		inline bool performTransform(SVGUserAgentInfo& uai);
		inline bool performUse(SVGUserAgentInfo& uai, UseDirective& ud);
		inline bool spoolPath(SVGUserAgentInfo& uai, const wxString& transform = "");

	public:
		SVGFileParser(const wxString& fn, CncControl* cnc);
		virtual ~SVGFileParser();
		
		virtual bool createPreview(const wxString& resultingFileName, bool withErrorInfo);
		
		
		virtual void setPathHandler(PathHandlerBase* ph);
		
		SVGPathHandlerCnc* getPathHandler() { return pathHandler; }
		
		virtual void broadcastDebugState(bool state);
		virtual void clearControls();
		
		virtual void broadcastDebugControls(DebugControls& dc) {
			if ( pathHandler != NULL )
				pathHandler->setDebuggerControl(dc.debuggerControlDetail);
		}
		
		virtual void setUserAgentControls(UserAgentOutputControls& oc) {
			SvgUserAgentOutputControls soc;
			soc.detailInfo 			= oc.detailInfo;
			soc.inboundPathList		= oc.inboundPathList;
			soc.useDirectiveList	= oc.useDirectiveList;
			svgUserAgent.setOutputControls(soc);
		}
		
		virtual void displayUserAgentDetailInfo(unsigned int pos) {
			svgUserAgent.displayDetailInfo(pos);
		}
		
};

#endif