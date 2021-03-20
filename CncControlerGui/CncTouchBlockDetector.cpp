#include "CncConfig.h"
#include "CncControl.h"
#include "CncTouchBlockDetector.h"

#define ADD_ERROR_CODE(...) \
		errorCodes.assign(wxString::Format("%s: %s\n", CNC_LOG_FUNCT, wxString::Format(__VA_ARGS__))) \
		;
		
//////////////////////////////////////////////////
CncTouchBlockDetector::CncTouchBlockDetector(CncControl * cc)
: cnc							(cc)
, parameters					()
, startPos						()
, touchPos						()
, newCurrentPos					()
, errorCodes					("")
, prevSpeedMode					(CncSpeedRapid)
, prevSpeedValue				(0.0) 
, contactState					(CS_UNKNOWN)
//////////////////////////////////////////////////
{
	if ( cnc != NULL ) {
		startPos     .set(requestControllerPos());
		touchPos     .set(requestControllerPos());
		newCurrentPos.set(startPos);
	} 
	else {
		ADD_ERROR_CODE("Invalid cnc instance!")
	}
}
//////////////////////////////////////////////////
CncTouchBlockDetector::~CncTouchBlockDetector() {
//////////////////////////////////////////////////
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::moveXFree(const wxString& errMsg) {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;

	// determine x distance and direction
	double xDist = abs(moveAxisFreeDist_MM);
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:
		case Corner::TC_LEFT_BOTTOM:	xDist *= (-1.0);
										break;
									
		case Corner::TC_RIGHT_TOP:
		case Corner::TC_RIGHT_BOTTOM:	xDist *= (+1.0);
										break;
	}
	
	// moving X free (a small piece left/right)
	const bool ret = cnc->moveRelLinearMetricXY(xDist, 0.0, false);
	if ( ret == false )
		ADD_ERROR_CODE(errMsg)
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::moveYFree(const wxString& errMsg) {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	// determine y distance and direction
	double yDist = abs(moveAxisFreeDist_MM);
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:
		case Corner::TC_RIGHT_TOP:		yDist *= (+1.0);
										break;
										
		case Corner::TC_LEFT_BOTTOM:	
		case Corner::TC_RIGHT_BOTTOM:	yDist *= (-1.0);
										break;
	}
	
	// moving Y free (a small piece top/bottom)
	const bool ret = cnc->moveRelLinearMetricXY(0.0, yDist, false);
	if ( ret == false )
		ADD_ERROR_CODE(errMsg)
		
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::moveZFree(const wxString& errMsg) {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
	
	// moving Z free (a small piece up)
	const bool ret = cnc->moveRelMetricZ(moveAxisFreeDist_MM);
	if ( ret == false )
		ADD_ERROR_CODE(errMsg)

	// determine y distance and direction
		
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::moveUntilContact(const CncAxis axis, const double dist, const wxString& errMsg) {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	int			signValueX =  0;
	int			signValueY =  0;
	const int	signValueZ = -1;
	
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:		signValueX = +1; 
										signValueY = -1; 
										break;
									
		case Corner::TC_LEFT_BOTTOM:	signValueX = +1; 
										signValueY = +1; 
										break;
									
		case Corner::TC_RIGHT_TOP:		signValueX = -1; 
										signValueY = -1; 
										break;
		
		case Corner::TC_RIGHT_BOTTOM:	signValueX = -1; 
										signValueY = +1; 
										break;
	}
	
	const bool 		useDefault	= cnc::dblCmp::nu(dist) || dist < 0.0;
	const double	distToMove	= useDefault ? abs(moveUntilContactWidth_MM) : abs(dist);
	
	const int32_t	valueX	= THE_CONFIG->convertMetricToStepsX(distToMove) * signValueX;
	const int32_t	valueY	= THE_CONFIG->convertMetricToStepsY(distToMove) * signValueY;
	const int32_t	valueZ	= THE_CONFIG->convertMetricToStepsZ(distToMove) * signValueZ;
	
	const unsigned int	size = 3;
	int32_t	values[size];
	values[0] = axis == Axis_X ? valueX : 0;
	values[1] = axis == Axis_Y ? valueY : 0;
	values[2] = axis == Axis_Z ? valueZ : 0;
	
	CncLongPosition prevPos(requestControllerPos());
	const bool ret = cnc->getSerial()->processMoveUntilContact(size, values);
	
	if ( ret == true ) {
		CncLongPosition currPos(requestControllerPos());
		
		// check moved distance vs. given max value
		switch (axis) {
			case Axis_X:	if ( useDefault == true ) {
								if ( abs( prevPos.getX() - currPos.getX() ) >= abs(valueX) ) {
									ADD_ERROR_CODE(errMsg)
									ADD_ERROR_CODE("Can't find touch contact for X axis!")
								}
							}
							
							break;
							
			case Axis_Y:	if ( useDefault == true ) {
								if ( abs( prevPos.getY() - currPos.getY() ) >= abs(valueY) ) {
									ADD_ERROR_CODE(errMsg)
									ADD_ERROR_CODE("Can't find touch contact for Y axis!")
								}
							}
							
							break;
			
			case Axis_Z:	if ( useDefault == true ) {
								if ( abs( prevPos.getZ() - currPos.getZ() ) >= abs(valueZ) ) {
									ADD_ERROR_CODE(errMsg)
									ADD_ERROR_CODE("Can't find touch contact for Z axis!")
								}
							}
							
							break;
							
			default:		ADD_ERROR_CODE(errMsg)
							ADD_ERROR_CODE("Axis not supported")
		}
	}
	else {
		ADD_ERROR_CODE(errMsg);
	}
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::moveRel(int32_t dx, int32_t dy, int32_t dz, const wxString& errMsg) {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	const bool ret = cnc->moveRelLinearStepsXYZ(dx, dy, dz, false);
	if ( ret == false )
		ADD_ERROR_CODE(errMsg);
		
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::moveAbs(int32_t px, int32_t py, int32_t pz, const wxString& errMsg) {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
	
	const bool ret = cnc->moveAbsLinearStepsXYZ(px, py, pz, false);
	if ( ret == false )
		ADD_ERROR_CODE(errMsg);
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::findStartPosX() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	// log position
	CncLongPosition	prevPos(requestControllerPos());
	
	if ( hasError() )
		return false;
	
	const double  zDistMaxVal	= moveAxisFreeDist_MM + parameters.touchBlockZThickness / 2.0;
	const int32_t zDistMin		= abs(THE_CONFIG->convertMetricToStepsZ(moveAxisFreeDist_MM));
	
	// ............................................................
	auto zDepthOk = [&]() {
		CncLongPosition	currPos(requestControllerPos());
		
		if ( hasError() )
			return false;
		
		return abs( currPos.getZ() - prevPos.getZ() ) > zDistMin;
	};
	
	// determine x distance and direction
	uint32_t xDist = abs(THE_CONFIG->convertMetricToStepsX(moveUntilContactWidth_MM));
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:
		case Corner::TC_LEFT_BOTTOM:	xDist *= (-1);
										break;
									
		case Corner::TC_RIGHT_TOP:
		case Corner::TC_RIGHT_BOTTOM:	xDist *= (+1);
										break;
	}
	
	// try x positions - stepwise
	for ( int i = 0; i < 3; i++ ) {
		moveZFree(								"Cant move Z free!");
		moveRel(xDist, 0, 0,					"Error while moving X a small piece to left!");
		moveUntilContact(Axis_Z, zDistMaxVal,	"Error while moving Z until contact!");
		
		if ( hasError() )
			return false;
			
		if ( zDepthOk() == true )
			break;
	}
	
	if ( zDepthOk() == false )
		ADD_ERROR_CODE("Error while moving Z down!")
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::findStartPosY() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	// log position
	CncLongPosition	prevPos(requestControllerPos());
	
	if ( hasError() )
		return false;
	
	const double  xDistMaxVal	= moveAxisFreeDist_MM + parameters.touchBlockXCheekThickness * 2;
	const int32_t xDistMin		= abs(THE_CONFIG->convertMetricToStepsZ(moveAxisFreeDist_MM));

	// ............................................................
	auto xDistOk = [&]() {
		CncLongPosition	currPos(requestControllerPos());
		
		if ( hasError() )
			return false;
		
		return abs( currPos.getX() - prevPos.getX() ) > xDistMin;
	};
	
	// determine y distance and direction
	uint32_t yDist = abs(THE_CONFIG->convertMetricToStepsY(moveUntilContactWidth_MM));
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:
		case Corner::TC_RIGHT_TOP:		yDist *= (+1);
										break;
										
		case Corner::TC_LEFT_BOTTOM:	
		case Corner::TC_RIGHT_BOTTOM:	yDist *= (-1);
										break;
	}
	
	const wxString dirText(yDist < 0.0 ? "bottom" : "top");
	
	// try y positions - stepwise
	for ( int i = 0; i < 3; i++ ) {
		moveXFree(								"Cant move X free!");
		moveRel(0, yDist, 0,					"Error while moving Y a small piece to " + dirText + "!");
		moveUntilContact(Axis_X, xDistMaxVal,	"Error while moving X until contact!");
		
		if ( hasError() )
			return false;
			
		if ( xDistOk() == true )
			break;
	}
	
	if ( xDistOk() == false )
		ADD_ERROR_CODE(wxString::Format("Error while moving X %s", dirText ))
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::checkEnvrionment() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	if ( cnc->isConnected() == false ) {
		ADD_ERROR_CODE("Not connected!");
		return false;
	}
	
	if ( cnc->isInterrupted() == true ) {
		ADD_ERROR_CODE("Interrupted!");
		return false;
	}
	
	if ( cnc->getSerial() == NULL ) {
		ADD_ERROR_CODE("Invalid serial instance!");
		return false;
	}
		
	if ( cnc->getSerial()->isCommandActive() == true ) {
		ADD_ERROR_CODE("Other command already active!");
		return false;
	}
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::checkParameters() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_TEST )
		return hasError() == false;
		
	if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_Z ) {
		if ( cnc::dblCmp::nu(parameters.touchBlockZThickness) || parameters.touchBlockZThickness < 0.0 )
			ADD_ERROR_CODE("Invalid touch block z thickness: ", parameters.touchBlockZThickness);
			
		// ...
	}
	
	if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_XYZ ) {
		if ( cnc::dblCmp::nu(parameters.touchBlockXCheekThickness) || parameters.touchBlockXCheekThickness < 0.0 )
			ADD_ERROR_CODE("Invalid touch block x cheek thickness: ", parameters.touchBlockXCheekThickness);
		
		if ( cnc::dblCmp::nu(parameters.touchBlockYCheekThickness) || parameters.touchBlockYCheekThickness < 0.0 )
			ADD_ERROR_CODE("Invalid touch block y cheek thickness: ", parameters.touchBlockYCheekThickness);
		// ...
	}
	
	if ( cnc::dblCmp::nu(parameters.touchDiameter) || parameters.touchDiameter < 0.0 )
		ADD_ERROR_CODE("Invalid touch diameter: ", parameters.touchDiameter);
		
	// ...
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::evaluateCurrentValues() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	prevSpeedMode	= cnc->getConfiguredSpeedMode();
	prevSpeedValue	= cnc->getConfiguredFeedSpeed_MM_MIN();

	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::reduceSpeed() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
	
	const bool ret = cnc->changeCurrentFeedSpeedXYZ_MM_MIN(600.0, CncSpeedRapid);
	if ( ret == false )
		ADD_ERROR_CODE("Error while reduce feed speed!");
		
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::finalize() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	// moving Z to start position
	CncLongPosition prevPos(requestControllerPos());
	moveAbs(prevPos.getX(), prevPos.getY(), startPos.getZ(), "Error while moving Z to start position!");
		
	// moving XY to start position
	moveAbs(startPos.getX(), startPos.getY(), startPos.getZ(), "Error while moving XY to start position!");
	
	// synchronize app position
	cnc->curAppPos = requestControllerPos();
	
	checkResult();
	
	if ( hasError() )
		return false;
	
	// reset feed speed to previous values
	const bool ret = cnc->changeCurrentFeedSpeedXYZ_MM_MIN(prevSpeedValue, prevSpeedMode);
	if ( ret == false )
		ADD_ERROR_CODE("Error while reset feed speed!");
	
	return hasError() == false;
}
///////////////////////////////////////////////////////////////////
const CncLongPosition CncTouchBlockDetector::requestControllerPos() {
///////////////////////////////////////////////////////////////////
	CncLongPosition controllerPos;
	
	if ( hasError() )
		return controllerPos;
	
	GetterValues list;
	cnc->getSerial()->processGetter(PID_XYZ_POS, list);
		
	if ( list.size() != 3 ){
		ADD_ERROR_CODE("Unable to evaluate controllers position");
	} 
	else {
		controllerPos.setX(list.at(0));
		controllerPos.setY(list.at(1));
		controllerPos.setZ(list.at(2));
	}
	
	return controllerPos;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::determineNewCurrentPosX() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
	
	// Calculate and set new zero pos X considering all offsets.
	// But this is only valid if the tool moves back to the start position 
	// at the end.

	// Determine the distance by which the move have to be corrected
	double xCorr = abs(parameters.touchDiameter) / 2.0
				 + abs(parameters.touchBlockXCheekThickness);
				 
	
	// Determine the corrected move distance
	const int32_t xDist1 = abs(startPos.getX() - touchPos.getX());
	const int32_t xDist2 = xDist1 - THE_CONFIG->convertMetricToStepsX(xCorr);
	
	// Determine the new Y coordinate of the new current position
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:
		case Corner::TC_LEFT_BOTTOM:	newCurrentPos.setX(+xDist2);
										break;
									
		case Corner::TC_RIGHT_TOP:
		case Corner::TC_RIGHT_BOTTOM:	newCurrentPos.setX(-xDist2);
										break;
	}
	
	if ( false ) {
		std::cout << "xCorr     : " << wxString::Format("%8.3lf mm --> % 4ld steps\n", xCorr, THE_CONFIG->convertMetricToStepsY(xCorr) );
		std::cout << "xDist1    : " << wxString::Format("%8.3lf mm --> % 4ld steps\n", THE_CONFIG->convertStepsToMetricY(xDist1), xDist1 );
		std::cout << "xDist2    : " << wxString::Format("%8.3lf mm --> % 4ld steps\n", THE_CONFIG->convertStepsToMetricY(xDist2), xDist2 );
	}
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::determineNewCurrentPosY() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
	
	// Calculate and set new zero pos Y considering all offsets.
	// But this is only valid if the tool moves back to the start position 
	// at the end.

	// Determine the distance by which the move have to be corrected
	double yCorr = abs(parameters.touchDiameter) / 2.0
				 + abs(parameters.touchBlockYCheekThickness);
				 
	// Determine the corrected move distance
	const int32_t yDist1 = abs(startPos.getY() - touchPos.getY());
	const int32_t yDist2 = yDist1 - THE_CONFIG->convertMetricToStepsY(yCorr);
	
	// Determine the new Y coordinate of the new current position
	switch ( parameters.touchCorner ) {
		case Corner::TC_LEFT_TOP:
		case Corner::TC_RIGHT_TOP:		newCurrentPos.setY(-yDist2);
										break;
										
		case Corner::TC_LEFT_BOTTOM:	
		case Corner::TC_RIGHT_BOTTOM:	newCurrentPos.setY(+yDist2);
										break;
	}
	
	if ( false ) {
		std::cout << "yCorr     : " << wxString::Format("%8.3lf mm --> % 4ld steps\n", yCorr, THE_CONFIG->convertMetricToStepsY(yCorr) );
		std::cout << "yDist1    : " << wxString::Format("%8.3lf mm --> % 4ld steps\n", THE_CONFIG->convertStepsToMetricY(yDist1), yDist1 );
		std::cout << "yDist2    : " << wxString::Format("%8.3lf mm --> % 4ld steps\n", THE_CONFIG->convertStepsToMetricY(yDist2), yDist2 );
	}
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::determineNewCurrentPosZ() {
//////////////////////////////////////////////////
	if ( hasError() )
		return false;
		
	// Calculate and set new zero pos Z considering all offsets.
	// But this is only valid if the tool moves back to the start position 
	// at the end.
	
	int32_t zDist = abs(startPos.getZ() - touchPos.getZ()) 
				  + abs(THE_CONFIG->convertMetricToStepsZ(parameters.touchBlockZThickness));
	
	newCurrentPos.setZ(+zDist);
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::evaluateContact() {
//////////////////////////////////////////////////
	contactState = CS_UNKNOWN;
	
	if ( hasError() )
		return false;
		
	GetterValues list;
	
	bool ret = cnc->processGetter(PID_TOUCH_CONTACT_STATE, list);
	if ( ret == false )
		ADD_ERROR_CODE("Error while fetching touch contact!")
		
	if ( hasError() )
		return false;
	
	ret = list.size() > 0;
	if ( ret == false )
		ADD_ERROR_CODE("Error while decoding touch contact!")
	
	if ( hasError() )
		return false;
	
	contactState = (list.at(0) != 0 ) ? CS_CLOSED : CS_OPENED;
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::checkResult() {
//////////////////////////////////////////////////
	/*
	CncDoublePosition p;
	std::cout << "startPos      : " << cnc::longFormat(startPos)			<< " --> " << cnc::dblFormat(THE_CONFIG->convertStepsToMetric(p, startPos))			<< std::endl;
	std::cout << "touchPos      : " << cnc::longFormat(touchPos)			<< " --> " << cnc::dblFormat(THE_CONFIG->convertStepsToMetric(p, touchPos))			<< std::endl;
	std::cout << "newCurrentPos : " << cnc::longFormat(newCurrentPos)		<< " --> " << cnc::dblFormat(THE_CONFIG->convertStepsToMetric(p, newCurrentPos))	<< std::endl;
	*/

	#warning - Cant the check below always correct ? may be it is not necessary - check this again
	return true;
	
	if ( hasError() )
		return false;
		
	auto check = [&](int32_t lowValue, int32_t highValue, const char* errMsg) {
		
		const bool condition = lowValue <= highValue;
		if ( condition == false ) {
			wxString m(errMsg);
			m.append(": ");
			m.append(wxString::Format("low greater than high: low value: %ld, high value: %ld", lowValue, highValue));
			ADD_ERROR_CODE(m)
		}
	};
	
	if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_TEST ) {
		return hasError() == false;
	}
	else {
		check(touchPos.getZ(),	startPos.getZ(),		wxString::Format("TP: Invalid Z distance"));
		check(touchPos.getZ(),	newCurrentPos.getZ(),	wxString::Format("CP: Invalid Y distance"));
		
		if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_XYZ ) {
			
			switch ( parameters.touchCorner ) {
				
				case Corner::TC_LEFT_BOTTOM:	check(startPos.getX(),		touchPos.getX(),		wxString::Format("TP: Invalid X distance"));
												check(touchPos.getY(),		startPos.getY(),		wxString::Format("TP: Invalid Y distance"));
												check(touchPos.getX(),		newCurrentPos.getX(),	wxString::Format("CP: Invalid X distance"));
												check(touchPos.getY(),		newCurrentPos.getY(),	wxString::Format("CP: Invalid Y distance"));
												break;
												
				case Corner::TC_RIGHT_BOTTOM:	check(startPos.getX(),		touchPos.getX(),		wxString::Format("TP: Invalid X distance"));
												check(touchPos.getY(),		startPos.getY(),		wxString::Format("TP: Invalid Y distance"));
												check(newCurrentPos.getX(),	touchPos.getX(),		wxString::Format("CP: Invalid X distance"));
												check(touchPos.getY(),		newCurrentPos.getY(),	wxString::Format("CP: Invalid Y distance"));
												break;
												
				case Corner::TC_RIGHT_TOP:		check(startPos.getX(),		touchPos.getX(),		wxString::Format("TP: Invalid X distance"));
												check(startPos.getY(),		touchPos.getY(),		wxString::Format("TP: Invalid Y distance"));
												check(newCurrentPos.getX(),	touchPos.getX(),		wxString::Format("CP: Invalid X distance"));
												check(newCurrentPos.getY(),	touchPos.getY(),		wxString::Format("CP: Invalid Y distance"));
												break;
												
				case Corner::TC_LEFT_TOP:		check(touchPos.getX(),		startPos.getX(),		wxString::Format("TP: Invalid X distance"));
												check(startPos.getY(),		touchPos.getY(),		wxString::Format("TP: Invalid Y distance"));
												check(touchPos.getX(),		newCurrentPos.getX(),	wxString::Format("CP: Invalid X distance"));
												check(newCurrentPos.getY(),	touchPos.getY(),		wxString::Format("CP: Invalid Y distance"));
												break;
			}
		}
	}
	
	return hasError() == false;
}
//////////////////////////////////////////////////
bool CncTouchBlockDetector::touch(const Parameters& para) {
//////////////////////////////////////////////////
	contactState = CS_UNKNOWN;
	
	auto notifyProgess = [&](const char* msg) {
		if ( para.caller != NULL )
			para.caller->notifyProgess(msg ? msg : "???");
	};
	
	if ( hasError() )
		return false;
	
	// ............................................................
	// setup
	parameters = para;
	
	checkParameters();
	checkEnvrionment();
	evaluateCurrentValues();
	reduceSpeed();
	
	// ************************************************************
	// touch TEST
	// ************************************************************
	if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_TEST ) {
		evaluateContact();
		finalize();
		return hasError() == false;
	}

	// ************************************************************
	// touch Z
	// ************************************************************
	{
		if ( hasError() )
			return false;
		
		notifyProgess("Touch Z started");
		
		if ( moveUntilContact(Axis_Z, DefaultDist, "Error while moving Z until contact!") ) {

			CncLongPosition currPos(requestControllerPos());
			touchPos.setZ(currPos.getZ());

			determineNewCurrentPosZ();
			notifyProgess("Touch Z successfully finished");
		}
		else {
			notifyProgess("Touch Z failed");
		}
		
		// finish here if mode are corresponding
		if ( parameters.touchMode == Parameters::TouchMode::TM_TOUCH_Z ) {
			finalize();
			return hasError() == false;
		}
	}
	// ************************************************************
	// touch X
	// ************************************************************
	{
		if ( hasError() )
			return false;

		notifyProgess("Touch X started");
		
		findStartPosX();
		if ( moveUntilContact(Axis_X, DefaultDist, "Error while moving X until contact!") ) {
			
			CncLongPosition currPos(requestControllerPos());
			touchPos.setX(currPos.getX());
			
			determineNewCurrentPosX();
			notifyProgess("Touch X successfully finished");
		}
		else {
			notifyProgess("Touch X failed");
		}
	}
	// ************************************************************
	// touch Y
	// ************************************************************
	{
		if ( hasError() )
			return false;
		
		notifyProgess("Touch Y started");

		findStartPosY();
		if ( moveUntilContact(Axis_Y, DefaultDist, "Error while moving Y until contact!") ) {
			
			CncLongPosition currPos(requestControllerPos());
			touchPos.setY(currPos.getY());

			determineNewCurrentPosY();
			notifyProgess("Touch Y successfully finished");
		}
		else {
			notifyProgess("Touch Y failed");
		}
	}
	
	moveYFree("Error while moving Y free!");
	
	if ( hasError() )
		return false;
		
	finalize();
	return hasError() == false;
}
