#ifndef SVG_USER_AGENT_INFO_H
#define SVG_USER_AGENT_INFO_H

#include <map>
#include <vector>
#include <string>
#include <wx/dataview.h>
#include <wx/xml/xml.h>

typedef std::map<wxString, wxString> DoubleStringMap;

struct PathInfo {
	char cmd			= '\0';
	unsigned int count	= 0;
	double values[MAX_PARAMETER_VALUES];
};

typedef std::vector<PathInfo> PathInfoVector;
typedef std::map<wxString, DoubleStringMap> UseDirectiveMap;

struct UseDirective {
	wxString id;
	DoubleStringMap attributes;
};

typedef std::vector<UseDirective> UseDirectiveVector;
typedef std::vector<wxString> TransformVector;
typedef std::vector<wxString> StyleVector;


////////////////////////////////////////////////////////////////
struct SVGUserAgentInfo {
////////////////////////////////////////////////////////////////

	private:
		wxString transformInfotring;
		wxString styleInfotring;
		
		/////////////////////////////////////////////////////////
		bool isMemberOf(const wxString& id, const char* type) {
			if ( id.length() > 0 ) {
				DoubleStringMap::iterator it;
				it = ids.find(id);
				if ( it != ids.end() ) {
					if ( it->second == type )
						return true;
				}
			} else {
				for (DoubleStringMap::iterator it=ids.begin(); it!=ids.end(); ++it) {
					if ( it->second == type )
						return true;
				}
			}
			
			return false;
		}
		
	public:
	
		enum NodeType { NT_UNDEFINED, NT_PATH, NT_CNC_PARAM };
	
		unsigned int lineNumber = UNDEFINED_LINE_NUMBER;
		NodeType nodeType = NT_UNDEFINED;
		wxString nodeName;
		wxString originalPath;
		
		CncWorkingParameters workingParameters;
		
		DoubleStringMap attributes;
		DoubleStringMap ids;
		PathInfoVector pathInfoList;
		TransformVector transformList;
		StyleVector styleList;
		
		/////////////////////////////////////////////////////////
		SVGUserAgentInfo() {
			//preallocate space
			pathInfoList.reserve(1000);
		}
		
		/////////////////////////////////////////////////////////
		~SVGUserAgentInfo() {
			attributes.clear();
			ids.clear();
			pathInfoList.clear();
		}
		
		/////////////////////////////////////////////////////////
		bool isMemberOfSymbol(const wxString& id = "") {
			return isMemberOf(id, "symbol");
		}
		
		/////////////////////////////////////////////////////////
		bool isMemberOfGroup(const wxString& id = "") {
			return isMemberOf(id, "g");
		}
		
		/////////////////////////////////////////////////////////
		bool hasTransform() {
			return (transformList.size() > 0 );
		}
		
		/////////////////////////////////////////////////////////
		bool hasStyle() {
			return (styleList.size() > 0 );
		}
		
		/////////////////////////////////////////////////////////
		void debug(DoubleStringMap& dsm, std::ostream& out) {
			for (DoubleStringMap::iterator it=dsm.begin(); it!=dsm.end(); ++it) {
				out << it->first;
				out << "=";
				out << it->second;
				out << std::endl;
			}
		}
		
		/////////////////////////////////////////////////////////
		void debug(PathInfo& pi, std::ostream& out) {
			out << "Command: " << pi.cmd << ", Count: " << pi.count << std::endl;
			for (unsigned int i=0; i< pi.count; i++) {
				out << "Value( " << i << "): " << pi.values[i] << std::endl;
			}
		}
		
		/////////////////////////////////////////////////////////
		void getBaseDetails(DcmItemList& rows) {
			wxString value;
			
			DataControlModel::addKeyValueRow(rows, "Line Number", 	(int)lineNumber);
			DataControlModel::addKeyValueRow(rows, "Node", 			nodeName);
			
			workingParameters.getParameterList(rows);
						
			if ( nodeType == NT_PATH ) {
				value.clear();
				for (DoubleStringMap::iterator it=attributes.begin(); it!=attributes.end(); ++it) {
					value += it->first;
					value += "=";
					value += it->second;
					value += " ";
				}
				DataControlModel::addKeyValueRow(rows, "Attributes", value);

				value.clear();
				DataControlModel::addKeyValueRow(rows, "IDs", (int)ids.size());
				for (DoubleStringMap::iterator it=ids.begin(); it!=ids.end(); ++it) {
					value  = it->second;
					value += "(";
					value += it->first;
					value += ")   ";
					DataControlModel::addKeyValueRow(rows, "  id", value);
				}
				
				DataControlModel::addKeyValueRow(rows, "transform", (int)transformList.size());
				for (TransformVector::iterator it=transformList.begin(); it!=transformList.end(); ++it) {
					DataControlModel::addKeyValueRow(rows, "  cmd", *it);
				}
				
				DataControlModel::addKeyValueRow(rows, "style", (int)styleList.size());
				for (StyleVector::iterator it=styleList.begin(); it!=styleList.end(); ++it) {
					DataControlModel::addKeyValueRow(rows, "  style", *it);
				}
			}
		}
		
		/////////////////////////////////////////////////////////
		const char* getTransformInfoAsString() {
			transformInfotring.clear();
			for (TransformVector::iterator it=transformList.begin(); it!=transformList.end(); ++it) {
				transformInfotring << *it;
				transformInfotring << " ";
			}
			
			return transformInfotring.c_str();
		}
		
		/////////////////////////////////////////////////////////
		const char* getStyleInfoAsString() {
			styleInfotring.clear();
			for (StyleVector::iterator it=styleList.begin(); it!=styleList.end(); ++it) {
				styleInfotring << *it;
				styleInfotring << " ";
			}
			
			return styleInfotring.c_str();
		}
		
		/////////////////////////////////////////////////////////
		bool shouldProceed() {
			if ( nodeType == NT_CNC_PARAM )
				return false;
				
			// if 'display:none' is configured don't spool this path
			// != wxNOT_FOUND == FOUND
			if ( wxString(getStyleInfoAsString()).Find("display:none") != wxNOT_FOUND )
				return false;
				
			return true;
		}
		
		/////////////////////////////////////////////////////////
		void getPathDetails(DcmItemList& rows) {
			if ( nodeType != NT_PATH ) 
				return;
				
			DataControlModel::addKeyValueRow(rows, "Path", originalPath);
			for ( PathInfoVector::iterator it=pathInfoList.begin(); it!=pathInfoList.end(); ++it ) {
				PathInfo pi = *it;
				wxString command;
				command << pi.cmd;
				command << ", Count=";
				command << (int)pi.count;
				DataControlModel::addKeyValueRow(rows, "  Command", command);
				
				for ( unsigned int i=0; i<pi.count; i++ ) {
					DataControlModel::addKeyValueRow(rows, wxString::Format(wxT("    Token(%d)"), i+1), pi.values[i]);
				}
			}
		}
		
		/////////////////////////////////////////////////////////
		void getDetails(DcmItemList& rows) {
			getBaseDetails(rows);
			getPathDetails(rows);
		}
};

#endif
