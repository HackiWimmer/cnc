#include "SVGUserAgent.h"


/////////////////////////////////////////////////////////
SVGUserAgent::SVGUserAgent() 
: nodeName						()
, elementId						()
, defaultUserAgent				()
, userAgent						()
, collectedAttributes			()
, collectedIds					()
, collectedTransforms			()
, collectedStyles				()
, useInfo						()
, oCtl							()
/////////////////////////////////////////////////////////
{
	oCtl.inboundPathList 	= NULL;
	oCtl.useDirectiveList 	= NULL;
	oCtl.detailInfo			= NULL;
	
	//preallocate space
	userAgent.reserve(THE_CONFIG->getConstReserveCapacity());
}
/////////////////////////////////////////////////////////
SVGUserAgent::~SVGUserAgent() {
/////////////////////////////////////////////////////////
	userAgent.clear();
	collectedIds.clear();
	collectedAttributes.clear();
	collectedStyles.clear();
}
/////////////////////////////////////////////////////////
SVGUserAgentInfo& SVGUserAgent::getCurentUserAgent() {
/////////////////////////////////////////////////////////
	return userAgent.back();
}
/////////////////////////////////////////////////////////
SVGUserAgentInfo& SVGUserAgent::getUserAgent(unsigned int pos) {
/////////////////////////////////////////////////////////
	if ( pos < userAgent.size() )
		return userAgent.at(pos);
	
	std::cerr	<< CNC_LOG_FUNCT << ": invalid index: "
				<< pos 
				<< std::endl
				;
	
	return defaultUserAgent;
}
/////////////////////////////////////////////////////////
void SVGUserAgent::displayUseDirective() {
/////////////////////////////////////////////////////////
	if ( oCtl.useDirectiveList == NULL )
		return;
		
	DcmItemList rows;
	
	for ( auto it1 = useInfo.begin(); it1 != useInfo.end(); ++it1 ) {
		UseDirective& ud = *it1;
		wxString data;
		for ( auto it2 = ud.attributes.begin(); it2 != ud.attributes.end(); ++it2 ) {
			data << it2->first;
			data << "=";
			data << it2->second;
			data << " ";
		}
		DataControlModel::addKeyValueRow(rows, ud.id, data);
	}
	
	for ( auto it = rows.begin(); it != rows.end(); ++it ) {
		oCtl.useDirectiveList->AppendItem(*it);
	}
}
/////////////////////////////////////////////////////////
void SVGUserAgent::setOutputControls(SvgUserAgentOutputControls& oc) {
/////////////////////////////////////////////////////////
	oCtl.inboundPathList 	= oc.inboundPathList;
	oCtl.useDirectiveList	= oc.useDirectiveList;
	oCtl.detailInfo			= oc.detailInfo;
}
/////////////////////////////////////////////////////////
void SVGUserAgent::initalize() {
/////////////////////////////////////////////////////////
	userAgent.clear();
	collectedIds.clear();
	collectedAttributes.clear();
	collectedStyles.clear();
	
	if ( oCtl.inboundPathList != NULL ) 
		oCtl.inboundPathList->DeleteAllItems();
		
	if ( oCtl.useDirectiveList != NULL ) 
		oCtl.useDirectiveList->DeleteAllItems();
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::initNextPath(SvgCncContext& cwp, const wxString& origPath) {
/////////////////////////////////////////////////////////
	SVGUserAgentInfo sua;
	sua.lineNumber 			= cwp.getCurrentClientID();
	sua.nodeName 			= nodeName;
	sua.elementId			= ( elementId.IsEmpty() == false ? elementId : "");
	sua.nodeType			= SVGUserAgentInfo::NT_PATH;
	sua.originalPath		= origPath;
	sua.cncParameters 		= cwp;
	
	// move the following list
	sua.attributes.swap(collectedAttributes);
	
	// copy the following lists
	sua.ids					= collectedIds;
	sua.transformList		= collectedTransforms;
	sua.styleList			= collectedStyles;

	userAgent.push_back(std::move(sua));
	return true;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::initNextCncParameterNode(const SvgCncContext& cwp) {
/////////////////////////////////////////////////////////
	SVGUserAgentInfo sua;
	sua.lineNumber 			= cwp.getCurrentLineNumber();
	sua.nodeName 			= nodeName;
	sua.elementId			= "";
	sua.nodeType			= SVGUserAgentInfo::NT_CNC_PARAM;
	sua.originalPath		= "";
	sua.cncParameters 		= cwp;
	
	// copy the following lists - why ????
	sua.styleList		= collectedStyles;
	
	userAgent.push_back(std::move(sua));
	return true;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::initNextCncBreakNode(const SvgCncBreak& scb) {
/////////////////////////////////////////////////////////
	SVGUserAgentInfo sua;
	sua.lineNumber 			= scb.getCurrentLineNumber();
	sua.nodeName 			= nodeName;
	sua.elementId			= "";
	sua.nodeType			= SVGUserAgentInfo::NT_CNC_BREAK;
	sua.originalPath		= "";
	sua.cncBreak			= scb;
	
	userAgent.push_back(std::move(sua));
	return true;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::initNextCncPauseNode(const SvgCncPause& scp) {
/////////////////////////////////////////////////////////
	SVGUserAgentInfo sua;
	sua.lineNumber 			= scp.getCurrentLineNumber();
	sua.nodeName 			= nodeName;
	sua.elementId			= "";
	sua.nodeType			= SVGUserAgentInfo::NT_CNC_PAUSE;
	sua.originalPath		= "";
	sua.cncPause			= scp;
	
	userAgent.push_back(std::move(sua));
	return true;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::setNodeType(const wxString& t) {
/////////////////////////////////////////////////////////
	nodeName = t;
	return true;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::addID(const wxString& id, const char* nodeName) {
/////////////////////////////////////////////////////////
	if ( id.IsEmpty() == false ) {
		collectedIds[id] = nodeName;
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::addTransform(const wxString& cmd) {
/////////////////////////////////////////////////////////
	if ( cmd.IsEmpty() == false ) {
		collectedTransforms.push_back(cmd);
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::addStyle(const wxString& s) {
/////////////////////////////////////////////////////////
	if ( s.IsEmpty() == false ) {
		collectedStyles.push_back(s);
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::removeId(const wxString& id) {
/////////////////////////////////////////////////////////
	if ( auto it = collectedIds.find(id); it != collectedIds.end() ) {
		collectedIds.erase(it);
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::removeLastTransform() {
/////////////////////////////////////////////////////////
	if ( collectedTransforms.size() > 0 ) {
		collectedTransforms.pop_back();
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::removeLastStyle() {
/////////////////////////////////////////////////////////
	if ( collectedStyles.size() > 0 ) {
		collectedStyles.pop_back();
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::addXMLAttributes(wxXmlAttribute *attribute) {
/////////////////////////////////////////////////////////
	if ( attribute == NULL )
		return true;
		
	if ( attribute->GetName() == "id" ) {
		addID(attribute->GetValue(), "#elem");
		elementId = attribute->GetValue();
	}
	
	collectedAttributes[attribute->GetName()] = attribute->GetValue();
	
	// recursion call to get the compelete depth
	return addXMLAttributes(attribute->GetNext());
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::hasCurrentAttribute(const wxString& key) {
/////////////////////////////////////////////////////////
	// first search it at the current collection
	const DoubleStringMap& collAttrMap = collectedAttributes;
	if ( auto it = collAttrMap.find(key); it != collAttrMap.end() )
		return true;
		
	if ( userAgent.size() > 0 ) {
		// second try to find it here
		const DoubleStringMap& attrMap = getCurentUserAgent().attributes;
		if ( auto it = attrMap.find(key); it != attrMap.end() )
			return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////
const wxString& SVGUserAgent::getCurrentAttribute(const wxString& key, const wxString& defValue) {
/////////////////////////////////////////////////////////
	// first search it at the current collection
	const DoubleStringMap& collAttrMap = collectedAttributes;
	if ( auto it = collAttrMap.find(key); it != collAttrMap.end() )
		return it->second;
		
	if ( userAgent.size() > 0 ) {
		// second try to find it here
		const DoubleStringMap& attrMap = getCurentUserAgent().attributes;
		if ( auto it = attrMap.find(key); it != attrMap.end() )
			return it->second;
	}
	
	return defValue;
}
/////////////////////////////////////////////////////////
UseDirectiveVector& SVGUserAgent::getUseInfoVector() {
/////////////////////////////////////////////////////////
	return useInfo;
}
/////////////////////////////////////////////////////////
const UserAgentVector& SVGUserAgent::getList() {
/////////////////////////////////////////////////////////
	return userAgent;
}
/////////////////////////////////////////////////////////
UseDirective& SVGUserAgent::evaluateUseDirective(UseDirective& ud) {
/////////////////////////////////////////////////////////
	// search xlink:href, fill ud.id and remove xlink:href key from map
	if ( auto it = ud.attributes.find("xlink:href"); it != ud.attributes.end() ) {
		ud.id = it->second.substr(1);
		ud.attributes.erase(it);
	} 
	
	return ud;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::addPathElement(char c, unsigned int count, double values[]) {
/////////////////////////////////////////////////////////
	SVGUserAgentInfo& sua = getCurentUserAgent();
	if ( sua.lineNumber > 0 ) {
		sua.pathInfoList.push_back(std::move(PathInfo(c, count, values)));
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::displayDetailInfo(unsigned int pos) {
/////////////////////////////////////////////////////////
	if ( oCtl.detailInfo != NULL ) {
		
		const SVGUserAgentInfo& sua = getUserAgent(pos);
		DcmItemList rows;
		sua.getDetails(rows);
		
		for (auto it = rows.begin(); it != rows.end(); ++it)
			oCtl.detailInfo->AppendItem(*it);
			
		return true;
	}
	
	return false;
}
/////////////////////////////////////////////////////////
bool SVGUserAgent::expand() {
/////////////////////////////////////////////////////////
	displayUseDirective();
	
	// first fill the inbount path list
	if ( oCtl.inboundPathList != NULL ) {
		for ( auto it=userAgent.begin(); it != userAgent.end(); ++it) {
			
			DcmItemList rows;
			wxString val(it->nodeName);
			
			if ( it->elementId.IsEmpty() == false ) {
				val.append(",  id = ");
				val.append(it->elementId);
			}
			
			// append items
			DataControlModel::addKeyCheckValueRow(rows, (int)it->lineNumber, it->shouldProceed(), val);
			for (DcmItemList::iterator itr = rows.begin(); itr != rows.end(); ++itr)
				oCtl.inboundPathList->AppendItem(*itr);
		}
		
		// fill the detail list for the first item
		if ( oCtl.inboundPathList->GetItemCount() > 0 ) {
			oCtl.inboundPathList->Select(oCtl.inboundPathList->RowToItem(0));
			
			if ( oCtl.detailInfo != NULL )
				oCtl.detailInfo->DeleteAllItems();
				
			displayDetailInfo(0);
		}
	}
	
	return true;
}
/////////////////////////////////////////////////////////
void SVGUserAgent::clearControls() {
/////////////////////////////////////////////////////////
	if ( oCtl.inboundPathList != NULL )
		oCtl.inboundPathList->DeleteAllItems();
		
	if ( oCtl.useDirectiveList != NULL )
		oCtl.useDirectiveList->DeleteAllItems();
		
	if ( oCtl.detailInfo != NULL )
		oCtl.detailInfo->DeleteAllItems();
}
/////////////////////////////////////////////////////////
void SVGUserAgent::evaluateTraceInfo(wxXmlNode* tr) {
/////////////////////////////////////////////////////////
	// over all stored pathes
	for ( auto itUav = userAgent.begin(); itUav != userAgent.end(); ++itUav ) {
		SVGUserAgentInfo& uai	= *itUav;
		wxXmlNode*			n 	= new wxXmlNode();
		
		n->SetName("UserAgentInfo");
		n->SetType( wxXML_ELEMENT_NODE);
		n->AddAttribute("type", uai.nodeName);
		n->AddAttribute("line", wxString() << uai.lineNumber / CLIENT_ID.TPL_FACTOR);
		
		tr->AddChild(n);
	}
	
	for ( auto it1 = useInfo.begin(); it1 != useInfo.end(); ++it1 ) {
		UseDirective&  ud	= *it1;
		wxXmlNode*		n	= new wxXmlNode();
		
		n->SetName("UseDirective");
		n->SetType( wxXML_ELEMENT_NODE);
		n->AddAttribute("ID", ud.id);
		
		for ( auto it2 = ud.attributes.begin(); it2 != ud.attributes.end(); ++it2 ) 
			n->AddAttribute(it2->first, it2->second);
		
		tr->AddChild(n);
	}
}
