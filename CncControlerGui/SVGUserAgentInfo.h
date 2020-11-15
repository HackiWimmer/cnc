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
struct PathInfo {
	char cmd			= '\0';
	unsigned int count	= 0;
	double values[MAX_PARAMETER_VALUES];
};

typedef std::vector<PathInfo> 					PathInfoVector;
typedef std::map<wxString, DoubleStringMap>		UseDirectiveMap;

// ------------------------------------------------------------
struct UseDirective {
	wxString 		id;
	DoubleStringMap	attributes;
};

typedef std::vector<UseDirective>				UseDirectiveVector;
typedef std::vector<wxString>					TransformVector;
typedef std::vector<wxString>					StyleVector;

////////////////////////////////////////////////////////////////
struct SVGUserAgentInfo {
////////////////////////////////////////////////////////////////
	
	private:
		wxString				transformInfoString;
		wxString				styleInfoString;
		
		bool isMemberOf(const wxString& id, const char* type);
		
	public:
		enum NodeType { NT_UNDEFINED, NT_PATH, NT_CNC_PARAM, NT_CNC_BREAK, NT_CNC_PAUSE };
		
		unsigned int			lineNumber;
		NodeType 				nodeType;
		wxString				nodeName;
		wxString				elementId;
		wxString				originalPath;
		
		SvgCncBreak				cncBreak;
		SvgCncPause				cncPause;
		SvgCncContext		cncParameters;
		
		DoubleStringMap			attributes;
		DoubleStringMap			ids;
		PathInfoVector			pathInfoList;
		TransformVector			transformList;
		StyleVector				styleList;
		
		/////////////////////////////////////////////////////////
		SVGUserAgentInfo();
		~SVGUserAgentInfo();
		
		bool isMemberOfSymbol(const wxString& id = "");
		bool isMemberOfGroup(const wxString& id = "");
		bool hasTransform();
		bool hasStyle();
		bool shouldProceed();
		void debug(DoubleStringMap& dsm, std::ostream& out);
		void debug(PathInfo& pi, std::ostream& out);
		void getBaseDetails(DcmItemList& rows);
		void getPathDetails(DcmItemList& rows);
		void getDetails(DcmItemList& rows);
		const char* getTransformInfoAsString();
		const char* getStyleInfoAsString();
};

#endif
