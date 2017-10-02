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
class wxDataViewListCtrl;

/////////////////////////////////////////////////////////////////////////////
class SVGFileParser : public SVGNodeParser, public FileParser {
	

	
	protected:

	
		/////////////////////////////////////////////////////////////////////
		bool cncNodeBreak;
		SVGPathHandlerCnc* pathHandler;
		SVGUserAgent svgUserAgent;

		wxXmlDocument svgTrace;

		wxXmlNode* debugBase;
		wxXmlNode* debugPath;
		wxXmlNode* debugDetail;
		
		bool setSVGWH(wxString w, wxString h);
		bool setSVGViewBox(wxString vb);
		
		SVGUnit determineUnit (wxString uw, wxString uh);

		virtual long getCurrentLineNumber();
		virtual bool addPathElement(char c, unsigned int count, double values[]);
		
		bool processXMLNode(wxXmlNode *child);
		bool evaluateCncParameters(wxXmlNode *child);
		void evaluateUse(wxXmlAttribute *attribute, DoubleStringMap& dsm);

		virtual bool preprocess();
		virtual bool process();
		bool spool();
		
		virtual void evaluateDebugState(bool force = false);
		virtual bool shouldStop();
		bool checkIfBreakpointIsActive();
		void initNextRunPhase(FileParserRunInfo::RunPhase p);
		void freezeDebugControls(bool freeze);
		void clearDebugControlBase();
		
		void clearDebugControlDetail();
		void appendDebugValue(wxDataViewListCtrl* ctl, const char* key, wxVariant value);
		void appendDebugValue(wxDataViewListCtrl* ctl, DcmItemList& rows);
		void appendDebugValueDetail(const char* key, wxVariant value);
		
		virtual void clearDebugControlPath();
		virtual void appendDebugValuePath(const char* key, wxVariant value);
		virtual void appendDebugValueBase(const char* key, wxVariant value);
		
		void appendDebugValueBase(DcmItemList& rows);
		void appendDebugValuePath(DcmItemList& rows);
		void appendDebugValueDetail(DcmItemList& rows);
		void appendDebugValuesToTrace(wxDataViewListCtrl* ctl, wxXmlNode* node);
		
		virtual void initNextPath(const wxString& data);

		void initXMLNode(wxXmlNode *child);
		void debugXMLNode(wxXmlNode *child);
		void debugXMLAttribute(wxXmlAttribute *attribute, wxString& attrString);
		
		inline bool performPath(SVGUserAgentInfo& uai);
		inline bool performPathByIds(SVGUserAgentInfo& uai);
		inline bool performTransform(SVGUserAgentInfo& uai);
		
		inline bool performUse(SVGUserAgentInfo& uai, UseDirective& ud);
		
		inline bool spoolPath(SVGUserAgentInfo& uai, const wxString& transform = "");
		
		void createSvgTraceRoot();
		inline bool collectUserAgentTrace();
		
		inline const char* convertToXmlString(wxString& value);

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