
#include <iostream>
#include <fstream>
#include <wx/string.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/stdpaths.h>
#include "PathGeneratorFrame.h"

const char* PD_Preview_fileName						= "PathGeneratorPreview.svg";
wxString PathGeneratorFrame::globaGeneratedPath		= _T("");

///////////////////////////////////////////////////////////////////
PathGeneratorFrame::PathGeneratorFrame(wxWindow* parent, wxStyledTextCtrl* tplEditor)
: PathGeneratorFrameBase(parent)
, prevSizeHeight(-1)
, leftSplitterWidth(m_mainSplitter->GetSashPosition())
, leftSplitterMinimized(false)
, hasErrorInfo(false)
, templateEditor(tplEditor)
, previewHeight(1500)
, previewWidth(1500)
, viewBoxX(0)
, viewBoxY(0)
, viewBoxW(0)
, viewBoxH(0)
, lastSearchIndex(-1)
{
///////////////////////////////////////////////////////////////////
	globaGeneratedPath.clear();
	initControls();
	
	SetMinSize(wxSize(-1, minSizeHeight));
	SetSize(wxSize(-1, -1));
	m_pgMainBook->SetSelection(0);
	decorateSizeButton();
	decorateTreeSizeButton();
}
///////////////////////////////////////////////////////////////////
PathGeneratorFrame::~PathGeneratorFrame() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectPreview() {
///////////////////////////////////////////////////////////////////
	m_tbOutput->SetSelection(0);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectResult() {
///////////////////////////////////////////////////////////////////
	m_tbOutput->SetSelection(1);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectProcessInfo() {
///////////////////////////////////////////////////////////////////
	m_tbOutput->SetSelection(2);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearProcessInfo() {
///////////////////////////////////////////////////////////////////
	hasErrorInfo = false;
	m_processInfo->Clear();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::appendInfoMessage(const wxString& m) {
///////////////////////////////////////////////////////////////////
	m_processInfo->SetDefaultStyle(wxColour(201,201,201));
	m_processInfo->AppendText(m);
	
	if ( m.Last() != '\n')
		m_processInfo->AppendText('\n');
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::appendWarningMessage(const wxString& m) {
///////////////////////////////////////////////////////////////////
	m_processInfo->SetDefaultStyle(wxColour(255,201,15));
	m_processInfo->AppendText(m);
	
	if ( m.Last() != '\n')
		m_processInfo->AppendText('\n');
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::appendErrorMessage(const wxString& m) {
///////////////////////////////////////////////////////////////////
	hasErrorInfo = true;
	m_processInfo->SetDefaultStyle(wxColour(255,128,128));
	m_processInfo->AppendText(m);
	
	if ( m.Last() != '\n')
		m_processInfo->AppendText('\n');
}
///////////////////////////////////////////////////////////////////
bool PathGeneratorFrame::Show(bool show) {
///////////////////////////////////////////////////////////////////
	bool ret = PathGeneratorFrameBase::Show(show);
	
	if ( show == true ) {
		if ( m_pgPathSelector->GetSelection() < 0 ) {
			setupPathSelector();
			
			wxCommandEvent dummy;
			selectPathSelector(dummy);
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::closeWindow() {
///////////////////////////////////////////////////////////////////
	Show(false);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onCloseWindow(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	closeWindow();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onCloseWindowFromButton(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	closeWindow();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::copyPath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_generatedResult->SelectAll();
	m_generatedResult->Copy();
	m_generatedResult->SetSelection(0,0);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupPathSelector() {
///////////////////////////////////////////////////////////////////
	// Init path selector
	pathGeneratorStore.setupSelector(m_pgPathSelector);
	
	// the image list will be deletet by the tree control (m_templateTree)
	wxImageList* imageList = new wxImageList(16, 16);
	imageList->Add(ImageLib16().Bitmap("BMP_TEMPLATE_ROOT"));
	imageList->Add(ImageLib16().Bitmap("BMP_TEMPLATE_FOLDER_OPEN"));
	imageList->Add(ImageLib16().Bitmap("BMP_TEMPLATE_FOLDER_CLOSE"));
	imageList->Add(ImageLib16().Bitmap("BMP_TEMPLATE"));

	pathGeneratorStore.setupSelectorTree(m_templateTree, treeIndex, imageList);
	
	if ( pathGeneratorStore.getGenertorCount() > 0 ) {
		m_pgPathSelector->SetSelection(0);
		wxString item = m_pgPathSelector->GetStringSelection().SubString(0,2);
		searchAndSelectFirstTreeItem(item);
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::enableControls(bool state){
///////////////////////////////////////////////////////////////////
	m_btPGGenerate->Enable(state);
	m_btPGClear->Enable(state);
	m_btPGCopy->Enable(state);
	m_btPGClose->Enable(state);
	
	state == true ? m_generatedPreview->Thaw() : m_generatedPreview->Freeze();
	state == true ? m_generatedResult->Thaw() : m_generatedResult->Freeze();
	state == true ? wxSetCursor(wxNullCursor): wxSetCursor(wxCURSOR_WAIT);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::initControls() {
///////////////////////////////////////////////////////////////////
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_RETURN);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_PAGEDOWN);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_PREV_PROPERTY, WXK_PAGEUP);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY, WXK_DOWN);
	m_pgParameterMgr->GetGrid()->AddActionTrigger(wxPG_ACTION_PREV_PROPERTY, WXK_UP);
	
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_RETURN);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_PAGEUP);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_PAGEDOWN);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_UP);
	m_pgParameterMgr->GetGrid()->DedicateKey(WXK_DOWN);
	
	m_pgParameterMgr->GetGrid()->ResetColours();
	m_pgParameterMgr->GetGrid()->SetCaptionBackgroundColour(wxColour(255,255,128));
	m_pgParameterMgr->GetGrid()->SetCellBackgroundColour(wxColour(255,255,255));
	
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	
	validator.SetPrecision(3);
	validator.SetRange(0, 30);
	m_pgPropToolDiameter->SetValidator(validator);
	
	validator.SetPrecision(3);
	validator.SetRange(-400, 400);
	m_pgPropTranslateX->SetValidator(validator);
	m_pgPropTranslateY->SetValidator(validator);
	
	validator.SetPrecision(1);
	validator.SetRange(0, 10);
	m_pgPropScaleX->SetValidator(validator);
	m_pgPropScaleY->SetValidator(validator);
	
	validator.SetPrecision(1);
	validator.SetRange(-360, 360);
	m_pgPropRotateA->SetValidator(validator);
	validator.SetPrecision(3);
	validator.SetRange(-400, 400);
	m_pgPropRotateX->SetValidator(validator);
	m_pgPropRotateY->SetValidator(validator);
	
	validator.SetPrecision(1);
	validator.SetRange(-360, 360);
	m_pgPropSkewX->SetValidator(validator);
	m_pgPropSkewY->SetValidator(validator);
	
	validator.SetPrecision(0);
	validator.SetRange(0, 1500);
	m_pgPropSvgWidth->SetValidator(validator);
	m_pgPropSvgHeight->SetValidator(validator);
	m_pgPropSvgWidth->SetValue(wxString::Format("%d", previewWidth));
	m_pgPropSvgHeight->SetValue(wxString::Format("%d", previewHeight));
	
	validator.SetPrecision(0);
	validator.SetRange(0, 1500);
	m_pgPropSvgVbX->SetValidator(validator);
	m_pgPropSvgVbY->SetValidator(validator);
	m_pgPropSvgVbW->SetValidator(validator);
	m_pgPropSvgVbH->SetValidator(validator);
	m_pgPropSvgVbX->SetValue(wxString::Format("%d", viewBoxX));
	m_pgPropSvgVbY->SetValue(wxString::Format("%d", viewBoxY));
	m_pgPropSvgVbW->SetValue(wxString::Format("%d", viewBoxW));
	m_pgPropSvgVbH->SetValue(wxString::Format("%d", viewBoxH));
	
	validator.SetPrecision(3);
	validator.SetRange(-20, 100);
	m_pgPropCncZDepth->SetValidator(validator);
	m_pgPropCncZDepth->SetValue(wxString::Format("%.3lf", 0.0));
	
	validator.SetPrecision(0);
	validator.SetRange(1, 100);
	m_pgPropGridSize->SetValidator(validator);
	m_pgPropGridSize->SetValue(wxString::Format("%.0lf", 5.0));
	
	m_pgCatViewBox->SetExpanded(false);
	m_pgCatTranslate->SetExpanded(true);
	m_pgCatScale->SetExpanded(false);
	m_pgCatRotate->SetExpanded(false);
	m_pgCatSkrew->SetExpanded(false);
	
	setHelpInfos();
}
///////////////////////////////////////////////////////////////////
int PathGeneratorFrame::getPathSelection(const wxString& item) {
///////////////////////////////////////////////////////////////////
	wxString idStr = item.SubString(0,3);
	long id;
	idStr.ToLong(&id);
	
	if ( id >= 0 && id <= 999 )
		return id;
	
	return -1;
}
///////////////////////////////////////////////////////////////////
int PathGeneratorFrame::getPathSelection() {
///////////////////////////////////////////////////////////////////
	wxString item = m_pgPathSelector->GetStringSelection();
	return getPathSelection(item);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectPathSelector(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_pgPathSelector->IsEnabled() ) {
		selectPathSelector(getPathSelection());
		evaluateValues();
		generatePath();
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectTemplateTree(wxTreeEvent& event) {
///////////////////////////////////////////////////////////////////
	wxTreeItemId item = event.GetItem();
	int idx = getPathSelection(m_templateTree->GetItemText(item));
	if ( idx >= 0 && idx < pathGeneratorStore.getGenertorCount() ) {
		selectPathSelector(idx);
	
		evaluateValues();
		generatePath();
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::selectPathSelector(int id) {
///////////////////////////////////////////////////////////////////
	clearParameters();
	m_generatedPreview->Clear();
	m_generatedResult->Clear();
	
	if ( id != getPathSelection() )
		m_pgPathSelector->SetSelection(id);
	
	if ( id >= 0 ) {
		pathGeneratorStore.initPathGenerator(id);
		setupCommonValues(pathGeneratorStore.getCommonValues(id));
		setupCncParameterValues(pathGeneratorStore.getCncParameterValues(id));
		pathGeneratorStore.setupParameter(id, m_pgCatPath);
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearParameters() {
///////////////////////////////////////////////////////////////////
	if ( m_pgParameterMgr->IsShownOnScreen() == false)
		return;
	
	m_pgCatPath->DeleteChildren();
	m_pgParameterMgr->Refresh();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearView(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	clearView();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::clearView() {
///////////////////////////////////////////////////////////////////
	m_generatedPreview->Clear();
	m_generatedResult->Clear();
	globaGeneratedPath.Clear();
	updatePreview();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onActivate(wxActivateEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( templateEditor == NULL )
		return;
		
	m_btPGReplaceEdit->Enable(templateEditor->IsShownOnScreen());
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::relaceEditControlSelection(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( templateEditor == NULL )
		return;
		
	if ( templateEditor->IsShownOnScreen() == false )
		return;
		
	templateEditor->ReplaceSelection(m_generatedResult->GetValue());
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::toogleSize(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	wxSize s = GetSize();
	int height = s.GetHeight();
	
	if ( height > minSizeHeight ) {
		prevSizeHeight = height;
		SetSize(-1, minSizeHeight);
	} else {
		SetSize(-1, prevSizeHeight);
	}
	
	decorateSizeButton();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::toogleTreeSize(wxCommandEvent& event) {
	if ( m_mainSplitter->GetSashPosition() > m_mainSplitter->GetMinimumPaneSize() ) {
		int pos = m_mainSplitter->GetSashPosition();
		m_mainSplitter->SetSashPosition(m_mainSplitter->GetMinimumPaneSize());
		leftSplitterWidth = pos;
		leftSplitterMinimized = true;
	} else {
		m_mainSplitter->SetSashPosition(leftSplitterWidth);
		leftSplitterMinimized = false;
	}
	
	decorateTreeSizeButton();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::mainShashPositionChanged(wxSplitterEvent& event) {
///////////////////////////////////////////////////////////////////
	//leftSplitterWidth = m_mainSplitter->GetSashPosition();
	decorateTreeSizeButton();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::mainShashPositionChanging(wxSplitterEvent& event) {
///////////////////////////////////////////////////////////////////
	//leftSplitterMinimized = (m_mainSplitter->GetSashPosition() == m_mainSplitter->GetMinimumPaneSize());
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::decorateSizeButton() {
///////////////////////////////////////////////////////////////////
	if ( GetSize().GetHeight() > minSizeHeight )
		m_btPGMinimize->SetBitmap(ImageLib16().Bitmap("BMP_MINUS"));
	else											
		m_btPGMinimize->SetBitmap(ImageLib16().Bitmap("BMP_PLUS"));
		
	m_btPGMinimize->Refresh();
	m_btPGMinimize->Update();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::decorateTreeSizeButton() {
///////////////////////////////////////////////////////////////////
	if ( m_mainSplitter->GetSashPosition() > m_mainSplitter->GetMinimumPaneSize() )
		m_btPGMinimizeTree->SetBitmap(ImageLib16().Bitmap("BMP_MINUS"));
	else
		m_btPGMinimizeTree->SetBitmap(ImageLib16().Bitmap("BMP_PLUS"));
		
	m_btPGMinimizeTree->Refresh();
	m_btPGMinimizeTree->Update();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onSize(wxSizeEvent& event) {
///////////////////////////////////////////////////////////////////
cout << "onSize"<< endl;
	if ( leftSplitterMinimized == true )
		m_mainSplitter->SetSashPosition(m_mainSplitter->GetMinimumPaneSize());
		
	decorateSizeButton();
	decorateTreeSizeButton();
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::onMaximize(wxMaximizeEvent& event) {
///////////////////////////////////////////////////////////////////
cout << "onMaximize"<< endl;
	
	//if ( leftSplitterMinimized == true )
		m_mainSplitter->SetSashPosition(m_mainSplitter->GetMinimumPaneSize());
		
	decorateSizeButton();
	decorateTreeSizeButton();
	event.Skip(false);
}
///////////////////////////////////////////////////////////////////
bool PathGeneratorFrame::regenerateSvgBlock(PathGeneratorStore::RegenerateParameter & rp) {
///////////////////////////////////////////////////////////////////
	rp.out.errorInfo.clear();
	
	PathGeneratorBase::XmlPatternResult result;
	if ( PathGeneratorBase::decodeXmlPattern(rp.in.cncPattern, result) == false ) {
		rp.out.errorInfo << result.errorInfo << "\n";
		return false;
	}
	
	PathGeneratorBase* pGen = pathGeneratorStore.getPathGenerator(result.tplName);
	if ( pGen == NULL) {
		rp.out.errorInfo << "Cant find Template Generator: " << result.tplName << "\n";
		return false;
	}
	
	if ( pGen->getCheckSumAsString() != result.checkSum ) {
		rp.out.errorInfo << "Invalid CheckSum detected: " << pGen->getCheckSumAsString() << "!=" << result.checkSum  << "\n";
		return false;
	}
	
	selectPathSelector(pGen->getSelectorIndex());
	updateParameterValues(result.pMap);
	result.setToolDiameter(rp.in.toolDiameter);
	updateCommonValues(result.commonValues);
	updateCncParameterValues(result.cncParameterValues);
	updateTransformValues(result.transformValues);
	
	evaluateValues();
	generatePath();
	
	//todo replace selection
	
	return true;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateValues() {
///////////////////////////////////////////////////////////////////
	int id = getPathSelection();
	if ( id >= 0 ) {
		evaluateCommonValues(id);
		evaluateSvgParameterValues(id);
		evaluateCncParameterValues(id);
		evaluatePathParameterValues(id);
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePath(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	evaluateValues();
	generatePath();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::generatePath() {
///////////////////////////////////////////////////////////////////
	enableControls(false);
	clearProcessInfo();
	m_pgMainBook->SetSelection(0);

	int id = getPathSelection();
	if ( id >= 0 ) {

		// generate path
		cnc::pg::trc.clear();
		pathGeneratorStore.resetErrorInfo(id);

		// prepare preview and result
		wxString svgFragment(pathGeneratorStore.generatePath(id));
		
		wxString preview(svgFragment);
		wxString result(svgFragment);
		
		SvgGeneratorBase::finalizeSvgFragment(preview, true);
		SvgGeneratorBase::finalizeSvgFragment(result, false);
		
		// update result
		m_generatedResult->Clear();
		m_generatedResult->SetDefaultStyle(*wxWHITE);
		m_generatedResult->AppendText(result);
		
		// update global Value
		globaGeneratedPath = result;
		
		// update preview
		cnc::pg::trc.blue();
		cnc::pg::trc << preview;

		if ( pathGeneratorStore.hasErrorInfo(id) == true )
			appendErrorMessage(pathGeneratorStore.getErrorInfo(id));

		// update transform values
		if ( m_pgPropTransformMode->GetValue().GetBool() == true )
			updateTransformValues(pathGeneratorStore.getTransformValues(id));
			
	} else {
		clearView();
		appendErrorMessage(wxString::Format("PathGeneratorFrame::generatePath: Invalid id: %d\n", id));
	}
	
	if ( hasErrorInfo == true )	selectProcessInfo();
	else						selectPreview();
	
	updatePreview();
	enableControls(true);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateCommonValues(const PathGeneratorBase::CommonValues& cv) {
///////////////////////////////////////////////////////////////////
	m_pgPropCorrection->SetValue(cv.toolCorrection);
	m_pgPropToolDiameter->SetValue(cv.toolDiameter);
	//((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour;
	m_pgPropConfigBlock->SetValue(cv.configBlock);
	m_pgPropRefCross->SetValue(cv.referenceCross);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateCommonValues(int id) {
///////////////////////////////////////////////////////////////////
	PathGeneratorBase::CommonValues cv;
	cv.toolCorrection	= m_pgPropCorrection->GetValue().GetBool();
	cv.toolDiameter 	= m_pgPropToolDiameter->GetValue();
	cv.pathColour		= ((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour;
	cv.configBlock		= m_pgPropConfigBlock->GetValue().GetBool();
	cv.referenceCross	= m_pgPropRefCross->GetValue().GetBool();
	
	pathGeneratorStore.setCommonValues(id, cv);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateTransformValues(const PathGeneratorBase::TransformValues& tv) {
///////////////////////////////////////////////////////////////////
	m_pgPropTransformMode->SetValue(tv.autoMode);
	
	m_pgPropTranslateX->SetValue(tv.translateX);
	m_pgPropTranslateY->SetValue(tv.translateY);
	m_pgPropScaleX->SetValue(tv.scaleX);
	m_pgPropScaleY->SetValue(tv.scaleY);
	m_pgPropRotateA->SetValue(tv.rotateA);
	m_pgPropRotateX->SetValue(tv.rotateX);
	m_pgPropRotateY->SetValue(tv.rotateY);
	m_pgPropSkewX->SetValue(tv.skewX);
	m_pgPropSkewY->SetValue(tv.skewY);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateSvgParameterValues(int id) {
///////////////////////////////////////////////////////////////////
	previewWidth	= m_pgPropSvgWidth->GetValue().GetDouble();
	previewHeight	= m_pgPropSvgHeight->GetValue().GetLong();
	
	viewBoxX		= m_pgPropSvgVbX->GetValue().GetLong();
	viewBoxY		= m_pgPropSvgVbY->GetValue().GetLong();
	viewBoxW		= m_pgPropSvgVbW->GetValue().GetLong();
	viewBoxH		= m_pgPropSvgVbH->GetValue().GetLong();
	
	PathGeneratorBase::TransformValues tv;
	tv.autoMode		= m_pgPropTransformMode->GetValue().GetBool();
	tv.translateX 	= m_pgPropTranslateX->GetValue();
	tv.translateY 	= m_pgPropTranslateY->GetValue();
	tv.scaleX		= m_pgPropScaleX->GetValue();
	tv.scaleY		= m_pgPropScaleY->GetValue();
	tv.rotateA		= m_pgPropRotateA->GetValue();
	tv.rotateX		= m_pgPropRotateX->GetValue();
	tv.rotateY		= m_pgPropRotateY->GetValue();
	tv.skewX		= m_pgPropSkewX->GetValue();
	tv.skewY		= m_pgPropSkewY->GetValue();
	
	pathGeneratorStore.setTransformValues(id, tv);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateCncParameterValues(const PathGeneratorBase::CncParameterValues& cpv) {
///////////////////////////////////////////////////////////////////
	m_pgPropCncInclude->SetValue(cpv.include);
	m_pgPropCncZDepthMode->SetValue(cpv.zDepthMode);
	m_pgPropCncZDepth->SetValue(cpv.zDepth);
	m_pgPropCncReversePath->SetValue(cpv.reverse);
	m_pgPropCncToolCorrection->SetValue(cpv.correct);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluateCncParameterValues(int id) {
///////////////////////////////////////////////////////////////////
	PathGeneratorBase::CncParameterValues cpv;
	cpv.include		= m_pgPropCncInclude->GetValue().GetBool();
	cpv.zDepthMode 	= m_pgPropCncZDepthMode->GetValue().GetInteger();
	cpv.zDepth 		= m_pgPropCncZDepth->GetValue().GetDouble();
	cpv.reverse 	= m_pgPropCncReversePath->GetValue().GetBool();
	cpv.correct 	= m_pgPropCncToolCorrection->GetValue().GetInteger();
	
	pathGeneratorStore.setCncParameterValues(id, cpv);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateParameterValues(const PathGeneratorBase::ParameterMap& pMap) {
///////////////////////////////////////////////////////////////////
	//update parameter values
	wxPropertyGrid* grid = m_pgParameterMgr->GetGrid();
	wxPGProperty* pathProp = grid->GetFirstChild(m_pgCatPath);
	if ( pathProp != NULL ) {
		
		// over all path properties
		wxPropertyGridIterator it = grid->GetIterator(wxPG_ITERATE_DEFAULT, pathProp);
		wxPGProperty* prop = NULL;
		while ( (prop = it.GetProperty()) != NULL && grid->GetPropertyCategory(prop) == m_pgCatPath) {
			unsigned int index = prop->GetIndexInParent();
			
			PathGeneratorBase::ParameterMap::const_iterator pmIt = pMap.find(index);
			if ( pmIt != pMap.end() ) {
				PathGeneratorBase::ParameterInfo pi = pmIt->second;

				// specialize and set value
				if ( pi.propertyType == wxPG_VARIANT_TYPE_DOUBLE ) {
					prop->SetValue(wxString::Format("%.3lf", pi.value.GetDouble()));
					
				} else if ( pi.propertyType == wxPG_VARIANT_TYPE_LIST ) {
					prop->SetValueFromString(pi.value);
					
				} else {
					prop->SetValue(pi.value);
				}
			}
			
			it.Next();
		}
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::evaluatePathParameterValues(int id) {
///////////////////////////////////////////////////////////////////
	wxPropertyGrid* grid = m_pgParameterMgr->GetGrid();
	wxPGProperty* pathProp = grid->GetFirstChild(m_pgCatPath);
	if ( pathProp != NULL ) {
		
		// over all path properties
		wxPropertyGridIterator it = grid->GetIterator(wxPG_ITERATE_DEFAULT, pathProp);
		wxPGProperty* prop = NULL;
		int cnt = 0;
		while ( (prop = it.GetProperty()) != NULL && grid->GetPropertyCategory(prop) == m_pgCatPath) {
			pathGeneratorStore.setParameterValue(id, cnt++, prop);
			it.Next();
		}
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupProperty(wxPGProperty* property, bool show) {
///////////////////////////////////////////////////////////////////
	if ( property == NULL )
		return;
		
	property->Hide(!show);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupProperty(wxPGProperty* property, bool show, wxVariant value) {
///////////////////////////////////////////////////////////////////
	if ( property == NULL )
		return;
		
	property->SetValue(value);
	property->Hide(!show);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupCommonValues(const PathGeneratorBase::CommonValues& cv) {
///////////////////////////////////////////////////////////////////
	setupProperty(m_pgPropCorrection, cv.canToolCorrection, cv.canToolCorrection);
	setupProperty(m_pgPropToolDiameter, cv.canToolDiameter, cv.toolDiameter);
	setupProperty(m_pgPropPathColour, cv.canPathColour);
	setupProperty(m_pgPropOutputMode, cv.canPathOutputMode);
	
	if ( cv.canPathColour == true )
		((wxSystemColourProperty*)m_pgPropPathColour)->GetVal().m_colour = cv.pathColour;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::setupCncParameterValues(const PathGeneratorBase::CncParameterValues& cpv) {
///////////////////////////////////////////////////////////////////
	setupProperty(m_pgPropCncToolCorrection, cpv.canCorrect, cpv.correct);
	setupProperty(m_pgPropCncReversePath, cpv.canReverse, cpv.reverse);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::checkAutoGenerate(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_autoGenerate->IsChecked() == true )
		generatePath();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::propertyChanging(wxPropertyGridEvent& event) {
///////////////////////////////////////////////////////////////////
	PathGeneratorBase::ChangeCategory cc = PathGeneratorBase::ChangeCategory::CC_UNKNOWN_CAT;
	
	wxPGProperty* cat = event.GetProperty();
	while ( cat != NULL ) {
		if      ( cat == m_pgCatCommon ) 	{ cc = PathGeneratorBase::ChangeCategory::CC_COMMON_CAT; 	break; }
		else if ( cat == m_pgCatPath )		{ cc = PathGeneratorBase::ChangeCategory::CC_TPL_CAT; 		break; }
		else if ( cat == m_pgCatGrid )		{ cc = PathGeneratorBase::ChangeCategory::CC_GRID_CAT; 		break; }
		else if ( cat == m_pgCatCncBlock )	{ cc = PathGeneratorBase::ChangeCategory::CC_CNC_CAT; 		break; }
		else if ( cat == m_pgCatSvg )		{ cc = PathGeneratorBase::ChangeCategory::CC_SVG_CAT; 		break; }
		
		cat = cat->GetParent();
	}
	
	int id = getPathSelection();
	if ( id >= 0 ) {
		PathGeneratorBase* pGen = pathGeneratorStore.getPathGenerator(id);
		if ( pGen != NULL ) {
			
			evaluateValues();
			
			int indexAtCat = -1;
			if ( cat != NULL )
				indexAtCat = cat->Index(event.GetProperty());
			
			if ( pGen->parameterChanging(cc, indexAtCat, event.GetValue()) == false )
				event.Veto();
		}
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::propertyChanged(wxPropertyGridEvent& event) {
///////////////////////////////////////////////////////////////////
	if ( m_autoGenerate->IsChecked() == false )
		return;

	PathGeneratorBase::ChangeCategory cc = PathGeneratorBase::ChangeCategory::CC_UNKNOWN_CAT;
	
	wxPGProperty* cat = event.GetProperty();
	while ( cat != NULL ) {
		if      ( cat == m_pgCatCommon ) 	{ cc = PathGeneratorBase::ChangeCategory::CC_COMMON_CAT; 	break; }
		else if ( cat == m_pgCatPath )		{ cc = PathGeneratorBase::ChangeCategory::CC_TPL_CAT; 		break; }
		else if ( cat == m_pgCatGrid )		{ cc = PathGeneratorBase::ChangeCategory::CC_GRID_CAT; 		break; }
		else if ( cat == m_pgCatCncBlock )	{ cc = PathGeneratorBase::ChangeCategory::CC_CNC_CAT; 		break; }
		else if ( cat == m_pgCatSvg )		{ cc = PathGeneratorBase::ChangeCategory::CC_SVG_CAT; 		break; }
		
		cat = cat->GetParent();
	}
	
	int id = getPathSelection();
	if ( id >= 0 ) {
		PathGeneratorBase* pGen = pathGeneratorStore.getPathGenerator(id);
		if ( pGen != NULL ) {
			
			evaluateValues();
			
			int indexAtCat = -1;
			if ( cat != NULL )
				indexAtCat = cat->Index(event.GetProperty());
				
			if ( pGen->parameterChanged(cc, indexAtCat, event.GetValue()) == true )
				generatePath();
		}
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::displayGrid(std::fstream& fs){
///////////////////////////////////////////////////////////////////
	if ( m_pgPropGridDisplay->GetValue().GetBool() == false )
		return;

	double h = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(mm) * previewWidth);
	double w = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(mm) * previewHeight);
	double s = SvgUnitCalculator::convertMM2ReferenceUnit(SvgUnitCalculator::getFactorMM2Unit(mm) * m_pgPropGridSize->GetValue().GetDouble());
	
	wxColour colour(0,0,0);
	wxColour c1(0,64,255);
	wxColour c2(255,64,0);
	
	int cnt = 0;
	for (double i=0; i<=h; i+=s, cnt++) {
		cnt%5 == 0 ? colour = c1 : colour = c2;
		fs << wxString::Format("<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"0.3\"/>", 0.0, i, w, i, colour.Red(), colour.Green(), colour.Blue());
	}
	
	cnt = 0;
	for ( double i=0; i<=w; i+=s, cnt++ ) {
		cnt%5 == 0 ? colour = c1 : colour = c2;
		fs << wxString::Format("<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"#%02X%02X%02X\" fill=\"none\" stroke-width=\"0.3\"/>", i, 0.0, i, h, colour.Red(), colour.Green(), colour.Blue());
	}
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updatePreview() {
///////////////////////////////////////////////////////////////////
	wxString pfn(PD_Preview_fileName);
	wxFileName fn(pfn);
	if ( fn.HasVolume() == false ) {
		wxString exPath = wxStandardPaths::Get().GetExecutablePath();
		pfn = wxFileName(exPath).GetPath(wxPATH_GET_SEPARATOR) + pfn;
	} 
	
	std::fstream fs;
	fs.open(pfn.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
	if ( fs.is_open() ) {
		fs << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
		fs << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"" << std::endl;
		fs << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << std::endl;
		fs << "<svg xmlns=\"http://www.w3.org/2000/svg\" "; 
		fs << "width=\"" << previewWidth << "mm\" "; 
		fs << "height=\"" << previewHeight << "mm\" ";
		fs << "viewBox=\"" << viewBoxX << " " <<  viewBoxY << " " << viewBoxW << " " <<  viewBoxH << "\" ";
		fs << "xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
		fs << "<title>SVG Path Generator</title>" << std::endl;
		fs << "<desc>Preview</desc>" << std::endl;
		
		displayGrid(fs);
		
		fs << m_generatedPreview->GetValue() << std::endl;
		
		fs << "</svg>" << std::endl;
		fs.close();
	}

	wxFileName check(pfn);
	if ( !check.Exists() )
		pfn = "about:blank";

	m_pgPreview->LoadURL(pfn);
}
///////////////////////////////////////////////////////////////////
wxTreeItemId PathGeneratorFrame::searchFirstTreeItem(wxTreeItemId parent, const wxString& label, bool fullMatch) {
///////////////////////////////////////////////////////////////////
	wxTreeItemId invalid;
	if ( !parent )
		return invalid;
		
	if ( label.length() < 3 )
		return invalid;
		
	wxTreeItemIdValue cookie;
	wxTreeItemId current = m_templateTree->GetFirstChild(parent, cookie);
	
	while ( current ) {
		if ( m_templateTree->ItemHasChildren(current) == true ) {
			// recusive call
			wxTreeItemId found = searchFirstTreeItem(current, label);
			if ( found )
				return found;
				
		} else {
			if ( fullMatch == true ) {
				if ( m_templateTree->GetItemText(current) == label ) {
					return current;
				}
			} else {
				if ( m_templateTree->GetItemText(current).Find(label) != wxNOT_FOUND ) {
					return current;
				}
			}
		}
		
		current = m_templateTree->GetNextChild(parent, cookie);
	}
	
	return invalid;
}
///////////////////////////////////////////////////////////////////
int PathGeneratorFrame::searchAndSelectTreeItem(const wxString& search, const int startIndex, bool fullMatch, const unsigned int minCharMatchCount) {
///////////////////////////////////////////////////////////////////
	int ret = -1;
	
	if ( search.length() < minCharMatchCount )
		return -1;
	
	// over all template tree items
	int cnt = 0;
	wxString s(search);
	for (TreeIndex::iterator it=treeIndex.begin(); it!=treeIndex.end(); ++it) {
		if ( cnt > startIndex ) {
			TreeItemInfo tii = *it;
			
			if ( tii.itemName.MakeUpper().Find(s.MakeUpper()) != wxNOT_FOUND ) {
				m_templateTree->SelectItem(tii.itemId);
				m_templateTree->EnsureVisible(tii.itemId);
				
				ret = cnt;
				break;
			}
		}
		
		cnt++;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
int PathGeneratorFrame::searchAndSelectFirstTreeItem(const wxString& search) {
///////////////////////////////////////////////////////////////////
	m_templateTree->SelectItem(m_templateTree->GetRootItem().GetID());
	m_templateTree->EnsureVisible(m_templateTree->GetRootItem().GetID());
	
	wxString s;
	search.IsEmpty() ? s.assign(m_treeSearchText->GetValue()) : s.assign(search);

	lastSearchIndex = searchAndSelectTreeItem(s, -1, false, 3);
	
	return lastSearchIndex;
}
///////////////////////////////////////////////////////////////////
int PathGeneratorFrame::searchAndSelectNextTreeItem(const wxString& search) {
///////////////////////////////////////////////////////////////////
	wxString s;
	search.IsEmpty() ? s.assign(m_treeSearchText->GetValue()) : s.assign(search);

	lastSearchIndex = searchAndSelectTreeItem(s, lastSearchIndex, false, 3);
	// to start once more on top
	if ( lastSearchIndex < 0 )
		lastSearchIndex = searchAndSelectTreeItem(s, -1, false, 3);
		
	return lastSearchIndex;
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::keyDownTreeSearch(wxKeyEvent& event) {
///////////////////////////////////////////////////////////////////
	int c = event.GetKeyCode();
	if ( c == WXK_RETURN || c == WXK_NUMPAD_ENTER ) {
		searchAndSelectNextTreeItem();
		
		event.Skip(false);
		return;
	}
	
	event.Skip(true);
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::treeSearch(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	searchAndSelectNextTreeItem();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::updateTreeSearch(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	searchAndSelectFirstTreeItem();
}
///////////////////////////////////////////////////////////////////
void PathGeneratorFrame::pgMainBookChanged(wxNotebookEvent& event) {
///////////////////////////////////////////////////////////////////
	//const unsigned int preview 	= 0;
	const unsigned int info 	= 1;
	
	unsigned int sel = event.GetSelection();
	
	if ( sel == info ) {
		int id = getPathSelection();
		if ( id >= 0 ) {
			PathGeneratorBase* pGen = pathGeneratorStore.getPathGenerator(id);
			if ( pGen != NULL ) {
				pGen->getInternalInformation(m_additionalInfo);
			}
		}
	}
}
