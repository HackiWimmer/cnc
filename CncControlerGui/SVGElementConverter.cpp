#include <iostream>
#include "SVGElementConverter.h"

wxXmlNode* SVGElementConverter::_xmlNode 	= NULL;
wxString SVGElementConverter::_errorInfo 	= _T("");

/////////////////////////////////////////////////////////////////////////
class SVGElementConverterInitializer {
	public:
		SVGElementConverterInitializer() {
			SVGElementConverter::init();
		}
		~SVGElementConverterInitializer() {
			SVGElementConverter::cleanUp();
		}
		
}; SVGElementConverterInitializer seci;


//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertCircleToPathData(double cx, double cy, double r, wxString& ret) {
//////////////////////////////////////////////////////////////////
	wxXmlNode* n = getNewSvgElementNode("circle");
	n->AddAttribute("cx", wxString::Format("%lf", cx));
	n->AddAttribute("cy", wxString::Format("%lf", cy));
	n->AddAttribute("r",  wxString::Format("%lf", r));
	
	return convertCircleToPathData(n, ret);
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertCircleToPathData(wxXmlNode* child, wxString& ret) {
//////////////////////////////////////////////////////////////////
	if ( child == NULL )
		return false;

	// <circle cx="10" cy="10" r="5" fill="red" stroke="blue" stroke-width="10"  />
	// --> M 5,10 a 5,5 0 1,0 10,0 a 5,5 0 1,0 -10,0
	const wxString& cx = child->GetAttribute("cx", "");
	const wxString& cy = child->GetAttribute("cy", "");
	const wxString& r  = child->GetAttribute("r", "");

	double cxd, cyd, rd;
	cx.ToDouble(&cxd);
	cy.ToDouble(&cyd);
	r.ToDouble(&rd);

	ret.assign("M ");
	ret << (cxd - rd) << "," << cy;
	
	ret << " a ";
	ret << r   	<< ",";
	ret << r   	<< " 0 0 1 ";
	ret << (rd*2) 	<< ",0";
	
	ret << " a ";
	ret << r   	<< ",";
	ret << r   	<< " 0 0 1 -";
	ret << (rd*2)	<< ",0";
	
	//std::cerr << ret << std::endl;
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertEllipseToPathData(double cx, double cy, double rx, double ry, wxString& ret) {
//////////////////////////////////////////////////////////////////
	wxXmlNode* n = getNewSvgElementNode("ellipse");
	n->AddAttribute("cx", wxString::Format("%lf", cx));
	n->AddAttribute("cy", wxString::Format("%lf", cy));
	n->AddAttribute("rx", wxString::Format("%lf", rx));
	n->AddAttribute("ry", wxString::Format("%lf", ry));
	
	return convertEllipseToPathData(n, ret);
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertEllipseToPathData(wxXmlNode* child, wxString& ret) {
//////////////////////////////////////////////////////////////////
	if ( child == NULL )
		return false;

	// <ellipse cx="10" cy="10" rx="5" ry="4" fill="red" stroke="blue" stroke-width="10"  />
	// --> M 5,10 a 5,4 0 1,0 10,0 a 5,4 0 1,0 -10,0
	const wxString& cx = child->GetAttribute("cx", "");
	const wxString& cy = child->GetAttribute("cy", "");
	const wxString& rx = child->GetAttribute("rx", "");
	const wxString& ry = child->GetAttribute("ry", "");
	
	double cxd, cyd, rxd, ryd;
	cx.ToDouble(&cxd);
	cy.ToDouble(&cyd);
	rx.ToDouble(&rxd);
	ry.ToDouble(&ryd);

	ret.assign("M ");
	ret << (cxd - rxd) << "," << cy;
	
	ret << " a ";
	ret << rx   	<< ",";
	ret << ry   	<< " 0 0 1 ";
	ret << (rxd*2) << ",0";
	
	ret << " a ";
	ret << rx   	<< ",";
	ret << ry   	<< " 0 0 1 -";
	ret << (rxd*2)	<< ",0";
	
	return true;
} 
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertLineToPathData(double x1, double y1, double x2, double y2, wxString& ret) {
//////////////////////////////////////////////////////////////////
	wxXmlNode* n = getNewSvgElementNode("line");
	n->AddAttribute("x1", wxString::Format("%lf", x1));
	n->AddAttribute("y1", wxString::Format("%lf", y1));
	n->AddAttribute("x2", wxString::Format("%lf", x2));
	n->AddAttribute("y2", wxString::Format("%lf", y2));
	
	return convertLineToPathData(n, ret);
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertLineToPathData(wxXmlNode* child, wxString& ret) {
//////////////////////////////////////////////////////////////////
	if ( child == NULL )
		return false;

	 // <line x1="100" y1="300" x2="300" y2="100" stroke-width="5"  />
	ret.assign("M ");
	const wxString& x1 = child->GetAttribute("x1", "");
	const wxString& y1 = child->GetAttribute("y1", "");
	const wxString& x2 = child->GetAttribute("x2", "");
	const wxString& y2 = child->GetAttribute("y2", "");
	
	ret << x1 << " ";
	ret << y1 << " ";
	ret << "L ";
	ret << x2 << " ";
	ret << y2;
	
	return true;
}
//////////////////////////////////////////////////////////////////
wxString& SVGElementConverter::evaluatePolyPoints(const wxString& points, wxString& path) {
//////////////////////////////////////////////////////////////////
	wxStringTokenizer tokenizerBlank(points, " ");
	unsigned int cnt=0; 
	while ( tokenizerBlank.HasMoreTokens() ) {
		const wxString& valuePair = tokenizerBlank.GetNextToken();
		wxStringTokenizer tokenizerComma(valuePair, ",");

		while ( tokenizerComma.HasMoreTokens() ) {
			const wxString& token = tokenizerComma.GetNextToken();
			path << token << " ";

			if ( ++cnt == 2 )
				path << "L ";
		}
	}
	return path;
} 
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertPolygonToPathData(wxString& points, wxString& ret) {
//////////////////////////////////////////////////////////////////
	wxXmlNode* n = getNewSvgElementNode("polygon");
	n->AddAttribute("points", points);

	return convertPolygonToPathData(n, ret);
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertPolygonToPathData(wxXmlNode* child, wxString& ret) {
//////////////////////////////////////////////////////////////////
	if ( child == NULL )
		return false;

	ret.assign("M ");
	const wxString& points = child->GetAttribute("points", "");
	SVGElementConverter::evaluatePolyPoints(points, ret);
	ret.append("z");
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertPolylineToPathData(wxString& points, wxString& ret) {
//////////////////////////////////////////////////////////////////
	wxXmlNode* n = getNewSvgElementNode("polyline");
	n->AddAttribute("points", points);

	return convertPolylineToPathData(n, ret);
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertPolylineToPathData(wxXmlNode* child, wxString& ret) {
//////////////////////////////////////////////////////////////////
	if ( child == NULL )
		return false;
	
	ret.assign("M ");
	const wxString& points = child->GetAttribute("points", "");
	SVGElementConverter::evaluatePolyPoints(points, ret);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertRectToPathData(double x, double y, double w, double h,  double rx, double ry, wxString& ret) {
//////////////////////////////////////////////////////////////////
	wxXmlNode* n = getNewSvgElementNode("line");
	n->AddAttribute("x",      wxString::Format("%lf", x));
	n->AddAttribute("y",      wxString::Format("%lf", y));
	n->AddAttribute("width",  wxString::Format("%lf", w));
	n->AddAttribute("height", wxString::Format("%lf", h));
	n->AddAttribute("rx",     wxString::Format("%lf", rx));
	n->AddAttribute("ry",     wxString::Format("%lf", ry));
	
	return convertRectToPathData(n, ret);
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertRectToPathData(wxXmlNode* child, wxString& ret) {
//////////////////////////////////////////////////////////////////
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
	
	const wxString& xs		= child->GetAttribute("x", "");
	const wxString& ys		= child->GetAttribute("y", "");
	const wxString& width	= child->GetAttribute("width", "");
	const wxString& height	= child->GetAttribute("height", "");
	
	const wxString& rxs		= child->GetAttribute("rx", "");
	const wxString& rys		= child->GetAttribute("ry", "");
	
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
	
	ret.assign("M ");
	
	if ( rx == 0 ) {
		ret	<< xs << " " << ys << " "
			<< "l " << width << " 0 "
			<< "0 " << height
			<< " -" << width << " 0 "
			<< "z"
			;
		
	} else {
		
		#warning convertRectToPathData sweepFlag = 0;
		// Note: With respect to the svg description below the sweep flag has to be 1
		// but this results in a wrong display regarding the further implementation. 
		// It's currently not clear were the wrong code is located and the rest of the 
		// elliptical arc drawing seams to be ok, therefore this work around
		const int sweepFlag = 0;
		 
		//perform an absolute moveto operation to location (x+rx,y)
		ret << x + rx << " " << y;
		//perform an absolute horizontal lineto operation to location (x+width-rx,y), 
		ret << " L " << x + w - rx << " " << y;
		//perform an absolute elliptical arc operation to coordinate (x+width,y+ry), 
		//where rx and ry attributes on the elliptical arc command, respectively, 
		//the x-axis-rotation is set to zero, the large-arc-flag is set to zero, and the sweep-flag is set to one
		ret << " A " << rx << " " << ry << " 0 0 " << sweepFlag << " " << x + w << " " << y + ry;
		//perform a absolute vertical lineto to location (x+width,y+height-ry)
		ret << " L " << x + w << " " << y + h - ry;
		
		//perform an absolute elliptical arc operation to coordinate (x+width-rx,y+height)
		ret << " A " << rx << " " << ry << " 0 0 " << sweepFlag << " " << x + w -rx << " " << y + h;
		//perform an absolute horizontal lineto to location (x+rx,y+height)
		ret << " L " << x + rx << " " << y + h;
		//perform an absolute elliptical arc operation to coordinate (x,y+height-ry)
		ret << " A " << rx << " " << ry << " 0 0 " << sweepFlag << " " << x << " " << y + h - ry;
		//perform an absolute vertical lineto to location (x,y+ry)
		ret << " L " << x << " " << y + ry;
		//perform an absolute elliptical arc operation to coordinate (x+rx,y)
		ret << " A " << rx << " " << ry << " 0 0 " << sweepFlag << " " << x + rx << " " << y;
	}
	
	//std::clog << ret << std::endl;
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGElementConverter::convertNodeToPathData(wxXmlNode* child, wxString& ret, SvgNodeAttributeMap& aMap) {
//////////////////////////////////////////////////////////////////
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
			_errorInfo.append("Can't find the attribute 'd' or it is empty\n");
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
bool SVGElementConverter::convertNodeToPathData(const wxString& node, wxString& ret, SvgNodeAttributeMap& aMap) {
//////////////////////////////////////////////////////////////////
	wxXmlDocument xmlDoc;
	wxStringInputStream xmlStream(node);
	//wxLogNull dummyToSuppressXmlDocErrorMessages;
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
