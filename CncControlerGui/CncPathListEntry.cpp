#include "CncArduino.h"
#include "CncPathListEntry.h"

const int					CncPathListEntry::ContentCFSP				= CONT_CLIENTID | CONT_SPEED | CONT_SPINDLE             | CONT_POSITION;
const int					CncPathListEntry::ContentCFS				= CONT_CLIENTID | CONT_SPEED | CONT_SPINDLE;
const int					CncPathListEntry::ContentFS					=                 CONT_SPEED | CONT_SPINDLE;
const int					CncPathListEntry::ContentCFSTP				= CONT_CLIENTID | CONT_SPEED | CONT_SPINDLE | CONT_TOOL | CONT_POSITION;
const int					CncPathListEntry::ContentCFST				= CONT_CLIENTID | CONT_SPEED | CONT_SPINDLE | CONT_TOOL;
const int					CncPathListEntry::ContentFST				=                 CONT_SPEED | CONT_SPINDLE | CONT_TOOL;

const bool 					CncPathListEntry::DefaultAlreadyRendered	= false;
const long 					CncPathListEntry::DefaultClientID   		= CLIENT_ID.INVALID;
const int					CncPathListEntry::DefaultToolID				= INVALID_TOOL_ID;
const CncSpeedMode 			CncPathListEntry::DefaultSpeedMode  		= CncSpeedUserDefined;
const double				CncPathListEntry::DefaultSpeedValue 		= -1.0;
const CncSpindlePowerState	CncPathListEntry::DefaultSpindleState		= SPINDLE_STATE_OFF;
const double				CncPathListEntry::DefaultSpindleSpeedValue	= -1.0;
const CncDoublePosition		CncPathListEntry::ZeroTarget				= {0.0, 0.0, 0.0};
const CncDoubleDistance		CncPathListEntry::NoDistance				= {0.0, 0.0, 0.0};
const CncNanoTimestamp		CncPathListEntry::NoReference				= -1LL;

//////////////////////////////////////////////////////////////////
std::ostream& CncPathListEntry::outputOperator(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	// ----------------------------------------------------------
	auto printSps = [](CncSpindlePowerState sps)
	{ 
		if ( sps == SPINDLE_STATE_ON ) 
			return "ON";
			
		return "OFF";
	};
	
	// ----------------------------------------------------------
	ostr << " CncPathListEntry: "									<< std::endl;
	ostr << "  PathList Reference  : "	<< pathListReference		<< std::endl;
	ostr << "  Content             : "	<< content					<< std::endl;

	ostr << "  Client ID           : "	<< clientId		 			<< std::endl;
	ostr << "  Already rendered    : "	<< alreadyRendered			<< std::endl;

	ostr << "  Entry Target        : "	<< entryTarget				<< std::endl;
	ostr << "  Entry Distance      : "	<< entryDistance 			<< std::endl;

	ostr << "  FeedSpeed Mode      : "	<< feedSpeedMode			<< std::endl;
	ostr << "  FeedSpeed Value     : "	<< feedSpeed_MM_MIN			<< std::endl;

	ostr << "  Tool ID             : "	<< toolId		 			<< std::endl;

	ostr << "  Spindle State       : "	<< printSps(spindleState)	<< std::endl;
	ostr << "  Spindle Speed Value : "	<< spindleSpeed_U_MIN		<< std::endl;

	ostr << "  Total Distance      : "	<< totalDistance			<< std::endl;

	return ostr;
}
//////////////////////////////////////////////////////////////////
void CncPathListEntry::traceEntry(std::ostream& ostr) const {
//////////////////////////////////////////////////////////////////
	wxString content;
	content.append( isNothingChanged()		? 'L' : '-' );
	content.append( hasClientIdChange()		? 'C' : '-' );
	content.append( hasSpeedChange()		? 'F' : '-' );
	content.append( hasSpindleChange()		? 'S' : '-' );
	content.append( hasToolChange()			? 'T' : '-' );
	content.append( hasPositionChange()		? 'P' : '-' );
	
	ostr << traceIndent
		 << wxString::Format("%05ld", listIndex) 				<< " "
		 << "PLE: "
		 << pathListReference
		 << wxString::Format("(%s): ", content)
		;
		
	if ( isNothingChanged() == true )
		return;
		 
	ostr << wxString::Format("% 5ld", clientId)					<< ", "
		 
		 << "("													<< " "
		 << cnc::dblFormat1(feedSpeed_MM_MIN)					<< " "
		 << cnc::getCncSpeedTypeAsCharacter(feedSpeedMode)
		 << ")"													<< ", "
		 
		 << "("
		 << std::fixed << std::setw(4) << std::setfill(' ') << std::right
		 << wxString::Format("% 5ld", toolId)					<< " " 
		 << ")"													<< ", "
		 
		 << "("
		 << (spindleState == SPINDLE_STATE_ON ?  "ON" : "OFF")	<< " "
		 << cnc::dblFormat1(spindleSpeed_U_MIN)
		 << ")"													<< ", "
		 
		 << "+D("
		 << cnc::dblFormat(entryDistance)						<< ") = T("
		 << cnc::dblFormat(entryTarget)							<< ") | "
		 
		 << "td = " << cnc::dblFormat1(totalDistance)			<< " | "
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

