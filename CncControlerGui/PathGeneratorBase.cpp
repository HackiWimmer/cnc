#include <wx/string.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>
#include "CncPatternDefinitions.h"
#include "PathGeneratorBase.h"

///////////////////////////////////////////////////////////////////
const wxString& PathGeneratorBase::generatePath() {
///////////////////////////////////////////////////////////////////
	static wxString s;

	// determine result
	SvgPathGroup spg(mm);
	generatePathIntern(spg);
	s.assign(spg.get());
	
	// config block
	fillConfigBlock(s);
	
	// add error infos
	addErrorInfo(spg.getErrorInfo());
	
	return s;
}
///////////////////////////////////////////////////////////////////
const wxString& PathGeneratorBase::generatePath(double toolDiameter, const ParameterMap& pMap) {
///////////////////////////////////////////////////////////////////
	static wxString s;
	
	//update parameter values
	for (ParameterMap::const_iterator it = pMap.begin(); it != pMap.end(); ++it) {
		setParameterValue(it->first, it->second.value);
	}
	
	// determine result
	SvgPathGroup spg(mm);
	generatePathIntern(spg);
	s.assign(spg.get());
	
	// config block
	fillConfigBlock(s);
	
	// add error infos
	addErrorInfo(spg.getErrorInfo());
	
	return s;
}
///////////////////////////////////////////////////////////////////
wxXmlNode* PathGeneratorBase::evaluateCncPattern(double toolDiameter, const ParameterMap& pMap) {
///////////////////////////////////////////////////////////////////
	static wxXmlNode* root;
//todo
	return root;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorBase::generatePathIntern(SvgPathGroup& spg) {
///////////////////////////////////////////////////////////////////
	spg.setIncludeReferenceCross(commonValues.referenceCross);
	
	// set center point
	if ( setReferencePoint(spg) == false )
		addErrorInfo("PathGenertor::setReferencePoint() failed");
	
	// configure common values
	spg.pGen().setColour(commonValues.pathColour);
	createCncParameterBlock(spg);
	
	// generate path 
	if ( generate(spg, commonValues.toolDiameter) == false )
		addErrorInfo("PathGenertor::generate() failed");
	
	// transform group
	transform(spg);
	
	if ( commonValues.xmlPattern == true ) {
		//todo
		encodeXmlPattern(spg);
		wxString p(xmlPattern);
		spg.setXmlPattern(maskXmlPattern(p));
	} else {
		spg.setXmlPattern("");
	}
}
///////////////////////////////////////////////////////////////////
const wxString& PathGeneratorBase::getConfigBlock(wxString& block) {
///////////////////////////////////////////////////////////////////
	block.clear();
	
	if ( commonValues.configBlock == true ) {
		block << "\n<!-- Config Block";
		block << getXmlPattern();
		block << " -->";
	}
	
	return block;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorBase::encodeXmlPattern(const SvgPathGroup& spg) {
///////////////////////////////////////////////////////////////////
	wxXmlDocument xmlDoc;
	wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, CncPatternRootName);
	xmlDoc.SetRoot(root);
	
	// over all parameters
	for ( ParameterMap::reverse_iterator it=parameterMap.rbegin(); it!=parameterMap.rend(); ++it) {
		ParameterInfo pi = it->second;
		wxXmlNode* node = new wxXmlNode(root, wxXML_ELEMENT_NODE, CncPatternParameterNode);
		node->AddAttribute(CncPatternParameterIndex, 	wxString::Format("%d", it->first));
		node->AddAttribute(CncPatternParameterName, 	pi.label);
		node->AddAttribute(CncPatternParameterType, 	pi.propertyType);
		node->AddAttribute(CncPatternParameterValue, 	pi.getValueAsString());
	}
	
	wxXmlNode* cnc = new wxXmlNode(root, wxXML_ELEMENT_NODE, CncPatternCncParameterBlockNode);
	CncParameterBlockMap cpm;
	getCncParameterBlock(cpm);
	// over all attributes
	for ( CncParameterBlockMap::reverse_iterator it=cpm.rbegin(); it!=cpm.rend(); ++it)
		cnc->AddAttribute(it->first, it->second);
	
	wxXmlNode* tra = new wxXmlNode(root, wxXML_ELEMENT_NODE, CncPatternTransformNode);
	tra->AddAttribute(CncPatternTransformTranslateX,	wxString::Format("%.3lf", transformValues.translateX));
	tra->AddAttribute(CncPatternTransformTranslateY,	wxString::Format("%.3lf", transformValues.translateY));
	tra->AddAttribute(CncPatternTransformScaleX,		wxString::Format("%.3lf", transformValues.scaleX));
	tra->AddAttribute(CncPatternTransformScaleY,		wxString::Format("%.3lf", transformValues.scaleY));
	tra->AddAttribute(CncPatternTransformRotateA,		wxString::Format("%.3lf", transformValues.rotateA));
	tra->AddAttribute(CncPatternTransformRotateX,		wxString::Format("%.3lf", transformValues.rotateX));
	tra->AddAttribute(CncPatternTransformRotateY,		wxString::Format("%.3lf", transformValues.rotateY));
	tra->AddAttribute(CncPatternTransformSkewX,			wxString::Format("%.3lf", transformValues.skewX));
	tra->AddAttribute(CncPatternTransformSkewY,			wxString::Format("%.3lf", transformValues.skewY));
	
	wxXmlNode* ref = new wxXmlNode(root, wxXML_ELEMENT_NODE, CncPatternReferenceNode);
	ref->AddAttribute(CncPatternReferenceX, 			wxString::Format("%.3lf", getReferencePoint().x));
	ref->AddAttribute(CncPatternReferenceY, 			wxString::Format("%.3lf", getReferencePoint().y));
	
	wxXmlNode* tpl = new wxXmlNode(root, wxXML_ELEMENT_NODE, CncPatternTemplateNode);
	tpl->AddAttribute(CncPatternTemplateName, 			name);
	tpl->AddAttribute(CncPatternTemplateToolDiameter, 	wxString::Format("%.3lf", commonValues.toolDiameter));
	tpl->AddAttribute(CncPatternTemplateToolCorrection, wxString::Format("%d",    commonValues.toolCorrection));
	tpl->AddAttribute(CncPatternTemplateCheckSum, 		getCheckSumAsString());
	
	wxStringOutputStream stream;
	xmlDoc.Save(stream);
	xmlPattern = (stream.GetString());
	
	int p = xmlPattern.Find("?>");
	if ( p != wxNOT_FOUND )
		xmlPattern = xmlPattern.SubString(p + 2 , xmlPattern.length()-1);
}
///////////////////////////////////////////////////////////////////
bool PathGeneratorBase::decodeXmlPattern(const wxString& pattern, XmlPatternResult& result) {
///////////////////////////////////////////////////////////////////
	wxString p(pattern);
	result.reset();

	demaskXmlPattern(p);
	
	wxStringInputStream stream(p);
 	wxXmlDocument xmlDoc(stream);
	wxXmlNode* root = xmlDoc.GetRoot();
	
	if ( root == NULL ) {
		result.errorInfo << "Cant evaluate XML root node:\n";
		result.errorInfo << p;
		return false;
	}
	
	ParameterInfo pi;
	wxXmlNode* child = root->GetChildren();
	while ( child ) {
		
		if ( child->GetName() == CncPatternTemplateNode ) {
			result.tplName.assign(child->GetAttribute(CncPatternTemplateName, ""));
			result.checkSum.assign(child->GetAttribute(CncPatternTemplateCheckSum, ""));
			
			wxString td(child->GetAttribute(CncPatternTemplateToolDiameter, "0.0"));
			td.ToDouble(&result.commonValues.toolDiameter);
			
			wxString tc(child->GetAttribute(CncPatternTemplateToolCorrection, "1"));
			result.commonValues.toolCorrection = ( tc == "0" ? false : true );
			
		} else if ( child->GetName() == CncPatternCncParameterBlockNode ) {
			wxString a;
			result.cncParameterValues.include	= true;
			a.assign(child->GetAttribute(CncPatternCncParameterDepth, 		"z-0.000")); 	result.cncParameterValues.setDepth(a);
			a.assign(child->GetAttribute(CncPatternCncParameterReverse, 	"no"));			result.cncParameterValues.setReverse(a); 
			a.assign(child->GetAttribute(CncPatternCncParameterCorrection, 	"none"));		result.cncParameterValues.setCorrection(a);
			
		} else if ( child->GetName() == CncPatternTransformNode ) {
			wxString a;
			result.transformValues.autoMode 	= false;
			a.assign(child->GetAttribute(CncPatternTransformTranslateX, 	"0")); 	a.ToDouble(&result.transformValues.translateX);
			a.assign(child->GetAttribute(CncPatternTransformTranslateY, 	"0"));	a.ToDouble(&result.transformValues.translateY);
			a.assign(child->GetAttribute(CncPatternTransformScaleX, 		"1"));	a.ToDouble(&result.transformValues.scaleX);
			a.assign(child->GetAttribute(CncPatternTransformScaleY, 		"1"));	a.ToDouble(&result.transformValues.scaleY);
			a.assign(child->GetAttribute(CncPatternTransformRotateA, 		"0"));	a.ToDouble(&result.transformValues.rotateA);
			a.assign(child->GetAttribute(CncPatternTransformRotateX, 		"0"));	a.ToDouble(&result.transformValues.rotateX);
			a.assign(child->GetAttribute(CncPatternTransformRotateY, 		"0"));	a.ToDouble(&result.transformValues.rotateY);
			a.assign(child->GetAttribute(CncPatternTransformSkewX, 			"0"));	a.ToDouble(&result.transformValues.skewX);
			a.assign(child->GetAttribute(CncPatternTransformSkewY, 			"0"));	a.ToDouble(&result.transformValues.skewY);
			
		} else if ( child->GetName() == CncPatternParameterNode) {
			wxString index(child->GetAttribute(CncPatternParameterIndex, ""));
			wxString name(child->GetAttribute(CncPatternParameterName, ""));
			wxString type(child->GetAttribute(CncPatternParameterType, ""));
			wxString value(child->GetAttribute(CncPatternParameterValue, ""));
			
			int idx = pi.setupParameter(index, name, type, value);
			if ( idx < 0 ) {
				result.errorInfo << wxString::Format("Invalid parameter index: %d\n", idx);
				result.errorInfo << p;
				return false;
			}
			
			ParameterMap::iterator it;
			it = result.pMap.find(idx);
			if ( it != result.pMap.end() ) {
				result.errorInfo << wxString::Format("Duplicate parameter index: %d\n", idx);
				result.errorInfo << p;
				return false;
			}

			result.pMap[idx] = pi;
		}

		child = child->GetNext();
	}

	return true;
}
///////////////////////////////////////////////////////////////////
const wxString& PathGeneratorBase::maskXmlPattern(wxString& pattern) {
///////////////////////////////////////////////////////////////////
	pattern.Replace("\"", "'",   true);
	pattern.Replace("<",  "#lt", true);
	pattern.Replace(">",  "#gt", true);

	pattern.Replace("\n", "",    true);
	pattern.Replace("  ", "",    true);
	
	return pattern;
}
///////////////////////////////////////////////////////////////////
const wxString& PathGeneratorBase::demaskXmlPattern(wxString& pattern) {
///////////////////////////////////////////////////////////////////
	pattern.Replace("'",  "\"", true);
	pattern.Replace("#lt", "<", true);
	pattern.Replace("#gt", ">", true);

	return pattern;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorBase::createCncParameterBlock(SvgPathGroup& spg) {
///////////////////////////////////////////////////////////////////
	if ( cncParameterValues.include == true ) {
		CncParameterBlockMap cpm;
		getCncParameterBlock(cpm);
		
		wxString block(wxString::Format("<%s", CncPatternCncParameterBlockNode));
		
		// over all attributes
		for ( CncParameterBlockMap::reverse_iterator it=cpm.rbegin(); it!=cpm.rend(); ++it)
			block.append(wxString::Format(" %s=\"%s\"", it->first, it->second));

		block.append("/>");
		spg.addCncParameterBlock(block);
	}
}

///////////////////////////////////////////////////////////////////
const CncParameterBlockMap& PathGeneratorBase::getCncParameterBlock(CncParameterBlockMap& cpm) {
///////////////////////////////////////////////////////////////////
	cpm.clear();
	cpm[CncPatternCncParameterDepth]		= wxString::Format("%c%.3lf", cncParameterValues.getModeAsChar(), cncParameterValues.getDepth());
	cpm[CncPatternCncParameterReverse]		= wxString::Format("%s",      cncParameterValues.reverse ? "yes" : "no");
	cpm[CncPatternCncParameterCorrection] 	= wxString::Format("%s",      cncParameterValues.getCorrectAsString());

	return cpm;
}