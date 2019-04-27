#include "CncPathListEntry.h"

const bool 				CncPathListEntry::DefaultAlreadyRendered  = false;
const long 				CncPathListEntry::DefaultClientID   	  = -1L;
const CncSpeedMode 		CncPathListEntry::DefaultSpeedMode  	  =  CncSpeedUserDefined;
const double			CncPathListEntry::DefaultSpeedValue 	  = -1.0;
const CncDoublePosition	CncPathListEntry::ZeroTarget			  = {0.0, 0.0, 0.0};
const CncDoubleDistance	CncPathListEntry::NoDistance			  = {0.0, 0.0, 0.0};
const CncNanoTimestamp	CncPathListEntry::NoReference			  = -1LL;

//////////////////////////////////////////////////////////////////
std::ostream& CncPathListEntry::outputOperator(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	ostr << "CncPathListEntry: " 								<< std::endl;
	ostr << " PathList Reference : "	<< pathListReference 	<< std::endl;
	ostr << " Type               : "	<< type      			<< std::endl;

	ostr << " Client ID          : "	<< clientId		 		<< std::endl;
	ostr << " Already rendered   : "	<< alreadyRendered 		<< std::endl;

	ostr << " Entry Target       : "	<< entryTarget			<< std::endl;
	ostr << " Entry Distance     : "	<< entryDistance 		<< std::endl;

	ostr << " FeedSpeed Mode     : "	<< feedSpeedMode   		<< std::endl;
	ostr << " FeedSpeed Value    : "	<< feedSpeed_MM_MIN		<< std::endl;

	ostr << " Total Distance     : "	<< totalDistance 		<< std::endl;

	return ostr;
}
//////////////////////////////////////////////////////////////////
void CncPathListEntry::traceEntry(std::ostream& ostr) const {
//////////////////////////////////////////////////////////////////

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
const wxString& CncPathListEntry::traceEntryToString(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	traceEntry(ss);
	ret.assign(ss.str().c_str());

	return ret;
}
