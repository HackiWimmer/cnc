#include "CncArduino.h"
#include "CncConfig.h"
#include "ContextInterface.h"

//////////////////////////////////////////////////////////
#define TRACE_INDEX(stream) \
	stream << wxString::Format("%6ld: ", std::distance(contextInterfaceEntries.begin(), it) + 1 ); 
	
#define TRACE_ENTRY(stream) \
	{ \
		/*TRACE_INDEX(stream);*/ \
		it->traceTo(stream); \
		stream << std::endl; \
	}
	
////////////////////////////////////////////////////////////////////
const char* ContextInterface::Entry::getTypeAsString(Type t) {
////////////////////////////////////////////////////////////////////
	switch ( t )
	{
		case START:		return "Start";
		case END:		return "End";
		case CID:		return "CID";
		case LIMIT:		return "Limit";
		case MOVE:		return "Move";
		case SPINDLE:	return "Spindle";
		case SPEED_F:	return "Speed F";
		case SPEED_S:	return "Speed S";
		case UNKNOWN:	return "Unknown";
	}
	
	return "???";
}

//////////////////////////////////////////////////////////
std::ostream& ContextInterface::Result::traceTo(std::ostream &o) const {
//////////////////////////////////////////////////////////
	o	<< "Summary:\n"
		<< wxString::Format(" Min/Max F Value      [mm/min] : %6ld, %6ld\n", 			( sum.minF != Result::minD ? sum.minF : 0 ), sum.maxF)
		<< wxString::Format(" Min/Max S Value       [U/min] : %6ld, %6ld\n",			( sum.minS != Result::minD ? sum.minS : 0 ), sum.maxS)
		<< wxString::Format(" Limit Summary                 : %14s\n",					sum.limitStates.getValueAsReadableString())
		<< wxString::Format(" Total Distance X         [mm] : %14.3lf\n",				double(sum.lenX) * THE_CONFIG->getDisplayFactX())
		<< wxString::Format(" Total Distance Y         [mm] : %14.3lf\n",				double(sum.lenY) * THE_CONFIG->getDisplayFactY())
		<< wxString::Format(" Total Distance Z         [mm] : %14.3lf\n",				double(sum.lenZ) * THE_CONFIG->getDisplayFactZ())
	;
	o	<< "Distances without Spindle = OFF\n"
		<< wxString::Format(" Total Distance X          [mm] : %14.3lf\n",				double(err.lenX) * THE_CONFIG->getDisplayFactX())
		<< wxString::Format(" Total Distance Y          [mm] : %14.3lf\n",				double(err.lenY) * THE_CONFIG->getDisplayFactY())
		<< wxString::Format(" Total Distance Z          [mm] : %14.3lf\n",				double(err.lenZ) * THE_CONFIG->getDisplayFactZ())
	;
	
	return o;
};

////////////////////////////////////////////////////////////////////
std::ostream& ContextInterface::Entry::traceTo(std::ostream &ostr) const {
////////////////////////////////////////////////////////////////////
	// ------------------------------------------------------------
	auto displayType= [&]()
	{
		return wxString::Format("%-8s", getTypeAsString(lastType));
	};
	
	// ------------------------------------------------------------
	auto displayClientId= [&]()
	{
		return wxString::Format("%8ld", clientId);
	};
	
	// ------------------------------------------------------------
	auto displayLimit = [&]()
	{
		return wxString::Format("[%s]", limitStates.getValueAsString());
	};
	
	// ------------------------------------------------------------
	auto displaySpindleState = [&]()
	{
		return spindleState == SPINDLE_STATE_OFF ? "OFF" : "ON ";
	};
	
	// ------------------------------------------------------------
	auto displayMove = [&]()
	{
		return wxString::Format("%-32s (%6ld, %6ld, %6ld)",
								moveCmd != CMD_INVALID ? ArduinoCMDs::getCMDLabel(moveCmd) : "",
								moveDx,
								moveDy,
								moveDz
		);
	};
	
	// ------------------------------------------------------------
	auto displayStepperSpeed = [&]()
	{
		return wxString::Format("%c %6ld", 
								ArdoObj::SpeedTuple::decodeMode(speedTupleVal), 
								ArdoObj::SpeedTuple::decodeValue_MMMin(speedTupleVal)
		);
	};
	
	// ------------------------------------------------------------
	auto displaySpindleSpeed = [&]()
	{
		return wxString::Format("%6d", ArdoObj::SpindleTuple::decodeValue(spindleTupleVal));
	};
	
	// ------------------------------------------------------------
	ostr	<< "Type = "		<< displayType()				<< ", "
			<< "CID = "			<< displayClientId()			<< ", "
			<< "Limit = "		<< displayLimit()				<< ", "
			<< "Spindle = "		<< displaySpindleState()		<< ", "
			<< "XYZ = "			<< displayMove()				<< ", "
			<< "F = "			<< displayStepperSpeed()		<< ", "
			<< "S = "			<< displaySpindleSpeed()
	;
	
	return ostr;
}

