#include <wx/event.h>
#include <wx/utils.h> 
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/tokenzr.h>
#include <wx/textctrl.h>
#include <wx/slider.h>

#include "StdStreamRedirector.h"
#include "GlobalFunctions.h"
#include "CncConfig.h"
#include "CncLoggerProxy.h"
#include "CncFloatingPointValidator.h"
#include "CncSpeedPlayground.h"

#define SLIDER_FACT  10

////////////////////////////////////////////////////////////////////
CncSpeedPlayground::CncSpeedPlayground(wxWindow* parent)
: CncSpeedPlaygroundBase	(parent)
, ArduinoAccelManager		()
, ArduinoPositionRenderer	()
, displayTypeH				(DTH_IMPULSE)
, displayTypeV				(DTV_MM_MIN)
, valF						(1, NULL, wxNUM_VAL_DEFAULT)
, valX						(3, NULL, wxNUM_VAL_DEFAULT)
, valY						(3, NULL, wxNUM_VAL_DEFAULT)
, valZ						(3, NULL, wxNUM_VAL_DEFAULT)
, minDA						(0L)
, maxDA						(0L)
, minFA						(0L)
, maxFA						(0L)
, minDD						(0L)
, maxDD						(0L)
, minFD						(0L)
, maxFD						(0L)
, testData					()
, logger					(new CncTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_loggerPlaceholder->GetWindowStyle()))
, streamRedirector			(NULL)
, accelerationDiagram		(NULL)
, deaccelerationDiagram		(NULL)
, graphA					(NULL)
, graphD					(NULL)
////////////////////////////////////////////////////////////////////
{
	// setup controls
	GblFunc::cloneAttributes(m_loggerPlaceholder, 	logger);
	GblFunc::replaceControl (m_loggerPlaceholder, 	logger);
	
	accelerationDiagram = new CncSpeedAccelerationDiagram(this);
	GblFunc::cloneAttributes(m_accelerationDiagramPlaceholder, 		accelerationDiagram);
	GblFunc::replaceControl (m_accelerationDiagramPlaceholder, 		accelerationDiagram);
	
	deaccelerationDiagram = new CncSpeedAccelerationDiagram(this);
	GblFunc::cloneAttributes(m_deaccelerationDiagramPlaceholder, 	deaccelerationDiagram);
	GblFunc::replaceControl (m_deaccelerationDiagramPlaceholder, 	deaccelerationDiagram);

	
	accelerationDiagram  ->initGraphCount(1);
	deaccelerationDiagram->initGraphCount(1);
	graphA = accelerationDiagram  ->getGraph(0);
	graphD = deaccelerationDiagram->getGraph(0);
	
	determineDisplayType();
	
	const int minF 	=    1;
	const int maxF 	= THE_CONFIG->getMaxSpeedXYZ_MM_MIN();
	const int minX	=    0;
	const int maxX	=  100;
	const int minY	=    0;
	const int maxY	=  100;
	const int minZ	=    0;
	const int maxZ	=   50;

	const wxColour colA(wxColour(255, 128,  64).ChangeLightness(84));
	const wxColour colD(wxColour(128, 128, 255).ChangeLightness(84));
	
	if ( graphA != NULL ) {	graphA->name.assign("Acceleration");	graphA->pen = wxPen(colA, 2, wxSOLID); }
	if ( graphD != NULL ) {	graphD->name.assign("Deacceleration");	graphD->pen = wxPen(colD, 2, wxSOLID); }
	
	// init controls
	double f = 725, x = 6.0, y = 3.0, z = 1.0; 
	
	valF.      SetRange(minF, maxF); m_valF->SetValidator(valF);	m_valF->SetValue(wxString::Format("%4.1lf", f ));
	m_sliderF->SetRange(minF, maxF);								m_sliderF->SetValue(round(f));
	
	valX.      SetRange(minX, maxX); m_valX->SetValidator(valX);	m_valX->SetValue(wxString::Format("%4.3lf", x ));
	m_sliderX->SetRange(minX, maxX * SLIDER_FACT);					m_sliderX->SetValue(round(x * SLIDER_FACT));
	
	valY.      SetRange(minY, maxY); m_valY->SetValidator(valY);	m_valY->SetValue(wxString::Format("%4.3lf", y ));
	m_sliderY->SetRange(minY, maxY * SLIDER_FACT);					m_sliderY->SetValue(round(y * SLIDER_FACT));
	
	valZ.      SetRange(minZ, maxZ); m_valZ->SetValidator(valZ);	m_valZ->SetValue(wxString::Format("%4.3lf", z ));
	m_sliderZ->SetRange(minZ, maxZ * SLIDER_FACT);					m_sliderZ->SetValue(round(z * SLIDER_FACT));
	
	m_staticMinF->SetLabel(wxString::Format("%d", minF));
	m_staticMaxF->SetLabel(wxString::Format("%d", maxF));
	
	m_staticMinX->SetLabel(wxString::Format("%d", minX));
	m_staticMaxX->SetLabel(wxString::Format("%d", maxX));
	
	m_staticMinY->SetLabel(wxString::Format("%d", minY));
	m_staticMaxY->SetLabel(wxString::Format("%d", maxY));
	
	m_staticMinZ->SetLabel(wxString::Format("%d", minZ));
	m_staticMaxZ->SetLabel(wxString::Format("%d", maxZ));
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	
	val.SetPrecision(3); val.SetRange(-5,    +5); m_valAA->SetValidator(val);
	val.SetPrecision(3); val.SetRange(-5,    +5); m_valDA->SetValidator(val);

	val.SetPrecision(3); val.SetRange(-5,    +5); m_valAB->SetValidator(val);
	val.SetPrecision(3); val.SetRange(-5,    +5); m_valDB->SetValidator(val);

	val.SetPrecision(1); val.SetRange( 0, +1000); m_valAC->SetValidator(val);
	val.SetPrecision(1); val.SetRange( 0, +1000); m_valDC->SetValidator(val);
	
	m_valAA->ChangeValue("0.000");
	m_valDA->ChangeValue("0.000");
	m_valAB->ChangeValue("0.05");
	m_valDB->ChangeValue("0.05");
	m_valAC->ChangeValue("333.0");
	m_valDC->ChangeValue("333.0");
	
	val.SetPrecision(0); val.SetRange(1, 10000); 
	valF.SetRange(0.0, 10000.0);
	
	m_cbSpecialAccelDisplayWidth		->SetValue(false);
	m_ebSpecialAccelDisplayWidthValue	->Enable(false);
	m_ebSpecialAccelDisplayWidthValue	->SetValidator(val);
	
	m_cbSpecialSpeedDisplayHeight		->SetValue(false);
	m_ebSpecialSpeedDisplayHeightValue	->Enable(false);
	m_ebSpecialSpeedDisplayHeightValue	->SetValidator(valF);
	
	val.SetPrecision(0); val.SetRange(0, 10000); 
	m_ebSpecialTriggerImpluse1->SetValidator(val);
	m_ebSpecialTriggerImpluse2->SetValidator(val);
	m_ebSpecialTriggerImpluse3->SetValidator(val);
	
	m_ebSpecialTriggerSpeed1->SetValidator(valF);
	m_ebSpecialTriggerSpeed2->SetValidator(valF);
	m_ebSpecialTriggerSpeed3->SetValidator(valF);
}
////////////////////////////////////////////////////////////////////
CncSpeedPlayground::~CncSpeedPlayground() {
////////////////////////////////////////////////////////////////////
	wxDELETE( streamRedirector );
	wxDELETE( accelerationDiagram );
	wxDELETE( deaccelerationDiagram );
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::onActivate(wxActivateEvent& event) {
////////////////////////////////////////////////////////////////////
	if ( event.GetActive() == true ) {
		if ( streamRedirector != NULL )
			wxDELETE( streamRedirector );
		
		streamRedirector = new StdStreamRedirector(logger);
		std::cout << CNC_LOG_FUNCT << ": Enter . . ." << std::endl;
		
		run();
		
	} else {
		std::cout << CNC_LOG_FUNCT << ": Leave . . ." << std::endl;
		wxDELETE( streamRedirector );
	}
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::onValKeyDown(wxKeyEvent& event) {
////////////////////////////////////////////////////////////////////
	wxTextCtrl* c = static_cast<wxTextCtrl*>(event.GetEventObject());
	
	float minVal = 0.0;
	float maxVal = 0.0;
	
	if      ( c == m_valF ) { minVal = valF.getMin(); maxVal = valF.getMax(); } 
	else if ( c == m_valX ) { minVal = valX.getMin(); maxVal = valX.getMax(); } 
	else if ( c == m_valY ) { minVal = valY.getMin(); maxVal = valY.getMax(); } 
	else if ( c == m_valZ ) { minVal = valZ.getMin(); maxVal = valZ.getMax(); } 
	else					{ event.Skip(); return; }
	
	bool inFocus	= false;
	double toAdd 	= 0.1;
	double dVal  	= 0.0;

	const wxString sVal(c->GetValue());
	sVal.ToDouble(&dVal);

	switch ( event.GetKeyCode() ) {
		case WXK_UP:		if ( dVal + toAdd <= maxVal ) 	dVal += toAdd;
							else							dVal  = maxVal;
							inFocus = true;
							break;
							
		case WXK_DOWN:		if ( dVal - toAdd >= minVal )	dVal -= toAdd;
							else							dVal  = minVal;
							inFocus = true;
							break;
	}

	if ( inFocus == true ) {
		const int precision = sVal.AfterLast('.').length();
		wxString format("%"); format.append(wxString::Format(".%dlf", precision));

		c->SetValue(wxString::Format(format, dVal));
		c->Validate();
	}

	event.Skip();
}
////////////////////////////////////////////////////////////////////
bool CncSpeedPlayground::determineDisplayType() {
////////////////////////////////////////////////////////////////////
	wxString v("???"), h("impulses");
	DisplayTypeV prevTypeV = displayTypeV;
	
	char dt = '1';
	if ( m_cbDisplayTypeV->GetStringSelection().length() > 0 )
		dt = m_cbDisplayTypeV->GetStringSelection()[0];
	
	switch ( dt ) {
		case '2':	displayTypeV = DTV_MM_SEC; v.assign("mm/sec"); break;
		case '1':	displayTypeV = DTV_MM_MIN; v.assign("mm/min"); break;
	}
	
	accelerationDiagram  ->initUnit(h, v);
	deaccelerationDiagram->initUnit(h, v);
	
	return ( displayTypeV != prevTypeV );
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::onChangeDisplayType(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	if ( determineDisplayType() )
		run();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::changeSliderF(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////////
	m_valF->ChangeValue(wxString::Format("%4.1lf", (double)m_sliderF->GetValue()));
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::changeSliderX(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////////
	m_valX->ChangeValue(wxString::Format("%4.3lf", (double)m_sliderX->GetValue() / SLIDER_FACT ));
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::changeSliderY(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////////
	m_valY->ChangeValue(wxString::Format("%4.3lf", (double)m_sliderY->GetValue() / SLIDER_FACT ));
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::changeSliderZ(wxScrollEvent& event) {
////////////////////////////////////////////////////////////////////
	m_valZ->ChangeValue(wxString::Format("%4.3lf", (double)m_sliderZ->GetValue() / SLIDER_FACT ));
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateValueF(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long val;
	m_valF->GetValue().ToLong(&val);
	m_sliderF->SetValue(val);
	
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateValueX(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long val;
	m_valX->GetValue().ToLong(&val);
	m_sliderX->SetValue(val * SLIDER_FACT);
	
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateValueY(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long val;
	m_valY->GetValue().ToLong(&val);
	m_sliderY->SetValue(val * SLIDER_FACT);
	
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateValueZ(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long val;
	m_valZ->GetValue().ToLong(&val);
	m_sliderZ->SetValue(val * SLIDER_FACT);
	
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::enableSpecialAccelDisplayWidth(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	m_ebSpecialAccelDisplayWidthValue->Enable(m_cbSpecialAccelDisplayWidth->GetValue());
	
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateSpecialAccelDisplayWidthValue(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::enableSpecialSpeedDisplayHeight(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	m_ebSpecialSpeedDisplayHeightValue->Enable(m_cbSpecialSpeedDisplayHeight->GetValue());
	
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateSpecialSpeedDisplayHeightValue(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateTriggerF1(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long v1; m_ebSpecialTriggerImpluse1->GetValue().ToLong(&v1);
	long v2; m_ebSpecialTriggerSpeed1  ->GetValue().ToLong(&v2);
	
	//if ( v1 != 0 && v2 != 0 )
		runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateTriggerF2(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long v1; m_ebSpecialTriggerImpluse2->GetValue().ToLong(&v1);
	long v2; m_ebSpecialTriggerSpeed2  ->GetValue().ToLong(&v2);
	
	//if ( v1 != 0 && v2 != 0 )
		runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateTriggerF3(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long v1; m_ebSpecialTriggerImpluse3->GetValue().ToLong(&v1);
	long v2; m_ebSpecialTriggerSpeed3  ->GetValue().ToLong(&v2);
	
	//if ( v1 != 0 && v2 != 0 )
		runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateTriggerI1(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long v1; m_ebSpecialTriggerImpluse1->GetValue().ToLong(&v1);
	long v2; m_ebSpecialTriggerSpeed2  ->GetValue().ToLong(&v2);
	
	//if ( v1 != 0 && v2 != 0 )
		runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateTriggerI2(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long v1; m_ebSpecialTriggerImpluse2->GetValue().ToLong(&v1);
	long v2; m_ebSpecialTriggerSpeed2  ->GetValue().ToLong(&v2);
	
	//if ( v1 != 0 && v2 != 0 )
		runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateTriggerI3(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	long v1; m_ebSpecialTriggerImpluse3->GetValue().ToLong(&v1);
	long v2; m_ebSpecialTriggerSpeed3  ->GetValue().ToLong(&v2);
	
	//if ( v1 != 0 && v2 != 0 )
		runFromGui();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::updateListControl(wxListCtrl* ctrl, int columns, const char* values) {
////////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return ;
		
	if ( values == NULL )
		return ;
		
	ctrl->DeleteAllItems();
	
	if ( ctrl->GetColumnCount() != columns ) {
		ctrl->DeleteAllColumns();
		ctrl->AppendColumn("Parameter", wxLIST_FORMAT_LEFT,   200);
		ctrl->AppendColumn("Value", 	wxLIST_FORMAT_RIGHT,  100);
		ctrl->AppendColumn("Unit", 		wxLIST_FORMAT_LEFT,    70);
	}
	
	const wxString v(values);
	
	// token values
	unsigned int index = 0;
	wxString parameter, rest, value, unit, token;
	
	wxStringTokenizer tokenizer(v, "\n");
	while ( tokenizer.HasMoreTokens() ) {
		token.assign(tokenizer.GetNextToken());
		if ( token.IsEmpty() )
			continue;
			
		parameter.assign(token.BeforeFirst('=').Trim(true));
		rest     .assign(token.AfterFirst ('=').Trim(false));
		value    .assign(rest .BeforeFirst('[').Trim(true));
		unit     .assign(rest .AfterFirst ('[').Trim(true).Trim(false).RemoveLast());
		
		index = ctrl->GetItemCount();
		ctrl->InsertItem(index, "",  0);
		ctrl->SetItem(index, 0, parameter);
		ctrl->SetItem(index, 1, value);
		ctrl->SetItem(index, 2, unit);
	}
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::notifyACMStateChange(State s) {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::notifyACMInitMove() {
////////////////////////////////////////////////////////////////////

	//traceACM(std::clog, 0);

}
////////////////////////////////////////////////////////////////////
byte CncSpeedPlayground::performNextStep(AxisId aid) {
////////////////////////////////////////////////////////////////////
	return RET_OK;
}
////////////////////////////////////////////////////////////////////
byte CncSpeedPlayground::performStep(AxisId aid) {
////////////////////////////////////////////////////////////////////
	return RET_OK;
}
////////////////////////////////////////////////////////////////////
byte CncSpeedPlayground::initiateStep(AxisId aid) {
////////////////////////////////////////////////////////////////////
	return RET_OK;
}
////////////////////////////////////////////////////////////////////
byte CncSpeedPlayground::finalizeStep(AxisId aid) {
////////////////////////////////////////////////////////////////////
	return RET_OK;
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::onClearLogger(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	logger->Clear();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::notifyMovePartBefore() {
////////////////////////////////////////////////////////////////////
	#warning using stepSignature
	initNextImpulse(ASGI_MASTER);
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::notifyMovePartAfter() {
////////////////////////////////////////////////////////////////////
	const double F = displayTypeV == DTV_MM_MIN ? getCurrentTargetSpeed1000_MMSec() * 60 / 1000 
												: getCurrentTargetSpeed1000_MMSec()      / 1000;
	
	// acceleration
	if ( testData.totalImpulseCounter >= minDA && testData.totalImpulseCounter <= maxDA ) {
		
		CncSpeedAccelerationDiagram::Impulse impulse;
		impulse.x = RS::dx();
		impulse.y = RS::dy();
		impulse.z = RS::dz();
		
		wxRealPoint p(testData.totalImpulseCounter, F);
		accelerationDiagram->appendPointToGraph(0, p, impulse);
		
		testData.A.impulses++;
		if ( RS::stepSignature & ASG_X ) testData.A.xSteps++;
		if ( RS::stepSignature & ASG_Y ) testData.A.ySteps++;
		if ( RS::stepSignature & ASG_Z ) testData.A.zSteps++;
	}
	
	// deacceleration
	if ( testData.totalImpulseCounter >= minDD && testData.totalImpulseCounter <= maxDD ) {
		
		CncSpeedAccelerationDiagram::Impulse impulse;
		impulse.x = RS::dx();
		impulse.y = RS::dy();
		impulse.z = RS::dz();
		
		wxRealPoint p(testData.totalImpulseCounter, F);
		deaccelerationDiagram->appendPointToGraph(0, p, impulse);
		
		testData.D.impulses++;
		if ( RS::stepSignature & ASG_X ) testData.D.xSteps++;
		if ( RS::stepSignature & ASG_Y ) testData.D.ySteps++;
		if ( RS::stepSignature & ASG_Z ) testData.D.zSteps++;
	}
	
	testData.totalImpulseCounter++;
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::onRun(wxCommandEvent& event) {
////////////////////////////////////////////////////////////////////
	run();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::runFromGui() {
////////////////////////////////////////////////////////////////////
	run();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::onTimer(wxTimerEvent& event) {
////////////////////////////////////////////////////////////////////
	//run();
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::appendArdoMessage(char type, const wxString& msg, const wxString& context) {
////////////////////////////////////////////////////////////////////
	if ( m_cbEnhancedTracing->GetValue() == false )
		return;
		
	//-------------------------------------------------------------
	auto log2Std = [&](std::ostream & o) {
		o << "ArdoMsg: " << type 
						 << " " 
						 << msg 
						 /*
						 << std::setw(80) 
						 << " [[ " 
						 << context 
						 << " ]]"
						 */ 
						 << std::endl;
	};
	
	switch ( type ) {
		case 'E':	log2Std(std::cerr); break;
		case 'D':
		case 'W':	log2Std(std::clog); break;
		case 'S':	log2Std(cnc::cex1); break;
		default:	log2Std(std::cout);
	}
}
////////////////////////////////////////////////////////////////////
void CncSpeedPlayground::run() {
////////////////////////////////////////////////////////////////////
	if ( IsShown() == false )
		return;

	if ( logger->IsFrozen() == false )
		logger->Freeze();

	typedef ArduinoAccelManager::Function Function;

	double 	F; m_valF->GetValue().ToDouble(&F);
	// metric
	double 	X; m_valX->GetValue().ToDouble(&X);
	double 	Y; m_valY->GetValue().ToDouble(&Y);
	double 	Z; m_valZ->GetValue().ToDouble(&Z);
	
	// steps ...
	const double x = X / THE_CONFIG->getDisplayFactX();
	const double y = Y / THE_CONFIG->getDisplayFactY();
	const double z = Z / THE_CONFIG->getDisplayFactZ();
	
	// -------------------------------------------------------------
	auto setupACM = [&](float AA, float AB, float AC, float DA, float DB, float DC) {
		Function fA(AA, AB, AC), fD(DA, DB, DC);
		ArduinoAccelManager::initialize(fA, fD);
	};
	
	testData.reset();
	
	// -------------------------------------------------------------
	double dValAA, dValAB, dValAC, dValDA, dValDB, dValDC;
	m_valAA->GetValue().ToDouble(&dValAA);	m_valDA->GetValue().ToDouble(&dValDA);
	m_valAB->GetValue().ToDouble(&dValAB);	m_valDB->GetValue().ToDouble(&dValDB);
	m_valAC->GetValue().ToDouble(&dValAC);	m_valDC->GetValue().ToDouble(&dValDC);

	setupACM(dValAA, dValAB, dValAC / 60.0, dValDA, dValDB, dValDC / 60.0);
	
	determineDisplayType();
	ArduinoAccelManager::activate(m_cbWithAcceleration->GetValue());
	
	const int32_t  maxD 	= ArduinoImpulseCalculator().calculate(x, y, z);
	const uint32_t speed 	= 1000 * F / 60 ;
	if ( initMove(maxD, speed) == false ) {
		std::cerr << wxString::Format("initMove(%u, %.1lf) failed!", maxD, speed) << std::endl;
		
		if ( logger->IsFrozen() == true )
			logger->Thaw();
		return;
	}
	
	// -------------------------------------------------------------
	// setup diagramms
	const int32_t aRampWidth = round(getARampWidth());
	const int32_t dRampWidth = round(getDRampWidth());
	const float extendFactH  = aRampWidth + aRampWidth * 0.15 + dRampWidth + dRampWidth * 0.15 >= maxD ? 0 : 0.15;
	const float extendFactV  = 0.15;
	
	long aWidth = aRampWidth;
	if ( m_cbSpecialAccelDisplayWidth->GetValue() == false ) {
		m_ebSpecialAccelDisplayWidthValue->ChangeValue(wxString::Format("%ld", maxD));
	}
	else {
		m_ebSpecialAccelDisplayWidthValue->GetValue().ToLong(&aWidth);
	}
	
	maxDA  = aWidth;
	maxDA += aWidth * extendFactH;
	minDA  = 0;
	
	maxDD  = maxD - 1;
	minDD  = maxD - dRampWidth;
	minDD -= dRampWidth * extendFactH;
	minDD  = minDD < 0 ? 0 : minDD;
	
	maxDD  = minDD < maxDA ? maxDA + aWidth : maxDD;
	minDD  = minDD < maxDA ? maxDA  : maxDA;
	
	maxFA  = m_cbSpeedRelative->GetValue() == true ? F : THE_CONFIG->getMaxSpeedXYZ_MM_MIN(); 
	maxFA  = displayTypeV == DTV_MM_SEC ? maxFA / 60 : maxFA ; 
	maxFA  = maxFA < dValAC ? dValAC : maxFA;
	maxFA += maxFA * extendFactV; 
	minFA  = 0;
	
	if ( m_ebSpecialSpeedDisplayHeightValue->IsEnabled() == false ) {
		m_ebSpecialSpeedDisplayHeightValue->ChangeValue(wxString::Format("%ld", maxFA));
	}
	else {
		long val;
		m_ebSpecialSpeedDisplayHeightValue->GetValue().ToLong(&val);
		maxFA = val > 0 ? (uint32_t)val : maxFA;
	}
	
	maxFD  = maxFA;
	minFD  = minFA;
	
	accelerationDiagram  ->initRange(minFA, maxFA, minDA, maxDA); 
	deaccelerationDiagram->initRange(minFD, maxFD, minDD, maxDD);

	accelerationDiagram->  setShowImpulseState(m_btExpandImpulse->GetValue());
	deaccelerationDiagram->setShowImpulseState(m_btExpandImpulse->GetValue());

	// -------------------------------------------------------------
	// simulate 
	if ( graphA != NULL ) graphA->points.clear();
	if ( graphD != NULL ) graphD->points.clear();
	
	// update test data control
	std::stringstream ss;
	
	const float avgMinPulseWidth = ( THE_CONFIG->getHighPulsWidthX() 
								 +   THE_CONFIG->getHighPulsWidthY() 
								 +   THE_CONFIG->getHighPulsWidthZ()
								 ) / 3;
	const float implPeriod = avgMinPulseWidth + avgMinPulseWidth * 0.05;
	
	ss << "Steps X Axis                    =" << round(x) 			<< "[steps]"	<< std::endl;
	ss << "Steps Y Axis                    =" << round(y) 			<< "[steps]"	<< std::endl;
	ss << "Steps Z Axis                    =" << round(z) 			<< "[steps]"	<< std::endl;
	ss << "Total Impulse Count             =" << maxD 				<< "[#]"		<< std::endl;
	ss << "Impulse Period                  =" << implPeriod			<< "[us]"		<< std::endl;
	
	long stI1, stI2, stI3;
	m_ebSpecialTriggerImpluse1->GetValue().ToLong(&stI1);
	m_ebSpecialTriggerImpluse2->GetValue().ToLong(&stI2);
	m_ebSpecialTriggerImpluse3->GetValue().ToLong(&stI3);
	
	double stF1, stF2, stF3;
	m_ebSpecialTriggerSpeed1->GetValue().ToDouble(&stF1);
	m_ebSpecialTriggerSpeed2->GetValue().ToDouble(&stF2);
	m_ebSpecialTriggerSpeed3->GetValue().ToDouble(&stF3);
	
	/*
	if ( stI1 > 0 && stF1 > 0.0 ) 	setSpecialSpeedTrigger1(stI1, (float)stF1 / 60);
	else							setSpecialSpeedTrigger1(   0, (float)0.0);
	
	if ( stI2 > 0 && stF2 > 0.0 )	setSpecialSpeedTrigger2(stI2, (float)stF2 / 60);
	else							setSpecialSpeedTrigger2(   0, (float)0.0);
	
	if ( stI3 > 0 && stF3 > 0.0 )	setSpecialSpeedTrigger3(stI3, (float)stF3 / 60);
	else							setSpecialSpeedTrigger3(   0, (float)0.0);
	*/
	// simulated the stepping
	if ( renderMove(x, y, z) == RET_OK ) {
		// the graphs are refilled by virtual void notifyMovePart()
		m_lastTimestamp->ChangeValue(wxDateTime().UNow().FormatISOTime());
		
		const float xRampADist = (float)testData.A.xSteps * THE_CONFIG->getDisplayFactX();
		const float yRampADist = (float)testData.A.ySteps * THE_CONFIG->getDisplayFactY();
		const float zRampADist = (float)testData.A.zSteps * THE_CONFIG->getDisplayFactZ();
		const float xRampDDist = (float)testData.D.xSteps * THE_CONFIG->getDisplayFactX();
		const float yRampDDist = (float)testData.D.ySteps * THE_CONFIG->getDisplayFactY();
		const float zRampDDist = (float)testData.D.zSteps * THE_CONFIG->getDisplayFactZ();
		
		const float vRampADist = sqrt( pow(xRampADist, 2) + pow(yRampADist, 2) + pow(zRampADist, 2) );
		const float vRampDDist = sqrt( pow(xRampDDist, 2) + pow(yRampDDist, 2) + pow(zRampDDist, 2) );
		
		const uint32_t rampADuration = testData.A.impulses * implPeriod;
		const uint32_t rampDDuration = testData.D.impulses * implPeriod;

		ss << "Accleration Ramp distance     x =" << testData.A.xSteps						<< "[steps]"	<< std::endl;
		ss << "Accleration Ramp distance     y =" << testData.A.ySteps						<< "[steps]"	<< std::endl;
		ss << "Accleration Ramp distance     z =" << testData.A.zSteps						<< "[steps]"	<< std::endl;
		ss << "Accleration Ramp distance     x =" << wxString::Format("%.3lf", xRampADist)	<< "[mm]"		<< std::endl;
		ss << "Accleration Ramp distance     y =" << wxString::Format("%.3lf", yRampADist)	<< "[mm]"		<< std::endl;
		ss << "Accleration Ramp distance     z =" << wxString::Format("%.3lf", zRampADist)	<< "[mm]"		<< std::endl;
		ss << "Accleration Ramp distance     v =" << wxString::Format("%.3lf", vRampADist)	<< "[mm]"		<< std::endl;
		ss << "Accleration Ramp impulses     v =" << testData.A.impulses					<< "[#]"		<< std::endl;
		ss << "Accleration Ramp duration     v =" << rampADuration							<< "[us]"		<< std::endl;
		
		ss << "Deaccleration Ramp distance   x =" << testData.D.xSteps						<< "[steps]"	<< std::endl;
		ss << "Deaccleration Ramp distance   y =" << testData.D.ySteps						<< "[steps]"	<< std::endl;
		ss << "Deaccleration Ramp distance   z =" << testData.D.zSteps						<< "[steps]"	<< std::endl;
		ss << "Deaccleration Ramp distance   x =" << wxString::Format("%.3lf", xRampDDist)	<< "[mm]"		<< std::endl;
		ss << "Deaccleration Ramp distance   y =" << wxString::Format("%.3lf", yRampDDist)	<< "[mm]"		<< std::endl;
		ss << "Deaccleration Ramp distance   z =" << wxString::Format("%.3lf", zRampDDist)	<< "[mm]"		<< std::endl;
		ss << "Deaccleration Ramp distance   v =" << wxString::Format("%.3lf", vRampDDist)	<< "[mm]"		<< std::endl;
		ss << "Deaccleration Ramp impulses   v =" << testData.D.impulses					<< "[#]"		<< std::endl;
		ss << "Deaxcleration Ramp duration   v =" << rampDDuration							<< "[us]"		<< std::endl;
		
	} else {
		
		std::cerr << "Evaluation: The call of renderMove() is failed!" << std::endl;
	}
	
	#
	//traceACM(ss, 2);
	updateTestListControl(m_listCtrlTestData, wxString(ss.str()));
	
	accelerationDiagram  ->display();
	deaccelerationDiagram->display();
	
	if ( logger->IsFrozen() == true )
		logger->Thaw();

}
