#ifndef CNC_CONTROL_H
#define CNC_CONTROL_H

#include <vector>
#include <time.h>
#include <assert.h>
#include <wx/string.h>
#include <wx/geometry.h>
#include <wx/menuitem.h>
#include <wx/checkbox.h>
#include <wx/dragimag.h>
#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/pen.h>
#include "CommandTemplates.h"
#include "CncToolStateControl.h"
#include "CncCommon.h"
#include "PenHandler.h"
#include "GuiControlSetup.h"
#include "CncConfig.h"
#include "SerialPort.h"
#include "CncLimitStates.h"
#include "CncPosition.h"

const double endSwitchStepBackMertic = 2.5;

///////////////////////////////////////////////////////////////////
class CncControl {
	public:
		enum DimensionMode   { DM_2D, DM_3D };
		enum StepSensitivity { FINEST = 1, FINE = 10 , MEDIUM = 50, ROUGH = 100, ROUGHEST = 200 };
		const short STEP_SENSITIVITY_FACTOR = 100;

	private:
		long currentClientId;
		bool runContinuousMove;
		
		///////////////////////////////////////////////////////////////////
		struct SetterTuple {
			unsigned char 	pid;
			cnc::SetterValueList values;
			
			SetterTuple(unsigned char i, int32_t v)
			: pid(i)
			, values()
			{
				values.push_back(v);
			}
			
			SetterTuple(unsigned char i, cnc::SetterValueList v)
			: pid(i)
			, values(v)
			{}
		};
		typedef std::vector<SetterTuple> Setters;
		
		SetterMap setterMap;
		
	protected:
		CncPortType portType;
		// internal port object
		Serial* serialPort;
		// handels the cnc configuration
		CncConfig* cncConfig;
		// Defines the absolute zero pos as a reference pos 
		CncLongPosition zeroPos;
		// Defines the start postion of an object in relation to zeroPos 
		CncLongPosition startPos;
		// Defines the current pos
		CncLongPosition curAppPos;
		// Stores the lateset requested control positions
		CncLongPosition curCtlPos;
		// Stores the lateset requested control positions
		CncLongPosition controllerPos;
		// speed values
		// actual rte (measured) speed value
		double realtimeFeedSpeed_MM_MIN;
		
		// default values for work and rapid
		double defaultFeedSpeedRapid_MM_MIN;
		double defaultFeedSpeedWork_MM_MIN;

		// actual configured speed type and value
		CncSpeedMode configuredSpeedMode;
		double configuredFeedSpeed_MM_MIN;

		// Duration counter
		unsigned int durationCounter;
		// Interrupt state
		bool interruptState;
		// position flf
		bool positionOutOfRangeFlag;
		// power state
		bool powerOn;
		// flag for updating tools
		bool toolUpdateState;
		// Artificially Step Delay
		unsigned int stepDelay;
		// heartbeat value
		int32_t lastCncHeartbeatValue;
		
		// output controls
		GuiControlSetup* guiCtlSetup;
		#define GET_GUI_CTL(ctl)           (guiCtlSetup->ctl)
		#define IS_GUI_CTL_VALID(ctl)      (guiCtlSetup != NULL && guiCtlSetup->ctl != NULL)
		#define IS_GUI_CTL_NOT_VALID(ctl)  (guiCtlSetup == NULL || guiCtlSetup->ctl == NULL)
		// Tool state handling
		CncToolStateControl toolState;
		//measurements variables
		//struct timeb endTime, startTime;
		// Flag to indicatate if a positions check aplies
		bool positionCheck;
		// margin of the draw pane
		unsigned int drawPaneMargin;
		//sets a value into the given text control
		void setValue(wxTextCtrl *ctl, int32_t val);
		void setValue(wxTextCtrl *ctl, double val);
		void setValue(wxTextCtrl *ctl, const char* t);

		// Tool management
		void setToolState(bool defaultStyle = false);
		
		// Limit management
		CncLimitStates limitStates;
		void displayLimitState(wxStaticText* ctl, bool value);
		void displayLimitStates(const int32_t x, const int32_t y, const int32_t z);
		void displayLimitStates(const CncInterface::ILS::States& ls);
		void displayUnknownSupportStates();
		void displaySupportStates(const CncInterface::ISP::States& sp);
		
		// simple move
		bool prepareSimpleMove(bool enaleEventHandling = true);
		void reconfigureSimpleMove(bool correctPositions);
		
		// secial convesion to transfer a double as long
		long convertDoubleToCtrlLong(unsigned char 	id, double d);
		
		// display the given pos in the open gl view
		void monitorPosition(const CncLongPosition& pos);
		
		inline void postAppPosition(unsigned char pid);
		inline void postCtlPosition(unsigned char pid);
		
		bool manualContinuousMoveStart_CtrlBased(const double x, const double y, const double z, bool correctLimit=true);
		
