#ifndef SVG_PATH_STRING_H
#define SVG_PATH_STRING_H

#include "SvgGeneratorBase.h"
#include "SvgPathFragment.h"

////////////////////////////////////////////////////////////////////////////////////
class SvgPathString : public SvgGeneratorBase {
	
	protected:
		SVGUnit inputUnit;
		wxString pathStr;
		SvgPathFragment generator;
		
		void initPath();
		void finishPath();
		
	public:
		////////////////////////////////////////////////////////////////////////////
		SvgPathString(SVGUnit i = mm) 
		: SvgGeneratorBase()
		, inputUnit(i)
		, pathStr()
		, generator(i)
		{
			initPath();
		}
		////////////////////////////////////////////////////////////////////////////
		virtual ~SvgPathString() {
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setColour(const wxColour& c) {
			SvgGeneratorBase::setColour(c);
			fGen().setColour(c);
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setReferencePoint(const wxRealPoint& p) {
			SvgGeneratorBase::setReferencePoint(p);
			fGen().setReferencePoint(p);
		}
		
		////////////////////////////////////////////////////////////////////////////
		virtual void setXmlPattern(const wxString& pattern) {
			SvgGeneratorBase::setXmlPattern(pattern);
			fGen().setXmlPattern(pattern);
		}
		
		////////////////////////////////////////////////////////////////////////////
		SvgPathFragment& fGen() { return generator; }
		
		////////////////////////////////////////////////////////////////////////////
		void add(const wxString& pf);
		bool addPocketWhole(double td, double rx, double ry, bool correctRadius);
		
		////////////////////////////////////////////////////////////////////////////
		bool addPath(const wxString& p);
		
		////////////////////////////////////////////////////////////////////////////
		const wxString& get();
		
		////////////////////////////////////////////////////////////////////////////
		virtual bool hasError() { return (SvgGeneratorBase::hasError() || fGen().hasError()); }
		
		////////////////////////////////////////////////////////////////////////////
		virtual const wxString& getErrorInfo() {
			static wxString s;
			
			s = "";
			s << fGen().getErrorInfo();
			s << SvgGeneratorBase::getErrorInfo();
			
			return s;
		}
};

#endif