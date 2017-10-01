#ifndef SVG_USER_AGENT_H
#define SVG_USER_AGENT_H

#include <map>
#include <vector>
#include <string>
#include <wx/dataview.h>
#include <wx/xml/xml.h>
#include "SVGUserAgentInfo.h"
#include "CncWorkingParameters.h"
#include "DataControlModel.h"
#include "CncPosition.h"

struct SvgUserAgentOutputControls {
	wxDataViewListCtrl* inboundPathList 	= NULL;
	wxDataViewListCtrl* useDirectiveList	= NULL;
	wxDataViewListCtrl* detailInfo			= NULL;
};

typedef std::vector<SVGUserAgentInfo> UserAgentVector;

////////////////////////////////////////////////////////////////
class SVGUserAgent{
////////////////////////////////////////////////////////////////

	private:
		SVGUnit unit;
		CncXYDoubleDimension dimension;
		wxString nodeName;
		wxString elementId;
		
		SVGUserAgentInfo defaultUserAgent;
		UserAgentVector userAgent;
		
		DoubleStringMap collectedAttributes;
		DoubleStringMap collectedIds;
		TransformVector collectedTransforms;
		StyleVector     collectedStyles;
		
		UseDirectiveVector useInfo;
		
		SvgUserAgentOutputControls oCtl;
		
		/////////////////////////////////////////////////////////
		SVGUserAgentInfo& getCurentUserAgent() {
			return userAgent.back();
		}
		
		/////////////////////////////////////////////////////////
		void updateCurrentUserAgent(SVGUserAgentInfo& data) {
			userAgent.back() = data;
		}
		
		/////////////////////////////////////////////////////////
		SVGUserAgentInfo& getUserAgent(unsigned int pos) {
			if ( pos < userAgent.size() ) {
				return userAgent.at(pos);
			}

			std::cerr << "SVGUserAgent::getUserAgent: iinfalid index: "<< pos << std::endl;
			return defaultUserAgent;
		}
		
		/////////////////////////////////////////////////////////
		void displayUseDirective() {
			if ( oCtl.useDirectiveList == NULL )
				return;
				
			DcmItemList rows;
			
			for ( UseDirectiveVector::iterator it1 = useInfo.begin(); it1 != useInfo.end(); ++it1 ) {
				UseDirective ud = *it1;
				wxString data;
				for ( DoubleStringMap::iterator it2 = ud.attributes.begin(); it2 != ud.attributes.end(); ++it2 ) {
					data << it2->first;
					data << "=";
					data << it2->second;
					data << " ";
				}
				DataControlModel::addKeyValueRow(rows, ud.id, data);
			}
			
			for ( DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it ) {
				oCtl.useDirectiveList->AppendItem(*it);
			}
		}
	
	public:
		/////////////////////////////////////////////////////////
		SVGUserAgent() {
			oCtl.inboundPathList 	= NULL;
			oCtl.useDirectiveList 	= NULL;
			oCtl.detailInfo			= NULL;
			
			//preallocate space
			userAgent.reserve(1000 * 1000);
		}
		
		/////////////////////////////////////////////////////////
		virtual ~SVGUserAgent() {
			userAgent.clear();
			collectedIds.clear();
			collectedAttributes.clear();
			collectedStyles.clear();
		}
		
		/////////////////////////////////////////////////////////
		void setOutputControls(SvgUserAgentOutputControls& oc) {
			oCtl.inboundPathList 	= oc.inboundPathList;
			oCtl.useDirectiveList	= oc.useDirectiveList;
			oCtl.detailInfo			= oc.detailInfo;
		}
		
