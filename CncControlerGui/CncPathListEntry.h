#ifndef CNC_PATH_LIST_ENTRY_H
#define CNC_PATH_LIST_ENTRY_H

#include <ostream>
#include <vector>
#include "OSD/CncTimeFunctions.h"
#include "CncPosition.h"
#include "CncCommon.h"

struct CncPathListEntry {

	// -----------------------------------------------------------
	// Default
	static const bool 				DefaultAlreadyRendered;
	static const long 				DefaultClientID;
	static const CncNanoTimestamp	NoReference;
	static const CncSpeedMode 		DefaultSpeedMode;
	static const double				DefaultSpeedValue;
	static const CncDoublePosition	ZeroTarget;
	static const CncDoubleDistance	NoDistance;

	static const int				CONT_NOTHING	= 1;
	static const int				CONT_CLIENTID	= 2;
	static const int				CONT_SPEED		= 4;
	static const int				CONT_POSITION	= 8;
	
	enum Type { 
		TYPE_NOTHING	= CONT_NOTHING, 
		TYPE_CLIENTID	= CONT_CLIENTID, 
		TYPE_SPEED		= CONT_SPEED, 
		TYPE_POSITION	= CONT_POSITION
	};
	
	static int  transform(Type t) { return (int)t;  }
	static Type transform(int i)  { return (Type)i; }
	
	// -----------------------------------------------------------
	CncPathListEntry() = default;
	CncPathListEntry(Type t) : content(transform(t)) {}
	
	// -----------------------------------------------------------
	// copy semantic
	CncPathListEntry(const CncPathListEntry&) = default;
	CncPathListEntry& operator= (const CncPathListEntry&) = default;

	// -----------------------------------------------------------
	// move semantic
	CncPathListEntry(CncPathListEntry&&) = default;
	CncPathListEntry& operator= (CncPathListEntry&&) = default;

	// -----------------------------------------------------------
	// Format
	wxString traceIndent					= "  ";
	
	// -----------------------------------------------------------
	// Data
	long				clipperIndex		= -1;
	long				listIndex			= -1;

	int					content				= CONT_NOTHING;
	CncNanoTimestamp	pathListReference  	= NoReference;

	long				clientId			= DefaultClientID;

	CncDoublePosition	entryTarget			= ZeroTarget;
	CncDoubleDistance	entryDistance		= NoDistance;
	bool 				alreadyRendered		= DefaultAlreadyRendered;

	double				totalDistance		=  0.0;

	CncSpeedMode		feedSpeedMode		= DefaultSpeedMode;
	double				feedSpeed_MM_MIN	= DefaultSpeedValue;

	// -----------------------------------------------------------
	// Interface
	void setNothingChanged()			{ content  = CONT_NOTHING;  }
	void setClientIdChange()			{ content  = CONT_CLIENTID; }
	void setSpeedChange() 				{ content  = CONT_SPEED;    }
	void setPositionChange()			{ content  = CONT_POSITION; }

	void addClientIdChange()			{ content |= CONT_CLIENTID; }
	void addSpeedChange() 				{ content |= CONT_SPEED;    }
	void addPositionChange()			{ content |= CONT_POSITION; }

	bool isNothingChanged()		const	{ return content == CONT_NOTHING;  }
	bool isClientIdChange()		const	{ return content == CONT_CLIENTID; }
	bool isSpeedChange() 		const	{ return content == CONT_SPEED;    }
	bool isPositionChange()		const	{ return content == CONT_POSITION; }

	bool hasClientIdChange()	const	{ return content  & CONT_CLIENTID; }
	bool hasSpeedChange()		const	{ return content  & CONT_SPEED;    }
	bool hasPositionChange()	const	{ return content  & CONT_POSITION; }

	void traceEntry(std::ostream& ostr) const;
	const wxString& traceEntryToString(wxString& ret) const;

	std::ostream& outputOperator(std::ostream &ostr) const;
	friend std::ostream &operator<<(std::ostream &ostr, const CncPathListEntry &a) {
		return a.outputOperator(ostr);
	}
};

typedef std::vector<CncPathListEntry>	CncPathList;

#endif
