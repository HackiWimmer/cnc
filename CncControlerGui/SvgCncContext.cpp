#include "CncConfig.h"
#include "CncContext.h"
#include "MainFrame.h"
#include "SvgCncContext.h"


//////////////////////////////////////////////////////////////////
#define CTX_ADD_SEP(msg)	THE_CONTEXT->addParsingSynopisSeparator(wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));
#define CTX_LOG_INF(msg)	THE_CONTEXT->addParsingSynopisInfo     (wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));
#define CTX_LOG_WAR(msg)	THE_CONTEXT->addParsingSynopisWarning  (wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));
#define CTX_LOG_ERR(msg)	THE_CONTEXT->addParsingSynopisError    (wxString::Format("[LN: %8ld]: %s", getCurrentLineNumber(), msg));

//////////////////////////////////////////////////////////////////
SvgCncContextBase::SvgCncContextBase()
: currentLineNumber		(UNDEFINED_LINE_NUMBER)
, parameterMap			()
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgCncContextBase::SvgCncContextBase(const SvgCncContextBase& scc)
: currentLineNumber		(scc.getCurrentLineNumber())
, parameterMap			(scc.parameterMap)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgCncContextBase& SvgCncContextBase::operator= (const SvgCncContextBase& scc) {
//////////////////////////////////////////////////////////////////
	currentLineNumber	= scc.getCurrentLineNumber();
	parameterMap		= scc.parameterMap;
	
	return *this;
}
//////////////////////////////////////////////////////////////////
SvgCncContextBase::~SvgCncContextBase() {
//////////////////////////////////////////////////////////////////
	parameterMap.clear();
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::addExternInfo(const wxString& msg) {
//////////////////////////////////////////////////////////////////
	CTX_LOG_INF(msg)
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::addExternWarning(const wxString& msg) {
//////////////////////////////////////////////////////////////////
	CTX_LOG_WAR(msg)
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::addExternError(const wxString& msg) {
//////////////////////////////////////////////////////////////////
	CTX_LOG_ERR(msg)
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::reconstruct() {
//////////////////////////////////////////////////////////////////
	for( auto it=parameterMap.begin(); it != parameterMap.end(); ++it )
		provide(it->first, it->second);
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::reset() {
//////////////////////////////////////////////////////////////////
	currentLineNumber = UNDEFINED_LINE_NUMBER;
	parameterMap.clear();
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::setCurrentLineNumber(long ln) {
//////////////////////////////////////////////////////////////////
	currentLineNumber = ln < 0 ? UNDEFINED_LINE_NUMBER	: (ln);
}
//////////////////////////////////////////////////////////////////
long SvgCncContextBase::getCurrentClientID(CLIENT_ID_OFFSET o) const { 
//////////////////////////////////////////////////////////////////
	const bool b = currentLineNumber == UNDEFINED_LINE_NUMBER;
	return b ? CLIENT_ID.INVALID : (currentLineNumber * CLIENT_ID.TPL_FACTOR) + o;
}
//////////////////////////////////////////////////////////////////
void SvgCncContextBase::traceTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////////
	const wxString prefix(' ', (int)indent);
	
	for ( auto it = parameterMap.begin(); it != parameterMap.end(); ++it )
		o << wxString::Format("%s%s%s = %s\n", prefix, it->first, wxString(' ', 25 - it->first.length()), it->second);
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::deleteParameter(const wxString& key) {
//////////////////////////////////////////////////////////////////
	auto it = parameterMap.find(key);
	if ( it == parameterMap.end() )
		return false;
	
	parameterMap.erase(it);
	return true;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::provide(const wxString& name, const wxString& value) {
//////////////////////////////////////////////////////////////////
	wxString n(name); n.Trim().Trim(true);
	
	if ( n.IsEmpty() )
		return false;
	
	bool ret = true;
	
	if ( value.IsSameAs(DEL_Modifyer) ) {
		deleteParameter(n);
		manageParameter(Delete, n, value);
	}
	else {
		wxString v;
		ret = replaceVariables(value, v);
		// always do this
		parameterMap[n] = v;
		manageParameter(Update, n, v);
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::provide(const wxXmlAttribute* attribute) {
//////////////////////////////////////////////////////////////////
	if ( attribute == NULL )
		return false;
	
	return provide(attribute->GetName(), attribute->GetValue());
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::provide(const wxXmlNode* node) {
//////////////////////////////////////////////////////////////////
	if ( node == NULL )
		return false;
		
	wxXmlAttribute* a = node->GetAttributes();
	while ( a != NULL ) {
		if ( provide(a) == false )
			std::cerr << CNC_LOG_FUNCT_A(" Invalid attribute name ='%s', storage failed\n", a->GetName());
			
		a = a->GetNext();
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::provideVariables(const wxString& name, const wxString& value) {
//////////////////////////////////////////////////////////////////
	wxString varName(wxString::Format("{%s}", name));
	return provide(varName, value);
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::provideVariables(const wxXmlAttribute* attribute) {
//////////////////////////////////////////////////////////////////
	if ( attribute == NULL )
		return false;
	
	return provideVariables(attribute->GetName(), attribute->GetValue());
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::provideVariables(const wxXmlNode *node) {
//////////////////////////////////////////////////////////////////
	if ( node == NULL )
		return false;
		
	wxXmlAttribute* a = node->GetAttributes();
	while ( a != NULL ) {
		if ( provideVariables(a) == false )
			std::cerr << CNC_LOG_FUNCT_A(" Invalid variables name, storage failed\n");
			
		a = a->GetNext();
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
const wxVariant SvgCncContextBase::getParameter(const wxString& key, const char* def) const {
//////////////////////////////////////////////////////////////////
	if ( def != NULL ) {
		auto it = parameterMap.find(key);
		const wxVariant ret( it != parameterMap.end() ? it->second : wxString(def));
		return ret;
	}
	
	const wxVariant ret(getParameterValue(key));
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::convertToBool(const wxString& key) const {
//////////////////////////////////////////////////////////////////
	auto it = parameterMap.find(key);
	if ( it != parameterMap.end() ) {
	
		const wxString& val = it->second;
		
		if		( val.IsSameAs("TRUE",	false) )	return true;
		else if	( val.IsSameAs("YES",	false) )	return true;
		else if	( val.IsSameAs("OK",	false) )	return true;
		else if	( val.IsSameAs("JA",	false) )	return true;
		else if	( val.IsSameAs("ON",	false) )	return true;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::hasParameter(const wxString& key) const {
//////////////////////////////////////////////////////////////////
	return parameterMap.find(key) != parameterMap.end();
}
//////////////////////////////////////////////////////////////////
const wxString SvgCncContextBase::getParameterValue(const wxString& key) const {
//////////////////////////////////////////////////////////////////
	auto it = parameterMap.find(key);
	
	if ( it == parameterMap.end() ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Can't find key = '%s'\n", key));
		return "";
	}
	
	return it->second;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContextBase::replaceVariables(const wxString& in, wxString& out) {
//////////////////////////////////////////////////////////////////
	
	// -----------------------------------------------------------
	auto check = [](const wxString& s) {
		bool b1 = false;
		
		for ( auto it = s.begin(); it != s.end(); ++it ) {
			
			if ( *it == '{' )
				b1 = true;
				
			if ( b1 && *it == '}' )
				return true;
		}
		
		return false;
	};
	
	out.assign(in);
	unsigned int errCount = 0;
	while ( check(out) ) {
		const wxString var(out.AfterFirst('{').BeforeFirst('}'));
		const wxString name(wxString::Format("{%s}", var));
		
		auto it = parameterMap.find(name);
		
		errCount += (unsigned int)(it == parameterMap.end());
		
		if ( it != parameterMap.end() ) out.Replace(wxString::Format("{%s}", var), it->second);
		else							out.Replace(wxString::Format("{%s}", var), wxString::Format("!Not initialized variable '%s'!", var));
	}
	
	return check(out) == false && errCount == 0;
}

//////////////////////////////////////////////////////////////////
SvgCncContext::SvgCncContext()
: SvgCncContextBase			()
, isUseColouScheme			(false)
, currentZMaxFeedStep		(0.0)
, currentZDepth				(0.0)
, currentZDepthMode			('Z')
, currentRapidSpeed_MM_MIN	(THE_CONFIG->getDefaultRapidSpeed_MM_MIN())
, currentWorkSpeed_MM_MIN	(THE_CONFIG->getDefaultWorkSpeed_MM_MIN())
, currentSpindleSpeed_U_MIN	(THE_CONFIG->getSpindleSpeedDefault())
, toolList					()
, pathModification			(CncPM_None)
, pathRule					(CncPR_None)
, fillColourDecoder			()
, strokeColourDecoder		()
//////////////////////////////////////////////////////////////////
{
	// defaults
	provide(ID_COLOUR_SCHEME,	THE_CONFIG->getSvgUseColourScheme() ? "YES" : "NO" );
	provide(ID_TOOL_LIST,		wxString::Format("%s=[0.00]",	ID_DEFAULT_TOOL_ID));
	provide(ID_TOOL_SEL,		wxString::Format("%s",			ID_DEFAULT_TOOL_ID));
	provide(ID_DEPT,			"Z-0.0");
	provide(ID_MAX_FEED_STEP,	wxString::Format("Z%+.1lf",		THE_CONFIG->getMaxDurationThickness()));
	provide(ID_RAPID_SPEED,		wxString::Format("R%+.1lf",		currentRapidSpeed_MM_MIN));
	provide(ID_WORK_SPEED,		wxString::Format("W%+.1lf",		currentWorkSpeed_MM_MIN));
	provide(ID_SPINDLE_SPEED,	wxString::Format("S%+.1lf",		currentSpindleSpeed_U_MIN));
	provide(ID_ARG_SWEEP_FLAG,	"No");
}
//////////////////////////////////////////////////////////////////
SvgCncContext::SvgCncContext(const SvgCncContext& scc) 
: SvgCncContextBase			(scc)
// further initialization will be done by the = operator
//////////////////////////////////////////////////////////////////
{
	*this = scc;
}
//////////////////////////////////////////////////////////////////
SvgCncContext& SvgCncContext::operator= (const SvgCncContext& scc) {
//////////////////////////////////////////////////////////////////
	SvgCncContextBase::operator= (scc);
	
	reconstruct();

	isUseColouScheme			= scc.useColourScheme();
	currentZMaxFeedStep			= scc.getCurrentZMaxFeedStep();
	currentZDepth				= scc.getCurrentZDepth();
	currentZDepthMode			= scc.getCurrentZDepthMode();
	currentRapidSpeed_MM_MIN	= scc.getCurrentRapidSpeed_MM_MIN();
	currentWorkSpeed_MM_MIN		= scc.getCurrentWorkSpeed_MM_MIN();
	toolList					= scc.toolList;
	pathModification			= scc.pathModification;
	pathRule					= scc.pathRule;
	fillColourDecoder			= scc.fillColourDecoder;
	strokeColourDecoder			= scc.strokeColourDecoder;
	
	return *this;
}
//////////////////////////////////////////////////////////////////
SvgCncContext::~SvgCncContext() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::reconstruct() {
//////////////////////////////////////////////////////////////////
	SvgCncContextBase::reconstruct();
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::traceTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////////
	const wxString prefix(' ', (int)indent);
	
	o	<< prefix
		<< "Current Cnc Parameter, line: " 
		<< ClientIds::lineNumber(getCurrentClientID())
		<< std::endl
		<< prefix
		<< "{"
		<< std::endl
		;
		
		SvgCncContextBase::traceTo(o, indent + 2);
		
		const unsigned int fillTo = 21;
		wxString name;
		
		name.assign("Is Guide Path");
		o	 << wxString::Format("%s  --> %s%s = %s\n", prefix, name, wxString(' ', fillTo - name.length()), (isGuidePath() ? "Yes" : "No"));
		
		name.assign("Is Zero Ref Path");
		o	 << wxString::Format("%s  --> %s%s = %s\n", prefix, name, wxString(' ', fillTo - name.length()), (isZeroPosPath() ? "Yes" : "No"));
		
		name.assign("Use Colour Scheme");
		o	 << wxString::Format("%s  --> %s%s = %s\n", prefix, name, wxString(' ', fillTo - name.length()), (isUseColouScheme ? "Yes" : "No"));
		
		name.assign("Tool Diameter");
		o	 << wxString::Format("%s  --> %s%s = %.3lf\n", prefix, name, wxString(' ', fillTo - name.length()), getCurrentToolDiameter());
		
		name.assign("ZDepth");
		o	 << wxString::Format("%s  --> %s%s = %s [mm]\n", prefix, name, wxString(' ', fillTo - name.length()), getCurrentZDeptAsStr());
		
		name.assign("ZMAxFeedStep");
		o	 << wxString::Format("%s  --> %s%s = %.3lf [mm]\n", prefix, name, wxString(' ', fillTo - name.length()), getCurrentZMaxFeedStep());
		
		name.assign("Speed Rapid");
		o	 << wxString::Format("%s  --> %s%s = %+.1lf [mm/min]\n", prefix, name, wxString(' ', fillTo - name.length()), currentRapidSpeed_MM_MIN);
		
		name.assign("Speed Work");
		o	 << wxString::Format("%s  --> %s%s = %+.1lf [mm/min]\n", prefix, name, wxString(' ', fillTo - name.length()), currentWorkSpeed_MM_MIN);
		
		name.assign("Speed Spindle");
		o	 << wxString::Format("%s  --> %s%s = %+.1lf [U/min]\n", prefix, name, wxString(' ', fillTo - name.length()), currentSpindleSpeed_U_MIN);
		// ...
	
	o	<< prefix
		<< "}" 
		<< std::endl
		;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::traceVariablesOnlyTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////////
	const wxString prefix(' ', (int)indent +2);
	for ( auto it = parameterMap.begin(); it != parameterMap.end(); ++it ) {
		if ( it->first.StartsWith("{") )
			o << wxString::Format("%s%s%s = %s\n", prefix, it->first, wxString(' ', 25 - it->first.length()), it->second);
	}
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::manageParameter(const Mode mode, const wxString& name, const wxString& value) {
//////////////////////////////////////////////////////////////////
	bool commandExists = false;
	
	// ----------------------------------------------------------
	if ( name.IsSameAs(ID_DEPT) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				currentZDepth		= 0.0;
				currentZDepthMode	= 'Z';
				setCurrentZDepth("Z-0.0");
				break;
			}
			case Update:
			{
				setCurrentZDepth(value.IsEmpty() ? wxString::Format("Z-%lf", 0.0) : value);
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_MAX_FEED_STEP) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				setCurrentZFeedStep(wxString::Format("Z+%lf", THE_CONFIG->getMaxDurationThickness()));
				break;
			}
			case Update:
			{
				setCurrentZFeedStep(value.IsEmpty() ? wxString::Format("Z+%lf", 0.0) : value);
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_TOOL_LIST) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				toolList.clear();
				break;
			}
			case Update:
			{
				setToolList(value);
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_TOOL_ADD) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				break;
			}
			case Update:
			{
				addTool(value);
				break;
			}
		}
		
		deleteParameter(name);
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_TOOL_DEL) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				break;
			}
			case Update:
			{
				deleteTool(value);
				break;
			}
		}
		
		deleteParameter(name);
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_TOOL_SEL) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				break;
			}
			case Update:
			{
				if ( checkToolExists(value) == false ) {
					CTX_LOG_ERR(wxString::Format(" A tool with id = '%s' didn't exist within the tool list.\n", value));
				}
				else {
					THE_CONTEXT->setCurrentToolDiameter(getCurrentToolDiameter());
				}
					
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_PATH_MODE) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				pathModification = CncPM_None;
				break;
			}
			case Update:
			{
				if      ( value.IsSameAs("Center", false) )		pathModification = CncPM_Center;
				else if ( value.IsSameAs("Inner",  false) )		pathModification = CncPM_Inner;
				else if ( value.IsSameAs("Outer",  false) )		pathModification = CncPM_Outer;
				else if ( value.IsSameAs("Pocket", false) )		pathModification = CncPM_Pocket;
				else 											pathModification = CncPM_None;
				
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_PATH_RULE) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				pathRule = CncPR_None;
				break;
			}
			case Update:
			{
				if      ( value.IsSameAs("EnsureClockwise",			false) )	pathRule = CncPR_EnsureClockwise;
				else if ( value.IsSameAs("EnsureCounterClockwise",	false) )	pathRule = CncPR_EnsureCounterClockwise;
				else if ( value.IsSameAs("ReversePath",				false) )	pathRule = CncPR_Reverse;
				else 															pathRule = CncPR_None;
				
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_COLOUR_SCHEME) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				isUseColouScheme = convertToBool(name);
				pathModification = CncPM_None;
				break;
			}
			case Update:
			{
				isUseColouScheme = convertToBool(name);
				pathModification = CncPM_None;
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_RAPID_SPEED) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				currentRapidSpeed_MM_MIN = THE_CONFIG->getDefaultRapidSpeed_MM_MIN();
				break;
			}
			case Update:
			{	
				setCurrentSpeed('R', value);
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_WORK_SPEED) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				currentWorkSpeed_MM_MIN = THE_CONFIG->getDefaultWorkSpeed_MM_MIN();
			}
			case Update:
			{
				setCurrentSpeed('W', value);
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_SPINDLE_SPEED) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			{
				currentSpindleSpeed_U_MIN = THE_CONFIG->getSpindleSpeedDefault();
			}
			case Update:
			{
				setCurrentSpindleSpeed('S', value);
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_SPINDLE_STATE) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			case Update:
			{
				// handled by the parameter map 
				break;
			}
		}
	}
	// ----------------------------------------------------------
	else if ( name.IsSameAs(ID_ARG_SWEEP_FLAG) ) {
		commandExists = true;
		
		switch ( mode ) {
			case Delete:
			case Update:
			{
				// handled by the parameter map 
				break;
			}
		}
		
	}
		
	// ----------------------------------------------------------
	// final checks
	if ( commandExists == false ) {
		if ( name.StartsWith("{") == false )
			CTX_LOG_WAR(wxString::Format(" Unknown CncParameter '%s' = '%s'\n", name, value));
	}
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::isCurrentZFeedStepToStrong() const {
//////////////////////////////////////////////////////////////////
	const double dpt = getCurrentZDepth();
	const double nfs = getCurrentZMaxFeedStep();
	const double ctd = getCurrentToolDiameter();
	const double val = std::min(fabs(getCurrentZDepth()), fabs(getCurrentZMaxFeedStep()));
	
	// only if something in Z direction has to be done
	if ( cnc::dblCmp::nu(dpt) == false ) {
		
		// only if the tool diameter is well known
		if ( ctd > 0.0 ) {
			if ( val > ctd / 2.0 ) {
				std::cerr << CNC_LOG_FUNCT_A(wxString::Format("The current feed step of %.2lf [mm] is to strong for tool diameter %.2lf. The Processing stops here!\n", nfs, ctd));
				return true;
			}
			
			// generate a warning 
			if ( val * 0.8 > ctd / 2.0 ) 
				cnc::cex1 << CNC_LOG_FUNCT_A(wxString::Format("The current feed step of %.2lf [mm] is may be to strong for tool diameter %.2lf.\n", nfs, ctd));
		}
		else {
			
			// generate a warning 
			const double ctdStatic = 4.0;
			if ( val > ctdStatic ) 
				cnc::cex1 << CNC_LOG_FUNCT_A(wxString::Format("The current feed step of %.2lf [mm] is may be to strong for tool diameter %.2lf.\n", nfs, ctdStatic));
		}
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::hasPathModifications() const {
//////////////////////////////////////////////////////////////////
	switch ( pathModification ) {
		case CncPM_Pocket:
		case CncPM_Inner:
		case CncPM_Outer:
		case CncPM_Guide:
		case CncPM_ZeroRef:		return true; 
		
		case CncPM_Center:
		case CncPM_None:		return false;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::hasPathRules() const {
//////////////////////////////////////////////////////////////////
	switch ( pathRule )
	{
		case CncPR_EnsureClockwise:
		case CncPR_EnsureCounterClockwise:
		case CncPR_Reverse:				return true; 
			
		case CncPR_None:				return false;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentZDepth(const wxString& parameter) {
//////////////////////////////////////////////////////////////////
	//ZDepth="z-1.0" || zDepth="Z5.0"
	wxString para(parameter);
	para.Trim(true).Trim(false);
	
	if ( para.IsSameAs(RESET_Modifyer) ) {
		currentZDepth		= 0.0;
		currentZDepthMode	= 'Z';
		return;
	}
	
	if ( para.IsEmpty() )
		return;
		
	const char mode = (char)para[0];
	if ( mode != 'z' && mode != 'Z' ) {
		CTX_LOG_ERR(wxString::Format("Invalid mode token: '%s'", para));
		return;
	}
	
	double depthValue	= 0.0;
	para = para.SubString(1, para.Length() - 1);
	if ( para.ToDouble(&depthValue) == false ) {
		CTX_LOG_ERR(wxString::Format("Invalid depth token: '%s'", para));
		return;
	}
	
	currentZDepth		= depthValue;
	currentZDepthMode 	= mode;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentZFeedStep(const wxString& parameter) {
//////////////////////////////////////////////////////////////////
	//ZMaxFeedStep="Z+1.0"
	wxString para(parameter);
	para.Trim(true).Trim(false);
	
	if ( para.IsSameAs(RESET_Modifyer) ) {
		currentZMaxFeedStep = 0.0;
		return;
	}
	
	if ( para.IsEmpty() )
		return;
		
	const char mode = (char)para[0];
	if ( mode != 'Z' ) {
		CTX_LOG_ERR(wxString::Format("Invalid mode token: '%s'", para));
		return;
	}
	
	double maxValue	= 0.0;
	para = para.SubString(1, para.Length() - 1);
	if ( para.ToDouble(&maxValue) == false ) {
		CTX_LOG_ERR(wxString::Format("Invalid max feed step token: '%s'", para));
		return;
	}
	
	currentZMaxFeedStep = maxValue;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentSpeed(const char type, const wxString& parameter) {
//////////////////////////////////////////////////////////////////
	//Speed="R+2345" || Speed="W+1345"
	wxString para(parameter);
	para.Trim(true).Trim(false);
	
	if ( para.IsEmpty() )
		return;
		
	if ( para.IsSameAs(RESET_Modifyer) ) {
		if      ( type == 'R')	currentRapidSpeed_MM_MIN = THE_CONFIG->getDefaultRapidSpeed_MM_MIN();
		else if ( type == 'W')	currentWorkSpeed_MM_MIN  = THE_CONFIG->getDefaultWorkSpeed_MM_MIN();
		return;
	}
	
	const char mode = (char)para[0];
	if ( mode != type ) {
		CTX_LOG_ERR(wxString::Format("Invalid mode token: '%s'", para));
		return;
	}
	
	double speedValue	= 0.0;
	para = para.SubString(1, para.Length() - 1);
	if ( para.ToDouble(&speedValue) == false ) {
		CTX_LOG_ERR(wxString::Format("Invalid speed token: '%s'", para));
		return;
	}
	
	if      ( mode == 'R')	currentRapidSpeed_MM_MIN = speedValue;
	else if ( mode == 'W')	currentWorkSpeed_MM_MIN  = speedValue;
	
	// warnings
	if ( currentRapidSpeed_MM_MIN > THE_CONFIG->getDefaultRapidSpeed_MM_MIN() )
		CTX_LOG_WAR(wxString::Format("Warning: High rapid speed value: %.lf.", currentRapidSpeed_MM_MIN));
	
	if ( currentWorkSpeed_MM_MIN > THE_CONFIG->getDefaultRapidSpeed_MM_MIN() )
		CTX_LOG_WAR(wxString::Format("Warning: High work speed value: %.lf.", currentWorkSpeed_MM_MIN));
	
	// errors
	if ( currentRapidSpeed_MM_MIN > THE_CONFIG->getMaxSpeedXYZ_MM_MIN() ) {
		CTX_LOG_ERR(wxString::Format("Invalid rapid speed value: %.lf is greater the max speed %.lf. Value will be reset to its default value.",
									  currentRapidSpeed_MM_MIN, THE_CONFIG->getMaxSpeedXYZ_MM_MIN()));
		currentRapidSpeed_MM_MIN = THE_CONFIG->getDefaultRapidSpeed_MM_MIN();
	}
	
	if ( currentWorkSpeed_MM_MIN > THE_CONFIG->getMaxSpeedXYZ_MM_MIN() ) {
		CTX_LOG_ERR(wxString::Format("Invalid work speed value: %.lf is greater the max speed %.lf. Value will be reset to its default value.",
									  currentWorkSpeed_MM_MIN, THE_CONFIG->getMaxSpeedXYZ_MM_MIN()));
		currentWorkSpeed_MM_MIN  = THE_CONFIG->getDefaultWorkSpeed_MM_MIN();
	}
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentSpindleSpeed(const char type, const wxString& parameter) {
//////////////////////////////////////////////////////////////////
	//Speed="S+2345""
	wxString para(parameter);
	para.Trim(true).Trim(false);
	
	if ( para.IsEmpty() )
		return;
		
	if ( para.IsSameAs(RESET_Modifyer) )
	{
		currentSpindleSpeed_U_MIN = THE_CONFIG->getSpindleSpeedDefault();
		return;
	}
	
	const char mode = (char)para[0];
	if ( mode != type )
	{
		CTX_LOG_ERR(wxString::Format("Invalid mode token: '%s'", para));
		return;
	}
	
	double speedValue = 0.0;
	para = para.SubString(1, para.Length() - 1);
	if ( para.ToDouble(&speedValue) == false ) 
	{
		CTX_LOG_ERR(wxString::Format("Invalid speed token: '%s'", para));
		return;
	}
	
	if ( speedValue < THE_CONFIG->getSpindleSpeedMin() )
	{
		currentSpindleSpeed_U_MIN = THE_CONFIG->getSpindleSpeedMin();
		CTX_LOG_WAR(wxString::Format("Warning: A Spindle speed of %.lf is to less. It would be adjusted to %.lf.", speedValue, currentSpindleSpeed_U_MIN));
	}
	else if ( speedValue > THE_CONFIG->getSpindleSpeedMax() )
	{
		currentSpindleSpeed_U_MIN = THE_CONFIG->getSpindleSpeedMin();
		CTX_LOG_WAR(wxString::Format("Warning: A Spindle speed of %.lf is to much. It would be adjusted to %.lf.", speedValue, currentSpindleSpeed_U_MIN));
	}
	else
	{
		currentSpindleSpeed_U_MIN = speedValue;
	}
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::reconstructToolList() {
//////////////////////////////////////////////////////////////////
	wxString toolListString;
	for ( auto it = toolList.begin(); it != toolList.end(); ++it )
		toolListString.append(wxString::Format("%s=%s;", it->first, it->second.trace()));
		
	parameterMap[ID_TOOL_LIST] = toolListString;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::deleteTool(const wxString& id) {
//////////////////////////////////////////////////////////////////
	auto it = toolList.find(id);
	if ( it != toolList.end() )
	{
		toolList.erase(it);
		reconstructToolList();
	}
	else 
	{
		CTX_LOG_ERR(wxString::Format(" Cant find tool with id='%s", id));
	}
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::addTool(const wxString& toolString) {
//////////////////////////////////////////////////////////////////
	const wxString id(toolString.BeforeFirst('='));
	wxString val(toolString.AfterFirst('='));
	
	if ( val.Contains("[") == false )
		std::cerr << "Invalid tool string: '" << toolString << "' : missing '['" << std::endl;
	
	if ( val.Contains("]") == false )
		std::cerr << "Invalid tool string: '" << toolString << "' : missing ']'" << std::endl;

	val.Replace("[","");
	val.Replace("]","");
	val.Replace(";","");
	
	// over all tool parameters
	Tool t;
	unsigned int counter = 0;
	wxStringTokenizer tool(val, ",");
	while ( tool.HasMoreTokens() ) {
		const wxString& para = tool.GetNextToken();
		
		switch ( counter ) {
			case 0:
			{
				para.ToDouble(&t.diameter);
				break;
			}
		}
		
		counter++;
	}
	
	// store
	toolList[id] = t;
	reconstructToolList();
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setToolList(const wxString& list) {
//////////////////////////////////////////////////////////////////
	toolList.clear();
	
	// over all tools
	wxStringTokenizer tools(list, ";");
	while ( tools.HasMoreTokens() ) {
		const wxString& token = tools.GetNextToken();
		addTool(token);
	}
	
	if ( checkToolExists(ID_DEFAULT_TOOL_ID) == false )
		addTool(wxString::Format("%s=[0.00]",	ID_DEFAULT_TOOL_ID));
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::checkToolExists(const wxString& id) const {
//////////////////////////////////////////////////////////////////
	return toolList.find(id) != toolList.end();
}
//////////////////////////////////////////////////////////////////
double SvgCncContext::getToolDiameter(const wxString& id) const {
//////////////////////////////////////////////////////////////////
	auto it = toolList.find(id);
	if ( it == toolList.end() )
		return 0.0;
		
	return it->second.diameter;
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getPathModificationTypeAsStr() const {
//////////////////////////////////////////////////////////////////
	switch ( pathModification )
	{
		case CncPM_Pocket:				return "Pocket";
		case CncPM_Center:				return "Center";
		case CncPM_Inner:				return "Inner";
		case CncPM_Outer:				return "Outer";
		case CncPM_ZeroRef:				return "ZeroRef";
		case CncPM_Guide:				return "Guide";
		case CncPM_None:				return "None";
	}
	
	return wxString::Format("%s: unkown", CNC_LOG_FUNCT);
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getPathRuleTypeAsStr() const {
//////////////////////////////////////////////////////////////////
	switch ( pathRule )
	{
		case CncPR_EnsureClockwise:			return "EnsureCW";
		case CncPR_EnsureCounterClockwise:	return "EnsureCCW";
		case CncPR_Reverse:					return "Reverse"; 
		case CncPR_None:					return "None";
	}
	
	return wxString::Format("%s: unkown", CNC_LOG_FUNCT);
}
//////////////////////////////////////////////////////////////////
double SvgCncContext::getCurrentSpeed_MM_MIN(CncSpeedMode m) const {
//////////////////////////////////////////////////////////////////
	switch ( m ) {
		case CncSpeedRapid:			return currentRapidSpeed_MM_MIN;
		case CncSpeedWork:			return currentWorkSpeed_MM_MIN;
		case CncSpeedMax:			return 0.0;
		case CncSpeedUserDefined:	return 0.0;
	}
	
	return 0.0;
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getCurrentZDeptAsStr() const {
//////////////////////////////////////////////////////////////////
	return _(wxString::Format("%c%+.3lf", currentZDepthMode, currentZDepth));
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::getParameterList(DcmItemList& rows) const {
//////////////////////////////////////////////////////////////////
	DataControlModel::addKeyValueRow(rows, "SVG CNC-Parameters", 	"");
	for ( auto it = parameterMap.begin(); it != parameterMap.end(); ++it ) 
		DataControlModel::addKeyValueRow(rows, wxString::Format("  %s", it->first), it->second);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setFillColour(const wxColour & col) {
//////////////////////////////////////////////////////////////////
	fillColourDecoder.setColour(col);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setStrokeColour(const wxColour & col) {
//////////////////////////////////////////////////////////////////
	strokeColourDecoder.setColour(col);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::determineColourEffects() {
//////////////////////////////////////////////////////////////////
	// reset
	pathModification	= CncPM_Center;
	
	if ( useColourScheme() == true ) {
		
		CTX_LOG_INF(wxString::Format("Stroke matches (Black, Gray, Blue, Red) == (%s, %s, %s, %s)\n" 
										, (strokeColourDecoder.matchesBlack() ?	"true" : "false") 
										, (strokeColourDecoder.matchesGray()  ?	"true" : "false")
										, (strokeColourDecoder.matchesBlue()  ?	"true" : "false")
										, (strokeColourDecoder.matchesRed()   ?	"true" : "false")
		));

		CTX_LOG_INF(wxString::Format("Fill   matches (Black, Gray, Blue, Red) == (%s, %s, %s, %s)\n" 
										, (fillColourDecoder.matchesBlack() ?	"true" : "false") 
										, (fillColourDecoder.matchesGray()  ?	"true" : "false")
										, (fillColourDecoder.matchesBlue()  ?	"true" : "false")
										, (fillColourDecoder.matchesRed()   ?	"true" : "false")
		));

		// ----------------------------------------------------------
		if ( strokeColourDecoder.matchesBlack() )
		{
			if ( fillColourDecoder.matchesWhite() )
			{
				// interior cut
				pathModification = CncPM_Inner;
			}
			else if ( fillColourDecoder.matchesBlack() )
			{
				// exterior cut
				pathModification = CncPM_Outer;
			}
			else if ( fillColourDecoder.matchesBlue() )
			{
				// guide
				pathModification	= CncPM_Guide;
			}
		}
		// ----------------------------------------------------------
		else if ( strokeColourDecoder.matchesGray() )
		{
			if ( fillColourDecoder.matchesWhite() )
			{
				// on line cut
				pathModification = CncPM_Center;
			}
			else if ( fillColourDecoder.matchesGray() )
			{
				// pocketing cut
				pathModification = CncPM_Pocket;
			}
			else if ( fillColourDecoder.matchesBlue() )
			{
				// guide
				pathModification	= CncPM_Guide;
			}
		}
		// ----------------------------------------------------------
		else if ( fillColourDecoder.matchesRed() )
		{
			pathModification	= CncPM_ZeroRef;
		}
		// ----------------------------------------------------------
		else if ( strokeColourDecoder.matchesBlue() )
		{
			// guide
			pathModification	= CncPM_Guide;
		}
		// ----------------------------------------------------------
	}
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::expand(const SvgCncContextMacro& macro) {
//////////////////////////////////////////////////////////////////
	const ParameterMap& mpm = macro.getParameterMap();

	for ( auto it = mpm.begin(); it != mpm.end(); ++it ) {
		if ( it->first.IsSameAs(macro.MACRO_IDENTIFIER) == true )
			continue;
		
		if ( provide(it->first, it->second) == false ) {
			std::cerr << CNC_LOG_FUNCT_A(": provide %s = '%s' failed\n", it->first, it->second);
			return false;
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
const std::ostream& SvgCncContext::provideUsage(std::ostream& o, unsigned int indent) {
//////////////////////////////////////////////////////////////////
	
	o <<	"  The following parameters are implemented as state. Therefore, they are valid until they are overriden or reseted.\n"
			"  To reset a parameter use '${DELETE}' as value.\n"
			"\n"
			"    ZDepth=\"Z-4.25|z-0.3\"\n" 
			"     Z = abs. Z depth, z = rel. Z distance. Processes the path until that dept by using he <ZMaxFeedStep> parameter. [mm]\n" 
			"\n"
			"    ZMaxFeedStep=\"Z+2.0\"\n"
			"     Z = max Z feed step per crossing. It is default initalized by configuration. [mm]\n"
			"\n"
			"    UseColourScheme=\"Yes|No\"\n"
			"     If 'Yes' the shaper colour scheme is used to define the path modifications like inner / outer / pocket etc.\n"
			"\n"
			"    PathModification=\"Inner|Outer|Center|Pocket\"\n" 
			"     If UseColourScheme is off this will define the path modictaion to process\n"
			"\n"
			"    PathRule=\"ReversePath|EnsureClockwise|EnsureCounterClockwise\"\n"
			"     Defines rule to control the path\n"
			"\n"
			"    ToolList=\"T_300={3.00};T_600={6.00}\"\n" 
			"     Defines a list of avialable tools, current parametr {diameter} [mm]\n"
			"\n"
			"    ToolSelect=\"T_600\"\n" 
			"     Select the tool, the given ID has to be part of the <ToolList> parameter\n"
			"\n"
			"    RapidSpeed=\"R+500\"\n"
			"     Defines the rapid speed value to use. It is default initalized by configuration. [mm/min]\n"
			"\n"
			"    WorkSpeed=\"W+400\"\n"
			"     Defines the work speed value to use.It is default initalized by configuration. [mm/min]\n"
			"\n"
			"";
 			
			
	return o;
}


//////////////////////////////////////////////////////////////////
SvgCncContextSummary::SvgCncContextSummary()
: SvgCncContext			()
, participants			(0)
, toolSelections		(0)
, curToolId				("")
, toolSelectionList		("")
, curZDept				(0.0)
, minZDept				(0.0)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
void SvgCncContextSummary::add(const SvgCncContext& c) {
//////////////////////////////////////////////////////////////////
	participants++;
	
	// ----------------------------------------------------------
	// tool management
	const SvgCncContext::ToolList& tl = c.getToolList();
	for ( auto it = tl.cbegin(); it != tl.cend(); ++it)
		toolList[it->first] = it->second;
		
	if ( c.hasParameter(c.ID_TOOL_SEL) ) {
		const wxString newTooId(c.getParameterAsString(c.ID_TOOL_SEL));
		
		// if not current and not default
		if ( newTooId.IsSameAs(curToolId) == false ) {
			if ( newTooId.IsSameAs(c.ID_DEFAULT_TOOL_ID) == false ) {
				toolSelections++; 
				curToolId.assign(newTooId);
				toolSelectionList.append(wxString::Format("%s;", newTooId));
			}
		}
	}
	
	// ----------------------------------------------------------
	// z axis management
	const char		m = c.getCurrentZDepthMode();
	const double	z = c.getCurrentZDepth();
	
	switch ( m ) {
		case 'z':	curZDept -= z;
		case 'Z':	curZDept  = z;
	}
		
	minZDept = std::min(minZDept, curZDept);
}
//////////////////////////////////////////////////////////////////
wxString SvgCncContextSummary::getToolTotList() const {
//////////////////////////////////////////////////////////////////
	wxString ret;
	
	for ( auto it = toolList.cbegin(); it != toolList.cend(); ++it) 
		ret.append(wxString::Format("%s=%s; ", it->first, it->second.trace()));
	
	return ret;
}

//////////////////////////////////////////////////////////////////
void SvgCncContextSummary::traceTo(std::ostream& o, unsigned int indent) const {
//////////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	o	<< prefix	<< "Participants             : " << getParticipantCount()		<< std::endl
		<< prefix	<< "Min Z Depth              : " << getMinZDepth()				<< std::endl
		<< prefix	<< "Tool Count               : " << getToolCount()				<< std::endl
		<< prefix	<< "Tool Sel. List           : " << getToolSelList()			<< std::endl
		<< prefix	<< "Tool Tot. List           : " << getToolTotList()			<< std::endl
	;
}