		/////////////////////////////////////////////////////////
		void initalize(SVGUnit u, const CncXYDoubleDimension& d) {
			unit = u;
			dimension = d;
			
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
		bool initNextPath(CncWorkingParameters& cwp, const wxString& origPath) {
			SVGUserAgentInfo sua;
			sua.lineNumber 			= cwp.currentLineNumber;
			sua.nodeName 			= nodeName;
			sua.nodeType			= SVGUserAgentInfo::NT_PATH;
			sua.originalPath		= origPath;
			sua.workingParameters 	= cwp;
			
			// move the following list
			sua.attributes.swap(collectedAttributes);
			// copy the following lists
			sua.ids				= collectedIds;
			sua.transformList 	= collectedTransforms;
			sua.styleList		= collectedStyles;
		
			userAgent.push_back(sua);
			
			// add inbound path to user agent list
			if ( oCtl.inboundPathList != NULL ) {
				DcmItemList rows;
				wxString val(sua.nodeName);
				
				if ( elementId.IsEmpty() == false ) {
					val << ",  id = ";
					val << elementId;
				}
				
				DataControlModel::addKeyCheckValueRow(rows, (int)sua.lineNumber, sua.shouldProceed(), val);
				for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
					oCtl.inboundPathList->AppendItem(*it);
				}
			}
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool initNextCncNode(CncWorkingParameters& cwp) {
			SVGUserAgentInfo sua;
			sua.lineNumber 			= cwp.currentLineNumber;
			sua.nodeName 			= nodeName;
			sua.nodeType			= SVGUserAgentInfo::NT_CNC_PARAM;
			sua.originalPath		= "";
			sua.workingParameters 	= cwp;
			
			// copy the following lists
			sua.styleList		= collectedStyles;
			
			userAgent.push_back(sua);
			
			// add inbound path to user agent list
			if ( oCtl.inboundPathList != NULL ) {
				DcmItemList rows;
				wxString val(sua.nodeName);
				
				DataControlModel::addKeyCheckValueRow(rows, (int)sua.lineNumber, sua.shouldProceed(), val);
				for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
					oCtl.inboundPathList->AppendItem(*it);
				}
			}
			return true;
		}
		
		/////////////////////////////////////////////////////////
		bool setNodeType(const wxString& t) {
			nodeName = t;
			return true;
		}

		/////////////////////////////////////////////////////////
		bool addID(const wxString& id, const char* nodeName = "") {
			if ( id != "" ) {
				collectedIds[id] = nodeName;
				return true;
			}
			return false;
		}
		/////////////////////////////////////////////////////////
		bool addTransform(const wxString& cmd) {
			if ( cmd != "" ) {
				collectedTransforms.push_back(cmd);
				return true;
			}
			return false;
		}
		/////////////////////////////////////////////////////////
		bool addStyle(const wxString& s) {
			if ( s != "" ) {
				collectedStyles.push_back(s);
				return true;
			}
			return false;
		}
		/////////////////////////////////////////////////////////
		bool removeId(const wxString& id) {
			DoubleStringMap::iterator it;
			it = collectedIds.find(id);
			if ( it != collectedIds.end() ) {
				collectedIds.erase(it);
				return true;
			}
			return false;
		}
		
		/////////////////////////////////////////////////////////
		bool removeLastTransform() {
			if ( collectedTransforms.size() > 0 ) {
				collectedTransforms.pop_back();
				return true;
			}
			return false;
		}
		
		/////////////////////////////////////////////////////////
		bool removeLastStyle() {
			if ( collectedStyles.size() > 0 ) {
				collectedStyles.pop_back();
				return true;
			}
			return false;
		}
		/////////////////////////////////////////////////////////
		bool addXMLAttributes(wxXmlAttribute *attribute) {
			if ( attribute == NULL )
				return true;
				
			if ( attribute->GetName() == "id" ) {
				addID(attribute->GetValue(), "#elem");
				elementId = attribute->GetValue();
			}
				
			if ( attribute->GetName() == "transform" )
				addTransform(attribute->GetValue());

			collectedAttributes[attribute->GetName()] = attribute->GetValue();

			// recursion call to get the compelete depth
			return addXMLAttributes(attribute->GetNext());
		}
		
