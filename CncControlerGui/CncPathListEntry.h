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
	// Data
	enum Type {CHG_NOTHING=0, CHG_CLIENTID=1, CHG_POSITION=2, CHG_SPEED=3};

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
	bool isClientIdChange() const { return type == CHG_CLIENTID; }
	bool isPositionChange()	const { return type == CHG_POSITION; }
	bool isSpeedChange() 	const { return type == CHG_SPEED; 	 }


	//////////////////////////////////////////////////////////////////
	friend std::ostream &operator<< (std::ostream &ostr, const CncPathListEntry &a) {
		ostr << "CncPathListEntry: " 								<< std::endl;
		ostr << " PathList Reference : "	<< a.pathListReference 	<< std::endl;
		ostr << " Type               : "	<< a.type      			<< std::endl;

		ostr << " Client ID          : "	<< a.clientId		 	<< std::endl;
		ostr << " Already rendered   : "	<< a.alreadyRendered 	<< std::endl;

		ostr << " Entry Target       : "	<< a.entryTarget		<< std::endl;
		ostr << " Entry Distance     : "	<< a.entryDistance 		<< std::endl;

		ostr << " FeedSpeed Mode     : "	<< a.feedSpeedMode   	<< std::endl;
		ostr << " FeedSpeed Value    : "	<< a.feedSpeed_MM_MIN	<< std::endl;

		ostr << " Total Distance     : "	<< a.totalDistance 		<< std::endl;

		return ostr;
	}

	//////////////////////////////////////////////////////////////////
	void traceEntry(std::ostream& ostr) const {

		if ( isClientIdChange() ) {
			ostr << " PLE: "
				 << pathListReference 	<< "("
				 << " C "				<< "): "
				 << clientId			<< std::endl;

		}
		else if ( isPositionChange() ) {
			ostr << " PLE: "
				 << pathListReference 	<< "("
				 << " P "				<< "): "
				 << entryDistance 		<< " > "
				 << entryTarget 		<< " | "
				 << alreadyRendered		<< std::endl;

		}
		else if ( isSpeedChange() ) {
			ostr << " PLE: "
				 << pathListReference 	<< "("
				 << " S "				<< "): "
				 << feedSpeedMode 		<< ", "
				 << feedSpeed_MM_MIN	<< std::endl;
		}
		else {
			//if ( type != CHG_NOTHING )
				ostr << (*this);
		}
	}
	//////////////////////////////////////////////////////////////////
	const wxString& traceEntryToString(wxString& ret) const {
		std::stringstream ss;
		traceEntry(ss);
		ret.assign(ss.str().c_str());

		return ret;
	}
};

typedef std::vector<CncPathListEntry> CncPathList;

#endif
