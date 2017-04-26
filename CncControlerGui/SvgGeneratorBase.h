#ifndef SVG_GENERATOR_BASE_H
#define SVG_GENERATOR_BASE_H

#include <vector>
#include <wx/string.h>
#include "SvgUnitCalculator.h"

////////////////////////////////////////////////////////////////////////////////////
class SvgGeneratorBase {
	protected:
	
		std::vector<wxString> errors;
		wxColour colour;
		wxString xmlPattern;
		wxRealPoint referencePoint;
		
		////////////////////////////////////////////////////////////////////////////
		SvgGeneratorBase()
		: colour(0,0,0)
		, xmlPattern("")
		, referencePoint(0,0)
		{}
		
		////////////////////////////////////////////////////////////////////////////
		virtual ~SvgGeneratorBase() 
		{}
		
		////////////////////////////////////////////////////////////////////////////
		void clearErrors() { errors.clear(); }
		void addError(const wxString& et) { errors.push_back(et); }
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& getDefaultStroke() {
			static wxString s;
			s.assign(wxString::Format("\n   stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"1.0\"", colour.Red(), colour.Green(), colour.Blue()));
			return s;
		}
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& getDefaultSvgElementEnd() {
			static wxString s;
			s.assign(getDefaultStroke());
			return s;
		}
		
	public:
	
		////////////////////////////////////////////////////////////////////////////
		static wxString svgCommentStart;
		static wxString svgCommentEnd;
		static wxString svgCommentPlaceholderStart;
		static wxString svgCommentPlaceholderEnd;
		static wxString svgConfigPlaceholderBlock;
		
		////////////////////////////////////////////////////////////////////////////
		static const wxString& finalizeSvgFragment(wxString& fragment, bool preview) {
			wxString s(( preview == true ? "" : SvgGeneratorBase::svgCommentStart ));
			wxString e(( preview == true ? "" : SvgGeneratorBase::svgCommentEnd ));
			
			fragment.Replace(SvgGeneratorBase::svgCommentPlaceholderStart, s, true);
			fragment.Replace(SvgGeneratorBase::svgCommentPlaceholderEnd, e, true);
			
			return fragment;
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setColour(const wxColour& c) {
			colour = c;
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setReferencePoint(const wxRealPoint& p) {
			referencePoint = p;
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setXmlPattern(const wxString& pattern) {
			xmlPattern = pattern;
		}
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& getXmlPattern() const {
			return xmlPattern;
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual bool hasError() { return (errors.size() != 0); }
		
		////////////////////////////////////////////////////////////////////////////
		virtual const wxString& getErrorInfo() {
			static wxString s;
			s.clear();
			
			for (std::vector<wxString>::iterator it = errors.begin() ; it != errors.end(); ++it)
				s << *it << '\n';
				
			return s;
		}
};

#endif