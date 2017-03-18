#ifndef SVG_FILE_PARSER_H
#define SVG_FILE_PARSER_H

#include <iostream>
#include "SVGPathHandler.h" 
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

class SVGFileParser {
	
	public:
		/////////////////////////////////////////////////////////////////////
		struct SvgDebugControls {
			wxStaticText*			debugPhase 				= NULL;
			wxDataViewListCtrl* 	debuggerControlBase 	= NULL;
			wxDataViewListCtrl* 	debuggerControlPath 	= NULL;
			wxDataViewListCtrl* 	debuggerControlDetail 	= NULL;
			wxMenuItem*				debugPreprocessing 		= NULL;
			wxMenuItem*				debugUserAgent			= NULL;
			wxMenuItem*				debugSpooling			= NULL;
		};
	
	protected:
		/////////////////////////////////////////////////////////////////////
		class SvgRunInfo {
			
			public:
				///////////////////////////////////////////////////////////////
				enum SvgRunPhase {Svg_RP_Unknown=0, Svg_RP_Preprocesser=1, Svg_RP_UserAgent=2,  Svg_RP_Spool=3, Svg_RP_LAST=4};
				
			private:
				SvgRunPhase runPhase;
				SvgRunPhase finalizeAfter;
				int lastLineNumber;
				bool state;
				bool stop;
				bool next;
				bool pause;
				
				bool debugMode[Svg_RP_LAST];
				
			public:
				///////////////////////////////////////////////////////////////
				SvgRunInfo() 
				: runPhase(Svg_RP_Unknown)
				, finalizeAfter(Svg_RP_LAST)
				, lastLineNumber(UNDEFINED_LINE_NUMBER)
				, state(false)
				, stop(false)
				, next(false)
				, pause(false)
				{
					for ( unsigned int i=0; i<Svg_RP_LAST; i++)
						debugMode[i] = false;
				}
				///////////////////////////////////////////////////////////////
				~SvgRunInfo() {
				}
				
				///////////////////////////////////////////////////////////////
				void stopProcessingAfter(SvgRunPhase fa) {
					finalizeAfter = fa;
				}
				
				bool processMore() {
					if ( runPhase >= finalizeAfter ) {
						std::clog << "Run would be finalized after step: " << getCurrentDebugPhaseAsString() << std::endl;
						return false;
					}
						
					return true;
				}
				
				///////////////////////////////////////////////////////////////
				bool getCurrentDebugMode() { return debugMode[runPhase]; }
				
				void setDebugMode(SvgRunPhase p, bool mode) {
					if ( p == Svg_RP_Unknown)
						return;
						
					if ( p == Svg_RP_LAST)
						return;
						
					debugMode[p] = mode;
				}
				///////////////////////////////////////////////////////////////
				void debugAllPhases() {
					for ( unsigned int i=1; i<Svg_RP_LAST; i++)
						debugMode[i] = true;
				}
				///////////////////////////////////////////////////////////////
				void releaseAllPhases() {
					for ( unsigned int i=1; i<Svg_RP_LAST; i++)
						debugMode[i] = false;
				}
				///////////////////////////////////////////////////////////////
				void setCurrentRunPhase(SvgRunPhase p) { 
					if ( p == Svg_RP_LAST ) runPhase = Svg_RP_Unknown;
					else					runPhase = p; 
					
					setCurrentDebugState(debugMode[runPhase]);
				}
				SvgRunPhase getCurrentRunPhase() { return runPhase; }
				
				///////////////////////////////////////////////////////////////
				void setCurrentDebugState(bool s) { state = s; }
				bool getCurrentDebugState() { return state; }
				
				///////////////////////////////////////////////////////////////
				void setStopFlag(bool s) { stop = s; }
				bool getStopFlag() { return stop; }
				
				///////////////////////////////////////////////////////////////
				void setNextFlag(bool n) { next = n; }
				bool getNextFlag() { return next; }
				
				///////////////////////////////////////////////////////////////
				void setPauseFlag(bool p) { pause = p; }
				bool getPauseFlag() { return pause; }
				///////////////////////////////////////////////////////////////
				int getLastLineNumber() { return lastLineNumber; }
				void setLastLineNumber(int ln) { lastLineNumber = ln; }
				
				///////////////////////////////////////////////////////////////
				const char* getCurrentDebugPhaseAsString() {
					switch ( runPhase) {
						case Svg_RP_Preprocesser: 	return "SVG Preprocessor";
						case Svg_RP_UserAgent:		return "SVG User Agent";
						case Svg_RP_Spool:			return "SVG Spooling";
						default:					return "";
					}
				}
				
				////////////////////////////////////////////////////////////////
				const char* getDebugModeAsString() {
					if ( getCurrentDebugMode() == true ) 	return "Debug Mode";
					else									return "Release Mode";
				}
		};
	
