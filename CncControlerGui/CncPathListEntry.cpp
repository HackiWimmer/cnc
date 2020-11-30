#include "CncPathListEntry.h"

const bool 				CncPathListEntry::DefaultAlreadyRendered	= false;
const long 				CncPathListEntry::DefaultClientID   		= CLIENT_ID.INVALID;
const CncSpeedMode 		CncPathListEntry::DefaultSpeedMode  		=  CncSpeedUserDefined;
const double			CncPathListEntry::DefaultSpeedValue 		= -1.0;
const CncDoublePosition	CncPathListEntry::ZeroTarget				= {0.0, 0.0, 0.0};
const CncDoubleDistance	CncPathListEntry::NoDistance				= {0.0, 0.0, 0.0};
const CncNanoTimestamp	CncPathListEntry::NoReference				= -1LL;

//////////////////////////////////////////////////////////////////
std::ostream& CncPathListEntry::outputOperator(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	ostr << " CncPathListEntry: "								<< std::endl;
	ostr << "  PathList Reference : "	<< pathListReference 	<< std::endl;
	ostr << "  Content            : "	<< content				<< std::endl;

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
	wxString content;
	content.append( isNothingChanged()		? 'L' : ' ' );
	content.append( hasClientIdChange()		? 'C' : ' ' );
	content.append( hasSpeedChange()		? 'S' : ' ' );
	content.append( hasPositionChange()		? 'P' : ' ' );
	
	ostr << traceIndent
		 << wxString::Format("%05ld", listIndex) 			<< " "
		 << "PLE: "
		 << pathListReference
		 << wxString::Format("(%s): ", content)
		;
		
	if ( isNothingChanged() == true )
		return;
		 
	ostr << wxString::Format("% 5ld", clientId)				<< ", "
		 
		 << cnc::dblFormat1(feedSpeed_MM_MIN)				<< " "
		 << cnc::getCncSpeedTypeAsCharacter(feedSpeedMode)	<< ", "
		 
		 << cnc::dblFormat(entryDistance)					<< " --> "
		 << cnc::dblFormat(entryTarget)						<< " | "
		 << "td = " << cnc::dblFormat1(totalDistance)		<< " | "
		 << "ar = " << alreadyRendered
		 
		 << std::endl;
		; 
}
//////////////////////////////////////////////////////////////////
const wxString& CncPathListEntry::traceEntryToString(wxString& ret) const {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	traceEntry(ss);
	ret.assign(ss.str().c_str());

	return ret;
}
