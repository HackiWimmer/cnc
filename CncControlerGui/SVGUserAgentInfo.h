#ifndef SVG_USER_AGENT_INFO_H
#define SVG_USER_AGENT_INFO_H

#include <map>
#include <vector>
#include <string>
#include <wx/dataview.h>
#include <wx/xml/xml.h>
#include "CncCommon.h"
#include "SvgCncContext.h"

// ------------------------------------------------------------
typedef std::map<wxString, wxString>			DoubleStringMap;

// ------------------------------------------------------------
struct PathInfo 
{
	
	char			cmd;
	unsigned int	cnt;
	
	double values[MAX_PARAMETER_VALUES];
	
	PathInfo(char c, unsigned int i, double v[])
	: cmd	(c)
	, cnt	(i)
	{
		memcpy(values, v, sizeof(double) * MAX_PARAMETER_VALUES);
	}
};

typedef std::vector<PathInfo> 					PathInfoVector;
typedef std::map<wxString, DoubleStringMap>		UseDirectiveMap;

// ------------------------------------------------------------
struct UseDirective
{
	wxString 		id;
	DoubleStringMap	attributes;
};

typedef std::vector<UseDirective>				UseDirectiveVector;
typedef std::vector<wxString>					TransformVector;
typedef std::vector<wxString>					StyleVector;

////////////////////////////////////////////////////////////////
struct SVGUserAgentInfo
////////////////////////////////////////////////////////////////
{
	private:
		bool isMemberOf(const wxString& id, const char* type)	const;
		
	public:
		enum NodeType { NT_UNDEFINED, NT_PATH, NT_CNC_PARAM, NT_CNC_VAR, NT_CNC_MACRO, NT_CNC_BREAK, NT_CNC_PAUSE };
		
		unsigned int			lineNumber;
		NodeType 				nodeType;
		wxString				nodeName;
		wxString				elementId;
		wxString				originalPath;
		
		SvgCncBreak*			cncBreak;
		SvgCncPause*			cncPause;
		SvgCncContextMacro*		cncMacro;
		SvgCncContext*			cncParameters;
		
		DoubleStringMap			attributes;
		DoubleStringMap			ids;
		PathInfoVector			pathInfoList;
		TransformVector			transformList;
		StyleVector				styleList;
		
		explicit SVGUserAgentInfo(const SvgCncContext& ssc);
		explicit SVGUserAgentInfo(const SvgCncContextMacro& sccm);
		explicit SVGUserAgentInfo(const SvgCncBreak& scb);
		explicit SVGUserAgentInfo(const SvgCncPause& scp);
		explicit SVGUserAgentInfo(const SVGUserAgentInfo& uai);
		~SVGUserAgentInfo();
		
		SVGUserAgentInfo& operator= (const SVGUserAgentInfo&);
		
		bool isMemberOfSymbol(const wxString& id = "")			const;
		bool isMemberOfGroup(const wxString& id = "")			const;
		bool hasTransform()										const;
		bool hasStyle()											const;
		bool shouldProceed()									const;
		void getBaseDetails(DcmItemList& rows)					const;
		void getPathDetails(DcmItemList& rows)					const;
		void getDetails(DcmItemList& rows)						const;
		const char* getTransformInfoAsString()					const;
		const char* getStyleInfoAsString()						const;
		
		const PathInfoVector& getPathInfoList()					const { return pathInfoList; }
		
		void debug(const DoubleStringMap& dsm, std::ostream& out)	const;
		void debug(const PathInfo& pi, std::ostream& out)			const;
};

#endif