	public:
		
		CncControl(CncPortType pt);
		virtual ~CncControl();
		
		bool isReadyToRun();
		
		void setClientId(long id) { currentClientId = id; }
		const long getClientId() const { return currentClientId; }
		
		//Get the current port type
		CncPortType getPortType(void) { return portType; }
		// Connection to portName
		bool connect(const char * portName);
		// Disconnection the serial connection
		bool disconnect();
		// Check the connection
		bool isConnected();
		
		void resetPositionOutOfRangeFlag() { positionOutOfRangeFlag = false; }
		bool getPositionOutOfRangeFlag() { return positionOutOfRangeFlag; }
		
		// validating given pos due to the given reference coordinates
		bool isPositionOutOfRange(const CncLongPosition& pos, bool trace=true);
		
		void onPeriodicallyAppEvent();
		
		//Make Serial available
		Serial* getSerial() { return serialPort; }
		
		// Get the current speed parameter
		CncSpeedMode getConfiguredSpeedMode() 		{ return configuredSpeedMode; }
		double getRealtimeFeedSpeed_MM_SEC()		{ return realtimeFeedSpeed_MM_MIN / 60.0; }
		double getRealtimeFeedSpeed_MM_MIN()		{ return realtimeFeedSpeed_MM_MIN; }
		double getConfiguredFeedSpeed_MM_SEC() 		{ return configuredFeedSpeed_MM_MIN / 60.0; }
		double getConfiguredFeedSpeed_MM_MIN() 		{ return configuredFeedSpeed_MM_MIN; }
		
		// Change the current speed parameter
		void changeSpeedToDefaultSpeed_MM_MIN(CncSpeedMode s);
		void setDefaultRapidSpeed_MM_MIN(double s);
		void setDefaultWorkSpeed_MM_MIN(double s);
		
		void changeCurrentFeedSpeedXYZ_MM_SEC(double value = 0.0, CncSpeedMode s = CncSpeedUserDefined);
		void changeCurrentFeedSpeedXYZ_MM_MIN(double value = 0.0, CncSpeedMode s = CncSpeedUserDefined);
		
		// signal wrapper
		bool sendInterrupt() 	{ wxASSERT(serialPort); return serialPort->sendInterrupt(); }
		bool sendHalt() 		{ wxASSERT(serialPort); return serialPort->sendHalt(); }
		bool sendPause() 		{ wxASSERT(serialPort); return serialPort->sendPause(); }
		bool sendResume() 		{ wxASSERT(serialPort); return serialPort->sendResume(); }
		
		// comand wrapper
		bool processCommand(const unsigned char c, std::ostream& txtCtl);
		
		// getter list wrapper
		bool displayGetterList(const PidList& pidlist);
		
		void displayToolState(const bool state);
		void displaySpeedValue(const double value);
		void displaySpeedMode(const CncSpeedMode mode);
		
