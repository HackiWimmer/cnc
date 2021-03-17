#ifndef SVG_USER_AGENT_H
#define SVG_USER_AGENT_H

#include <map>
#include <vector>
#include <string>
#include <wx/dataview.h>
#include <wx/xml/xml.h>
#include "SvgCncContext.h"
#include "DataControlModel.h"
#include "CncConfig.h"
#include "CncPosition.h"
#include "SVGUserAgentInfo.h"

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
		wxString					nodeName;
		wxString					elementId;
		
		SVGUserAgentInfo			defaultUserAgent;
		UserAgentVector				userAgent;
		
		DoubleStringMap				collectedAttributes;
		DoubleStringMap				collectedIds;
		TransformVector				collectedTransforms;
		StyleVector					collectedStyles;
		
		UseDirectiveVector			useInfo;
		SvgUserAgentOutputControls	oCtl;
		
		SVGUserAgentInfo& getUserAgent(unsigned int pos);
		
		void displayUseDirective();
	
	public:
		SVGUserAgent();
		virtual ~SVGUserAgent();
		
		void setOutputControls(SvgUserAgentOutputControls& oc);
		void initalize();
		void clearControls();
		void evaluateTraceInfo(wxXmlNode* tr);
		
		bool initNextPath(SvgCncContext& cwp, const wxString& origPath);
		bool initNextCncParameterNode(const SvgCncContext& cwp);
		bool initNextCncVaribalesNode(const SvgCncContext& cwp);
		bool initNextCncBreakNode(const SvgCncBreak& scb);
		bool initNextCncPauseNode(const SvgCncPause& scp);
		bool setNodeType(const wxString& t);
		bool addID(const wxString& id, const char* nodeName = "");
		bool addTransform(const wxString& cmd);
		bool addStyle(const wxString& s);
		bool removeId(const wxString& id);
		bool removeLastTransform();
		bool removeLastStyle();
		bool addXMLAttributes(wxXmlAttribute *attribute);
		bool addPathElement(char c, unsigned int count, double values[]);
		bool displayDetailInfo(unsigned int pos);
		bool hasCurrentAttribute(const wxString& key);
		bool expand();
		
		SVGUserAgentInfo&		getCurentUserAgent();
		UseDirectiveVector&		getUseInfoVector();
		const UserAgentVector&	getList();
		UseDirective&			evaluateUseDirective(UseDirective& ud);
		
		const wxString&			getCurrentAttribute(const wxString& key, const wxString& defValue = _(""));
		
};

#endif
