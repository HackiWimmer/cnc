#ifndef CncWorkingParameters_H
#define CncWorkingParameters_H

#include <map>
#include <inttypes.h>
#include <wx/xml/xml.h>
#include "CncCommon.h"
#include "SvgColourScheme.h"
#include "DataControlModel.h"

//////////////////////////////////////////////////////////////////
class SvgCncContextBase {
	
	public:
		enum Mode 
		{ 
			Update, 
			Delete 
		};
		
		enum CLIENT_ID_OFFSET
		{ 
			MAIN				= 0, 
			START_POS			= 1, 
			Z_TO_LOGICAL_TOP	= 2,
			Z_TO_SUREFACE		= 3,
			Z_NEXT_STEP_DOWN	= 4,
			
			FINALIZE_PATH		= 9
		};
		
	private:
		
		long			currentLineNumber;
		
	protected:
		
		typedef std::map<wxString, wxString> ParameterMap;
		
		const char*		RESET_Modifyer			= "${RESET}";
		const char*		DEL_Modifyer			= "${DELETE}";
		
		ParameterMap	parameterMap;
		
		SvgCncContextBase();
		explicit SvgCncContextBase(const SvgCncContextBase& scc);
		
		SvgCncContextBase& operator= (const SvgCncContextBase& scp);
		
		const wxVariant		getParameter(const wxString& key, const char* def = NULL) const;
		bool				deleteParameter(const wxString& key);
		
		virtual void		reconstruct();
		virtual void		manageParameter(const Mode mode, const wxString& name, const wxString& value) = 0;
		
		const ParameterMap&	getParameterMap() const { return parameterMap; }
		
	public:
		virtual ~SvgCncContextBase();
		
		void reset();
		
		void			setCurrentLineNumber(long ln);
		long			getCurrentLineNumber()												const	{ return currentLineNumber; }
		long			getCurrentClientID(CLIENT_ID_OFFSET o = CLIENT_ID_OFFSET::MAIN)		const;
		
		bool			provide(const wxString& name, const wxString& value);
		bool			provide(const wxXmlAttribute* attribute);
		bool			provide(const wxXmlNode *child);
		
		bool			hasParameter(const wxString& key)									const;
		const wxString 	getParameterValue(const wxString& key)								const;
		
		bool			convertToBool(const wxString& key)									const;
		
		bool			getParameterAsBool	(const wxString& key, const char* def = NULL)	const	{ return getParameter(key).GetBool(); }
		char			getParameterAsChar	(const wxString& key, const char* def = NULL)	const	{ return getParameter(key).GetChar(); }
		int				getParameterAsInt	(const wxString& key, const char* def = NULL)	const	{ return getParameter(key).GetInteger(); }
		double			getParameterAsDouble(const wxString& key, const char* def = NULL)	const	{ return getParameter(key).GetDouble(); }
		const wxString	getParameterAsString(const wxString& key, const char* def = NULL)	const	{ return getParameterValue(key); }
		
		virtual void traceTo(std::ostream& o, unsigned int indent=0) const;
};

//////////////////////////////////////////////////////////////////
class SvgCncBreak : public SvgCncContextBase {
	
	protected:
		virtual void manageParameter(const Mode mode, const wxString& name, const wxString& value) {}
	
	public:
		SvgCncBreak() 
		: SvgCncContextBase() 
		{}
		
		explicit SvgCncBreak(const SvgCncBreak& scb) 
		: SvgCncContextBase	(scb) 
		{}
		
		virtual ~SvgCncBreak()
		{}
		
		SvgCncBreak& operator= (const SvgCncBreak& scb) { 
			SvgCncContextBase::operator= (scb);
			
			return *this;
		}
};

//////////////////////////////////////////////////////////////////
class SvgCncPause : public SvgCncContextBase {
	
	protected:
		virtual void manageParameter(const Mode mode, const wxString& name, const wxString& value) {}
	
	public:
		SvgCncPause() 
		: SvgCncContextBase() 
		{}
		
		explicit SvgCncPause(const SvgCncPause& scp) 
		: SvgCncContextBase	(scp) 
		, microseconds		(scp.microseconds)
		{}
		
		virtual ~SvgCncPause()
		{}
		
		SvgCncPause& operator= (const SvgCncPause& scp) {
			SvgCncContextBase::operator= (scp);
			
			microseconds = scp.microseconds;
			return *this;
		}
		
		virtual void traceTo(std::ostream& o, unsigned int indent=0) const {
			const wxString prefix(' ', (int)indent);
			const unsigned int fillTo = 21;
			wxString name;
				
			name.assign("Delay");
			o << wxString::Format("%s  --> %s%s = %lld\n", prefix, name, wxString(' ', fillTo - name.length()), (long long)microseconds);
		}
		
		int64_t microseconds = 0LL;
};

//////////////////////////////////////////////////////////////////
class SvgCncContext : public SvgCncContextBase {
	
	public:
		struct Tool {
			double diameter		= 0.0;
			
			wxString trace() const {
				return wxString::Format("{%.3lf}", diameter);
			}
		};
		
		typedef std::map<wxString, Tool> ToolList;
		
	protected:
		
		bool					guidePath;
		bool					isUseColouScheme;
		
