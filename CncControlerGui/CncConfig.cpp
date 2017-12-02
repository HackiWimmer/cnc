#include <sstream>
#include <wx/msgdlg.h>
#include "MainFrame.h"
#include "wxcrafter.h"
#include "CncConfigCommon.h"
#include "CncConfig.h"

wxDEFINE_EVENT(wxEVT_CONFIG_UPDATE_NOTIFICATION, wxCommandEvent);

////////////////////////////////////////////////////////////////////////
// init static members
unsigned int CncConfig::globalPropertyCounter	= 0;
CncConfig* CncConfig::globalCncConfig 			= NULL;
wxComboBox* CncConfig::gblCurveLibSelector 		= NULL; 

////////////////////////////////////////////////////////////////////////
// global variables - uses in dedicated pages
wxPropertyGridManager* globlSetupGrid 	= NULL;

ConfigPGEventMap  globalPGEventMap;
ConfigPropertyMap globalPropertyMap;

////////////////////////////////////////////////////////////////////////
const wxString& CncConfig::ToolMagazineEntry::serialize(wxString& ret ) {
////////////////////////////////////////////////////////////////////////
	ret.assign(wxString::Format("T=%s", 		type));
	ret.append(wxString::Format(";D=%.3lf", 	diameter));
	ret.append(wxString::Format(";C=%s", 		comment));
	return ret;
}
//////////////////////////////////////////////////////////////////
bool CncConfig::ToolMagazineEntry::deserialize(const wxString& input) {
////////////////////////////////////////////////////////////////////////
	wxStringTokenizer tokenizer(input, ";");
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		if ( token.IsEmpty() )
			continue;
		
		int pos = token.First('=');
		if ( pos != wxNOT_FOUND ) {
			wxString key = token.BeforeFirst('=');
			wxString val = token.AfterFirst('=');
			
			if      ( key == 'D' )		val.ToDouble(&diameter);
			else if ( key == 'C' )		comment.assign(val);
			else if ( key == 'T' )		type.assign(val);
		}
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////////
CncConfig::CncConfig(MainFrame* app) 
: changed(true)
, currentUnit(CncSteps)
, theApp(app)
, toolMagazine()
, registeredWindows()
, dispFactX(1.0), dispFactY(1.0), dispFactZ(1.0)
, calcFactX(1.0), calcFactY(1.0), calcFactZ(1.0)
, dispFactX3D(1.0), dispFactY3D(1.0), dispFactZ3D(1.0)
, replyThresholdX(1), replyThresholdY(1), replyThresholdZ(1)
, currentZDepth(0.0)
, maxZDistance(50.0)
, referenceIncludesWpt(false)
, onlineUpdateCoordinates(true)
, onlineUpdateDrawPane(true)
, allowEventHandling(true)
, updateInterval(100)
////////////////////////////////////////////////////////////////////////
{
	registerWindowForConfigNotification(app);
	init();
}
////////////////////////////////////////////////////////////////////////
CncConfig::~CncConfig() {
////////////////////////////////////////////////////////////////////////
	toolMagazine.clear();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::init() {
////////////////////////////////////////////////////////////////////////
	calculateFactors();
	calculateSpeedValues();
	calculateThresholds();
	initZAxisValues();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::sc() { 
//sc = set changed
////////////////////////////////////////////////////////////////////////
	changed = true; 
	
	broadcastConfigUpdateNotification();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::rc() { 
//rc = reset changed
////////////////////////////////////////////////////////////////////////
	changed = false; 
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerWindowForConfigNotification(wxWindow* wnd) {
////////////////////////////////////////////////////////////////////////
	if ( wnd == NULL )
		return;
	
	// to avoid duplicate registrations a map is used
	registeredWindows[wnd] = wnd;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::broadcastConfigUpdateNotification() {
////////////////////////////////////////////////////////////////////////
	wxCommandEvent evt(wxEVT_CONFIG_UPDATE_NOTIFICATION);
	
	for ( auto it = registeredWindows.begin(); it != registeredWindows.end(); ++it ) {
		wxWindow* wnd = it->first;
		if ( wnd != NULL && wnd->GetEventHandler() != NULL )
			wnd->GetEventHandler()->AddPendingEvent(evt);
	}
}
////////////////////////////////////////////////////////////////////////
const unsigned int CncConfig::calcSpeedOffsetX(double s) {
////////////////////////////////////////////////////////////////////////
	CncSpeedCalculator scX(getPitchX(), getStepsX(), getPulsWidthOffsetX());
	return scX.calcSpeedOffset(s);
}
////////////////////////////////////////////////////////////////////////
const unsigned int CncConfig::calcSpeedOffsetY(double s) {
////////////////////////////////////////////////////////////////////////
	CncSpeedCalculator scY(getPitchY(), getStepsY(), getPulsWidthOffsetY());
	return scY.calcSpeedOffset(s);
}
////////////////////////////////////////////////////////////////////////
const unsigned int CncConfig::calcSpeedOffsetZ(double s) {
////////////////////////////////////////////////////////////////////////
	CncSpeedCalculator scZ(getPitchZ(), getStepsZ(), getPulsWidthOffsetZ());
	return scZ.calcSpeedOffset(s);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::calculateSpeedValues() {
////////////////////////////////////////////////////////////////////////
	CncSpeedCalculator scX(getPitchX(), getStepsX(), getPulsWidthOffsetX());
	CncSpeedCalculator scY(getPitchY(), getStepsY(), getPulsWidthOffsetY());
	CncSpeedCalculator scZ(getPitchZ(), getStepsZ(), getPulsWidthOffsetZ());
	
	double maxSpeedX = scX.getMaxSpeed_MM_MIN();
	double maxSpeedY = scY.getMaxSpeed_MM_MIN();
	double maxSpeedZ = scZ.getMaxSpeed_MM_MIN();
	
	double smallestMaxSpeed = std::min(maxSpeedX, maxSpeedY);
	smallestMaxSpeed = std::min(smallestMaxSpeed, maxSpeedZ);
	
	wxPGProperty* prop = NULL;
	{ prop = getProperty(CncConfig_MAX_SPEED_X_MM_MIN); 		if (prop != NULL) prop->SetValue(maxSpeedX); }
	{ prop = getProperty(CncConfig_MAX_SPEED_Y_MM_MIN); 		if (prop != NULL) prop->SetValue(maxSpeedY); }
	{ prop = getProperty(CncConfig_MAX_SPEED_Z_MM_MIN); 		if (prop != NULL) prop->SetValue(maxSpeedZ); }
	{ prop = getProperty(CncConfig_MAX_SPEED_XYZ_MM_MIN); 		if (prop != NULL) prop->SetValue(smallestMaxSpeed); }
	
	{ prop = getProperty(CncConfig_DEF_RAPID_SPEED_PERCENT);
		if (prop != NULL) {
			double val = smallestMaxSpeed * prop->GetValue().GetDouble();
			{ prop = getProperty(CncConfig_DEF_RAPID_SPEED_MM_MIN); if (prop != NULL) prop->SetValue(val); }
		}
	}
	
	{ prop = getProperty(CncConfig_DEF_WORK_SPEED_PERCENT);
		if (prop != NULL) {
			double val = smallestMaxSpeed * prop->GetValue().GetDouble();
			{ prop = getProperty(CncConfig_DEF_WORK_SPEED_MM_MIN); if (prop != NULL) prop->SetValue(val); }
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::calculateFactors() {
////////////////////////////////////////////////////////////////////////
	dispFactX = 0.0 + (getPitchX()/getStepsX());
	dispFactY = 0.0 + (getPitchY()/getStepsY());
	dispFactZ = 0.0 + (getPitchZ()/getStepsZ());
	
	calcFactX = 0.0 + (getStepsX()/getPitchX());
	calcFactY = 0.0 + (getStepsY()/getPitchY());
	calcFactZ = 0.0 + (getStepsZ()/getPitchZ());
	
	dispFactX3D = 1.0 * getMaxDimensionX() * calcFactX;
	dispFactY3D = 1.0 * getMaxDimensionY() * calcFactY;
	dispFactZ3D = 1.0 * getMaxDimensionZ() * calcFactZ;
	
	updateCalculatedFactors();
}
////////////////////////////////////////////////////////////////////////
double CncConfig::convertX(CncUnit oldUnit, CncUnit newUnit, double value) {
////////////////////////////////////////////////////////////////////////
	if ( oldUnit == CncMetric && newUnit == CncSteps ) {
		return value * calcFactX;
	} else if ( oldUnit == CncSteps && newUnit == CncMetric) {
		return value * dispFactX;
	}
	return value;
}
////////////////////////////////////////////////////////////////////////
double CncConfig::convertY(CncUnit oldUnit, CncUnit newUnit, double value) {
////////////////////////////////////////////////////////////////////////
	if ( oldUnit == CncMetric && newUnit == CncSteps ) {
		return value * calcFactY;
	} else if ( oldUnit == CncSteps && newUnit == CncMetric) {
		return value * dispFactY;
	}
	return value;
}
////////////////////////////////////////////////////////////////////////
double CncConfig::convertZ(CncUnit oldUnit, CncUnit newUnit, double value) {
////////////////////////////////////////////////////////////////////////
	if ( oldUnit == CncMetric && newUnit == CncSteps ) {
		return value * calcFactZ;
	} else if ( oldUnit == CncSteps && newUnit == CncMetric) {
		return value * dispFactZ;
	}
	return value;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::initZAxisValues() {
	// example:
	// workpieceOffset		= 1.0;
	// maxDurationThickness	= 2.0;
	// currentZDepth		= 6.4;
////////////////////////////////////////////////////////////////////////
	workpieceOffset = 1.5;
		
	for (unsigned int i=0; i<maxDurations; i++ ) {
		durationSteps[i] = 0.0;
	}

	durationCount = 0;
	if ( getMaxDurationThickness() > 0 ) {
		durationCount = currentZDepth/ getMaxDurationThickness();						// --> 3
		if ( durationCount >= 1 ) {
			double rest = currentZDepth - (durationCount *  getMaxDurationThickness());	//--> 0.4
			
			if ( durationCount < maxDurations ) {
				for (unsigned int i=0; i<maxDurations; i++ ) {
					if ( i < durationCount )
						durationSteps[i] =  getMaxDurationThickness();
					else
						durationSteps[i] = 0.0;
				}

				if ( rest > 0 ) {
					durationSteps[durationCount] = rest;							// --> add 0.4
					durationCount++;												// --> 3 + 1 = 4 durations (3 * 2mm + 1 * 0.4mm )
				}
			} else {
				std::cerr << "CncConfig: maxDurations reached: " << durationCount << std::endl;
			}
		} else {
			// currentZDepth/maxDurationThickness < 1
			durationCount = 1;
			
			wxASSERT(currentZDepth <  getMaxDurationThickness());
			if ( currentZDepth >= 0.0 )	durationSteps[0] = currentZDepth;
			else 						durationSteps[0] = 0.0;
		}
	} else {
		std::cerr << "CncConfig: Invalid maxDurationThickness: " <<  getMaxDurationThickness() << std::endl;
	}
	
	updateCalculatedZAxisValues();
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::setCurrentZDepth(double dpt) {
////////////////////////////////////////////////////////////////////////
	sc();
	
	currentZDepth = dpt;
	
	if ( currentZDepth > getWorkpieceThickness() )
		currentZDepth = getWorkpieceThickness();
		
	// dpt is interpreted as abs value
	if ( currentZDepth < 0.0 )
		currentZDepth = 0.0;
	
	initZAxisValues();
	return currentZDepth;
}

////////////////////////////////////////////////////////////////////////
const double CncConfig::getDurationThickness(unsigned int duration) {
////////////////////////////////////////////////////////////////////////
	if ( duration < maxDurations ) {
		return durationSteps[duration];
	}
	return 0.0;
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::getDurationPositionAbs(unsigned int duration) {
////////////////////////////////////////////////////////////////////////
	if ( duration < maxDurations ) {
		double ret = workpieceOffset;
		for (unsigned int i=0; i<duration ;i++) {
			ret += durationSteps[i];
		}
		
		return getCurZDistance() - ret;
	}
	return 0.0;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerPageRoot(wxPGProperty* prop, PGFuncPtrStore& fps) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL ) {
		std::cerr << "CncConfig::registerPageRoot: Invalid property!" << std::endl;
		return;
	}
	
	auto it = globalPGEventMap.find(prop);
	if ( it != globalPGEventMap.end() ){
		//MessageBoxA(0,"CncConfig::registerPageRoot: Root property with name '' alreday exists!","CncConfig::registerPageRoot",0);
		std::cerr << "CncConfig::registerPageRoot: Root property with name '" << fps.name << "' alreday exists!" << std::endl;
		return;
	}
	
	globalPGEventMap[prop] = fps;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerCategory(const wxString& name, wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	/*
	static wxBitmap bmp;
	bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_CATEGORY"));
	
	globlSetupGrid->SetPropertyImage(prop, bmp);
	*/
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerProperty(const wxString& name, wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL ) {
		std::cerr << "CncConfig::registerProperty: Invalid property!" << std::endl;
		return;
	}
	
	auto it = globalPropertyMap.find(name);
	if ( it != globalPropertyMap.end() ){
		std::cerr << "CncConfig::registerProperty: Property with name '" << name << "' alreday exists!" << std::endl;
		return;
	}
	
	// decoration
	static wxBitmap bmp;
	bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_INACTIVE"));
	if ( prop->IsEnabled() == true ) {
		if      ( prop->GetValueType() == wxPG_VARIANT_TYPE_STRING ) 		bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_STRING"));
		else if ( prop->GetValueType() == wxPG_VARIANT_TYPE_LONG ) 			bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_LONG"));
		else if ( prop->GetValueType() == wxPG_VARIANT_TYPE_DOUBLE ) 		bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_DOUBLE"));
		else if ( prop->GetValueType() == wxPG_VARIANT_TYPE_BOOL ) 			bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_BOOL"));
		else if ( prop->GetValueType() == wxPG_VARIANT_TYPE_LIST ) 			bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_ENUM"));
		else													 			bmp = ImageLibConfig().Bitmap(_("BMP_VALUE_UNKNOWN"));
	}
	globlSetupGrid->SetPropertyImage(prop, bmp);
	
	// redecorate enabled = false
	if ( prop->IsEnabled() == false ) { 
		globlSetupGrid->SetPropertyAttribute(prop, Attribute_READONLY, "TRUE", 0);
		prop->Enable(true);
		prop->SetBackgroundColour(*wxLIGHT_GREY);
		prop->SetHelpString(wxString::Format("%s\n%s", "<Readonly>", prop->GetHelpString()));
	}
	
	prop->SetName(name);
	globalPropertyMap[name] = prop;
}
////////////////////////////////////////////////////////////////////////
wxPGProperty* CncConfig::getProperty(const wxString& name, bool silent) {
////////////////////////////////////////////////////////////////////////
	auto it = globalPropertyMap.find(name);
	if ( it != globalPropertyMap.end() )
		return it->second;
	
	if ( silent == false )
		std::cerr << "CncConfig::getProperty: No property with name '" << name << "' exists!" << std::endl;
		
	return NULL;
}
////////////////////////////////////////////////////////////////////////
wxPGProperty* CncConfig::getPageRoot(wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* pageParent = prop;
	
	while ( pageParent->GetParent() != NULL && 
	        pageParent->GetParent() != prop->GetGrid()->GetRoot() && 
			pageParent->GetParent()->GetName() != "<Root>") {
		pageParent = pageParent->GetParent() ;
	}
	
	//std::clog << pageParent->GetName() << std::endl;
	return pageParent;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGlobalConfigurationGrid(wxPropertyGridManager* sg, wxConfigBase& cfg) {
////////////////////////////////////////////////////////////////////////
	wxASSERT(sg);
	
	globlSetupGrid = sg;
	globlSetupGrid->Clear();
	globalPropertyCounter = 0;
	
	// decoration grid
	globlSetupGrid->GetGrid()->ResetColours();
	globlSetupGrid->GetGrid()->SetCaptionBackgroundColour(wxColour(255,182,108));
	globlSetupGrid->GetGrid()->SetCaptionTextColour(wxColour(255,255,255));
	globlSetupGrid->GetGrid()->SetCellBackgroundColour(wxColour(255,255,255));
	
	// decorate ans setup pages
	setupWorkingCfgPage(cfg);
	setupGeneralCfgPage(cfg);
	setupApplicationCfgPage(cfg);
	setupEnvironmentCfgPage(cfg);
	setupSvgCfgPage(cfg);
	setupGCodeCfgPage(cfg);
}
////////////////////////////////////////////////////////////////////////
bool CncConfig::setPropertyValueFromConfig(const wxString& groupName, const wxString& entryName, const wxString& value) {
////////////////////////////////////////////////////////////////////////
	wxString val(value);
	if ( val.Trim(true).Trim(false).IsEmpty() )
		return false;
		
	wxString propName(wxString::Format("%s/%s", groupName, entryName));
	wxPGProperty* prop = CncConfig::getProperty(propName, true);
	if ( prop == NULL )
		return loadNonGuiConfig(groupName, entryName, value);
	
	wxString propType(prop->GetValueType());
	prop->SetValueFromString(val);
	
	/*
	if ( propType == wxPG_VARIANT_TYPE_LONG ) {
		prop->SetValueFromString(val);
	}

	#define wxPG_VARIANT_TYPE_STRING        wxPGGlobalVars->m_strstring
	#define wxPG_VARIANT_TYPE_LONG          wxPGGlobalVars->m_strlong
	#define wxPG_VARIANT_TYPE_BOOL          wxPGGlobalVars->m_strbool
	#define wxPG_VARIANT_TYPE_LIST          wxPGGlobalVars->m_strlist
	#define wxPG_VARIANT_TYPE_DOUBLE        wxS("double")
	#define wxPG_VARIANT_TYPE_ARRSTRING     wxS("arrstring")
	#define wxPG_VARIANT_TYPE_DATETIME      wxS("datetime")
	#define wxPG_VARIANT_TYPE_LONGLONG      wxS("longlong")
	#define wxPG_VARIANT_TYPE_ULONGLONG     wxS("ulonglong")
	*/
	
	return true;
}
////////////////////////////////////////////////////////////////////////
bool CncConfig::loadNonGuiConfig(const wxString& groupName, const wxString& entryName, const wxString& value) {
////////////////////////////////////////////////////////////////////////
	// tool magazine
	if ( groupName == CncToolMagazine_SECTION_NAME ) {
		
		long toolId;
		entryName.ToLong(&toolId);
		
		CncConfig::ToolMagazineEntry tme;
		tme.deserialize(value);

		if ( toolId >= TOOL_MAGAZINE_MIN_ID && toolId <= TOOL_MAGAZINE_MAX_ID )
			toolMagazine[toolId] = tme;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::loadConfiguration(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	if ( globlSetupGrid == NULL )
		return;
		
	// ....... todo
	toolMagazine.clear();
	
	// enumeration variables
	wxArrayString groupNames;
	wxString groupName;
	long groupIndex;
	
	// store all groups...
	bool bGroup = config.GetFirstGroup(groupName, groupIndex);
	while ( bGroup ) {
		groupNames.Add(groupName);
		bGroup = config.GetNextGroup(groupName, groupIndex);
	}
	
	// over all groups
	for ( unsigned int i=0; i<groupNames.GetCount(); i++ ) {
		groupName.assign(groupNames.Item(i));
		config.SetPath(wxString::Format("/%s", groupName));
		
		// enumeration variables
		wxString entryName;
		long entryIndex;
	
		// over all group entries
		bool bEntry = config.GetFirstEntry(entryName, entryIndex);
		while ( bEntry ) {
			setPropertyValueFromConfig(groupName, entryName, config.Read(entryName, defaultConfigValue));
			bEntry = config.GetNextEntry(entryName, entryIndex);
		}
	}
	
	// reset the file configuration to the root path(group)
	config.SetPath("/");
	
	// perform the change flag
	sc();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::saveConfiguration(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	if ( globlSetupGrid == NULL )
		return;
		
	wxMessageDialog dlg(NULL, _T("Do you realy save the current configuration?"), _T("Save Configuration  . . . "), wxYES|wxNO|wxCENTRE|wxICON_QUESTION);
	if ( dlg.ShowModal() != wxID_YES )
		return;
	
	for ( wxPGVIterator it = globlSetupGrid->GetVIterator(wxPG_ITERATE_ALL); !it.AtEnd(); it.Next() ) {
		wxPGProperty* p = it.GetProperty();
		
		if ( p != NULL && p->IsCategory() == false ) {
			//std::clog << p->GetName() << "=" << p->GetValueAsString() << std::endl;
			wxString entry(p->GetName());
			if ( entry.StartsWith("#") == false )
				config.Write(wxString::Format("/%s", entry), p->GetValueAsString());
		}
	}
	
	saveNonGuiConfig(config);
	
	config.Flush();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::saveNonGuiConfig(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	wxString section;

	// tool magazine
	section.assign(CncToolMagazine_SECTION_NAME);
	config.DeleteGroup(section);
	for ( auto it = toolMagazine.begin(); it != toolMagazine.end(); ++it) {
		int id = it->first;
		
		if ( id < TOOL_MAGAZINE_MIN_ID || id > TOOL_MAGAZINE_MAX_ID)
			continue;
			
		wxString entry(wxString::Format("%d", id));
		wxString value;
		config.Write(wxString::Format("/%s/%s", section, entry), it->second.serialize(value));
	}
	
	//...
}
////////////////////////////////////////////////////////////////////////
void CncConfig::releaseChangedCallback(wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL )
		return;
		
	wxPropertyGridEvent event;
	event.SetProperty(prop);
	event.SetPropertyGrid(prop->GetGrid());
	event.SetPropertyValue(prop->GetValue());
	
	setupGridChanged(event);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGridChanged(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	// perform the change flag
	sc();
	
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	auto it = globalPGEventMap.find(CncConfig::getPageRoot(p));
	if ( it != globalPGEventMap.end() && it->second.propertyChanged != NULL)
		(*(it->second.propertyChanged))(event);
		
	wxASSERT( theApp != NULL );
	theApp->updateCncConfigTrace();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGridChanging(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
		
	if ( p->GetAttribute(Attribute_READONLY).IsNull() == false ) {
		// veto this event
		event.Veto();
		// Hack to do nothing and rstore the old value
		event.SetValidationFailureBehavior(wxPG_VFB_SHOW_MESSAGE_ON_STATUSBAR);
	} else {
		// process the event
		auto it = globalPGEventMap.find(CncConfig::getPageRoot(p));
		if ( it != globalPGEventMap.end()  && it->second.propertyChanging != NULL )
			(*(it->second.propertyChanging))(event);
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGridCommandButton(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = globlSetupGrid->GetSelectedProperty();
	if ( p == NULL )
		return;
	
	auto it = globalPGEventMap.find(CncConfig::getPageRoot(p));
	if ( it != globalPGEventMap.end() && it->second.propertyCommandButton != NULL )
		(*(it->second.propertyCommandButton))(event);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGridSelected(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	auto it = globalPGEventMap.find(CncConfig::getPageRoot(p));
	if ( it != globalPGEventMap.end() && it->second.propertySelected != NULL )
		(*(it->second.propertySelected))(event);
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::getToolDiameter(int toolId) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = getProperty(CncWork_Tool_DEFAULT);
	bool dflt = false;
	if ( p ) dflt = p->GetValue().GetDouble();
	
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() ) {
		// if tool id exists
		return it->second.diameter;
		
	} else {
		// if the default tool should used
		if ( dflt == true ) {
			it = toolMagazine.find(-1);
			if ( it != toolMagazine.end() )
				return it->second.diameter;
		}
	}
	
	return 0.0;
}
////////////////////////////////////////////////////////////////////////
const wxString& CncConfig::getToolType(wxString& ret, int toolId) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = getProperty(CncWork_Tool_DEFAULT);
	bool dflt = false;
	if ( p ) dflt = p->GetValue().GetDouble();
	
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() ) {
		// if tool id exists
		ret.assign(it->second.type);
		return ret;
		
	} else {
		// if the default tool should used
		if ( dflt == true ) {
			it = toolMagazine.find(-1);
			if ( it != toolMagazine.end() ) {
				ret.assign(it->second.type);
				return ret;
			}
		}
	}
	
	ret.assign("PEN");
	return ret;
}
////////////////////////////////////////////////////////////////////////
unsigned int CncConfig::calculateThreshold(double pitch, unsigned int steps) {
////////////////////////////////////////////////////////////////////////
	double metric		= getReplyThresholdMetric();
	if ( metric < 0.0 )
		return 1;
	
	double min = pitch/steps;
	if ( metric <= min )
		return 1;
		
	return (unsigned int)(metric/min);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::calculateThresholds() {
////////////////////////////////////////////////////////////////////////
	replyThresholdX = calculateThreshold(getPitchX(), getStepsX());
	replyThresholdY = calculateThreshold(getPitchY(), getStepsY());
	replyThresholdZ = calculateThreshold(getPitchZ(), getStepsZ());
	
	getProperty(CncWork_Ctl_REPLY_THRESHOLD_SETPS_X)->SetValue((int)replyThresholdX);
	getProperty(CncWork_Ctl_REPLY_THRESHOLD_SETPS_Y)->SetValue((int)replyThresholdY);
	getProperty(CncWork_Ctl_REPLY_THRESHOLD_SETPS_Z)->SetValue((int)replyThresholdZ);
}






#warning - todo reorganize getDefaultCurveLibResolution()
////////////////////////////////////////////////////////////////////////
float CncConfig::getDefaultCurveLibResolution() {
////////////////////////////////////////////////////////////////////////
	return CncSvgCurveLib::getDefaultResolution();
}


////////////////////////////////////////////////////////////////////////
float CncConfig::getCurveLibIncrement() { 
////////////////////////////////////////////////////////////////////////
	return CncSvgCurveLib::getIncement(); 
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setCurveLibIncrement(double v) { 
////////////////////////////////////////////////////////////////////////
	CncSvgCurveLib::setIncrement((float)v); 
	updateCurveLibIncrementSelector();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::updateCurveLibIncrementSelector() { 
////////////////////////////////////////////////////////////////////////
	if ( CncConfig::gblCurveLibSelector != NULL ) {
		CncConfig::gblCurveLibSelector->SetStringSelection(wxString::Format("%.3f", CncSvgCurveLib::getIncement()));
	}
}



////////////////////////////////////////////////////////////////////////
// config getters

const bool CncConfig::getAutoConnectFlag()							{ wxPGProperty* p = getProperty(CncApplication_AUTO_CONNECT); 			wxASSERT(p); return p->GetValue().GetBool(); }
const bool CncConfig::getAutoProcessFlag()							{ wxPGProperty* p = getProperty(CncApplication_AUTO_PROCESS); 			wxASSERT(p); return p->GetValue().GetBool(); }
const bool CncConfig::getShowTestMenuFlag()							{ wxPGProperty* p = getProperty(CncApplication_SHOW_TEST_MENU); 		wxASSERT(p); return p->GetValue().GetBool(); }
const bool CncConfig::getSvgResultWithOrigPathFlag()				{ wxPGProperty* p = getProperty(CncSvg_Emu_RSLT_WITH_ORIG_PATH); 		wxASSERT(p); return p->GetValue().GetBool(); }
const bool CncConfig::getSvgResultOnlyFirstCrossingFlag()			{ wxPGProperty* p = getProperty(CncSvg_Emu_RSLT_ONLY_WITH_FIRST_CROSS); wxASSERT(p); return p->GetValue().GetBool(); }
const bool CncConfig::getSvgReverseYAxisFlag()						{ wxPGProperty* p = getProperty(CncSvg_Parser_REVERSE_Y_AXIS); 			wxASSERT(p); return p->GetValue().GetBool(); }
const bool CncConfig::getAvoidDupSetterValuesFlag()					{ wxPGProperty* p = getProperty(CncWork_Ctl_AVOID_DUP_SETTER_VALUES); 	wxASSERT(p); return p->GetValue().GetBool(); }

const int CncConfig::getStepSignX()									{ return 1; } // currently not supported -1 = reverse
const int CncConfig::getStepSignY()									{ return 1; } // currently not supported -1 = reverse

const unsigned int CncConfig::getStepsX() 							{ wxPGProperty* p = getProperty(CncConfig_STEPS_X); 					wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsY() 							{ wxPGProperty* p = getProperty(CncConfig_STEPS_Y); 					wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsZ() 							{ wxPGProperty* p = getProperty(CncConfig_STEPS_Z); 					wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getPulsWidthOffsetX() 				{ wxPGProperty* p = getProperty(CncConfig_PULS_WIDTH_OFFSET_X); 		wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getPulsWidthOffsetY() 				{ wxPGProperty* p = getProperty(CncConfig_PULS_WIDTH_OFFSET_Y); 		wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getPulsWidthOffsetZ() 				{ wxPGProperty* p = getProperty(CncConfig_PULS_WIDTH_OFFSET_Z); 		wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getMultiplierX() 						{ wxPGProperty* p = getProperty(CncConfig_MULTIPLIER_X); 				wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getMultiplierY() 						{ wxPGProperty* p = getProperty(CncConfig_MULTIPLIER_Y); 				wxASSERT(p); return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getMultiplierZ()						{ wxPGProperty* p = getProperty(CncConfig_MULTIPLIER_Z); 				wxASSERT(p); return p->GetValue().GetInteger(); }

const double CncConfig::getMaxDimension() 							{ return std::max(std::max(getMaxDimensionX(), getMaxDimensionY()), getMaxDimensionZ()); }
const double CncConfig::getMaxDimensionX() 							{ wxPGProperty* p = getProperty(CncConfig_MAX_DIMENSION_X); 			wxASSERT(p); return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionY() 							{ wxPGProperty* p = getProperty(CncConfig_MAX_DIMENSION_Y); 			wxASSERT(p); return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionZ() 							{ wxPGProperty* p = getProperty(CncConfig_MAX_DIMENSION_Z); 			wxASSERT(p); return p->GetValue().GetDouble(); } 
const double CncConfig::getSvgEmulatorCopyFactor()					{ wxPGProperty* p = getProperty(CncSvg_Emu_COPY_FACTOR); 				wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getPitchX() 								{ wxPGProperty* p = getProperty(CncConfig_PITCH_X); 					wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getPitchY() 								{ wxPGProperty* p = getProperty(CncConfig_PITCH_Y);						wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getPitchZ() 								{ wxPGProperty* p = getProperty(CncConfig_PITCH_Z); 					wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getMaxDurationThickness()					{ wxPGProperty* p = getProperty(CncWork_Wpt_MAX_THICKNESS_CROSS);		wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getWorkpieceThickness()						{ wxPGProperty* p = getProperty(CncWork_Wpt_THICKNESS); 				wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getReplyThresholdMetric()					{ wxPGProperty* p = getProperty(CncWork_Ctl_REPLY_THRESHOLD_METRIC); 	wxASSERT(p); double ret; p->GetValueAsString().ToDouble(&ret); return ret; }

const double CncConfig::getDefaultRapidSpeed_MM_MIN()				{ wxPGProperty* p = getProperty(CncConfig_DEF_RAPID_SPEED_MM_MIN); 		wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getDefaultWorkSpeed_MM_MIN()				{ wxPGProperty* p = getProperty(CncConfig_DEF_WORK_SPEED_MM_MIN); 		wxASSERT(p); return p->GetValue().GetDouble(); }

const double CncConfig::getMaxSpeedX_MM_MIN()						{ wxPGProperty* p = getProperty(CncConfig_MAX_SPEED_X_MM_MIN); 			wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedY_MM_MIN()						{ wxPGProperty* p = getProperty(CncConfig_MAX_SPEED_Y_MM_MIN); 			wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedZ_MM_MIN()						{ wxPGProperty* p = getProperty(CncConfig_MAX_SPEED_Z_MM_MIN); 			wxASSERT(p); return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedXYZ_MM_MIN()						{ wxPGProperty* p = getProperty(CncConfig_MAX_SPEED_XYZ_MM_MIN); 		wxASSERT(p); return p->GetValue().GetDouble(); }

const CncUnit CncConfig::getDisplayUnit() 							{ return currentUnit; }
const CncUnit CncConfig::getDefaultDisplayUnit()					{ wxPGProperty* p = getProperty(CncApplication_DEF_DISPLAY_UNIT); 		wxASSERT(p); return ( p->GetValueAsString() == "mm" ? CncMetric : CncSteps ); }
const char*  CncConfig::getDefaultDisplayUnitAsStr()				{ wxPGProperty* p = getProperty(CncApplication_DEF_DISPLAY_UNIT); 		wxASSERT(p); return ( p->GetValueAsString() == "mm" ? "mm" : "steps" ); }

const wxString& CncConfig::getSVGFileViewer(wxString& ret)			{ wxPGProperty* p = getProperty(CncApplication_Tool_SVG_FILE_VIEWER); 	wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getGCodeFileViewer(wxString& ret)		{ wxPGProperty* p = getProperty(CncApplication_Tool_GCODE_FILE_VIEWER); wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getXMLFileViewer(wxString& ret)			{ wxPGProperty* p = getProperty(CncApplication_Tool_XML_FILE_VIEWER); 	wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getEditorTool(wxString& ret)				{ wxPGProperty* p = getProperty(CncApplication_Tool_EXTERNAL_EDITOR); 	wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getPyCamTool(wxString& ret)				{ wxPGProperty* p = getProperty(CncApplication_Tool_PY_CAM); 			wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultSpeedModeXYZ(wxString& ret)	{ wxPGProperty* p = getProperty(CncConfig_DEF_SPEED_MODE_XYZ); 			wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultPort(wxString& ret)			{ wxPGProperty* p = getProperty(CncApplication_Com_DEFALT_PORT); 		wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultTplDir(wxString& ret)			{ wxPGProperty* p = getProperty(CncApplication_Tpl_DEFALT_DIRECTORY); 	wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultTplFile(wxString& ret)			{ wxPGProperty* p = getProperty(CncApplication_Tpl_DEFALT_FILE); 		wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getRunConfirmationMode(wxString& ret)	{ wxPGProperty* p = getProperty(CncApplication_CONFIRMATION_MODE); 		wxASSERT(p); ret.assign(p->GetValueAsString()); return ret; }

////////////////////////////////////////////////////////////////////////
// config setters
CncConfig& CncConfig::setDisplayUnit(CncUnit unit) 					{ currentUnit = unit; sc(); return *this; }
CncConfig& CncConfig::setMaxDimensionX(const double val) 			{ wxPGProperty* p = getProperty(CncConfig_MAX_DIMENSION_X); 			wxASSERT(p); p->SetValue(val); releaseChangedCallback(p); return *this; }
CncConfig& CncConfig::setMaxDimensionY(const double val) 			{ wxPGProperty* p = getProperty(CncConfig_MAX_DIMENSION_Y); 			wxASSERT(p); p->SetValue(val); releaseChangedCallback(p); return *this; }
CncConfig& CncConfig::setMaxDimensionZ(const double val) 			{ wxPGProperty* p = getProperty(CncConfig_MAX_DIMENSION_Z); 			wxASSERT(p); p->SetValue(val); releaseChangedCallback(p); return *this; }


