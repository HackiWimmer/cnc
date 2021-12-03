#include <fstream>
#include <wx/clipbrd.h>
#include "CncCommon.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncFileNameService.h"
#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncPositionStorageView.h"

////////////////////////////////////////////////////////////////////
CncPositionStorageView::CncPositionStorageView(wxWindow* parent)
: CncPositionStorageViewBase(parent)
, overview			()
, storage			()
, editorTool		()
, exportTool		()
////////////////////////////////////////////////////////////////////
{
	THE_CONFIG->getEditorTool(editorTool);
	THE_CONFIG->getVeuszPlotterTool(exportTool);
	
	typedef OverviewItemInfo::Format		OFMT;
	typedef PositionStorage					STORE;
	
	// -------------------------------------------------------------
	// setup overview content . . . 
	{
		OverviewItemInfo oi; oi.format = OFMT::LONG, oi.label = "Trigger Controller Position"; 
		oi.help =	"The position will traced at the point the serial callback " \
					"receives the next controller position";
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_CTL_POS );
		addStorage(STORE::TRIGGER_CTL_POS);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::DOUBLE, oi.label = "Trigger Application Position"; 
		oi.help =	"The position will traced at the point the serial callback " \
					"receives the next application position";
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_APP_POS );
		addStorage(STORE::TRIGGER_APP_POS);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::LONG, oi.label = "Trigger Direct Move"; 
		oi.help =	"The position will traced at the point the controller will trigger " \
					"the next direct move.";
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_DRM_POS );
		addStorage(STORE::TRIGGER_DRM_POS);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::LONG, oi.label = "Trigger Render and Move"; 
		oi.help =	"The position will traced at the point the controller will trigger " \
					"the next render cycle.";
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_RAM_POS );
		addStorage(STORE::TRIGGER_RAM_POS);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::LONG, oi.label = "Trigger Stepping"; 
		oi.help =	"The position will traced at the point the controller produces the next motor step.";
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_STP_POS );
		addStorage(STORE::TRIGGER_STP_POS);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::DOUBLE, oi.label = "Pathhandler Output";
		oi.help = 	"The position will traced at the point the Pathhandler generates a PathListEntry " \
					"and hand over it to the PathListManager."; 
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_PH_CB_POS );
		addStorage(STORE::TRIGGER_PH_CB_POS);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::DOUBLE, oi.label = "PathListRunner Output";
		oi.help = 	"The position will traced at the point the PathListRunnner generates an Output."; 
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_PH_LST_RUN );
		addStorage(STORE::TRIGGER_PH_LST_RUN);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::LONG, oi.label = "MoveSequence Add as int32_t";
		oi.help = 	"The position will traced at the point ......."; 
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_MOV_SEQ_ADD );
		addStorage(STORE::TRIGGER_MOV_SEQ_ADD);
	}
	{
		OverviewItemInfo oi; oi.format = OFMT::LONG, oi.label = "TRIGGER_SERIAL_NULL";
		oi.help = 	"The position will traced at the point ......."; 
		overview.push_back(oi); wxASSERT( overview.size() == STORE::TRIGGER_SERIAL_NULL );
		addStorage(STORE::TRIGGER_SERIAL_NULL);
	}
	
	
	// -------------------------------------------------------------
	// setup overview gui . . . 
	m_overview->EnableCheckBoxes(true);
	m_overview->AppendColumn("Eval|Context");
	m_overview->SetColumnWidth(0, 300);
	
	for (auto it = overview.begin(); it != overview.end(); ++it )
		m_overview->InsertItem(m_overview->GetItemCount(), it->label);
	
	this->m_overview->Bind(wxEVT_LIST_ITEM_CHECKED,		&CncPositionStorageView::onOverviewItemChecked,		this);
	this->m_overview->Bind(wxEVT_LIST_ITEM_UNCHECKED,	&CncPositionStorageView::onOverviewItemUnChecked,	this);
	
	m_overview->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	updateDetails();
	
	// -------------------------------------------------------------
	// setup detail gui . . . 
	m_details->AppendColumn("#", wxLIST_FORMAT_RIGHT,  60);
	m_details->AppendColumn("Abs X", wxLIST_FORMAT_RIGHT, 100);
	m_details->AppendColumn("Abs Y", wxLIST_FORMAT_RIGHT, 100);
	m_details->AppendColumn("Abs Z", wxLIST_FORMAT_RIGHT, 100);
} 
////////////////////////////////////////////////////////////////////
CncPositionStorageView::~CncPositionStorageView() {
////////////////////////////////////////////////////////////////////
	this->m_overview->Unbind(wxEVT_LIST_ITEM_CHECKED,	&CncPositionStorageView::onOverviewItemChecked,		this);
	this->m_overview->Unbind(wxEVT_LIST_ITEM_UNCHECKED,	&CncPositionStorageView::onOverviewItemUnChecked,	this);

	overview.clear();
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onCloseWindow(wxCloseEvent& event) {
////////////////////////////////////////////////////////////////////
	Show(false);
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::isItemValid(long item) {
////////////////////////////////////////////////////////////////////
	return ( item >= 0 && item < (long)overview.size() );
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::popProcessMode() {
////////////////////////////////////////////////////////////////////
	if ( m_details->IsFrozen() == false )
		m_details->Freeze();
		
	clearStorage();
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::pushProcessMode() {
////////////////////////////////////////////////////////////////////
	if ( m_details->IsFrozen() == true )
		m_details->Thaw();
		
	updateDetails();
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::init(uint8_t sid, bool state) {
////////////////////////////////////////////////////////////////////
	const long item = sid2Item(sid);
	if ( isItemValid(item) == false )
		return false;
		
	m_overview->CheckItem(item, state);
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addStorage(int8_t sid) {
////////////////////////////////////////////////////////////////////
	if ( auto it = storage.find(sid); it != storage.end() ) {
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' already exists!"
					<< std::endl;
		return false;
	}
	
	storage[sid] = DetailList();
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::clear(uint8_t sid) {
////////////////////////////////////////////////////////////////////
	auto it = storage.find(sid);
	if ( it == storage.end() ) {
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl;
		return false;
	}
	
	it->second.clear();
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addPos(uint8_t sid, const CncLongPosition& p) {
////////////////////////////////////////////////////////////////////
	auto it = storage.find(sid);
	if ( it == storage.end() ) {
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl;
		return false;
	}
	
	CncDoublePosition dp((double)p.getX(), (double)p.getY(), (double)p.getZ());
	it->second.push_back(std::move(Entry(dp)));
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addPos(uint8_t sid, const CncDoublePosition& p) {
////////////////////////////////////////////////////////////////////
	auto it = storage.find(sid);
	if ( it == storage.end() ) {
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl;
		return false;
	}
	
	it->second.push_back(std::move(Entry(p)));
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addPos(uint8_t sid, int32_t px, int32_t py, int32_t pz) {
////////////////////////////////////////////////////////////////////
	CncDoublePosition p((double)px, (double)py, (double)pz);
	return addPos(sid, p);
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addPos(uint8_t sid, double px, double py, double pz) {
////////////////////////////////////////////////////////////////////
	CncDoublePosition p(px, py, pz);
	return addPos(sid, p);
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addMove(uint8_t sid, int32_t dx, int32_t dy, int32_t dz) {
////////////////////////////////////////////////////////////////////
	return addMove(sid, (double)dx, (double)dy, (double)dz);
}
////////////////////////////////////////////////////////////////////
bool CncPositionStorageView::addMove(uint8_t sid, double dx, double dy, double dz) {
////////////////////////////////////////////////////////////////////
	auto it = storage.find(sid);
	if ( it == storage.end() ) 
	{
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl
		;
		return false;
	}
	
	DetailList& l = it->second;
	if ( l.size() == 0 )
		l.push_back(std::move(Entry(CncDoublePosition(0.0, 0.0, 0.0))));
	
	l.push_back(Entry(l.back().pos.inc(dx, dy, dz)));
	return true;
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onOverviewItemChecked(wxListEvent& event) {
////////////////////////////////////////////////////////////////////
	const long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
		
	onOverviewItemStateChange(item);
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onOverviewItemUnChecked(wxListEvent& event) {
////////////////////////////////////////////////////////////////////
	const long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
		
	onOverviewItemStateChange(item);
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onOverviewItemStateChange(long item) {
////////////////////////////////////////////////////////////////////
	const uint8_t sid = item2Sid(item);
	PositionStorage::enablePosStorageFor(sid, m_overview->IsItemChecked(item));
	clear(sid);
	updateDetails();
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onOverviewItemSelected(wxListEvent& event) {
////////////////////////////////////////////////////////////////////
	// reset text colour
	for ( auto it= overview.begin(); it != overview.end(); ++it ) {
		m_overview->SetItemTextColour(std::distance(overview.begin(), it), m_overview->GetTextColour());
	}
	
	const long item = event.m_itemIndex;
	if ( item == wxNOT_FOUND )
		return;
	
	if ( isItemValid(item) == false )
		return;
		
	// highlight selected item
	m_overview->SetItemTextColour(item, *wxYELLOW);
		
	m_helpText->ChangeValue(overview.at(item).help);
	m_detailsSid->ChangeValue(wxString::Format("%d", item2Sid(item)));
	
	updateDetails();
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::clearStorage() {
////////////////////////////////////////////////////////////////////
	for ( auto it= storage.begin(); it != storage.end(); ++it ) {
		it->second.clear();
	}
	
	m_details->DeleteAllItems();
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::updateDetails() {
////////////////////////////////////////////////////////////////////
	long sid; m_detailsSid->GetValue().ToLong(&sid);
	
	auto it = storage.find((uint8_t)sid);
	if ( it == storage.end() ) {
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl;
		return;
	}
	
	OverviewItemInfo::Format format = overview.at(sid2Item(sid)).format;
	
	DetailList& l = it->second;
	m_btExport	->Enable(l.size() > 0);
	m_btSave	->Enable(l.size() > 0);
	m_btCopy	->Enable(l.size() > 0);
	
	wxString label;
	m_detailsEntries->ChangeValue(wxString::Format("%ld", (long)l.size()));
	m_detailsType->ChangeValue(OverviewItemInfo::getFormatAsString(format));
	
	m_details->DeleteAllItems();
	for ( auto it= l.begin(); it != l.end(); ++it ) 
	{
		const long item = std::distance(l.begin(), it);
		
		m_details->InsertItem(item, wxString::Format("% 8ld", item), 0);
		
		m_details->SetItem(item, 1, it->getX(label, format));
		m_details->SetItem(item, 2, it->getY(label, format));
		m_details->SetItem(item, 3, it->getZ(label, format));
		
		if ( item > 63 )
			break;
	}
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onCopyDetails(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long sid; m_detailsSid->GetValue().ToLong(&sid);
	
	auto it = storage.find((uint8_t)sid);
	if ( it == storage.end() ) 
	{
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl
		;
		return;
	}
	
	// Write the content of entries to the clipboard
	if ( wxTheClipboard->Open() )
	{
		OverviewItemInfo::Format format = overview.at(sid2Item(sid)).format;
		DetailList& l = it->second;
		wxString tmp;
		
		std::stringstream ss;
		for (auto it = l.begin(); it != l.end(); ++it )
			ss << it->trace(tmp, format) << std::endl;
		
		// This data objects are held by the clipboard,
		// so do not delete them in the app.
		if ( wxTheClipboard->SetData( new wxTextDataObject(ss.str().c_str()) ) == false )
		{
			std::cerr	<< CNC_LOG_FUNCT << " copy to clipboard failed! '"
						<< sid
						<< "'"
						<< std::endl
			;
		}
		
		wxTheClipboard->Close();
	}
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onSaveDetails(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long sid; m_detailsSid->GetValue().ToLong(&sid);
	
	auto it = storage.find((uint8_t)sid);
	if ( it == storage.end() ) 
	{
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl
		;
		return;
	}
	
	wxFileName fnCSV(CncFileNameService::getTempFileName("csv"));
	std::ofstream outCSV(fnCSV.GetFullPath().c_str().AsChar(), std::ofstream::out);
	if ( !outCSV.good() ) 
	{
		std::cerr	<< CNC_LOG_FUNCT << " Can't create file: '" 
					<< fnCSV.GetFullPath()
					<< "'"
					<< std::endl
		;
		return;
	}
	
	OverviewItemInfo::Format format = overview.at(sid2Item(sid)).format;
	DetailList& l = it->second;
	wxString tmp;
	
	for (auto it = l.begin(); it != l.end(); ++it )
		outCSV << it->trace(tmp, format) << std::endl;
	
	outCSV.close();
	GblFunc::executeExternalProgram(editorTool, fnCSV.GetFullPath(), true);
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onExportDetails(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long sid; m_detailsSid->GetValue().ToLong(&sid);
	
	auto it = storage.find((uint8_t)sid);
	if ( it == storage.end() )
	{
		std::cerr	<< CNC_LOG_FUNCT << " storage with id '"
					<< sid
					<< "' did not exists!"
					<< std::endl
		;
		return;
	}
	
	OverviewItemInfo::Format format	= overview.at(sid2Item(sid)).format;
	DetailList& l 					= it->second;
	
	// -------------------------------------------------------------
	// setup export
	double minX = cnc::dbl::MAX;
	double maxX = cnc::dbl::MIN;
	double minY = cnc::dbl::MAX;
	double maxY = cnc::dbl::MIN;
	double minZ = cnc::dbl::MAX;
	double maxZ = cnc::dbl::MIN;
	
	const char delCSV = ',';
	
	wxFileName fnVSZ(CncFileNameService::getTempFileName("vsz"));
	wxFileName fnCSV(CncFileNameService::getTempFileName("csv"));
	
	// -------------------------------------------------------------
	// perform csv an boundaries
	{
		std::ofstream outCSV(fnCSV.GetFullPath().c_str().AsChar(), std::ofstream::out);
		if ( !outCSV.good() ) 
		{
			std::cerr	<< CNC_LOG_FUNCT << " Can't create file: '" 
						<< fnCSV.GetFullPath()
						<< "'"
						<< std::endl
			;
			return;
		}
		
		// csv header
		outCSV << "x1, +-, y1, +-, z1, +-" << std::endl;
		
		// csv body
		typedef OverviewItemInfo::Format FMT;
		
		wxString tmp;
		for (auto it = l.begin(); it != l.end(); ++it ) 
		{
			minX = std::min(minX, it->pos.getX()); maxX = std::max(maxX, it->pos.getX());
			minY = std::min(minY, it->pos.getY()); maxY = std::max(maxY, it->pos.getY());
			minZ = std::min(minZ, it->pos.getZ()); maxZ = std::max(maxZ, it->pos.getZ());
			
			switch ( format ) 
			{
				case FMT::DOUBLE:	outCSV	<< wxString::Format("%lf,0,%lf,0,%lf", 
																it->pos.getX(), 
																it->pos.getY(), 
																it->pos.getZ()) 
											<< std::endl;
									break;
				case FMT::LONG:		outCSV	<< wxString::Format("%ld,0,%ld,0,%ld", 
																(long)it->pos.getX(), 
																(long)it->pos.getY(), 
																(long)it->pos.getZ()) 
											<< std::endl;
									break;
			}
		}
		outCSV.close();
		
		double min = std::min(minX, std::min(minY, minZ));
		double max = std::max(maxX, std::max(maxY, maxZ));
		min *= 1.1; max *= 1.1;
		
		minX = min; maxX = max;
		minY = min; maxY = max;
		minZ = min; maxZ = max;
	}
	
	// -------------------------------------------------------------
	// perform vsz content
	{
		wxString vt;
		
		wxString ps1(wxFileName::GetPathSeparator(), 1);
		wxString ps2(wxFileName::GetPathSeparator(), 2);
		
		wxString cvsPathDoublePS(fnCSV.GetPath());
		cvsPathDoublePS.Replace(ps1, ps2);
		
		vt.append("# CNC saved document\n");
		vt.append(wxString::Format("# Saved at %s\n", wxDateTime::UNow().FormatISOCombined()));
		vt.append(wxString::Format("AddImportPath(u'%s')\n", cvsPathDoublePS));
		vt.append(wxString::Format("ImportFileCSV(u'%s', delimiter=u'%c', headermode=u'1st', linked=True)\n", fnCSV.GetFullName(), delCSV));

		vt.append("Set('height', u'18cm')\n");
		vt.append("Set('width', u'18cm')\n");
		vt.append("Set('colorTheme', u'default-latest')\n");
		vt.append("Set('StyleSheet/axis-function/autoRange', u'next-tick')\n");
		vt.append("Set('StyleSheet/axis3d/TickLabels/size', u'10pt')\n");
		vt.append("Set('StyleSheet/axis3d/GridLines/color', u'#dcdcdc')\n");
		vt.append("Set('StyleSheet/axis3d/GridLines/hide', False)\n");
		
		vt.append("Add('page', name=u'CncPage1', autoadd=False)\n");
		vt.append("To(u'CncPage1')\n");
		
		vt.append("Add('scene3d', name=u'CncScene3D1', autoadd=False)\n");
		vt.append("To(u'CncScene3D1')\n");
		vt.append("Set('xRotation', 30.0)\n");
		vt.append("Set('yRotation',  0.0)\n");
		vt.append("Set('zRotation',  0.0)\n");
		
		vt.append("Add('graph3d', name=u'CncGraph3D1', autoadd=False)\n");
		vt.append("To(u'CncGraph3D1')\n");
		vt.append("Set('xSize', 1.0)\n");
		vt.append("Set('ySize', 1.0)\n");
		vt.append("Set('zSize', 1.0)\n");
		
		vt.append("Add('axis3d', name=u'X', autoadd=False)\n");
		vt.append("To(u'X')\n");
		vt.append(wxString::Format("Set('min', %lf)\n", minX));
		vt.append(wxString::Format("Set('max', %lf)\n", maxX));
		vt.append("Set('upperPosition', 1.0)\n");

		vt.append("To('..')\n");
		vt.append("Add('axis3d', name=u'Y', autoadd=False)\n");
		vt.append("To(u'Y')\n");
		vt.append(wxString::Format("Set('min', %lf)\n", minY));
		vt.append(wxString::Format("Set('max', %lf)\n", maxY));
		vt.append("Set('autoRange', u'exact')\n");
		vt.append("Set('direction', u'y')\n");
		vt.append("Set('upperPosition', 1.0)\n");
		
		vt.append("To('..')\n");
		vt.append("Add('axis3d', name=u'Z', autoadd=False)\n");
		vt.append("To(u'Z')\n");
		vt.append(wxString::Format("Set('min', %lf)\n", minZ));
		vt.append(wxString::Format("Set('max', %lf)\n", maxZ));
		vt.append("Set('direction', u'z')\n");
		vt.append("Set('upperPosition', 1.0)\n");

		vt.append("To('..')\n");
		vt.append("Add('point3d', name=u'CncPoint3D1', autoadd=False)\n");
		vt.append("To(u'CncPoint3D1')\n");
		vt.append("Set('marker', u'dot')\n");
		vt.append("Set('color', u'blue')\n");
		vt.append("Set('markerSize', 1.0)\n");
		
		vt.append("Set('xData', u'x1')\n");
		vt.append("Set('yData', u'y1')\n");
		vt.append("Set('zData', u'z1')\n");
		vt.append("Set('xAxis', u'X')\n");
		vt.append("Set('yAxis', u'Y')\n");
		vt.append("Set('zAxis', u'Z')\n");
		vt.append("Set('scalePoints', [])\n");
		vt.append("Set('Color/points', [])\n");
		vt.append("Set('PlotLine/hide', False)\n");
		vt.append("Set('MarkerLine/hide', True)\n");
		vt.append("Set('Error/hide', True)\n");
		
		vt.append("To('..')\n");
		vt.append("To('..')\n");
		vt.append("To('..')\n");
		vt.append("To('..')\n");
		
		std::ofstream outVSZ(fnVSZ.GetFullPath().c_str().AsChar(), std::ofstream::out);
		if ( !outVSZ.good() ) 
		{
			std::cerr	<< CNC_LOG_FUNCT << " Can't create file: '" 
						<< fnVSZ.GetFullPath()
						<< "'"
						<< std::endl
			;
			return;
		}
		
		outVSZ << vt;
		outVSZ.close();
		
		// for test only
		//GblFunc::executeExternalProgram(editorTool, fnVSZ.GetFullPath(), true);
	}
	
	// -------------------------------------------------------------
	// export
	CNC_PRINT_LOCATION
	GblFunc::executeExternalProgram(exportTool, fnVSZ.GetFullPath(), true);
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onActivateWindow(wxActivateEvent& event) {
////////////////////////////////////////////////////////////////////
	/*
	if ( event.GetActive() == true )
		m_activationTimer->Start(100, true);
	
	event.Skip();
	*/
}
////////////////////////////////////////////////////////////////////
void CncPositionStorageView::onActivationTimer(wxTimerEvent& event) {
////////////////////////////////////////////////////////////////////
	THE_APP->Raise();
}