		// wrapper
		bool processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);
		// Zero positioning
		void setZeroPosX();
		void setZeroPosY();
		void setZeroPosZ();
		void setZeroPos();
		// Start positioning
		void setStartPos();
		// Move from current position
		bool moveRelStepsZ(int32_t z);
		bool moveRelLinearStepsXY(int32_t x1, int32_t y1, bool alreadyRendered);
		bool moveRelLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);
		
		bool moveRelMetricZ(double z);
		bool moveRelLinearMetricXY(double x1, double y1, bool alreadyRendered);
		bool moveRelLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered);
		
		bool moveAbsMetricZ(double z);
		bool moveAbsLinearMetricXY(double x1, double y1, bool alreadyRendered);
		bool moveAbsLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered);
		
		bool correctLimitPositions();
		
		void waitActive(unsigned int millis);
		
		// Callback from Serial
		bool SerialCallback();
		// Callback from Serial with controller content
		bool SerialControllerCallback(const ContollerInfo& ci);
		bool SerialExecuteControllerCallback(const ContollerExecuteInfo& cei);
		
		// Callback fromS Serial with a controller message
		bool SerialMessageCallback(const ControllerMsgInfo& cmi);
		//interrupt the processing
		void interrupt();
		void resetInterrupt();
		bool isInterrupted();
		bool reset();
		bool resetWatermarks();
		// Setup the cnc control
		void resetSetterMap();
		bool setup(bool reset = true);
		// Sets the output controls for cooridinate infos
		void setGuiControls(GuiControlSetup* guiCtlSetup);
		//handle draw control
		unsigned int getDrawPaneMargin() { return drawPaneMargin; }
		void updateDrawControl();
		// Duration management
		unsigned int getDurationCount();
		unsigned int getDurationCounter();
		void resetDurationCounter();
		void initNextDuration();
		bool hasNextDuration();
		bool isFirstDuration();
		bool isLastDuration();
		// Z axis management
		bool moveZToTop();
		bool moveZToBottom();
		
		// Tool management
		void switchToolOn();
		void switchToolOff(bool force = false);
		bool getToolState() { return powerOn; }
		// Updates the config trace control
		void updateCncConfigTrace();
		
		const CncLongPosition getCurCtlPos() { return curCtlPos; }
		
		// returns the correponding pc postions
		CncLongPosition& getCurAppPosAsReference()		{ return curAppPos; }
		const CncLongPosition getCurPos()				{ return curAppPos; }
		const CncLongPosition getStartPos() 			{ return startPos; }
		const CncDoublePosition getStartPosMetric();
		const CncDoublePosition getCurPosMetric();
		const CncLongPosition getMinPositions();
		const CncDoublePosition getMinPositionsMetric();
		const CncLongPosition getMaxPositions();
		const CncDoublePosition getMaxPositionsMetric();
		const CncLongPosition::Watermarks getWaterMarks();
		const CncDoublePosition::Watermarks getWaterMarksMetric();
		// query the current controller position
		const CncLongPosition getControllerPos();
		// query the current controller limit state
		const CncLongPosition getControllerLimitState();
		const CncLimitStates& getLimitState() { return limitStates; }
		// validates pc and controller positions
		bool validateAppAgainstCtlPosition();
		// processing the given setter values
		bool processSetter(unsigned char pid, int32_t value);
		bool processSetter(unsigned char pid, const cnc::SetterValueList& values);
		bool processSetterList(const Setters& setup);
		// Sets a flag that the postions x/y min/max should be checked within the Serial callback
		void activatePositionCheck(bool a) { positionCheck = a; }
		// Sets the enable pin HIGH (s == false) or LOW ( s == true)
		bool enableStepperMotors(bool s);
		bool enableProbeMode(bool s);
		//Limit management
		wxString& getLimitInfoString(wxString& ret);
		void evaluateLimitState();
		bool isALimitSwitchActive() { return limitStates.hasLimit(); }
		
		bool moveXToMinLimit();
		bool moveXToMaxLimit();
		bool moveYToMinLimit();
		bool moveYToMaxLimit();
		bool moveZToMinLimit();
		bool moveZToMaxLimit();
		bool moveXToMid();
		bool moveYToMid();
		bool moveZToMid();
		
		bool manualMoveFinest(StepSensitivity s,  const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z, bool correctLimit=true);
		bool manualContinuousMoveStart(StepSensitivity s,  const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z, bool correctLimit=true);
		void manualContinuousMoveStop();
		bool isContinuousMoveActive() { return runContinuousMove; }
		
		bool manualSimpleMoveSteps(int32_t x, int32_t y, int32_t z, bool alreadyRendered = false);
		bool manualSimpleMoveSteps3D(int32_t x, int32_t y, int32_t z, bool alreadyRendered = false);
		bool manualSimpleMoveMetric(double x, double y, double z, bool alreadyRendered = false);
		bool manualSimpleMoveMetric3D(double x, double y, double z, bool alreadyRendered = false);
		
		// Moves curPos to zeroPos
		bool moveXYToZeroPos(CncDimensions dim = CncDimension1D);
		bool moveXYZToZeroPos(CncDimensions dim = CncDimension1D);
		bool moveZToZeroPos();
		
		// Moves curPos to startPos
		bool moveToStartPos();
		
		// Moves curPos to zeroPos
		bool simpleMoveXYToZeroPos(CncDimensions dim);
		bool simpleMoveXYZToZeroPos(CncDimensions dim);
		bool simpleMoveZToZeroPos();
		
		// controller configuration output
		bool hasControllerConfigControl();
		void clearControllerConfigControl();
		void appendPidKeyValueToControllerConfig(int pid, const char* key, const char* value, const char* unit);
		
		// controller pin report
		bool hasControllerPinControl();
		void clearControllerPinControl();
		void appendNumKeyValueToControllerPinInfo(const char* desc, int pin, int type, int mode, int value);
		
		void setUpdateToolControlsState(bool state) { toolUpdateState = state; }
		
		void setStepDelay(unsigned int d) { stepDelay = d; }
		unsigned int getStepDelay() { return stepDelay; }
		
		//meassurments
		bool meassureDimension(const char axis, wxCheckBox* min, wxCheckBox* max, double& result);
		bool meassureXDimension(wxCheckBox* min, wxCheckBox* max, double& result) { return meassureDimension('X', min, max, result); }
		bool meassureYDimension(wxCheckBox* min, wxCheckBox* max, double& result) { return meassureDimension('Y', min, max, result); }
		bool meassureZDimension(wxCheckBox* min, wxCheckBox* max, double& result) { return meassureDimension('Z', min, max, result); }
		
		// 3D control
		void updatePreview3D(bool force=false);
		
		// idle handling
		void sendIdleMessage();
		
};

#endif
