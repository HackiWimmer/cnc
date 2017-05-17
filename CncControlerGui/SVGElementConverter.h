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
		static bool convertCircleToPathData(double cx, double cy, double r, wxString& ret) {
			wxXmlNode* n = getNewSvgElementNode("circle");
			n->AddAttribute("cx", wxString::Format("%lf", cx));
			n->AddAttribute("cy", wxString::Format("%lf", cy));
			n->AddAttribute("r",  wxString::Format("%lf", r));
			
			return convertCircleToPathData(n, ret);
		}
			
		//////////////////////////////////////////////////////////////////
		static bool convertCircleToPathData(wxXmlNode* child, wxString& ret) {
			if ( child == NULL )
				return false;

			// <circle cx="10" cy="10" r="5" fill="red" stroke="blue" stroke-width="10"  />
			// --> M 5,10 a 5,5 0 1,0 10,0 a 5,5 0 1,0 -10,0
			wxString cx = child->GetAttribute("cx", "");
			wxString cy = child->GetAttribute("cy", "");
			wxString r  = child->GetAttribute("r", "");

			double cxd, cyd, rd;
			cx.ToDouble(&cxd);
			cy.ToDouble(&cyd);
			r.ToDouble(&rd);

			wxString path("M ");
			path << (cxd - rd) << "," << cy;
			
			path << " a ";
			path << r   	<< ",";
			path << r   	<< " 0 0 1 ";
			path << (rd*2) 	<< ",0";
			
			path << " a ";
			path << r   	<< ",";
			path << r   	<< " 0 0 1 -";
			path << (rd*2)	<< ",0";

			//std::cerr << path.c_str() << std::endl;
			ret.assign(path);
			return true;
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertEllipseToPathData(double cx, double cy, double rx, double ry, wxString& ret) {
			wxXmlNode* n = getNewSvgElementNode("ellipse");
			n->AddAttribute("cx", wxString::Format("%lf", cx));
			n->AddAttribute("cy", wxString::Format("%lf", cy));
			n->AddAttribute("rx", wxString::Format("%lf", rx));
			n->AddAttribute("ry", wxString::Format("%lf", ry));
			
			return convertEllipseToPathData(n, ret);
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertEllipseToPathData(wxXmlNode* child, wxString& ret) {
			if ( child == NULL )
				return false;

			// <ellipse cx="10" cy="10" rx="5" ry="4" fill="red" stroke="blue" stroke-width="10"  />
			// --> M 5,10 a 5,4 0 1,0 10,0 a 5,4 0 1,0 -10,0
			wxString cx = child->GetAttribute("cx", "");
			wxString cy = child->GetAttribute("cy", "");
			wxString rx = child->GetAttribute("rx", "");
			wxString ry = child->GetAttribute("ry", "");
			
			double cxd, cyd, rxd, ryd;
			cx.ToDouble(&cxd);
			cy.ToDouble(&cyd);
			rx.ToDouble(&rxd);
			ry.ToDouble(&ryd);

			wxString path("M ");
			path << (cxd - rxd) << "," << cy;
			
			path << " a ";
			path << rx   	<< ",";
			path << ry   	<< " 0 0 1 ";
			path << (rxd*2) << ",0";
			
			path << " a ";
			path << rx   	<< ",";
			path << ry   	<< " 0 0 1 -";
			path << (rxd*2)	<< ",0";	
			
			ret.assign(path);
			return true;
		} 
		
		//////////////////////////////////////////////////////////////////
		static bool convertLineToPathData(double x1, double y1, double x2, double y2, wxString& ret) {
			wxXmlNode* n = getNewSvgElementNode("line");
			n->AddAttribute("x1", wxString::Format("%lf", x1));
			n->AddAttribute("y1", wxString::Format("%lf", y1));
			n->AddAttribute("x2", wxString::Format("%lf", x2));
			n->AddAttribute("y2", wxString::Format("%lf", y2));
			
			return convertLineToPathData(n, ret);
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertLineToPathData(wxXmlNode* child, wxString& ret) {
			if ( child == NULL )
				return false;

			 // <line x1="100" y1="300" x2="300" y2="100" stroke-width="5"  />
			wxString path("M ");
			wxString x1 = child->GetAttribute("x1", "");
			wxString y1 = child->GetAttribute("y1", "");
			wxString x2 = child->GetAttribute("x2", "");
			wxString y2 = child->GetAttribute("y2", "");
			
			path << x1 << " ";
			path << y1 << " ";
			path << "L ";
			path << x2 << " ";
			path << y2;
			
			ret.assign(path);
			return true;
		}
		
		//////////////////////////////////////////////////////////////////
		static wxString& evaluatePolyPoints(wxString points, wxString& path) {
			wxStringTokenizer tokenizerBlank(points, " ");
			unsigned int cnt=0; 
			while ( tokenizerBlank.HasMoreTokens() ) {
				wxString valuePair = tokenizerBlank.GetNextToken();
				wxStringTokenizer tokenizerComma(valuePair, ",");

				while ( tokenizerComma.HasMoreTokens() ) {
					wxString token = tokenizerComma.GetNextToken();
					path << token << " ";

					if ( ++cnt == 2 )
						path << "L ";
				}
			}
			return path;
		} 
		
		//////////////////////////////////////////////////////////////////
		static bool convertPolygonToPathData(wxString& points, wxString& ret) {
			wxXmlNode* n = getNewSvgElementNode("polygon");
			n->AddAttribute("points", points);
		
			return convertPolygonToPathData(n, ret);
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertPolygonToPathData(wxXmlNode* child, wxString& ret) {
			if ( child == NULL )
				return false;

			//polygone fill="none" stroke="blue" stroke-width="10" 
			//	points="10,10
			//			15,15
			//			20,10
			wxString path("M ");
			wxString points = child->GetAttribute("points", "");
			SVGElementConverter::evaluatePolyPoints(points, path);
			path << "z";

			//std::cerr << path.c_str() << std::endl;
			ret.assign(path);
			return true;
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertPolylineToPathData(wxString& points, wxString& ret) {
			wxXmlNode* n = getNewSvgElementNode("polyline");
			n->AddAttribute("x1", points);
		
			return convertPolylineToPathData(n, ret);
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertPolylineToPathData(wxXmlNode* child, wxString& ret) {
			if ( child == NULL )
				return false;

			//polyline fill="none" stroke="blue" stroke-width="10" 
			//	points="10,10
			//			15,15
			//			20,10
			//			25,15"
		 
			wxString path("M ");
			wxString points = child->GetAttribute("points", "");
			SVGElementConverter::evaluatePolyPoints(points, path);
			
			//std::cerr << path.c_str() << std::endl;
			ret.assign(path);
			return true;
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertLineToPathData(double x, double y, double w, double h,  double rx, double ry, wxString& ret) {
			wxXmlNode* n = getNewSvgElementNode("line");
			n->AddAttribute("x",      wxString::Format("%lf", x));
			n->AddAttribute("y",      wxString::Format("%lf", y));
			n->AddAttribute("width",  wxString::Format("%lf", w));
			n->AddAttribute("height", wxString::Format("%lf", h));
			n->AddAttribute("rx",     wxString::Format("%lf", rx));
			n->AddAttribute("ry",     wxString::Format("%lf", ry));
			
			return convertLineToPathData(n, ret);
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertRectToPathData(wxXmlNode* child, wxString& ret) {
			if ( child == NULL )
				return false;
				
			//<rect x="1" y="1" width="1198" height="398" fill="none" stroke="blue" stroke-width="2"/>
			/*
			Let rx and ry be length values.
			If neither ‘rx’ nor ‘ry’ are properly specified, then set both rx and ry to 0. (This will result in square corners.)
			Otherwise, if a properly specified value is provided for ‘rx’, but not for ‘ry’, then set both rx and ry to the value of ‘rx’.
			Otherwise, if a properly specified value is provided for ‘ry’, but not for ‘rx’, then set both rx and ry to the value of ‘ry’.
			Otherwise, both ‘rx’ and ‘ry’ were specified properly. Set rx to the value of ‘rx’ and ry to the value of ‘ry’.
			If rx is greater than half of ‘width’, then set rx to half of ‘width’.
			If ry is greater than half of ‘height’, then set ry to half of ‘height’.
			The effective values of ‘rx’ and ‘ry’ are rx and ry, respectively.
			*/
			
			wxString xs = child->GetAttribute("x", "");
			wxString ys = child->GetAttribute("y", "");
			wxString width = child->GetAttribute("width", "");
			wxString height = child->GetAttribute("height", "");
			
			wxString rxs = child->GetAttribute("rx", "");
			wxString rys = child->GetAttribute("ry", "");
			long x=0, y=0, h=0, w=0, rx=0, ry=0;
			xs.ToLong(&x);
			ys.ToLong(&y);
			width.ToLong(&w);
			height.ToLong(&h);
			rxs.ToLong(&rx);
			rys.ToLong(&ry);

			// validation
			if ( rx > 0 && ry == 0)  ry = rx;
			if ( ry > 0 && rx == 0)  rx = ry;
			if ( rx > w/2 )			 rx = w/2;
			if ( ry > h/2 )			 ry = h/2;
			
			wxString path("M ");
			if ( rx == 0 ) {
				path << xs << " ";
				path << ys << " ";
				path << "l ";
				path << width  << " 0 ";
				path << "0 " << height;
				path << " -" << width << " 0 ";
				path << "z";
				
			} else {
				 
				//perform an absolute moveto operation to location (x+rx,y)
				path << x + rx << " " << y;
				//perform an absolute horizontal lineto operation to location (x+width-rx,y), 
				path << " L " << x + w - rx << " " << y;
				//perform an absolute elliptical arc operation to coordinate (x+width,y+ry), 
				//where rx and ry attributes on the elliptical arc command, respectively, 
				// the x-axis-rotation is set to zero, the large-arc-flag is set to zero, and the sweep-flag is set to one
				path << " A " << rx << " " << ry << " 0 0 1 " << x + w << " " << y + ry;
				//perform a absolute vertical lineto to location (x+width,y+height-ry)
				path << " L " << x + w << " " << y + h - ry;
				//perform an absolute elliptical arc operation to coordinate (x+width-rx,y+height)
				path << " A " << rx << " " << ry << " 0 0 1 " << x + w -rx << " " << y + h;
				//perform an absolute horizontal lineto to location (x+rx,y+height)
				path << " L " << x + rx << " " << y + h;
				//perform an absolute elliptical arc operation to coordinate (x,y+height-ry)
				path << " A " << rx << " " << ry << " 0 0 1 " << x << " " << y + h - ry;
				//perform an absolute vertical lineto to location (x,y+ry)
				path << " L " << x << " " << y + ry;
				//perform an absolute elliptical arc operation to coordinate (x+rx,y)		
				path << " A " << rx << " " << ry << " 0 0 1 " << x + rx << " " << y;
			}

			//std::clog << path.c_str() << std::endl;
			ret.assign(path);
			return true;
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertNodeToPathData(wxXmlNode* child, wxString& ret, SvgNodeAttributeMap& aMap) {
			if ( child == NULL ) {
				_errorInfo.append("The given node is NULL\n");
				return false;
			}
			
			// recycle map
			aMap.clear();
			
			// add element name
			aMap["SVG Element"] = child->GetName();
			
			// collect attributes
			wxXmlAttribute* attributes = child->GetAttributes();
			while ( attributes ) {
				aMap[attributes->GetName()] = attributes->GetValue();
				attributes = attributes->GetNext();
			}
			
			// perform ret
			if ( child->GetName().Upper() == "PATH" ) {
				ret.assign(child->GetAttribute("d", ""));
				if ( ret.IsEmpty() ) {
					_errorInfo.append("Cant find the attribute 'd' or it is empty\n");
					return false;
				}
				
				return true;
				
			} else if ( child->GetName().Upper() == "CIRCLE" ) {
				return convertCircleToPathData(child, ret);
				
			} else if ( child->GetName().Upper() == "ELLIPSE" ) {
				return convertEllipseToPathData(child, ret);
				
			} else if ( child->GetName().Upper() == "LINE" ) {
				return convertLineToPathData(child, ret);
				
			} else if ( child->GetName().Upper() == "POLYGON" ) {
				return convertPolygonToPathData(child, ret);
				
			} else if ( child->GetName().Upper() == "POLYLINE" ) {
				return convertPolylineToPathData(child, ret);
				
			} else if ( child->GetName().Upper() == "RECT" ) {
				return convertRectToPathData(child, ret);
				
			}
			
			_errorInfo.append(wxString::Format("Not supported element type: %s\n", child->GetName()));
			return false;
		}
		
		//////////////////////////////////////////////////////////////////
		static bool convertNodeToPathData(const wxString& node, wxString& ret, SvgNodeAttributeMap& aMap) {
			wxXmlDocument xmlDoc;
			wxStringInputStream xmlStream(node);
			wxLogNull dummyToSuppressXmlDocErrorMessages;
			if ( xmlDoc.Load(xmlStream) == false ) {
				_errorInfo.append("XmlDocument.Load failed\n");
				return false;
				
			}
			
			wxXmlNode* root = xmlDoc.GetRoot();
			if ( root == NULL ) {
				_errorInfo.append("XmlDocument.GetRoot is NULL\n");
				return false;
			}
			
			return convertNodeToPathData(root, ret, aMap);
		}
};

#endif