//////////////////////////////////////////////////////////
ContextInterface::ContextInterface()
: contextInterfaceEntries			()
, currentEntry						()
, errorFlags						(ERR_NO_ERROR)
//////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////
ContextInterface::~ContextInterface() {
//////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////
std::ostream& ContextInterface::traceContextEntriesTo(std::ostream &ostr) const {
////////////////////////////////////////////////////////////////////
	for ( auto it = contextInterfaceEntries.begin(); it != contextInterfaceEntries.end(); ++it )
		TRACE_ENTRY(ostr);
	
	return ostr;
}
//////////////////////////////////////////////////////////////
std::ostream& ContextInterface::traceErrorInfoTo(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////
	if ( hasErrors() == false )
	{
		ostr << "No error detected!\n";
	}
	else
	{
		if ( errorFlags & ERR_LIMIT )
			ostr << "Error: One ore more limit switches reached during the run!\n";
			
		if ( errorFlags & ERR_MOVE_WITHOUT_SPINDLE )
			ostr << "Error: One ore more movements detected as work path performed with Spindle = OFF!\n";
			
			// add more on demand
	}
	
	return ostr;
}

//////////////////////////////////////////////////////////////
void ContextInterface::notifyBeginRun() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	errorFlags					= ERR_NO_ERROR;
	currentEntry 				= ContextInterface::Entry();
	
	currentEntry.lastType		= ContextInterface::Entry::Type::START;
	
	contextInterfaceEntries.clear();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifyEndRun() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType		= ContextInterface::Entry::Type::END;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifyClientId(long id) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType		= ContextInterface::Entry::Type::CID;
	currentEntry.clientId		= id;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifyLimit(const CncInterface::ILS::States& s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType			= ContextInterface::Entry::Type::LIMIT;
	currentEntry.limitStates		= s;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifyMove(unsigned char cmd, int32_t dx, int32_t dy, int32_t dz) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType			= ContextInterface::Entry::Type::MOVE;
	currentEntry.moveCmd			= cmd;
	currentEntry.moveDx				= dx;
	currentEntry.moveDy				= dy;
	currentEntry.moveDz				= dz;
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifySpindleOn() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType			= ContextInterface::Entry::Type::SPINDLE;
	currentEntry.spindleState		= SPINDLE_STATE_ON;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifySpindleOff() {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType			= ContextInterface::Entry::Type::SPINDLE;
	currentEntry.spindleState		= SPINDLE_STATE_OFF;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifySpindleSpeed(unsigned char pid, ArdoObj::SpindleTupleValue s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType			= ContextInterface::Entry::Type::SPEED_S;
	currentEntry.spindleTupleVal	= s;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
//////////////////////////////////////////////////////////////
void ContextInterface::notifyStepperSpeed(unsigned char pid, ArdoObj::SpeedTupleValue s) {
//////////////////////////////////////////////////////////////
	//CNC_CEX2_FUNCT
	
	currentEntry.lastType			= ContextInterface::Entry::Type::SPEED_F;
	currentEntry.speedTupleVal		= s;
	currentEntry.resetMovement();
	contextInterfaceEntries.push_back(currentEntry);
}
////////////////////////////////////////////////////////////////////
bool ContextInterface::isEntryListValid() const {
////////////////////////////////////////////////////////////////////
	if ( contextInterfaceEntries.size() < 2 )
		{ CNC_CERR_FUNCT_A("Invalid size"); return false; }
	
	if ( contextInterfaceEntries.front().lastType != Entry::Type::START )
		{ CNC_CERR_FUNCT_A("Invalid first entry"); return false; }
	
	if ( contextInterfaceEntries.back().lastType != Entry::Type::END )
		{ CNC_CERR_FUNCT_A("Invalid last entry"); return false; }
		
	return true;
}
////////////////////////////////////////////////////////////////////
bool ContextInterface::filterAllLimitEntries(std::ostream& o) const {
////////////////////////////////////////////////////////////////////
	if ( isEntryListValid() == false )
		return false;
		
	// --------------------------------------------------------------
	for ( auto it = contextInterfaceEntries.begin(); it != contextInterfaceEntries.end(); ++it )
	{
		const Entry& entry	= *it;
		
		if ( entry.lastType == Entry::Type::LIMIT )
			TRACE_ENTRY(o);
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
bool ContextInterface::filterAllMovesWithoutSpindle(std::ostream& o) const {
////////////////////////////////////////////////////////////////////
	if ( isEntryListValid() == false )
		return false;
		
	// --------------------------------------------------------------
	for ( auto it = contextInterfaceEntries.begin(); it != contextInterfaceEntries.end(); ++it )
	{
		const Entry& entry	= *it;
		const bool b = entry.hasMovement() && entry.spindleState == SPINDLE_STATE_OFF && ArdoObj::SpeedTuple::decodeMode(entry.speedTupleVal) != 'R';
		
		if ( b )
			TRACE_ENTRY(o);
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
bool ContextInterface::analizeContextEntries(Result& result) {
////////////////////////////////////////////////////////////////////
	if ( isEntryListValid() == false )
	{
		errorFlags |= ERR_GENERAL;
		return false;
	}
		
	// --------------------------------------------------------------
	for ( auto it = contextInterfaceEntries.begin(); it != contextInterfaceEntries.end(); ++it )
	{
		//const size_t index	= std::distance(contextInterfaceEntries.begin(), it) + 1;
		const Entry& entry	= *it;
		
		// min / max F
		if ( entry.lastType == Entry::Type::SPEED_F )
		{
			result.sum.minF = std::min(result.sum.minF, ArdoObj::SpeedTuple::decodeValue_MMMin(entry.speedTupleVal));
			result.sum.maxF = std::max(result.sum.maxF, ArdoObj::SpeedTuple::decodeValue_MMMin(entry.speedTupleVal));
		}
		
		// min / max S
		if ( entry.lastType == Entry::Type::SPEED_S )
		{
			result.sum.minS = std::min(result.sum.minS, cnc::cnvSpidleSpeedRaw_To_U_MIN(ArdoObj::SpindleTuple::decodeValue(entry.spindleTupleVal)));
			result.sum.maxS = std::max(result.sum.maxS, cnc::cnvSpidleSpeedRaw_To_U_MIN(ArdoObj::SpindleTuple::decodeValue(entry.spindleTupleVal)));
		}
		
		// Limit
		if ( entry.limitStates.hasLimits() == true )
		{
			if ( entry.limitStates.xMin() == true ) result.sum.limitStates.setBit(LimitSwitch::BIT_LS_X_MIN, true);
			if ( entry.limitStates.xMax() == true ) result.sum.limitStates.setBit(LimitSwitch::BIT_LS_X_MAX, true);
			if ( entry.limitStates.yMin() == true ) result.sum.limitStates.setBit(LimitSwitch::BIT_LS_Y_MIN, true);
			if ( entry.limitStates.yMax() == true ) result.sum.limitStates.setBit(LimitSwitch::BIT_LS_Y_MAX, true);
			if ( entry.limitStates.zMin() == true ) result.sum.limitStates.setBit(LimitSwitch::BIT_LS_Z_MIN, true);
			if ( entry.limitStates.zMax() == true ) result.sum.limitStates.setBit(LimitSwitch::BIT_LS_Z_MAX, true);
			
			errorFlags |= ERR_LIMIT;
		}
		
		if ( entry.hasMovement() )
		{
			result.sum.lenX += abs(entry.moveDx);
			result.sum.lenY += abs(entry.moveDy);
			result.sum.lenZ += abs(entry.moveDz);
			
			if ( entry.spindleState == SPINDLE_STATE_OFF && ArdoObj::SpeedTuple::decodeMode(entry.speedTupleVal) != 'R' )
			{
				result.err.lenX += abs(entry.moveDx);
				result.err.lenY += abs(entry.moveDy);
				result.err.lenZ += abs(entry.moveDz);
				
				errorFlags |= ERR_MOVE_WITHOUT_SPINDLE;
			}
		}
	}
	
	return hasErrors() == false;
}