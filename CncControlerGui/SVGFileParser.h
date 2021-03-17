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
		
	public:
		enum Mode {
			CNV_PRETTY_WITH_CNC, 
			CNV_PRETTY_WITHOUT_CNC, 
			CNV_COMPACT_WITH_CNC, 
			CNV_COMPACT_WITHOUT_CNC 
		};
		
		SVGFileFormatter(const wxString& ifn);
		
		bool convert(SVGFileFormatter::Mode m, const wxString& ofn);
		
		bool format(const wxString& ofn);
		bool compact(const wxString& ofn, bool rmvCncTags = false);
		
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
		
		bool processXMLNode(wxXmlNode* node);
		bool processCncParameter(wxXmlNode* node);
		bool resetCncParameters(wxXmlNode* node);
		bool printCncParameters(wxXmlNode* node);
		bool evaluateCncParameters(wxXmlNode* node);
		bool evaluateCncVaribales(wxXmlNode* node);
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
		
		virtual bool shouldAToolChangeProcessed()								{ return (pathHandler != NULL ? pathHandler->shouldAToolChangeProcessed() : false); }

	public:
		SVGFileParser(const wxString& fn, CncControl* cnc);
		virtual ~SVGFileParser();
		
		virtual void setPathHandler(PathHandlerBase* ph);
		
		virtual void deligateTrigger(const Trigger::BeginRun& tr)				{ pathHandler->deligateTrigger(tr); }
		virtual void deligateTrigger(const Trigger::EndRun& tr)					{ pathHandler->deligateTrigger(tr); }
		virtual void changePathListRunnerInterface(const wxString& portName)	{ pathHandler->changePathListRunnerInterface(portName); }

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
		
};

#endif
