#ifndef CNC_TEMPLATE_CONTEXT_H
#define CNC_TEMPLATE_CONTEXT_H

#include <vector>
#include <iostream>
#include <wx/string.h>
#include "CncPosition.h"

class CncBoundarySpace;
class CncTemplateContext {
	
	private:
		
		typedef std::vector<wxString> LogInformation;
		
		wxString					name;
		wxString					path;
		wxString					toolTotList;
		wxString					toolSelList;
		unsigned int				validRunCount;
		
		CncBoundarySpace*			bounderySpace;
		CncDoubleBounderies			bounderies;
		LogInformation				logInformation;
		
	protected:
		
		explicit CncTemplateContext(const CncTemplateContext&)		= default;
		explicit CncTemplateContext(CncTemplateContext&&)			= default;
		CncTemplateContext& operator= (const CncTemplateContext&)	= default;
		CncTemplateContext& operator= (CncTemplateContext&&)		= default;
		
	public:
		
		CncTemplateContext(CncBoundarySpace*);
		
		bool				isValid()						const;
		void				reset();
		
		bool				init(const wxString& pathFileName);
		bool				init(const wxString& path, const wxString& name);
		
		void				resetValidRuns()						{ validRunCount = 0; }
		void				registerValidRun()						{ validRunCount++; updateGui(false); }
		bool				hasValidRuns()					const	{ return validRunCount > 0; }
		unsigned int		getValidunCount()				const	{ return validRunCount; }
		
		void				registerBounderies(const CncDoubleBounderies& b);
		bool				hasBounderies()					const	{ return bounderies.hasBounderies(); }
		
		const wxString&		getName()						const	{ return name; }
		const wxString&		getPath()						const	{ return path; }
		const wxString		getFileName()					const;
		
		void				registerToolTotList(const wxString& tl)	{ toolTotList.assign(tl); }
		void				registerToolSelList(const wxString& tl)	{ toolSelList.assign(tl); }
		unsigned int		getToolSelCount()				const	{ return toolSelList.Freq(';'); }
		
		void				updateGui(bool force)			const;
		void				traceTo(std::ostream& o, unsigned int indent) const;
		
		void				streamLogInfo(std::ostream& o, unsigned int indent) const;
		const wxString		getLastLogInfo(unsigned int indent = 0)				const;
		void				addLogInfo(const wxString& info)		{ logInformation.push_back(info); }
		void				clearLogInfo()							{ logInformation.clear(); }
};

#endif