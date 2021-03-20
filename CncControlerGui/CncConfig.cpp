#include <sstream>
#include <iomanip>
#include <wx/msgdlg.h>
#include "MainFrame.h"
#include "GlobalStrings.h"
#include "GlobalFunctions.h"
#include "CncConfigCommon.h"
#include "CncUnitCalculator.h"
#include "CncContext.h"
#include "CncPerspective.h"
#include "CncConfigProperties.h"
#include "wxCrafterImages.h"
#include "CncConfig.h"

#include "../Arduino/StepperEnvironment/ArdoObj.h"
#include "../Arduino/StepperEnvironment/CncAcmr.h"

extern GlobalConstStringDatabase globalStrings;

wxDEFINE_EVENT(wxEVT_CONFIG_UPDATE_NOTIFICATION, wxCommandEvent);

////////////////////////////////////////////////////////////////////////
// init static members
unsigned int CncConfig::globalPropertyCounter	= 0;
CncConfig*   CncConfig::globalCncConfig			= NULL;

////////////////////////////////////////////////////////////////////////
namespace Cnc {
	namespace Config {
		
		// global variables - uses in dedicated pages
		wxPropertyGridManager*	globlSetupGrid			= NULL;
		
		ConfigPGEventMap 		globalPGEventMap;
		ConfigCategoryMap		globalCategoryMap;
		ConfigPropertyMap		globalPropertyMap;
		
