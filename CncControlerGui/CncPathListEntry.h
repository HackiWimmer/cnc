#ifndef CNC_PATH_LIST_ENTRY_H
#define CNC_PATH_LIST_ENTRY_H

#include <ostream>
#include <vector>
#include "OSD/CncTimeFunctions.h"
#include "CncPosition.h"
#include "CncCommon.h"

struct CncPathListEntry{

	// -----------------------------------------------------------
	// Default
	static const bool 				DefaultAlreadyRendered;
	static const long 				DefaultClientID;
	static const CncNanoTimestamp	NoReference;
	static const CncSpeedMode 		DefaultSpeedMode;
	static const double				DefaultSpeedValue;
	static const CncDoublePosition	ZeroTarget;
	static const CncDoubleDistance	NoDistance;

	// -----------------------------------------------------------
	// Format
	wxString traceIndent					= "  ";
	
	// -----------------------------------------------------------
	// Data
	enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_SPEED=2, CHG_POSITION=3};

	long				listIndex			= -1;

	Type				type				= CHG_NOTHING;
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
	bool isNothingChanged() const { return type == CHG_NOTHING; }
	bool isClientIdChange() const { return type == CHG_CLIENTID; }
	bool isPositionChange()	const { return type == CHG_POSITION; }
	bool isSpeedChange() 	const { return type == CHG_SPEED; 	 }

	void traceEntry(std::ostream& ostr) const;
	const wxString& traceEntryToString(wxString& ret) const;

	std::ostream& outputOperator(std::ostream &ostr) const;
	friend std::ostream &operator<<(std::ostream &ostr, const CncPathListEntry &a) {
		return a.outputOperator(ostr);
	}
};

typedef std::vector<CncPathListEntry> CncPathList;

#endif
