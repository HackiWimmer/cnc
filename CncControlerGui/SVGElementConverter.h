#ifndef SVG_ELEMENT_CONVERTER_H
#define SVG_ELEMENT_CONVERTER_H

#include <map>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

typedef std::map<wxString, wxString> SvgNodeAttributeMap;

//////////////////////////////////////////////////////////////////////////
class SVGElementConverter {
	
	private:
		static wxXmlNode* _xmlNode;
		static wxString   _errorInfo;
		
	public:
		
		//////////////////////////////////////////////////////////////////
		static void init() {
		}
		
		//////////////////////////////////////////////////////////////////
		static void cleanUp() {
			if ( _xmlNode != NULL )
				delete _xmlNode;
		}
		
		//////////////////////////////////////////////////////////////////
		static const wxString& getErrorInfo() {
			return _errorInfo;
		}
		
		//////////////////////////////////////////////////////////////////
		static void resetErrorInfo() {
			_errorInfo.clear();
		}
		
		//////////////////////////////////////////////////////////////////
		static wxXmlNode* getNewSvgElementNode(const wxString& name) {
			if ( _xmlNode != NULL )
				delete _xmlNode;
				
			return new wxXmlNode(NULL, wxXML_ELEMENT_NODE, name);
		}
		
		//////////////////////////////////////////////////////////////////
		static wxString& evaluatePolyPoints(const wxString& points, wxString& path);

		static bool convertCircleToPathData(double cx, double cy, double r, wxString& ret);
		static bool convertCircleToPathData(wxXmlNode* child, wxString& ret);
		static bool convertEllipseToPathData(double cx, double cy, double rx, double ry, wxString& ret);
		static bool convertEllipseToPathData(wxXmlNode* child, wxString& ret);
		static bool convertLineToPathData(double x1, double y1, double x2, double y2, wxString& ret);
		static bool convertLineToPathData(wxXmlNode* child, wxString& ret);
		static bool convertPolygonToPathData(wxString& points, wxString& ret);
		static bool convertPolygonToPathData(wxXmlNode* child, wxString& ret);
		static bool convertPolylineToPathData(wxString& points, wxString& ret);
		static bool convertPolylineToPathData(wxXmlNode* child, wxString& ret);
		static bool convertRectToPathData(double x, double y, double w, double h,  double rx, double ry, wxString& ret);
		static bool convertRectToPathData(wxXmlNode* child, wxString& ret);
		static bool convertNodeToPathData(wxXmlNode* child, wxString& ret, SvgNodeAttributeMap& aMap);
		static bool convertNodeToPathData(const wxString& node, wxString& ret, SvgNodeAttributeMap& aMap);
};

#endif