		/////////////////////////////////////////////////////////////////////
		CncControl* cncControl;
		SVGPathHandlerCnc* pathHandler;
		SVGUserAgent svgUserAgent;
		wxString fileName;

		wxXmlDocument svgTrace;
		wxWebView* svgTraceControl;
		
		wxXmlNode* debugBase;
		wxXmlNode* debugPath;
		wxXmlNode* debugDetail;
		
		wxStyledTextCtrl* svgSourceControl;
	
		bool setSVGWH(wxString w, wxString h);
		bool setSVGViewBox(wxString vb);
		
		SvgRunInfo runInfo;
		SvgDebugControls debugControls;

		SVGUnit determineUnit (wxString uw, wxString uh);

		//<circle>, <ellipse>, <line>, <polygon>, <polyline>, <rect>
		bool convertCircleToPathData(wxXmlNode* child, wxString ret); 
		bool convertEllipseToPathData(wxXmlNode* child, wxString ret); 
		bool convertLineToPathData(wxXmlNode* child, wxString ret); 
		bool convertPolygonToPathData(wxXmlNode* child, wxString ret); 
		bool convertPolylineToPathData(wxXmlNode* child, wxString ret); 
		bool convertRectToPathData(wxXmlNode* child, wxString ret); 

		wxString& evaluatePolyPoints(wxString points, wxString& path);
		inline int getCommandParaCount(char c);
		
		bool processXMLNode(wxXmlNode *child);
		bool processPathCommand(wxString para);
		bool evaluatePath(wxString data);
		bool evaluateCncParameters(wxXmlNode *child);
		void evaluateUse(wxXmlAttribute *attribute, DoubleStringMap& dsm);

		virtual bool preprocess();
		bool process();
		bool spool();
		
		void evaluateDebugState(bool force = false);
		bool checkIfBreakpointIsActive();
		void initNextRunPhase(SvgRunInfo::SvgRunPhase p);
		void freezeDebugControls(bool freeze);
		void clearDebugControlBase();
		void clearDebugControlPath();
		void clearDebugControlDetail();
		void appendDebugValue(wxDataViewListCtrl* ctl, const char* key, wxVariant value);
		void appendDebugValue(wxDataViewListCtrl* ctl, DcmItemList& rows);
		void appendDebugValueBase(const char* key, wxVariant value);
		void appendDebugValuePath(const char* key, wxVariant value);
		void appendDebugValueDetail(const char* key, wxVariant value);
		void appendDebugValueBase(DcmItemList& rows);
		void appendDebugValuePath(DcmItemList& rows);
		void appendDebugValueDetail(DcmItemList& rows);
		void appendDebugValuesToTrace(wxDataViewListCtrl* ctl, wxXmlNode* node);

		void debugXMLNode(wxXmlNode *child);
		void debugXMLAttribute(wxXmlAttribute *attribute, wxString& attrString);
		
		void selectSourceControl(unsigned long pos);
		
		inline bool performPath(SVGUserAgentInfo& uai);
		inline bool performPathByIds(SVGUserAgentInfo& uai);
		inline bool performTransform(SVGUserAgentInfo& uai);
		
		inline bool performUse(SVGUserAgentInfo& uai, UseDirective& ud);
		
		inline bool spoolPath(SVGUserAgentInfo& uai, const wxString& transform = "");
		
		void createSvgTraceRoot();
		inline bool collectUserAgentTrace();
		
		inline const char* convertToXmlString(wxString& value);

	public:
		SVGFileParser(const char* fn, CncControl* cnc);
		virtual ~SVGFileParser();
		
		virtual bool createPreview(const wxString& resultingFileName, bool withErrorInfo);
		
		SVGPathHandlerCnc* getPathHandler() { return pathHandler; }
		
		bool processRelease();
		bool processDebug();
		
		bool pause();
		bool isPause() { return runInfo.getPauseFlag(); }
		
		// Debug handling
		void debugNextPath();
		void debugNextStep();
		void debugStop();
		void debugFinish();
		void setSvgSoureControl(wxStyledTextCtrl* stc) { svgSourceControl = stc; }
		void setSvgTraceControl(wxWebView* wv) 		   { svgTraceControl = wv; }
		
		void setDebuggerControls(SvgDebugControls& dc) { 
			debugControls = dc; 
			
			if ( pathHandler != NULL )
				pathHandler->setDebuggerControl(dc.debuggerControlDetail);
		}
		
		// User Agent handling
		void clearControls();
		
		void setUserAgentControls(SVGUserAgentOutputControls& oc) {
			svgUserAgent.setOutputControls(oc);
		}
		
		void displayUserAgentDetailInfo(unsigned int pos) {
			svgUserAgent.displayDetailInfo(pos);
		}
		
		void selectSourceControl(wxStyledTextCtrl* ctl, unsigned long pos);
		void displayCollectedTrace(bool blank = false);
		
};

#endif