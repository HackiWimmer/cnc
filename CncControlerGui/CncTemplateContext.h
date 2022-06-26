#ifndef CNC_TEMPLATE_CONTEXT_H
#define CNC_TEMPLATE_CONTEXT_H

#include <vector>
#include <iostream>
#include <wx/string.h>
#include "ContextInterface.h"
#include "CncPosition.h"

class CncBoundarySpace;
class CncPathListInterfaceCnc;

class CncTemplateContext : public ContextInterface 
{
	private:
		
		bool						modifyFlag;
		wxString					name;
		wxString					path;
		wxString					toolTotList;
		wxString					toolSelList;
		unsigned int				runCount;
		unsigned int				validRunCount;
		
		CncBoundarySpace*			boundarySpace;
		CncDoubleBoundaries			templateBounds;
		CncDoubleBoundaries			measuredBounds;
		
		CncPathListInterfaceCnc*	cncInterface;
		
	protected:
		
		explicit CncTemplateContext(const CncTemplateContext&)		= default;
		explicit CncTemplateContext(CncTemplateContext&&)			= default;
		CncTemplateContext& operator= (const CncTemplateContext&)	= default;
		CncTemplateContext& operator= (CncTemplateContext&&)		= default;
		
	public:
		
		enum BoundType { BT_TEMPLATE, BT_MEASURED };
		
		CncTemplateContext(CncBoundarySpace* bs);
		
		bool				isValid()						const;
		void				reset();
		
		bool				init(const wxString& pathFileName);
		bool				init(const wxString& path, const wxString& name);
		
		
		void				setModifyFlag(bool f)					{ modifyFlag = f;		updateGui(true); }
		void				registerRun()							{ runCount++;			updateGui(true); }
		void				resetValidRuns()						{ validRunCount = 0; 	updateGui(true); }
		void				registerValidRun()						{ validRunCount++; 		updateGui(true); }
		bool				registerCncInterface(CncPathListInterfaceCnc* ci);
		void				unregisterCncInterface();
		bool				hasValidRuns()					const	{ return validRunCount > 0; }
		bool				getModifyFlag()					const	{ return modifyFlag; }
		unsigned int		getValidRunCount()				const	{ return validRunCount; }
		unsigned int		getRunCount()					const	{ return runCount; }
		
		void				registerBoundaries(const CncDoubleBoundaries& b, BoundType bt);
		const CncDoubleBoundaries& getBoundaries(BoundType bt)	const;
		const CncDoubleBoundaries& getBoundaries()				const;
		bool				hasBoundaries(BoundType bt)			const;
		bool				hasBoundaries()						const;
		BoundType 			getBoundLevel()						const;
		
		const wxString&		getName()							const	{ return name; }
		const wxString&		getPath()							const	{ return path; }
		const wxString		getFileName()						const;
		CncTemplateFormat	getTemplateFormat()					const;
		bool				isTemplateFormat(CncTemplateFormat)	const;
		
		void				registerToolTotList(const wxString& tl)	{ toolTotList.assign(tl); }
		void				registerToolSelList(const wxString& tl)	{ toolSelList.assign(tl); }
		unsigned int		getToolSelCount()				const	{ return toolSelList.Freq(';'); }
		
		void				updateGui(bool force)							const;
		void				traceTo(std::ostream& o, unsigned int indent)	const;
		bool				save(const wxFileName& fn)						const;
		
		bool				fitsIntoCurrentHardwareBoundaries(std::ostream& o);
		
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
		
		
		bool executeCncInterface();
};

#endif