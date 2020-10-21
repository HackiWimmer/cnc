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
	ostr << " CncPathListEntry: "								<< std::endl;
	ostr << "  PathList Reference : "	<< pathListReference 	<< std::endl;
	ostr << "  Type               : "	<< type					<< std::endl;

	ostr << "  Client ID          : "	<< clientId		 		<< std::endl;
	ostr << "  Already rendered   : "	<< alreadyRendered		<< std::endl;

	ostr << "  Entry Target       : "	<< entryTarget			<< std::endl;
	ostr << "  Entry Distance     : "	<< entryDistance 		<< std::endl;

	ostr << "  FeedSpeed Mode     : "	<< feedSpeedMode		<< std::endl;
	ostr << "  FeedSpeed Value    : "	<< feedSpeed_MM_MIN		<< std::endl;

	ostr << "  Total Distance     : "	<< totalDistance		<< std::endl;

	return ostr;
}
//////////////////////////////////////////////////////////////////
void CncPathListEntry::traceEntry(std::ostream& ostr) const {
//////////////////////////////////////////////////////////////////

	if ( isClientIdChange() ) {
		ostr << traceIndent
			 << "PLE: "
			 << pathListReference								<< "("
			 << " C "											<< "): "
			 << wxString::Format("% 5ld", clientId)				<< std::endl;
	}
	else if ( isPositionChange() ) {
		ostr << traceIndent
			 << "PLE: "
			 << pathListReference								<< "("
			 << " P "											<< "): "
			 << wxString::Format("% 5ld", clientId)				<< "  "
			 << cnc::dblFormat(entryDistance)					<< " --> "
			 << cnc::dblFormat(entryTarget)						<< " | "
			 << "td = " << cnc::dblFormat1(totalDistance)		<< " | "
			 << "ar = " << alreadyRendered						<< std::endl;
	}
	else if ( isSpeedChange() ) {
		ostr << traceIndent
			 << "PLE: "
			 << pathListReference								<< "("
			 << " S "											<< "): "
			 << wxString::Format("% 5ld", clientId)				<< "  "
			 << cnc::getCncSpeedTypeAsCharacter(feedSpeedMode)	<< ", "
			 << cnc::dblFormat1(feedSpeed_MM_MIN)				<< std::endl;
	}
	else if ( isNothingChanged() ) {
		ostr << traceIndent
			 << "PLE: "
			 << pathListReference								<< "("
			 << " L "											<< "): "
			 << wxString::Format("% 5ld", clientId)				<< "  "
			 << cnc::dblFormat(entryTarget)						<< std::endl;
	}
	else {
		// should not appear
		ostr << "Undefined Type: [" << type << "]:"
			 << "Default output"								<< std::endl
			 << (*this);
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
