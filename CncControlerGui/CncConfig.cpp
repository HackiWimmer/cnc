#include <sstream>
#include <wx/msgdlg.h>
#include "MainFrame.h"
#include "CncConfigCommon.h"
#include "CncUnitCalculator.h"
#include "CncContext.h"
#include "CncConfig.h"

wxDEFINE_EVENT(wxEVT_CONFIG_UPDATE_NOTIFICATION, wxCommandEvent);

////////////////////////////////////////////////////////////////////////
// init static members
unsigned int CncConfig::globalPropertyCounter	= 0;
CncConfig*   CncConfig::globalCncConfig 		= NULL;

////////////////////////////////////////////////////////////////////////
// global variables - uses in dedicated pages
wxPropertyGridManager* globlSetupGrid 	= NULL;

ConfigPGEventMap  globalPGEventMap;
ConfigPropertyMap globalPropertyMap;

const wxString    renderSelectorFormat("%0.3lf mm - %u PPI");

////////////////////////////////////////////////////////////////////////
const wxString& CncConfig::ToolMagazineEntry::serialize(wxString& ret ) {
////////////////////////////////////////////////////////////////////////
	ret.assign(wxString::Format("T=%s", 		type));
	ret.append(wxString::Format(";D=%.3lf", 	diameter));
	ret.append(wxString::Format(";L=%.3lf", 	length));
	ret.append(wxString::Format(";O=%.3lf", 	offset));
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
			else if ( key == 'L' )		val.ToDouble(&length);
			else if ( key == 'O' )		val.ToDouble(&offset);
			else if ( key == 'C' )		comment.assign(val);
			else if ( key == 'T' )		type.assign(val);
		}
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////////
CncConfig::CncConfig(MainFrame* app) 
: changed(true)
, notificationActivated(true)
, osdConfigList()
, currentUnit(CncSteps)
, theApp(app)
, context(new CncContext())
, toolMagazine()
, registeredWindows()
, contMemAllocation(4)
, dispFactX(1.0), dispFactY(1.0), dispFactZ(1.0)
, calcFactX(1.0), calcFactY(1.0), calcFactZ(1.0)
, dispFactX3D(1.0), dispFactY3D(1.0), dispFactZ3D(1.0)
, replyThresholdX(1), replyThresholdY(1), replyThresholdZ(1)
, currentToolId(-1)
, currentZDepth(0.0)
, maxZDistance(50.0)
, workpieceThickness(0.0)
, referencePositionMode(CncRefPositionMode::CncRM_Mode1)
, referenceIncludesWpt(false)
, renderResolutionMM(0.1)
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
	
	if ( notificationActivated == true )
		broadcastConfigUpdateNotification();
}
////////////////////////////////////////////////////////////////////////
void CncConfig::rc() { 
//rc = reset changed
////////////////////////////////////////////////////////////////////////
	changed = false; 
}
////////////////////////////////////////////////////////////////////////
void CncConfig::deactivateConfigNotification() {
////////////////////////////////////////////////////////////////////////
	notificationActivated = false;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::activateConfigNotification(bool notify) {
////////////////////////////////////////////////////////////////////////
	notificationActivated = true;

	if ( notify == true ) {
		// release always a notification after the (re)activation
		broadcastConfigUpdateNotification();
	}
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
	
	THE_APP->GetLastConfigNotification()->ChangeValue(wxDateTime::UNow().Format("%H:%M:%S"));
	for ( auto it = registeredWindows.begin(); it != registeredWindows.end(); ++it ) {
		wxWindow* wnd = it->first;
		if ( wnd != NULL ) {
			wxPostEvent(wnd, evt);
		}
	}
}
////////////////////////////////////////////////////////////////////////
const CncDoublePosition& CncConfig::convertStepsToMetric(CncDoublePosition& ret, const CncLongPosition& pos) {
////////////////////////////////////////////////////////////////////////
	ret.setXYZ(convertStepsToMetricX(pos.getX()), convertStepsToMetricY(pos.getY()), convertStepsToMetricZ(pos.getZ()));
	return ret;
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::convertStepsToMetricX(int32_t val) {
////////////////////////////////////////////////////////////////////////
	return (double)val * dispFactX;
}
///////////////////////////////////////////////////////////////////////
const double CncConfig::convertStepsToMetricY(int32_t val) {
////////////////////////////////////////////////////////////////////////
	return (double)val * dispFactY;
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::convertStepsToMetricZ(int32_t val) {
////////////////////////////////////////////////////////////////////////
	return (double)val * dispFactZ;
}
////////////////////////////////////////////////////////////////////////
const CncLongPosition& CncConfig::convertMetricToSteps(CncLongPosition& ret, const CncDoublePosition& pos) {
////////////////////////////////////////////////////////////////////////
	ret.setXYZ(convertMetricToStepsX(pos.getX()), convertMetricToStepsY(pos.getY()), convertMetricToStepsZ(pos.getZ()));
	return ret;
}
////////////////////////////////////////////////////////////////////////
const int32_t CncConfig::convertMetricToStepsX(double val) {
////////////////////////////////////////////////////////////////////////
	return round(val * calcFactX);
}
////////////////////////////////////////////////////////////////////////
const int32_t CncConfig::convertMetricToStepsY(double val) {
////////////////////////////////////////////////////////////////////////
	return round(val * calcFactY);
}
////////////////////////////////////////////////////////////////////////
const int32_t CncConfig::convertMetricToStepsZ(double val) {
////////////////////////////////////////////////////////////////////////
	return round(val * calcFactZ);
}
////////////////////////////////////////////////////////////////////////
int32_t CncConfig::connvert_MM_SEC_TO_STP_SEC(double speed, unsigned int steps, double pitch) {
////////////////////////////////////////////////////////////////////////
	 return speed / (pitch / steps);
}
////////////////////////////////////////////////////////////////////////
int32_t CncConfig::connvert_MM_MIN_TO_STP_SEC(double speed, unsigned int steps, double pitch) {
////////////////////////////////////////////////////////////////////////
	return speed / 60.0 / (pitch / steps);
}
////////////////////////////////////////////////////////////////////////
double CncConfig::connvert_STP_SEC_TO_MM_SEC(int32_t speed, unsigned int steps, double pitch) {
////////////////////////////////////////////////////////////////////////
	return speed * (pitch / steps);
}
////////////////////////////////////////////////////////////////////////
double CncConfig::connvert_STP_SEC_TO_MM_MIN(int32_t speed, unsigned int steps, double pitch) {
////////////////////////////////////////////////////////////////////////
	return speed * (pitch / steps) * 60.0;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::calculateSpeedValues() {
////////////////////////////////////////////////////////////////////////
	CncSpeedController csc;
	csc.setup('X', getStepsX(), getPitchX(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, getLowPulsWidthX() + getHighPulsWidthX());
	csc.setup('Y', getStepsY(), getPitchY(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, getLowPulsWidthY() + getHighPulsWidthY());
	csc.setup('Z', getStepsZ(), getPitchZ(), SPEED_MANAGER_CONST_STATIC_OFFSET_US, SPEED_MANAGER_CONST_LOOP_OFFSET_US, getLowPulsWidthZ() + getHighPulsWidthZ());
	
	const int stepsXYZ     = ( getStepsX() +  getStepsY() +  getStepsZ() ) / 3;
	const double maxSpeedX = csc.X.maxDistPerMinute;
	const double maxSpeedY = csc.Y.maxDistPerMinute;
	const double maxSpeedZ = csc.Z.maxDistPerMinute;
	
	wxPGProperty* prop = NULL;
	{ prop = getProperty(CncConfig_STEPS_XYZ); 					if (prop != NULL) prop->SetValue(stepsXYZ); }
	{ prop = getProperty(CncConfig_MAX_SPEED_X_MM_MIN); 		if (prop != NULL) prop->SetValue(maxSpeedX); }
	{ prop = getProperty(CncConfig_MAX_SPEED_Y_MM_MIN); 		if (prop != NULL) prop->SetValue(maxSpeedY); }
	{ prop = getProperty(CncConfig_MAX_SPEED_Z_MM_MIN); 		if (prop != NULL) prop->SetValue(maxSpeedZ); }
	{ prop = getProperty(CncConfig_MAX_SPEED_XYZ_MM_MIN); 		if (prop != NULL) prop->SetValue(csc.getMaxFeedSpeed_MM_MIN()); }
	
	{ prop = getProperty(CncConfig_DEF_RAPID_SPEED_PERCENT);
		if (prop != NULL) {
			const double val = csc.getMaxFeedSpeed_MM_MIN() * prop->GetValue().GetDouble();
			{ prop = getProperty(CncConfig_DEF_RAPID_SPEED_MM_MIN); if (prop != NULL) prop->SetValue(val); }
		}
	}
	
	{ prop = getProperty(CncConfig_DEF_WORK_SPEED_PERCENT);
		if (prop != NULL) {
			const double val = csc.getMaxFeedSpeed_MM_MIN() * prop->GetValue().GetDouble();
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
	
	double maxDim = std::max(getMaxDimensionX(), getMaxDimensionY());
	maxDim        = std::max(getMaxDimensionZ(), maxDim);
	
	dispFactX3D = 1.0 * maxDim * calcFactX;
	dispFactY3D = 1.0 * maxDim * calcFactY;
	dispFactZ3D = 1.0 * maxDim * calcFactZ;
	
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
	globlSetupGrid->GetGrid()->SetCaptionBackgroundColour(wxColour(128,128,255));
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
	
	if ( entryName.StartsWith("OSD_") == true ) {
		// add the ODS list string
		osdConfigList.add(groupName, entryName, val);
		// get the correct OSD value
		osdConfigList.get(groupName, entryName, val);
	} 
	
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
	
	// tool magazine parameter
	if ( groupName == CncToolMagazineParam_SECTION_NAME ) {
		if      ( entryName == CncToolMagazineParam_USE_DEF_TOOL)		{ toolMagazineParameter.useDefaultTool  = ( value == "0" ? false : true ); }
		else if ( entryName == CncToolMagazineParam_MAP_DEF_TOOL_TO)	{ toolMagazineParameter.defaultMappedTo =  value; }
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
			
			wxString entry(p->GetName());
			if ( entry.StartsWith("#") == false ) {
				wxString val(p->GetValueAsString());
				
				if ( entry.Contains("OSD_") == true ) {
					// add the ODS value string
					osdConfigList.update(entry, val);
					// get the complete OSD list
					osdConfigList.getList(entry, val);
				}
				
				config.Write(wxString::Format("/%s", entry), val);
			}
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
	
	// tool magazine parameter
	section.assign(CncToolMagazineParam_SECTION_NAME);
	config.DeleteGroup(section);
	config.Write(wxString::Format("/%s/%s", section, CncToolMagazineParam_USE_DEF_TOOL), 	toolMagazineParameter.useDefaultTool);
	config.Write(wxString::Format("/%s/%s", section, CncToolMagazineParam_MAP_DEF_TOOL_TO), toolMagazineParameter.defaultMappedTo);
	
	
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
bool CncConfig::checkToolExists(int toolId) {
////////////////////////////////////////////////////////////////////////
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() )
		return true;
		
	// tool id did not exist
	return false;
}
////////////////////////////////////////////////////////////////////////
const wxString CncConfig::getToolParamAsString(int id) {
////////////////////////////////////////////////////////////////////////
	wxString ret;
	auto it = toolMagazine.find(id);
	if ( it == toolMagazine.end() ) {
		return wxString::Format("Unknown tool id: %d", id);
	}
	
	ToolMagazineEntry tme = it->second;
	ret = wxString::Format( "ID=%d; T=%s; D=%4.3lf; L=%4.3lf; O=%4.3lf",
							id,
							tme.type,
							tme.diameter,
							tme.length,
							tme.offset);
	return ret;
}
////////////////////////////////////////////////////////////////////////
const wxString CncConfig::getDefaultToolParamAsString() {
////////////////////////////////////////
	return getToolParamAsString(translateToolId(-1));
}
////////////////////////////////////////////////////////////////////////
const wxString CncConfig::getCurrentToolParamAsString() {
///////////////////////////////////////////////////////////////////////
	return getToolParamAsString(translateToolId(getCurrentToolId()));
}
////////////////////////////////////////////////////////////////////////
int CncConfig::translateToolId(int toolId) {
////////////////////////////////////////////////////////////////////////
	if ( toolId >= 0 ) {
		// try to find
		auto it = toolMagazine.find(toolId);
		if ( it != toolMagazine.end() )
			return toolId;
	}
	
	// tool id did not exist
	if ( toolMagazineParameter.useDefaultTool == false )
		return toolId;
		
	// evaluate tool mapping
	long id = -1;
	if ( toolMagazineParameter.defaultMappedTo.ToLong(&id) == false )
		return toolId;
		
	// search mapped tool
	auto it = toolMagazine.find(id);
	if ( it != toolMagazine.end() )
		return id;
		
	// mapped tool id did already not exist
	return toolId;
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::getToolDiameter(int toolId) {
////////////////////////////////////////////////////////////////////////
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() ) {
		return it->second.diameter;
	}
	
	return 0.0;
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::getToolLength(int toolId) {
////////////////////////////////////////////////////////////////////////
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() ) {
		return it->second.length;
	}
	
	return 0.0;
}
////////////////////////////////////////////////////////////////////////
const double CncConfig::getToolOffset(int toolId) {
////////////////////////////////////////////////////////////////////////
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() ) {
		return it->second.offset;
	}
	
	return 0.0;
}
////////////////////////////////////////////////////////////////////////
const wxString& CncConfig::getToolType(wxString& ret, int toolId) {
////////////////////////////////////////////////////////////////////////
	auto it = toolMagazine.find(toolId);
	if ( it != toolMagazine.end() ) {
		ret.assign(it->second.type);
		return ret;
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
////////////////////////////////////////////////////////////////////////
void CncConfig::setRenderResolution(double res) {
////////////////////////////////////////////////////////////////////////
	renderResolutionMM = res < 0.01 ? 0.01 : res;
	renderResolutionMM = res > 1.0  ? 1.0  : renderResolutionMM;
	
	if ( THE_APP == NULL )
		return;
		
	wxComboBox* cb = THE_APP->GetCbRenderResolution();
	unsigned int ppi = CncResolutionCalculator::getPointsPerInchForUnit(Unit::mm, renderResolutionMM);
	wxString item(wxString::Format(renderSelectorFormat, renderResolutionMM, ppi));

	int itemExits  = cb->FindString(item);
	if ( itemExits < 0 )
		cb->Append(item);
		
	cb->SetStringSelection(item);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setRenderResolution(const wxString& sel) {
////////////////////////////////////////////////////////////////////////
	int pos = sel.First('m'); // from mm
	if ( pos <= 0 )
		pos = sel.length();
	
	wxString selection = sel.SubString(0, pos - 1);
	double resolution; selection.ToDouble(&resolution);
	
	setRenderResolution(resolution);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupSelectorRenderResolution() {
////////////////////////////////////////////////////////////////////////
	if ( THE_APP != NULL ) {
		wxComboBox* cb = THE_APP->GetCbRenderResolution();
		cb->Clear();
		
		auto appendList = [&](float resolution) {
			
			unsigned int ppi = CncResolutionCalculator::getPointsPerInchForUnit(Unit::mm, resolution);
			cb->Append(wxString::Format(renderSelectorFormat, resolution, ppi));
		};
		
		appendList(0.01);
		appendList(0.02);
		appendList(0.03);
		appendList(0.04);
		appendList(0.05);
		appendList(0.06);
		appendList(0.07);
		appendList(0.08);
		appendList(0.09);
		appendList(0.10);
		appendList(0.20);
		appendList(0.262);
		appendList(0.30);
		appendList(0.40);
		appendList(0.50);
		appendList(0.60);
		appendList(0.70);
		appendList(0.80);
		appendList(0.90);
		
		setRenderResolution(0.262);
	}
}

////////////////////////////////////////////////////////////////////////
const char CncConfig::getRunConfirmationModeAsChar() {
////////////////////////////////////////////////////////////////////////
	wxString mode;
	getRunConfirmationMode(mode);

	if ( mode.IsEmpty() == true )
		mode.assign("Always");

	return (char)mode[0];
}

////////////////////////////////////////////////////////////////////////
// config getters
#define PROPERTY( id ) wxPGProperty* p = getProperty(id); wxASSERT(p); 

const bool CncConfig::getAutoConnectFlag()							{ PROPERTY(CncApplication_AUTO_CONNECT) 				return p->GetValue().GetBool(); }
const bool CncConfig::getAutoProcessFlag()							{ PROPERTY(CncApplication_AUTO_PROCESS) 				return p->GetValue().GetBool(); }
const bool CncConfig::getAutoOpenOglObserverFlag()					{ PROPERTY(CncApplication_AUTO_OPEN_OGL_OBSERVER) 		return p->GetValue().GetBool(); }
const bool CncConfig::getShowTestMenuFlag()							{ PROPERTY(CncApplication_SHOW_TEST_MENU) 				return p->GetValue().GetBool(); }
const bool CncConfig::getSvgConvertToRightHandFlag()				{ PROPERTY(CncSvg_Parser_REVERSE_Y_AXIS) 				return p->GetValue().GetBool(); }
const bool CncConfig::getAvoidDupSetterValuesFlag()					{ PROPERTY(CncWork_Ctl_AVOID_DUP_SETTER_VALUES) 		return p->GetValue().GetBool(); }
const bool CncConfig::getRequestIdleRequestFlag()					{ PROPERTY(CncWork_Ctl_REQUEST_IDLE_REQUESTS) 			return p->GetValue().GetBool(); }
const bool CncConfig::getInterruptByPosOutOfRangeFlag()				{ PROPERTY(CncWork_Ctl_INTERRUPT_BY_POS_OUTOFRANGE) 	return p->GetValue().GetBool(); }
const bool CncConfig::getInverseCtrlDirectionXFlag()				{ PROPERTY(CncWork_Ctl_INVERSE_CTL_DIRECTION_X) 		return p->GetValue().GetBool(); }
const bool CncConfig::getInverseCtrlDirectionYFlag()				{ PROPERTY(CncWork_Ctl_INVERSE_CTL_DIRECTION_Y) 		return p->GetValue().GetBool(); }
const bool CncConfig::getInverseCtrlDirectionZFlag()				{ PROPERTY(CncWork_Ctl_INVERSE_CTL_DIRECTION_Z) 		return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorAnalyseFlag()					{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_ANALYSE) 			return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessoSkipEmptyFlag()					{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_SKIP_EMPTY) 			return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorCombineMovesFlag()				{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_COMBINE_MOVES) 		return p->GetValue().GetBool(); }

const unsigned int CncConfig::getStepsX() 							{ PROPERTY(CncConfig_STEPS_X) 							return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsY() 							{ PROPERTY(CncConfig_STEPS_Y) 					 		return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsZ() 							{ PROPERTY(CncConfig_STEPS_Z) 							return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsXYZ()							{ PROPERTY(CncConfig_STEPS_XYZ) 					 	return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getLowPulsWidthX() 					{ PROPERTY(CncConfig_PULS_WIDTH_LOW_X) 			 		return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getLowPulsWidthY() 					{ PROPERTY(CncConfig_PULS_WIDTH_LOW_Y) 			 		return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getLowPulsWidthZ() 					{ PROPERTY(CncConfig_PULS_WIDTH_LOW_Z) 			 		return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthX() 					{ PROPERTY(CncConfig_PULS_WIDTH_HIGH_X) 			 	return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthY() 					{ PROPERTY(CncConfig_PULS_WIDTH_HIGH_Y) 			 	return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthZ() 					{ PROPERTY(CncConfig_PULS_WIDTH_HIGH_Z) 				return p->GetValue().GetInteger(); }

const double CncConfig::getMaxDimension() 							{ return std::max(std::max(getMaxDimensionX(), getMaxDimensionY()), getMaxDimensionZ()); }
const double CncConfig::getMaxDimensionX() 							{ PROPERTY(CncConfig_MAX_DIMENSION_X)					return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionY() 							{ PROPERTY(CncConfig_MAX_DIMENSION_Y) 				 	return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionZ() 							{ PROPERTY(CncConfig_MAX_DIMENSION_Z) 					return p->GetValue().GetDouble(); } 
const double CncConfig::getPitchX() 								{ PROPERTY(CncConfig_PITCH_X) 						 	return p->GetValue().GetDouble(); }
const double CncConfig::getPitchY() 								{ PROPERTY(CncConfig_PITCH_Y)							return p->GetValue().GetDouble(); }
const double CncConfig::getPitchZ() 								{ PROPERTY(CncConfig_PITCH_Z) 						 	return p->GetValue().GetDouble(); }
const double CncConfig::getMaxDurationThickness()					{ PROPERTY(CncWork_Wpt_MAX_THICKNESS_CROSS)				return p->GetValue().GetDouble(); }
const double CncConfig::getReplyThresholdMetric()					{ PROPERTY(CncWork_Ctl_REPLY_THRESHOLD_METRIC)			double ret; p->GetValueAsString().ToDouble(&ret); return ret; }

const double CncConfig::getDefaultRapidSpeed_MM_MIN()				{ PROPERTY(CncConfig_DEF_RAPID_SPEED_MM_MIN)			return p->GetValue().GetDouble(); }
const double CncConfig::getDefaultWorkSpeed_MM_MIN()				{ PROPERTY(CncConfig_DEF_WORK_SPEED_MM_MIN)				return p->GetValue().GetDouble(); }

const double CncConfig::getMaxSpeedX_MM_MIN()						{ PROPERTY(CncConfig_MAX_SPEED_X_MM_MIN) 				return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedY_MM_MIN()						{ PROPERTY(CncConfig_MAX_SPEED_Y_MM_MIN) 				return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedZ_MM_MIN()						{ PROPERTY(CncConfig_MAX_SPEED_Z_MM_MIN) 				return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedXYZ_MM_MIN()						{ PROPERTY(CncConfig_MAX_SPEED_XYZ_MM_MIN) 				return p->GetValue().GetDouble(); }

const double CncConfig::getAccelStartSpeedX_MM_MIN()				{ PROPERTY(CncConfig_ACCEL_START_SPEED_X_MM_MIN) 		return p->GetValue().GetDouble(); }
const double CncConfig::getAccelStopSpeedX_MM_MIN()					{ PROPERTY(CncConfig_ACCEL_STOP_SPEED_X_MM_MIN) 		return p->GetValue().GetDouble(); }
const double CncConfig::getAccelStartSpeedY_MM_MIN()				{ PROPERTY(CncConfig_ACCEL_START_SPEED_Y_MM_MIN) 		return p->GetValue().GetDouble(); }
const double CncConfig::getAccelStopSpeedY_MM_MIN()					{ PROPERTY(CncConfig_ACCEL_STOP_SPEED_Y_MM_MIN) 		return p->GetValue().GetDouble(); }
const double CncConfig::getAccelStartSpeedZ_MM_MIN()				{ PROPERTY(CncConfig_ACCEL_START_SPEED_Z_MM_MIN) 		return p->GetValue().GetDouble(); }
const double CncConfig::getAccelStopSpeedZ_MM_MIN()					{ PROPERTY(CncConfig_ACCEL_STOP_SPEED_Z_MM_MIN) 		return p->GetValue().GetDouble(); }
	
const double CncConfig::getMaxWorkSpeedX_MM_MIN()					{ PROPERTY(CncConfig_MAX_WORK_SPEED_X_MM_MIN)			return p->GetValue().GetDouble(); }
const double CncConfig::getMaxWorkSpeedY_MM_MIN()					{ PROPERTY(CncConfig_MAX_WORK_SPEED_Y_MM_MIN)			return p->GetValue().GetDouble(); }
const double CncConfig::getMaxWorkSpeedZ_MM_MIN()					{ PROPERTY(CncConfig_MAX_WORK_SPEED_Z_MM_MIN)			return p->GetValue().GetDouble(); }

const CncUnit CncConfig::getDisplayUnit() 							{ return currentUnit; }
const CncUnit CncConfig::getDefaultDisplayUnit()					{ PROPERTY(CncApplication_DEF_DISPLAY_UNIT) 			return ( p->GetValueAsString() == "mm" ? CncMetric : CncSteps ); }
const char*  CncConfig::getDefaultDisplayUnitAsStr()				{ PROPERTY(CncApplication_DEF_DISPLAY_UNIT) 			return ( p->GetValueAsString() == "mm" ? "mm" : "steps" ); }

const wxString& CncConfig::getFileBrowser(wxString& ret)			{ PROPERTY(CncApplication_Tool_FILE_BROWSER) 			ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getSVGFileViewer(wxString& ret)			{ PROPERTY(CncApplication_Tool_SVG_FILE_VIEWER) 		ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getBINFileViewer(wxString& ret)			{ PROPERTY(CncApplication_Tool_BIN_FILE_VIEWER) 		ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getGCodeFileViewer(wxString& ret)		{ PROPERTY(CncApplication_Tool_GCODE_FILE_VIEWER) 		ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getXMLFileViewer(wxString& ret)			{ PROPERTY(CncApplication_Tool_XML_FILE_VIEWER) 		ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getBrowser(wxString& ret)				{ PROPERTY(CncApplication_Tool_BROWSER) 				ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getEditorTool(wxString& ret)				{ PROPERTY(CncApplication_Tool_EXTERNAL_EDITOR) 		ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getPyCamTool(wxString& ret)				{ PROPERTY(CncApplication_Tool_PY_CAM); 				ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultSpeedModeXYZ(wxString& ret)	{ PROPERTY(CncConfig_DEF_SPEED_MODE_XYZ) 				ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultPort(wxString& ret)			{ PROPERTY(CncApplication_Com_DEFALT_PORT) 				ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultTplDir(wxString& ret)			{ PROPERTY(CncApplication_Tpl_DEFALT_DIRECTORY) 		ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getDefaultTplFile(wxString& ret)			{ PROPERTY(CncApplication_Tpl_DEFALT_FILE) 				ret.assign(p->GetValueAsString()); return ret; }
const wxString& CncConfig::getRunConfirmationMode(wxString& ret)	{ PROPERTY(CncApplication_CONFIRMATION_MODE) 			ret.assign(p->GetValueAsString()); return ret; }

////////////////////////////////////////////////////////////////////////
// config setters
CncConfig& CncConfig::setDisplayUnit(CncUnit unit) 					{ currentUnit = unit; sc(); return *this; }
CncConfig& CncConfig::setMaxDimensionX(const double val) 			{ PROPERTY(CncConfig_MAX_DIMENSION_X) 					p->SetValue(val); releaseChangedCallback(p); return *this; }
CncConfig& CncConfig::setMaxDimensionY(const double val) 			{ PROPERTY(CncConfig_MAX_DIMENSION_Y) 					p->SetValue(val); releaseChangedCallback(p); return *this; }
CncConfig& CncConfig::setMaxDimensionZ(const double val) 			{ PROPERTY(CncConfig_MAX_DIMENSION_Z) 					p->SetValue(val); releaseChangedCallback(p); return *this; }

#undef PROPERTY