		/////////////////////////////////////////////////////////
		UseDirectiveVector& getUseInfoVector() {
			return useInfo;
		}
		
		/////////////////////////////////////////////////////////
		const UserAgentVector& getList() {
			return userAgent;
		}
		
		/////////////////////////////////////////////////////////
		UseDirective& evaluateUseDirective(UseDirective& ud) {
			// search xlink:href, fill ud.id and remove xlink:href key from map
			DoubleStringMap::iterator it;
			it = ud.attributes.find("xlink:href");
			
			if ( it != ud.attributes.end() ) {
				ud.id = it->second.substr(1);
				ud.attributes.erase(it);
			} 
			
			return ud;
		}
		
		/////////////////////////////////////////////////////////
		bool addPathElement(char c, unsigned int count, double values[]) {
			SVGUserAgentInfo sua = getCurentUserAgent();
			if ( sua.lineNumber > 0 ) {
				PathInfo pi;
				pi.cmd 		= c;
				pi.count 	= count;
				for (unsigned int i=0; i<count; i++ )
					pi.values[i] = values[i];
					
				sua.pathInfoList.push_back(pi);
				updateCurrentUserAgent(sua);
				return true;
			}
			
			return false;
		}
		
		/////////////////////////////////////////////////////////
		bool displayDetailInfo(unsigned int pos) {
			if ( oCtl.detailInfo != NULL ) {
				DcmItemList rows;
				SVGUserAgentInfo sua = getUserAgent(pos);
				sua.getDetails(rows);
				
				for (DcmItemList::iterator it = rows.begin(); it != rows.end(); ++it) {
					oCtl.detailInfo->AppendItem(*it);
				}
				return true;
			}
			return false;
		}
		
		/////////////////////////////////////////////////////////
		bool expand() {
			displayUseDirective();
			if ( oCtl.inboundPathList != NULL && oCtl.inboundPathList->GetItemCount() > 0 ) {
				oCtl.inboundPathList->Select(oCtl.inboundPathList->RowToItem(0));
				if ( oCtl.detailInfo != NULL )
					oCtl.detailInfo->DeleteAllItems();
				displayDetailInfo(0);
			}
			return true;
		}
		
		/////////////////////////////////////////////////////////
		void clearControls() {
			if ( oCtl.inboundPathList != NULL )
				oCtl.inboundPathList->DeleteAllItems();
				
			if ( oCtl.useDirectiveList != NULL )
				oCtl.useDirectiveList->DeleteAllItems();
				
			if ( oCtl.detailInfo != NULL )
				oCtl.detailInfo->DeleteAllItems();
		}
		
		/////////////////////////////////////////////////////////
		void evaluateTraceInfo(wxXmlNode* tr) {
			// over all stored pathes
			for ( UserAgentVector::iterator itUav = userAgent.begin(); itUav != userAgent.end(); ++itUav ) {
				SVGUserAgentInfo uai  = *itUav;
				wxXmlNode* n = new wxXmlNode();
				n->SetName("UserAgentInfo");
				n->SetType( wxXML_ELEMENT_NODE);
				n->AddAttribute("type", uai.nodeName);
				n->AddAttribute("line", wxString() << uai.lineNumber);
				
				tr->AddChild(n);
			}
			
			for ( UseDirectiveVector::iterator it1 = useInfo.begin(); it1 != useInfo.end(); ++it1 ) {
				UseDirective ud = *it1;
				wxXmlNode* n = new wxXmlNode();
				n->SetName("UseDirective");
				n->SetType( wxXML_ELEMENT_NODE);
				n->AddAttribute("ID", ud.id);
				
				for ( DoubleStringMap::iterator it2 = ud.attributes.begin(); it2 != ud.attributes.end(); ++it2 ) {
					n->AddAttribute(it2->first, it2->second);
				}
				tr->AddChild(n);
			}
		}
};

#endif
