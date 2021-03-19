#include "CncConfig.h"
#include "SVGUserAgentInfo.h"

/////////////////////////////////////////////////////////
SVGUserAgentInfo::SVGUserAgentInfo() 
: lineNumber				(UNDEFINED_LINE_NUMBER)
, nodeType					(NT_UNDEFINED)
, nodeName					()
, elementId					()
, originalPath				()
, cncBreak					()
, cncPause					()
, cncParameters				()
, attributes				()
, ids						()
, pathInfoList				()
, transformList				()
, styleList					()
/////////////////////////////////////////////////////////
{
	//preallocate space
	pathInfoList.reserve(THE_CONFIG->getConstReserveCapacity());
}
/////////////////////////////////////////////////////////
SVGUserAgentInfo::~SVGUserAgentInfo() {
/////////////////////////////////////////////////////////
	attributes.clear();
	ids.clear();
	pathInfoList.clear();
}
/////////////////////////////////////////////////////////
bool SVGUserAgentInfo::isMemberOf(const wxString& id, const char* type) const {
/////////////////////////////////////////////////////////
	if ( id.length() > 0 ) {
		
		auto it = ids.find(id);
		if ( it != ids.end() ) {
			if ( it->second == type )
				return true;
		}
	} 
	else {
		
		for (auto it=ids.begin(); it!=ids.end(); ++it) {
			if ( it->second == type )
				return true;
		}
	}
	
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgentInfo::isMemberOfSymbol(const wxString& id) const {
/////////////////////////////////////////////////////////
	return isMemberOf(id, "symbol");
}
/////////////////////////////////////////////////////////
bool SVGUserAgentInfo::isMemberOfGroup(const wxString& id) const {
/////////////////////////////////////////////////////////
	return isMemberOf(id, "g");
}
/////////////////////////////////////////////////////////
bool SVGUserAgentInfo::hasTransform() const {
/////////////////////////////////////////////////////////
	return (transformList.size() > 0 );
}
/////////////////////////////////////////////////////////
bool SVGUserAgentInfo::hasStyle() const {
/////////////////////////////////////////////////////////
	return (styleList.size() > 0 );
}
/////////////////////////////////////////////////////////
void SVGUserAgentInfo::debug(const DoubleStringMap& dsm, std::ostream& out) const {
/////////////////////////////////////////////////////////
	for (auto it=dsm.begin(); it!=dsm.end(); ++it) {
		out << it->first;
		out << "=";
		out << it->second;
		out << std::endl;
	}
}
/////////////////////////////////////////////////////////
void SVGUserAgentInfo::debug(const PathInfo& pi, std::ostream& out) const {
/////////////////////////////////////////////////////////
	out << "Command: " << pi.cmd << ", Count: " << pi.cnt << std::endl;
	for (unsigned int i=0; i< pi.cnt; i++) {
		out << "Value( " << i << "): " << pi.values[i] << std::endl;
	}
}
/////////////////////////////////////////////////////////
void SVGUserAgentInfo::getBaseDetails(DcmItemList& rows) const {
/////////////////////////////////////////////////////////
	wxString value;
	
	// --------------------------------------------------
	auto addCncStr = [&](const char* str) {
		
		if ( str != NULL ) {
			wxStringTokenizer lines(str, "\n" );
			while ( lines.HasMoreTokens() ) {
				wxString token(lines.GetNextToken());
				wxString key(token.BeforeFirst('=').Trim().Trim(false));
				wxString val(token.AfterFirst ('=').Trim().Trim(false));
				
				DataControlModel::addKeyValueRow(rows, key, val);
			}
		}
	};
	
	switch ( nodeType) {
		
		case NT_PATH:
		{
			cncParameters.getParameterList(rows);
			
			value.clear();
			for (auto it=attributes.begin(); it!=attributes.end(); ++it) {
				value += it->first;
				value += "=";
				value += it->second;
				value += " ";
			}
			DataControlModel::addKeyValueRow(rows, "Attributes", value);

			value.clear();
			DataControlModel::addKeyValueRow(rows, "IDs", (int)ids.size());
			for (auto it=ids.begin(); it!=ids.end(); ++it) {
				value  = it->second;
				value += "(";
				value += it->first;
				value += ")   ";
				DataControlModel::addKeyValueRow(rows, "  id", value);
			}
			
			DataControlModel::addKeyValueRow(rows, "transform", (int)transformList.size());
			for (auto it=transformList.begin(); it!=transformList.end(); ++it) {
				DataControlModel::addKeyValueRow(rows, "  cmd", *it);
			}
			
			DataControlModel::addKeyValueRow(rows, "style", (int)styleList.size());
			for (auto it=styleList.begin(); it!=styleList.end(); ++it) {
				DataControlModel::addKeyValueRow(rows, "  style", *it);
			}
			
			break;
		}
		case NT_CNC_VAR:
		{
			std::stringstream ss; cncParameters.traceVariablesOnlyTo(ss);
			addCncStr(ss.str().c_str());
			break;
		}
		case NT_CNC_PARAM:
		{
			std::stringstream ss; cncParameters.traceTo(ss);
			addCncStr(ss.str().c_str());
			break;
		}
		case NT_CNC_MACRO:
		{
			std::stringstream ss; cncMacro.traceTo(ss);
			addCncStr(ss.str().c_str());
			break;
		}
		case NT_CNC_BREAK:
		{
			std::stringstream ss; cncBreak.traceTo(ss);
			addCncStr(ss.str().c_str());
			break;
		}
		case NT_CNC_PAUSE:
		{
			std::stringstream ss; cncPause.traceTo(ss);
			addCncStr(ss.str().c_str());
			break;
		}
		default: 
		{
		}
	}
}
/////////////////////////////////////////////////////////
const char* SVGUserAgentInfo::getTransformInfoAsString() const {
/////////////////////////////////////////////////////////
	wxString ret;
	for (auto it=transformList.begin(); it!=transformList.end(); ++it)
		ret.append(wxString::Format("%s ", *it));
	
	return ret;
}
/////////////////////////////////////////////////////////
const char* SVGUserAgentInfo::getStyleInfoAsString() const {
/////////////////////////////////////////////////////////
	wxString ret;
	for (auto it=styleList.begin(); it!=styleList.end(); ++it)
		ret.append(wxString::Format("%s ", *it));
	
	return ret;
}
/////////////////////////////////////////////////////////
bool SVGUserAgentInfo::shouldProceed() const {
/////////////////////////////////////////////////////////
	switch ( nodeType ) {
		case NT_CNC_VAR:
		case NT_CNC_MACRO:
		case NT_CNC_PARAM:
		{
			return false;
		}
		default:
		{
			// if 'display:none' is configured don't spool this path
			// != wxNOT_FOUND ==> means FOUND
			if ( wxString(getStyleInfoAsString()).Find("display:none") != wxNOT_FOUND )
				return false;
		}
	}
	
	return true;
}
/////////////////////////////////////////////////////////
void SVGUserAgentInfo::getPathDetails(DcmItemList& rows) const {
/////////////////////////////////////////////////////////
	if ( nodeType != NT_PATH ) 
		return;
		
	DataControlModel::addKeyValueRow(rows, "Path", originalPath);
	for ( auto it=pathInfoList.begin(); it!=pathInfoList.end(); ++it ) {
		
		const PathInfo& pi = *it;
		const wxString command(wxString::Format("%c, Count=&u", pi.cmd, pi.cnt));
		DataControlModel::addKeyValueRow(rows, "  Command", command);
		
		for ( unsigned int i=0; i<pi.cnt; i++ ) {
			DataControlModel::addKeyValueRow(rows, wxString::Format(wxT("    Token(%d)"), i+1), pi.values[i]);
		}
	}
}
/////////////////////////////////////////////////////////
void SVGUserAgentInfo::getDetails(DcmItemList& rows) const {
/////////////////////////////////////////////////////////
	getBaseDetails(rows);
	getPathDetails(rows);
}
