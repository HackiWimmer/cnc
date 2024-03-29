#ifndef SVG_FILE_PARSER_H
#define SVG_FILE_PARSER_H

#include <iostream>
#include "CncControl.h"
#include "FileParser.h"
#include "SVGParserBase.h"
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

// -------------------------------------------------
class SVGFileFormatter 
{
	private:
		wxString	inboundFileName;
		
		bool removeCncTags(wxXmlNode* child);
		bool removeCncTags(wxXmlDocument& doc);
		
		bool removeSvgFormatAttributs(wxXmlNode* child);
		bool removeSvgFormatAttributs(wxXmlDocument& doc);

	public:
		
		struct Setup
		{
			bool compact	= false;
			bool keepCnc	= true;
			bool keepFormat	= false;
		};
		
		SVGFileFormatter(const wxString& ifn);
		
		bool convert(SVGFileFormatter::Setup s, const wxString& ofn);
		
		bool format(const wxString& ofn);
		bool compact(const wxString& ofn, bool rmvCncTags = false);
		
		bool removeSvgFormatAttributs(const wxString& ofn);
		bool removeCncTags(const wxString& ofn);
};

// -------------------------------------------------
class SVGFileParser : public SVGParserBase
                    , public FileParser 
{
	protected:
		
		CncControl* 		cncControl;
		SVGUserAgent 		svgUserAgent;
		
		wxString 			currentNodeName;
		
		wxXmlNode* 			debugBase;
		wxXmlNode* 			debugPath;
		wxXmlNode* 			debugDetail;
		
		bool setSVGRootNode(const wxString& w, const wxString& h, const wxString& vb);
		
		virtual bool preprocess() override;
		virtual bool postprocess() override;
		virtual void initNextRunPhase(CncProcessingInfo::RunPhase p) override;
		virtual void initNextPath(const wxString& data) override;
		virtual bool evaluateProcessingCallback() override;
		virtual bool addPathElement(char c, unsigned int count, double values[]) override;
		virtual void initNextClientId(long id) override;
		
		virtual void logMeasurementStart() override;
		virtual void logMeasurementEnd() override;
		
		bool processXMLNode(wxXmlNode* node);
		bool processCncParameter(wxXmlNode* node);
		void evaluateUse(wxXmlAttribute *attribute, DoubleStringMap& dsm);
		
		bool spool();
		
		void registerXMLNode(wxXmlNode *child);
		void debugXMLAttribute(wxXmlAttribute *attribute, wxString& attrString);
		void registerMovementNode();
		
		inline bool performPath(const SVGUserAgentInfo& uai);
		inline bool performPathByIds(const SVGUserAgentInfo& uai);
		inline bool prepareTransformMatrix(const SVGUserAgentInfo& uai);
		inline bool performUse(const SVGUserAgentInfo& uai, UseDirective& ud);
		inline bool spoolPath(const SVGUserAgentInfo& uai, const wxString& transform = "");
		
		virtual bool shouldAToolChangeProcessed() override { return (pathHandler != NULL ? pathHandler->shouldAToolChangeProcessed() : false); }

		static bool determineUnit(const wxString& uw, const wxString& uh, Unit& u);
		static SVGRootNode evaluateSVGRootNode(const wxString& w, const wxString& h, const wxString& vb);

	public:
		SVGFileParser(const wxString& fn, CncControl* cnc);
		virtual ~SVGFileParser();
		
		virtual void setPathHandler(PathHandlerBase* ph);
		
		virtual void deligateTrigger(const Trigger::BeginRun& tr)				{ wxASSERT(pathHandler); pathHandler->deligateTrigger(tr); }
		virtual void deligateTrigger(const Trigger::EndRun& tr)					{ wxASSERT(pathHandler); pathHandler->deligateTrigger(tr); }
		virtual void changePathListRunnerInterface(const wxString& portName)	{ wxASSERT(pathHandler); pathHandler->changePathListRunnerInterface(portName); }
		virtual void initCurrentPos(const CncDoublePosition& pos)				{ wxASSERT(pathHandler); pathHandler->initCurrentPos(pos); } 

		SVGPathHandlerCnc* getPathHandler() {
			
			SVGPathHandlerCnc* ret = static_cast<SVGPathHandlerCnc*>(pathHandler);
			wxASSERT( ret );
			return ret;
		}
		
		virtual void broadcastDebugState(bool state);
		virtual void clearControls();
		
		virtual void selectSourceControl(unsigned long pos);
		virtual void enableUserAgentControls(bool state);
		
		virtual void displayUserAgentDetailInfo(unsigned int pos)				{ svgUserAgent.displayDetailInfo(pos); }
		
		static bool evaluateMetricSize(const wxString& fileName, CncDoubleRectangle& rect);
		
};

#endif