		// ------------------------------------------------------------
		wxString getPropertyShortNameFormId(const wxString& propId) {
			wxString name(propId);
			name.assign(name.AfterLast('|'));
			return name;
		}
		// ------------------------------------------------------------
		wxString getPropertySectionNameFormId(const wxString& propId) {
			if ( propId.Contains("|") == false )
				return _("/");
				
			return propId.BeforeLast('|');
		}
		// ------------------------------------------------------------
		wxString getPropertyShortName(const wxPGProperty& prop) {
			wxString name(prop.GetName());
			name.assign(name.AfterLast('.'));
			return name;
		}
		// ------------------------------------------------------------
		wxString getPropertyLongName(const wxPGProperty& prop) {
			return prop.GetName();
		}
		// ------------------------------------------------------------
		wxString getPropertySectionName(const wxPGProperty& prop) {
			return prop.GetAttribute(Attribute_SEC_NAME).GetString();
		}
		// ------------------------------------------------------------
		wxString getPropertyPath(const wxPGProperty& prop) {
			wxString path;
			const wxPGProperty* p = &prop;
			while ( p != NULL ) {
				path.Prepend(wxString::Format("%s+", getPropertyShortName(*p)));
				p = p->GetParent();
			}
			if ( path.IsEmpty() == false )
				path.Prepend('/');
			
			return path;
		}
		// ------------------------------------------------------------
		const wxPGProperty* getCategory(const wxPGProperty& prop) {
			const wxPGProperty* catProp = &prop;
			
			// initial check to avoid the assertion at the end
			if ( catProp->IsCategory() )
				return catProp;
				
			while ( catProp->GetParent() != NULL ) {
				catProp = catProp->GetParent();
				
				if ( catProp->IsCategory() )
					break;
			}
			
			return catProp;
		}
		// ------------------------------------------------------------
		wxString getCategoryName(const wxPGProperty& prop) {
			const wxPGProperty* cat = getCategory(prop);
			if ( cat == NULL)
				return _("");
			
			return cat->GetName();
		}
		// ------------------------------------------------------------
		bool compare(const wxPGProperty& prop, const wxString& propId) {
			const wxString sN1(getPropertyShortName(prop));
			const wxString sN2(getPropertyShortNameFormId(propId));
			
			return sN1 == sN2;
		}
		
	}; // namespace Config
}; // namespace Cnc

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
: loadTrace						()
, saveTrace						()
, obsoleteTrace					()
, changed						(true)
, notificationActivated			(true)
, osdConfigList					()
, currentUnit					(CncSteps)
, theApp						(app)
, context						(new CncContext())
, toolMagazine					()
, registeredWindows				()
, contMemAllocation				(4)
, dispFactX						(1.0)
, dispFactY						(1.0)
, dispFactZ						(1.0)
, dispFactH						(1.0)
, calcFactX						(1.0) 
, calcFactY						(1.0)
, calcFactZ						(1.0)
, calcFactH						(1.0)
, dispFactX3D					(1.0) 
, dispFactY3D					(1.0)
, dispFactZ3D					(1.0)
, replyThreshold				(1)
, currentToolId					(-1)
, currentZDepth					(0.0)
, maxZDistance					(50.0)
, renderResolutionMM			(0.1)
////////////////////////////////////////////////////////////////////////
{
	registerWindowForConfigNotification(app);
	init();
	APPEND_LOCATION_TO_STACK_TRACE_FILE
}
////////////////////////////////////////////////////////////////////////
CncConfig::~CncConfig() {
////////////////////////////////////////////////////////////////////////
	toolMagazine.clear();
	APPEND_LOCATION_TO_STACK_TRACE_FILE
}
////////////////////////////////////////////////////////////////////////
void CncConfig::init() {
////////////////////////////////////////////////////////////////////////
	wxEnumProperty* prop = wxDynamicCast(getProperty(CncWork_Ctl_PRE_PROSSOR_RENDER_RESOLUTION), wxEnumProperty);
	if ( prop )
		setRenderResolution(prop->GetValueAsString());
	
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
const double CncConfig::convertStepsToMetricH(int32_t val) {
////////////////////////////////////////////////////////////////////////
	return (double)val * dispFactH;
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
const int32_t CncConfig::convertMetricToStepsH(double val) {
////////////////////////////////////////////////////////////////////////
	return round(val * calcFactH);
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
	ArduinoAccelManager::Setup(getFeedrateX(), getFeedrateY(), getFeedrateZ());
	
	const double maxSpeed  = getMaxSpeedXYZ_MM_MIN();
	const double defRSpeed = getDefaultRapidSpeed_MM_MIN();
	const double defWSpeed = getDefaultWorkSpeed_MM_MIN();
	
	wxPGProperty* prop = NULL;
	prop = getProperty(CncConfig_DEF_RAPID_SPEED_MM_MIN); 
	if (prop != NULL) { prop->SetValue(std::min(maxSpeed, defRSpeed)); }
	
	prop = getProperty(CncConfig_DEF_WORK_SPEED_MM_MIN); 
	if (prop != NULL) { prop->SetValue(std::min(maxSpeed, defWSpeed)); }
}
////////////////////////////////////////////////////////////////////////
void CncConfig::calculateFactors() {
////////////////////////////////////////////////////////////////////////
	dispFactX = 0.0 + (getPitchX()/getStepsX());
	dispFactY = 0.0 + (getPitchY()/getStepsY());
	dispFactZ = 0.0 + (getPitchZ()/getStepsZ());
	dispFactH = 0.0 + (getPitchH()/getStepsH());
	
	calcFactX = 0.0 + (getStepsX()/getPitchX());
	calcFactY = 0.0 + (getStepsY()/getPitchY());
	calcFactZ = 0.0 + (getStepsZ()/getPitchZ());
	calcFactH = 0.0 + (getStepsH()/getPitchH());
	
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
}
////////////////////////////////////////////////////////////////////////
void CncConfig::collapse(wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL )
		return;
		
	wxPropertyGrid * grid = prop->GetGrid();
	if ( grid == NULL )
		return;
	
	grid->Collapse(prop);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::expand(wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL )
		return;
		
	wxPropertyGrid * grid = prop->GetGrid();
	if ( grid == NULL )
		return;
	
	grid->Expand(prop);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerPageRoot(wxPGProperty* prop, PGFuncPtrStore& fps) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL ) {
		std::cerr	<< CNC_LOG_FUNCT 
					<< " : Invalid property!" 
					<< std::endl;
		return;
	}
	
	if ( auto it = Cnc::Config::globalPGEventMap.find(prop); it != Cnc::Config::globalPGEventMap.end() ){
		std::cerr	<< CNC_LOG_FUNCT 
					<< " : Root property with name '" 
					<< fps.name 
					<< "' alreday exists!" 
					<< std::endl;
		return;
	}
	
	Cnc::Config::globalPGEventMap[prop] = fps;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerCategory(const wxString& name, wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL ) {
		std::cerr	<< CNC_LOG_FUNCT 
					<< " : Invalid Category!" 
					<< std::endl;
		return;
	}
	
	if ( auto it = Cnc::Config::globalCategoryMap.find(name); it != Cnc::Config::globalCategoryMap.end() ) {
		std::cerr	<< CNC_LOG_FUNCT 
					<< ": A Category with name '" 
					<< name 
					<< "' alreday exists!" 
					<< std::endl;
		return; 
	}
	
	const wxString catName(wxString::Format("CAT:%s:%ld", name, (long)Cnc::Config::globalCategoryMap.size()));
	
	Cnc::Config::globalCategoryMap[catName] = prop;
	prop->SetName(catName);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::registerProperty(const wxString& propId, wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	if ( prop == NULL ) {
		std::cerr	<< CNC_LOG_FUNCT 
					<< ": Invalid property!" << std::endl;
		return;
	}
	
	const wxString shortName(Cnc::Config::getPropertyShortNameFormId(propId));
	const wxString sectionName(Cnc::Config::getPropertySectionNameFormId(propId));
	
	if ( auto it = Cnc::Config::globalPropertyMap.find(shortName); it != Cnc::Config::globalPropertyMap.end() ){
		std::cerr	<< CNC_LOG_FUNCT 
					<< ": A Property with name '" 
					<< shortName 
					<< "' alreday exists!" 
					<< std::endl;
		return;
	}
	
	// this stores the property with its short name
	Cnc::Config::globalPropertyMap[shortName] = prop;
	// after the next line prop->GetName() contains the long name 
	// (property's name with all (non-category, non-root) parents). 
	prop->SetName(shortName);
	// this defines the file storage section
	prop->SetAttribute(Attribute_SEC_NAME, sectionName);
}
////////////////////////////////////////////////////////////////////////
wxPGProperty* CncConfig::getProperty(const wxString& propId, bool silent) {
////////////////////////////////////////////////////////////////////////
	const wxString shortName(Cnc::Config::getPropertyShortNameFormId(propId));

	if ( auto it = Cnc::Config::globalPropertyMap.find(shortName); it != Cnc::Config::globalPropertyMap.end() )
		return it->second;
	
	if ( silent == false ) {
		std::cerr	<< CNC_LOG_FUNCT 
					<< ": No property with short name '" 
					<< shortName 
					<< "' exists!" 
					<< std::endl;
	}
	
	return NULL;
}
////////////////////////////////////////////////////////////////////////
wxPGProperty* CncConfig::getPageRoot(wxPGProperty* prop) {
////////////////////////////////////////////////////////////////////////
	wxASSERT( prop != NULL );
	wxPGProperty* pageParent = prop;
	
	while ( pageParent->GetParent() != NULL && 
	        pageParent->GetParent() != prop->GetGrid()->GetRoot() && 
			pageParent->GetParent()->GetName() != "<Root>") {
		pageParent = pageParent->GetParent();
	}
	
	//std::clog << pageParent->GetName() << std::endl;
	return pageParent;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGlobalConfigurationGrid(wxPropertyGridManager* sg, wxConfigBase& cfg) {
////////////////////////////////////////////////////////////////////////
	wxASSERT(sg);
	
	// register custom editors
	wxPropertyGrid::RegisterEditorClass(new CncPGStaticTextCtrlEditor(),	"StaticTextCtrl");
	wxPropertyGrid::RegisterEditorClass(new CncPGSliderCtrlEditor(),		"SliderTextCtrl");
	
	Cnc::Config::globlSetupGrid = sg;
	Cnc::Config::globlSetupGrid->Clear();
	globalPropertyCounter = 0;
	
	// decoration grid part 1. 
	// do this before building the pages below
	{
		const wxColour bgCol1(140, 140, 140);
		const wxColour bgCol2( 64,  64,  64);
		const wxColour fgCol1(  0,   0,   0);
		
		//wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Consolas"));
		//Cnc::Config::globlSetupGrid->GetGrid()->SetFont(font);
		
		Cnc::Config::globlSetupGrid->GetGrid()->ResetColours();
		Cnc::Config::globlSetupGrid->GetGrid()->SetCaptionBackgroundColour	(bgCol2);
		Cnc::Config::globlSetupGrid->GetGrid()->SetCaptionTextColour		(wxColour(224, 224, 224));
		Cnc::Config::globlSetupGrid->GetGrid()->SetCellBackgroundColour		(bgCol1);
		Cnc::Config::globlSetupGrid->GetGrid()->SetCellTextColour			(fgCol1);
		Cnc::Config::globlSetupGrid->GetGrid()->SetCellDisabledTextColour	(fgCol1);
		Cnc::Config::globlSetupGrid->GetGrid()->SetMarginColour				(bgCol2);
		Cnc::Config::globlSetupGrid->GetGrid()->SetLineColour				(*wxLIGHT_GREY);
		Cnc::Config::globlSetupGrid->GetGrid()->SetEmptySpaceColour			(bgCol1);
		
		Cnc::Config::globlSetupGrid->GetGrid()->SetSelectionBackgroundColour(wxColour(239, 228, 176));
		Cnc::Config::globlSetupGrid->GetGrid()->SetSelectionTextColour		(wxColour(132,   0,   0));
		
		CncCfgStaticProperty::cellTextColour								= wxColour(fgCol1);
		CncCfgStaticProperty::cellBackgroungColour							= wxColour(bgCol1);
	}
	// decorate and setup pages
	{
		setupWorkingCfgPage		(cfg);
		setupGeneralCfgPage		(cfg);
		setupApplicationCfgPage	(cfg);
		setupEnvironmentCfgPage	(cfg);
		setupSvgCfgPage			(cfg);
		setupGCodeCfgPage		(cfg);
	}
	// decoration grid part 2
	// do this after building the pages above
	{
		for ( size_t i = 0; i< Cnc::Config::globlSetupGrid->GetPageCount(); i++) {
			Cnc::Config::globlSetupGrid->SelectPage(i);
			Cnc::Config::globlSetupGrid->GetGrid()->SetColumnCount(3);
		}
	}
}
////////////////////////////////////////////////////////////////////////
bool CncConfig::setPropertyValueFromConfig(const wxString& groupName, const wxString& entryName, const wxString& value) {
////////////////////////////////////////////////////////////////////////
	wxString val(value);
	if ( val.Trim(true).Trim(false).IsEmpty() )
		return false;
		
	wxPGProperty* prop = CncConfig::getProperty(entryName, true);
	if ( prop == NULL )
		return loadNonGuiConfig(groupName, entryName, value);
		
	if ( entryName.StartsWith(CncOSDConfigList::getOSDPrefix()) == true ) {
		// add the ODS list string
		osdConfigList.add(groupName, entryName, val);
		// get the correct OSD value
		osdConfigList.get(groupName, entryName, val);
	} 
	
	// SetValueFromString cut paths down to file if wxPG_FULL_VALUE isn't set
	prop->SetValueFromString(val, wxPG_FULL_VALUE);
	
	const wxString propType(prop->GetValueType());
	const wxString propKName(wxString::Format("%s.%s", groupName, entryName));
	const wxString className(prop->GetClassInfo()->GetClassName());

	loadTrace 	<< propKName								<< ":" 
				<< std::setw(60 - propKName.length())		<< "[" 
				<< propType									<< "], ["
				<< className								<< "], "
				<< prop->GetValueAsString()					<< ", " 
				<< prop->GetValueAsString(wxPG_FULL_VALUE)	<< ", " 
				<< prop->GetValue().GetString()				<< std::endl;
				
	return true;
}
////////////////////////////////////////////////////////////////////////
bool CncConfig::loadNonGuiConfig(const wxString& groupName, const wxString& entryName, const wxString& value) {
////////////////////////////////////////////////////////////////////////
	bool entryIsKnown = false;
	
	// "Perspectives"
	if ( groupName == CncPerspective::getConfigGroupName() ) {
		entryIsKnown = true;
	}
	// tool magazine
	else if ( groupName == CncToolMagazine_SECTION_NAME ) {
		long toolId;
		entryName.ToLong(&toolId);
		
		CncConfig::ToolMagazineEntry tme;
		tme.deserialize(value);

		if ( toolId >= TOOL_MAGAZINE_MIN_ID && toolId <= TOOL_MAGAZINE_MAX_ID ) {
			toolMagazine[toolId] = tme;
			entryIsKnown = true;
		}
	}
	// tool magazine parameter
	else if ( groupName == CncToolMagazineParam_SECTION_NAME ) {
		if      ( entryName == CncToolMagazineParam_USE_DEF_TOOL)		{ entryIsKnown = true; toolMagazineParameter.useDefaultTool  = ( value == "0" ? false : true ); }
		else if ( entryName == CncToolMagazineParam_MAP_DEF_TOOL_TO)	{ entryIsKnown = true; toolMagazineParameter.defaultMappedTo =  value; }
	}
	
	const wxString propKName(wxString::Format("%s.%s", groupName, entryName));
	
	if ( entryIsKnown == false ) {
		obsoleteTrace	<< propKName << ":"
						<< std::setw(60 - propKName.length()) << " "
						<< "Property isn't used currently."
						<< std::endl;
	}
	else {
		loadTrace		<< propKName << ":"
						<< std::setw(59 - propKName.length()) << " "
						<< "[*], [NonGui] Property is loaded."
						<< std::endl;
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////////
void CncConfig::loadConfiguration(wxConfigBase& config) {
////////////////////////////////////////////////////////////////////////
	if ( Cnc::Config::globlSetupGrid == NULL )
		return;
		
	// ....... todo
	toolMagazine.clear();
	
	loadTrace.str() = "";
	obsoleteTrace.str() = "";
	
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
	if ( Cnc::Config::globlSetupGrid == NULL )
		return;
		
	wxMessageDialog dlg(NULL,	_T("Do you realy save the current configuration?"), 
								_T("Save Configuration  . . . "), 
								wxYES|wxNO|wxCENTRE|wxICON_QUESTION
						);
						
	if ( dlg.ShowModal() != wxID_YES )
		return;
		
	saveTrace.str() = "";
	
	for ( wxPGVIterator it = Cnc::Config::globlSetupGrid->GetVIterator(wxPG_ITERATE_ALL); !it.AtEnd(); it.Next() ) {
		wxPGProperty* p = it.GetProperty();
		
		if ( p != NULL && p->IsCategory() == false ) {
			
			const wxString shortName(Cnc::Config::getPropertyShortName(*p));
			const wxString sectionName(Cnc::Config::getPropertySectionName(*p));
			
			if ( sectionName.StartsWith("#") == false ) {
				
				const wxString className(p->GetClassInfo()->GetClassName());
				const wxString savePath(wxString::Format("/%s/%s", sectionName, shortName));
				
				saveTrace	<< savePath									<< ":"
							<< std::setw(60 - savePath.length())		<< "[" 
							<< p->GetValueType()						<< "], ["
							<< className								<< "], " 
							<< p->GetValueAsString()					<< ", " 
							<< p->GetValueAsString(wxPG_FULL_VALUE)		<< ", " 
							<< p->GetValue().GetString()				<< std::endl;
				
				wxString val(p->GetValueAsString(wxPG_FULL_VALUE));
				
				if ( shortName.StartsWith(CncOSDConfigList::getOSDPrefix()) == true ) {
					
					const wxString osdId(CncOSDConfigList::makeId(sectionName, shortName));
					// add the ODS value string
					osdConfigList.update(osdId, val);
					// get the complete OSD list
					osdConfigList.getList(osdId, val);
				}
				
				//std::cout << "save: " << savePath << "=" << val << std::endl;
				config.Write(savePath, val);
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
void CncConfig::updateLoadTrace(wxTextCtrl* lTrace, wxTextCtrl*oTrace) {
////////////////////////////////////////////////////////////////////////
	if ( lTrace != NULL )
		lTrace->ChangeValue(loadTrace.str().c_str());
		
	if ( oTrace != NULL )
		oTrace->ChangeValue(obsoleteTrace.str().c_str());
}
////////////////////////////////////////////////////////////////////////
void CncConfig::updateSaveTrace(wxTextCtrl* sTrace) {
////////////////////////////////////////////////////////////////////////
	if ( sTrace != NULL )
		sTrace->ChangeValue(saveTrace.str().c_str());
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
	
	auto it = Cnc::Config::globalPGEventMap.find(CncConfig::getPageRoot(p));
	if ( it != Cnc::Config::globalPGEventMap.end() && it->second.propertyChanged != NULL)
		(*(it->second.propertyChanged))(event);
		
	wxASSERT( theApp != NULL );
	theApp->notifyConfigUpdate();
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
		auto it = Cnc::Config::globalPGEventMap.find(CncConfig::getPageRoot(p));
		if ( it != Cnc::Config::globalPGEventMap.end() && it->second.propertyChanging != NULL )
			(*(it->second.propertyChanging))(event);
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGridCommandButton(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = Cnc::Config::globlSetupGrid->GetSelectedProperty();
	if ( p == NULL )
		return;
	
	auto it = Cnc::Config::globalPGEventMap.find(CncConfig::getPageRoot(p));
	if ( it != Cnc::Config::globalPGEventMap.end() && it->second.propertyCommandButton != NULL )
		(*(it->second.propertyCommandButton))(event);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setupGridSelected(wxPropertyGridEvent& event) {
////////////////////////////////////////////////////////////////////////
	wxPGProperty* p = event.GetProperty();
	if ( p == NULL )
		return;
	
	auto it = Cnc::Config::globalPGEventMap.find(CncConfig::getPageRoot(p));
	if ( it != Cnc::Config::globalPGEventMap.end() && it->second.propertySelected != NULL )
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
	const double metric = getReplyThresholdMetric();
	if ( metric < 0.0 )
		return 1;
	
	const double min = pitch/steps;
	if ( metric <= min )
		return 1;
		
	return (unsigned int)(metric/min);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::calculateThresholds() {
////////////////////////////////////////////////////////////////////////
	replyThreshold = calculateThreshold(getPitchX(), getStepsX());
	getProperty(CncWork_Ctl_REPLY_THRESHOLD_STEPS)->SetValue((int)replyThreshold);
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setRenderResolution(double res) {
////////////////////////////////////////////////////////////////////////
	renderResolutionMM = res < 0.005 ? 0.005 : res;
	renderResolutionMM = res > 1.0   ? 1.0   : renderResolutionMM;
	
	wxEnumProperty* prop = wxDynamicCast(getProperty(CncWork_Ctl_PRE_PROSSOR_RENDER_RESOLUTION), wxEnumProperty);
	if ( prop ) {
		const unsigned int ppi = CncResolutionCalculator::getPointsPerInchForUnit(Unit::mm, renderResolutionMM);
		const wxString item(wxString::Format(globalStrings.renderSelectorFormat, renderResolutionMM, ppi));
		
		if ( prop->GetChoices().Index(item) < 0 )
			prop->AddChoice(item);
			
		prop->SetChoiceSelection(prop->GetChoices().Index(item));
	}
}
////////////////////////////////////////////////////////////////////////
void CncConfig::setRenderResolution(const wxString& sel) {
////////////////////////////////////////////////////////////////////////
	int pos = sel.First('m'); // from mm
	if ( pos <= 0 )
		pos = sel.length();
	
	//std::cout << CNC_LOG_LOCATION_A(": ") << sel << std::endl;
	const wxString selection = sel.SubString(0, pos - 1);
	double resolution; selection.ToDouble(&resolution);
	
	setRenderResolution(resolution);
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

const bool CncConfig::getSplashScreenFlag()							{ PROPERTY(CncApplication_SHOW_SPLASH_SCREEN) 				return p->GetValue().GetBool(); }
const bool CncConfig::getAutoConnectFlag()							{ PROPERTY(CncApplication_AUTO_CONNECT) 					return p->GetValue().GetBool(); }
const bool CncConfig::getAutoOpenLastFlag()							{ PROPERTY(CncApplication_AUTO_OPEN_LAST) 					return p->GetValue().GetBool(); }
const bool CncConfig::getAutoProcessFlag()							{ PROPERTY(CncApplication_AUTO_PROCESS) 					return p->GetValue().GetBool(); }
const bool CncConfig::getAutoOpenOglObserverFlag()					{ PROPERTY(CncApplication_AUTO_OPEN_OGL_OBSERVER) 			return p->GetValue().GetBool(); }
const bool CncConfig::getUseMainPreviewFlag()						{ PROPERTY(CncApplication_USE_MAIN_PREVIEW) 				return p->GetValue().GetBool(); }
const bool CncConfig::getUseMonitorPeviewFlag()						{ PROPERTY(CncApplication_USE_MONITOR_PREVIEW) 				return p->GetValue().GetBool(); }
const bool CncConfig::getShowTestMenuFlag()							{ PROPERTY(CncApplication_SHOW_TEST_MENU) 					return p->GetValue().GetBool(); }
const bool CncConfig::getSvgConvertToRightHandFlag()				{ PROPERTY(CncSvg_Parser_REVERSE_Y_AXIS) 					return p->GetValue().GetBool(); }
const bool CncConfig::getSvgConsiderViewboxFlag()					{ PROPERTY(CncSvg_Parser_CONSIDER_VIEWBOX) 					return p->GetValue().GetBool(); }
const bool CncConfig::getSvg3DViewFlag()							{ PROPERTY(CncSvg_Parser_DEFAULT_VIEW_TYPE)					return p->GetValueAsString().IsSameAs("3D"); }
const bool CncConfig::getSvgUseColourScheme()						{ PROPERTY(CncSvg_Parser_USE_COLOUR_SCHEME)					return p->GetValue().GetBool(); }
const bool CncConfig::getAvoidDupSetterValuesFlag()					{ PROPERTY(CncWork_Ctl_AVOID_DUP_SETTER_VALUES)				return p->GetValue().GetBool(); }
const bool CncConfig::getRequestIdleRequestFlag()					{ PROPERTY(CncWork_Ctl_REQUEST_IDLE_REQUESTS)				return p->GetValue().GetBool(); }
const bool CncConfig::getInterruptByPosOutOfRangeFlag()				{ PROPERTY(CncWork_Ctl_INTERRUPT_BY_POS_OUTOFRANGE)			return p->GetValue().GetBool(); }
const bool CncConfig::getInverseCtrlDirectionXFlag()				{ PROPERTY(CncWork_Ctl_INVERSE_CTL_DIRECTION_X)				return p->GetValue().GetBool(); }
const bool CncConfig::getInverseCtrlDirectionYFlag()				{ PROPERTY(CncWork_Ctl_INVERSE_CTL_DIRECTION_Y)				return p->GetValue().GetBool(); }
const bool CncConfig::getInverseCtrlDirectionZFlag()				{ PROPERTY(CncWork_Ctl_INVERSE_CTL_DIRECTION_Z)				return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorAnalyseFlag()					{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_ANALYSE)					return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessoSkipEmptyFlag()					{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_SKIP_EMPTY)				return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorCombineMovesFlag()				{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_COMBINE_MOVES)			return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorUseOperatingTrace()			{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_USE_OPERATING_TRACE)		return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorCntPathListEntries()			{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_CNT_PATH_LIST_ENTRIES)	return p->GetValue().GetBool(); }
const bool CncConfig::getPreProcessorCntMoveSequneces()				{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_CNT_SEQUENCE_MOVES)		return p->GetValue().GetBool(); }
const bool CncConfig::getSimulateMillingWithSoundFlag()				{ PROPERTY(CncWork_Ctl_SIMULATE_MILLING_WITH_SOUND)			return p->GetValue().GetBool(); }
const bool CncConfig::getCameraSupportFlag()						{ PROPERTY(CncWork_Ctl_CAMERA_SUPPORT)						return p->GetValue().GetBool(); }
const bool CncConfig::getSpindleSpeedSupportFlag()					{ PROPERTY(CncWork_Ctl_SPINDLE_SPEED_SUPPORT)				return p->GetValue().GetBool(); }

const unsigned int CncConfig::getStepsX()							{ PROPERTY(CncConfig_STEPS_X) 								return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsY()							{ PROPERTY(CncConfig_STEPS_Y) 								return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsZ()							{ PROPERTY(CncConfig_STEPS_Z) 								return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getStepsH()							{ PROPERTY(CncConfig_STEPS_H) 								return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthX()					{ PROPERTY(CncConfig_PULSE_WIDTH_HIGH_X) 				 	return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthY()					{ PROPERTY(CncConfig_PULSE_WIDTH_HIGH_Y) 				 	return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthZ()					{ PROPERTY(CncConfig_PULSE_WIDTH_HIGH_Z) 					return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getHighPulsWidthH()					{ PROPERTY(CncConfig_PULSE_WIDTH_HIGH_H) 					return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getArtificallyStepDelay()				{ PROPERTY(CncWork_Ctl_ARTIFICIALLY_STEP_DELAY) 			return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getCameraDeviceId()					{ PROPERTY(CncWork_Ctl_CAMERA_DEVICE_ID) 					return p->GetValue().GetInteger(); }
const unsigned int CncConfig::getSpindleSpeedStepRange()			{ PROPERTY(CncWork_Ctl_SPINDLE_SPEED_STEP_RANGE)			return p->GetValue().GetInteger(); }

const double CncConfig::getMaxDimension()							{ return std::max(std::max(getMaxDimensionX(), getMaxDimensionY()), getMaxDimensionZ()); }
const double CncConfig::getMaxDimensionX()							{ PROPERTY(CncConfig_MAX_DIMENSION_X)					return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionY()							{ PROPERTY(CncConfig_MAX_DIMENSION_Y) 				 	return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionZ()							{ PROPERTY(CncConfig_MAX_DIMENSION_Z) 					return p->GetValue().GetDouble(); } 
const double CncConfig::getMaxDimensionH()							{ PROPERTY(CncConfig_MAX_DIMENSION_H) 					return p->GetValue().GetDouble(); } 
const double CncConfig::getPitchX() 								{ PROPERTY(CncConfig_PITCH_X) 						 	return p->GetValue().GetDouble(); }
const double CncConfig::getPitchY() 								{ PROPERTY(CncConfig_PITCH_Y)							return p->GetValue().GetDouble(); }
const double CncConfig::getPitchZ() 								{ PROPERTY(CncConfig_PITCH_Z) 						 	return p->GetValue().GetDouble(); }
const double CncConfig::getPitchH() 								{ PROPERTY(CncConfig_PITCH_H) 						 	return p->GetValue().GetDouble(); }
const double CncConfig::getMaxDurationThickness()					{ PROPERTY(CncSvg_Parser_MAX_THICKNESS_CROSS)			return p->GetValue().GetDouble(); }
const double CncConfig::getSurefaceOffset()							{ PROPERTY(CncSvg_Parser_SUREFACE_Z_OFFSET)				return p->GetValue().GetDouble(); }
const double CncConfig::getReplyThresholdMetric()					{ PROPERTY(CncWork_Ctl_REPLY_THRESHOLD_METRIC)			double ret; p->GetValueAsString().ToDouble(&ret); return ret; }

const double CncConfig::getDefaultRapidSpeed_MM_MIN()				{ PROPERTY(CncConfig_DEF_RAPID_SPEED_MM_MIN)			return p->GetValue().GetDouble(); }
const double CncConfig::getDefaultWorkSpeed_MM_MIN()				{ PROPERTY(CncConfig_DEF_WORK_SPEED_MM_MIN)				return p->GetValue().GetDouble(); }
const double CncConfig::getMaxSpeedXYZ_MM_MIN()						{ PROPERTY(CncConfig_MAX_SPEED_XYZ_MM_MIN) 				return p->GetValue().GetDouble(); }

const double CncConfig::getMaxXYPitchToKeep()						{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_MAX_XY_PITCH) 		return p->GetValue().GetDouble(); }
const double CncConfig::getMaxZPitchToKeep()						{ PROPERTY(CncWork_Ctl_PRE_PROSSOR_MAX_Z_PITCH) 		return p->GetValue().GetDouble(); }

const double CncConfig::getAccelFunctParamA()						{ PROPERTY(CncConfig_ACCEL_FUNC_PARA_A)					return p->GetValue().GetDouble(); }
const double CncConfig::getAccelFunctParamB()						{ PROPERTY(CncConfig_ACCEL_FUNC_PARA_B)					return p->GetValue().GetDouble(); }
const double CncConfig::getAccelFunctParamC()						{ PROPERTY(CncConfig_ACCEL_FUNC_PARA_C)					return p->GetValue().GetDouble(); }
const double CncConfig::getDeaccelFunctParamA()						{ PROPERTY(CncConfig_DEACCEL_FUNC_PARA_A)				return p->GetValue().GetDouble(); }
const double CncConfig::getDeaccelFunctParamB()						{ PROPERTY(CncConfig_DEACCEL_FUNC_PARA_B)				return p->GetValue().GetDouble(); }
const double CncConfig::getDeaccelFunctParamC()						{ PROPERTY(CncConfig_DEACCEL_FUNC_PARA_C)				return p->GetValue().GetDouble(); }

const double CncConfig::getTouchblockZThickness()					{ PROPERTY(CncWork_Ctl_TOUCHBLOCK_Z_THICKNESS)			return p->GetValue().GetDouble(); }
const double CncConfig::getTouchblockXCheckThickness()				{ PROPERTY(CncWork_Ctl_TOUCHBLOCK_X_CHEEK_THICKNESS)	return p->GetValue().GetDouble(); }
const double CncConfig::getTouchblockYCheckThickness()				{ PROPERTY(CncWork_Ctl_TOUCHBLOCK_Y_CHEEK_THICKNESS)	return p->GetValue().GetDouble(); }

const double CncConfig::getCameraOffsetX()							{ PROPERTY(CncWork_Ctl_CAMERA_OFFSET_X)					return p->GetValue().GetDouble(); }
const double CncConfig::getCameraOffsetY()							{ PROPERTY(CncWork_Ctl_CAMERA_OFFSET_Y)					return p->GetValue().GetDouble(); }
const double CncConfig::getCameraOffsetZ()							{ PROPERTY(CncWork_Ctl_CAMERA_OFFSET_Z)					return p->GetValue().GetDouble(); }

const double CncConfig::getSpindleSpeedMin()						{ PROPERTY(CncWork_Ctl_SPINDLE_SPEED_MIN)				return p->GetValue().GetDouble(); }
const double CncConfig::getSpindleSpeedMax()						{ PROPERTY(CncWork_Ctl_SPINDLE_SPEED_MAX)				return p->GetValue().GetDouble(); }

const CncUnit CncConfig::getDisplayUnit() 							{ return currentUnit; }
const CncUnit CncConfig::getDefaultDisplayUnit()					{ PROPERTY(CncApplication_DEF_DISPLAY_UNIT) 			return ( p->GetValueAsString() == "mm" ? CncMetric : CncSteps ); }
const char*  CncConfig::getDefaultDisplayUnitAsStr()				{ PROPERTY(CncApplication_DEF_DISPLAY_UNIT) 			return ( p->GetValueAsString() == "mm" ? "mm" : "steps" ); }

const wxString& CncConfig::getFileBrowser(wxString& ret)			{ PROPERTY(CncApplication_Tool_FILE_BROWSER)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getSVGFileViewer(wxString& ret)			{ PROPERTY(CncApplication_Tool_SVG_FILE_VIEWER)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getBINFileViewer(wxString& ret)			{ PROPERTY(CncApplication_Tool_BIN_FILE_VIEWER)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getGCodeFileViewer(wxString& ret)		{ PROPERTY(CncApplication_Tool_GCODE_FILE_VIEWER)		ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getXMLFileViewer(wxString& ret)			{ PROPERTY(CncApplication_Tool_XML_FILE_VIEWER)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getBrowser(wxString& ret)				{ PROPERTY(CncApplication_Tool_BROWSER)					ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getEditorTool(wxString& ret)				{ PROPERTY(CncApplication_Tool_EXTERNAL_EDITOR)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getHexEditorTool(wxString& ret)			{ PROPERTY(CncApplication_Tool_EXTERNAL_HEX_EDITOR)		ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getVeuszPlotterTool(wxString& ret)		{ PROPERTY(CncApplication_Tool_VEUSZ_PLOTTER)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getPyCamTool(wxString& ret)				{ PROPERTY(CncApplication_Tool_PY_CAM)					ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getDefaultSpeedModeXYZ(wxString& ret)	{ PROPERTY(CncConfig_DEF_SPEED_MODE_XYZ)				ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getDefaultPort(wxString& ret)			{ PROPERTY(CncApplication_Com_DEFALT_PORT)				ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getDefaultTplDir(wxString& ret)			{ PROPERTY(CncApplication_Tpl_DEFALT_DIRECTORY)			ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getDefaultTplFile(wxString& ret)			{ PROPERTY(CncApplication_Tpl_DEFALT_FILE)				ret.assign(p->GetValue().GetString()); return ret; }
const wxString& CncConfig::getRunConfirmationMode(wxString& ret)	{ PROPERTY(CncApplication_CONFIRMATION_MODE)			ret.assign(p->GetValue().GetString()); return ret; }

////////////////////////////////////////////////////////////////////////
// config setters
CncConfig& CncConfig::setDisplayUnit(CncUnit unit) 					{ currentUnit = unit; sc(); return *this; }
CncConfig& CncConfig::setMaxDimensionX(const double val) 			{ PROPERTY(CncConfig_MAX_DIMENSION_X) 					p->SetValue(val); releaseChangedCallback(p); return *this; }
CncConfig& CncConfig::setMaxDimensionY(const double val) 			{ PROPERTY(CncConfig_MAX_DIMENSION_Y) 					p->SetValue(val); releaseChangedCallback(p); return *this; }
CncConfig& CncConfig::setMaxDimensionZ(const double val) 			{ PROPERTY(CncConfig_MAX_DIMENSION_Z) 					p->SetValue(val); releaseChangedCallback(p); return *this; }

#undef PROPERTY