		double					currentZMaxFeedStep;
		double					currentZDepth;
		char					currentZDepthMode;
		
		double					currentRapidSpeed_MM_MIN; 
		double					currentWorkSpeed_MM_MIN; 
		
		ToolList				toolList;
		CncPathModificationType	pathModification;
		CncPathRuleType			pathRule;
		SvgColourDecoder		fillColourDecoder;
		SvgColourDecoder		strokeColourDecoder;
		
		void					setToolList(const wxString& list);
		void					reconstructToolList();
		void					addTool(const wxString& toolString);
		void					deleteTool(const wxString& id);
		
		void					setCurrentSpeed(const char type, const wxString& parameter);
		void					setCurrentZFeedStep(const wxString& parameter);
		void					setCurrentZDepth(const wxString& parameter);
		
		bool					checkToolExists(const wxString& id) const;
		virtual void			reconstruct();
		virtual void			manageParameter(const Mode mode, const wxString& name, const wxString& value);
		
	public:
		
		const char * 			ID_DEFAULT_TOOL_ID	= "T_000";
		
		const char * 			ID_COLOUR_SCHEME	= "UseColourScheme";
		
		const char * 			ID_PATH_MODE		= "PathModification";
		const char * 			ID_PATH_RULE		= "PathRule";
		
		const char * 			ID_TOOL_LIST		= "ToolList";
		const char * 			ID_TOOL_ADD			= "ToolAdd";
		const char * 			ID_TOOL_DEL			= "ToolDelete";
		
		const char * 			ID_TOOL_SEL			= "ToolSelect";
		
		const char * 			ID_RAPID_SPEED		= "RapidSpeed";
		const char * 			ID_WORK_SPEED		= "WorkSpeed";
		
		const char * 			ID_DEPT				= "ZDepth";
		const char * 			ID_MAX_FEED_STEP	= "ZMaxFeedStep";

		SvgCncContext();
		explicit SvgCncContext(const SvgCncContext& scc);
		
		virtual ~SvgCncContext();
		
		SvgCncContext& operator= (const SvgCncContext&);
		
		const ToolList&			getToolList()								const	{ return toolList; }
		
		CncPathModificationType	getPathModificationType()					const	{ return pathModification; }
		const char* 			getPathModificationTypeAsStr()				const;
		
		CncPathRuleType			getPathRuleType()							const	{ return pathRule; }
		const char* 			getPathRuleTypeAsStr()						const;
		
		double					getToolDiameter(const wxString& id)			const;
		double					getCurrentToolDiameter()					const	{ return getToolDiameter(getParameterAsString(ID_TOOL_SEL)); }
		wxString				getCurrentToolId()							const	{ return getParameterAsString(ID_TOOL_SEL); }
		
		bool					useColourScheme()							const	{ return isUseColouScheme; }
		bool					hasPathModifications()						const;
		bool					hasPathRules()								const;
		
		bool					isGuidePath()								const	{ return guidePath; }
		bool					isCurrentZDepthValid()						const	{ return isCurrentZDepthAbs() || isCurrentZDepthRel(); }
		bool					isCurrentZDepthAbs()						const	{ return currentZDepthMode == 'Z'; }
		bool					isCurrentZDepthRel()						const	{ return currentZDepthMode == 'z'; }
		bool					isCurrentZFeedStepToStrong()				const;
		
		double					getCurrentSpeed_MM_MIN(CncSpeedMode m)		const;
		double					getCurrentRapidSpeed_MM_MIN()				const	{ return currentRapidSpeed_MM_MIN; }
		double					getCurrentWorkSpeed_MM_MIN()				const	{ return currentWorkSpeed_MM_MIN; }
		
		char					getCurrentZDepthMode()						const	{ return currentZDepthMode; }
		double					getCurrentZDepth() 							const	{ return currentZDepth; }
		const char* 			getCurrentZDeptAsStr()						const;
		double					getCurrentZMaxFeedStep()					const	{ return currentZMaxFeedStep; } 
		void					getParameterList(DcmItemList& rows)			const;
		
		void					setFillColour(const wxColour & col);
		void					setStrokeColour(const wxColour & col);
		void					determineColourEffects();
		
		virtual void			traceTo(std::ostream& o, unsigned int indent=0) const;
		
		static 
		const std::ostream&		provideUsage(std::ostream& o, unsigned int indent=0);
};


//////////////////////////////////////////////////////////////////
class SvgCncContextSummary  : public SvgCncContext {
	
	private:
		
		unsigned int			participants;
		unsigned int			toolSelections;
		wxString				curToolId;
		wxString				toolSelectionList;
		double					curZDept;
		double					minZDept;
		
	public:
		SvgCncContextSummary();
		
		void add(const SvgCncContext& c);
		
		double					getMinZDepth()				const	{ return minZDept; }
		unsigned int			getParticipantCount()		const	{ return participants; }
		unsigned int			getToolCount()				const	{ return toolList.size(); }
		unsigned int			getToolSeletionCount()		const	{ return toolSelections; }
		const wxString&			getToolSelList()			const	{ return toolSelectionList; }
		wxString				getToolTotList()			const;
		
		void traceTo(std::ostream& o, unsigned int indent) const;
};

#endif