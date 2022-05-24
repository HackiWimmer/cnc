#ifndef CNC_TEMPLATE_CONTEXT_H
#define CNC_TEMPLATE_CONTEXT_H

#include <vector>
#include <iostream>
#include <wx/string.h>
#include "ContextInterface.h"
#include "CncPosition.h"

class CncBoundarySpace;
class CncTemplateContext : public ContextInterface {
	
	private:
		
		typedef std::vector<wxString> LogInformation;
		
		wxString					name;
		wxString					path;
		wxString					toolTotList;
		wxString					toolSelList;
		unsigned int				runCount;
		unsigned int				validRunCount;
		
		CncBoundarySpace*			boundarySpace;
		CncDoubleBoundaries			boundaries;
		
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
		
		void				registerRun()							{ runCount++; updateGui(false); }
		void				resetValidRuns()						{ validRunCount = 0; }
		void				registerValidRun()						{ validRunCount++; updateGui(false); }
		bool				hasValidRuns()					const	{ return validRunCount > 0; }
		unsigned int		getValidRunCount()				const	{ return validRunCount; }
		unsigned int		getRunCount()					const	{ return runCount; }
		
		void				registerBoundaries(const CncDoubleBoundaries& b);
		bool				hasBoundaries()					const	{ return boundaries.hasBoundaries(); }
		
		const wxString&		getName()						const	{ return name; }
		const wxString&		getPath()						const	{ return path; }
		const wxString		getFileName()					const;
		
		void				registerToolTotList(const wxString& tl)	{ toolTotList.assign(tl); }
		void				registerToolSelList(const wxString& tl)	{ toolSelList.assign(tl); }
		unsigned int		getToolSelCount()				const	{ return toolSelList.Freq(';'); }
		
		void				updateGui(bool force)			const;
		void				traceTo(std::ostream& o, unsigned int indent) const;
		
		// context interface
		virtual void		notifyBeginRun();
		virtual void		notifyEndRun();
		
		virtual void		notifyClientId(long id);
		
		virtual void		notifyLimit(const CncInterface::ILS::States& s);
		virtual void		notifyMove(unsigned char cmd, int32_t dx, int32_t dy, int32_t dz);
		
		virtual void		notifySpindleOn();
		virtual void		notifySpindleOff();
		
		virtual void		notifySpindleSpeed(unsigned char pid, ArdoObj::SpindleTupleValue s);
		virtual void		notifyStepperSpeed(unsigned char pid, ArdoObj::SpeedTupleValue s);
};

#endif