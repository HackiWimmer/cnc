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
#include "CncConfig.h"
#include "SerialPort.h"
#include "CncLimitStates.h"
#include "CncPosition.h"

///////////////////////////////////////////////////////////////////
class CncControl {
	public:
		enum DimensionMode			{ DM_2D, DM_3D };

	private:
		
		struct InteractiveMoveInfo {
			
			CncInteractiveMoveDriver	driver;
			CncStepSensitivity			stepSensitivity;
			
			InteractiveMoveInfo()
			: driver			(IMD_NONE)
			, stepSensitivity	(FINEST)
			{}
			
			bool active() 		{ return driver != IMD_NONE; }
			void reset() 		{ *this = InteractiveMoveInfo(); }
		};
		
		long	 					currentClientId;
		InteractiveMoveInfo			currentInteractiveMoveInfo;
		
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
			
			SetterTuple(unsigned char i, const cnc::SetterValueList& v)
			: pid(i)
			, values(v)
			{}
		};
		typedef std::vector<SetterTuple> Setters;
		
		SetterMap 				setterMap;
		
		void appendToSetterMap(unsigned char pid, const cnc::SetterValueList& values);
		bool dispatchEventQueue();
		
	protected:
		// internal port object
		Serial* serialPort;
		// Defines the absolute zero pos as a reference pos 
		CncLongPosition zeroAppPos;
		// Defines the start postion of an object in relation to zeroPos 
		CncLongPosition startAppPos;
		// Defines the current pos
		CncLongPosition curAppPos;
		// Stores the lateset requested control positions
		CncLongPosition curCtlPos;
		// speed values
		// actual rte (measured) speed value
		float realtimeFeedSpeed_MM_MIN;
		
		// default values for work and rapid
		float defaultFeedSpeedRapid_MM_MIN;
		float defaultFeedSpeedWork_MM_MIN;

		// actual configured speed type and value
		CncSpeedMode configuredSpeedMode;
		float configuredFeedSpeed_MM_MIN;

		// Duration counter
		unsigned int durationCounter;
		// Interrupt state
		bool interruptState;
		// position flf
		bool positionOutOfRangeFlag;
		// power state
		bool toolPowerState;
		// Artificially Step Delay
		unsigned int stepDelay;
		// heartbeat value
		int32_t lastCncHeartbeatValue;
		
		Serial* getSerial() { wxASSERT(serialPort); return serialPort; }
		
		// Tool state handling
		CncToolStateControl toolState;
		//measurements variables
		//struct timeb endTime, startTime;
		// Flag to indicatate if a positions check aplies
		bool positionCheck;
		//sets a value into the given text control
		void setValue(wxTextCtrl *ctl, int32_t val);
		void setValue(wxTextCtrl *ctl, double val);
		void setValue(wxTextCtrl *ctl, const char* t);

		// Tool management
		void setToolState(bool defaultStyle = false);
		
		// Limit management
		CncLimitStates limitStates;
		void displayLimitState(wxWindow* ctl, bool value);
		void displayLimitStates(const int32_t x, const int32_t y, const int32_t z);
		void displayLimitStates(const CncInterface::ILS::States& ls);
		void displayUnknownSupportStates();
		void displaySupportStates(const CncInterface::ISP::States& sp);
		
		// simple move
		bool prepareSimpleMove(bool enaleEventHandling = true);
		void reconfigureSimpleMove(bool correctPositions);
		
		// secial convesion to transfer a double as long
		long convertDoubleToCtrlLong(unsigned char 	id, float d);
		
		// display the given pos in the open gl view
		void monitorPosition(const CncLongPosition& pos);
		
		inline void postAppPosition(unsigned char pid, bool force = false);
		inline void postCtlPosition(unsigned char pid);
		
	public:
		
		struct DimensionXYPlane {
			double dimensionX;
			double dimensionY;
			
			struct {
				CncLongPosition p1;
				CncLongPosition p2;
				CncLongPosition p3;
				CncLongPosition p4;
				CncLongPosition p5;
			} Details;
		};
		
		struct DimensionZAxis {
			double dimensionZ;
			
			struct {
				CncLongPosition pMin;
				CncLongPosition pMax;
			} Details;
		};
		
		CncControl(CncPortType pt);
		virtual ~CncControl();
		
		bool isReadyToRun();
		
		void setClientId(long id) 		{ currentClientId = id; }
		void resetClientId()			{ setClientId(-1L); }
		const long getClientId() const 	{ return currentClientId; }
		
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
		
		// Get the current speed parameter
		CncSpeedMode getConfiguredSpeedMode() 		{ return configuredSpeedMode; }
		float getRealtimeFeedSpeed_MM_SEC()			{ return realtimeFeedSpeed_MM_MIN / 60.0; }
		float getRealtimeFeedSpeed_MM_MIN()			{ return realtimeFeedSpeed_MM_MIN; }
		float getConfiguredFeedSpeed_MM_SEC() 		{ return configuredFeedSpeed_MM_MIN / 60.0; }
		float getConfiguredFeedSpeed_MM_MIN() 		{ return configuredFeedSpeed_MM_MIN; }
		
		// Change the current speed parameter
		bool changeSpeedToDefaultSpeed_MM_MIN(CncSpeedMode s);
		void setDefaultRapidSpeed_MM_MIN(float s);
		void setDefaultWorkSpeed_MM_MIN(float s);
		
		bool changeCurrentFeedSpeedXYZ_MM_SEC(float value = 0.0, CncSpeedMode s = CncSpeedUserDefined);
		bool changeCurrentFeedSpeedXYZ_MM_MIN(float value = 0.0, CncSpeedMode s = CncSpeedUserDefined);
		
		// signal wrapper
		bool sendInterrupt() 		{ wxASSERT(serialPort); return serialPort->sendInterrupt(); }
		bool sendHalt() 			{ wxASSERT(serialPort); return serialPort->sendHalt(); }
		bool sendPause() 			{ wxASSERT(serialPort); return serialPort->sendPause(); }
		bool sendResume() 			{ wxASSERT(serialPort); return serialPort->sendResume(); }
		bool sendSoftwareReset() 	{ wxASSERT(serialPort); return serialPort->sendSignal(SIG_SOFTWARE_RESET); }
		
		// comand wrapper
		bool processCommand(const unsigned char c, std::ostream& txtCtl);
		
		// getter list wrapper
		bool displayGetterList(const PidList& pidlist);
		void displayToolState(const bool state);
		
		// wrapper
		bool processMoveXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);
		
		bool processMoveSequence(CncMoveSequence& moveSequence);
		
		// Zero positioning
		void setZeroPos(bool x, bool y, bool z);
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
		bool resolveLimits(bool x, bool y, bool z);
		bool evaluateHardwareReference();
		bool evaluateHardwareDimensionsXYPlane(DimensionXYPlane& result);
		bool evaluateHardwareDimensionsZAxis(DimensionZAxis& result);
		
		void waitActive(unsigned int millis);
		
		// Callback from Serial
		bool SerialCallback();
		// Callback from Serial with controller content
		bool SerialControllerCallback(const ContollerInfo& ci);
		bool SerialExecuteControllerCallback(const ContollerExecuteInfo& cei);
		
		// Callback fromS Serial with a controller message
		bool SerialMessageCallback(const ControllerMsgInfo& cmi);
		//interrupt the processing
		void interrupt(const char* why = NULL);
		void resetInterrupt();
		bool isInterrupted();
		bool reset();
		bool resetWatermarks();
		// Setup the cnc control
		void resetSetterMap();
		bool setup(bool reset = true);
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
		bool getToolState() { return toolPowerState; }
		// Updates the config trace control
		void notifyConfigUpdate();
		
		CncLongPosition& 					getCurAppPosAsReference()	{ return curAppPos;   }
		const CncLongPosition 				getCurAppPos()				{ return curAppPos;   }
		const CncLongPosition 				getCurCtlPos() 				{ return curCtlPos;   }
		const CncLongPosition 				getAppStartPos() 			{ return startAppPos; }
		const CncLongPosition 				getAppZeroPos() 			{ return zeroAppPos;  }
		
		const CncDoublePosition 			getStartPosMetric();
		const CncDoublePosition 			getCurAppPosMetric();
		const CncDoublePosition 			getCurCtlPosMetric();
		const CncLongPosition 				getMinPositions();
		const CncDoublePosition 			getMinPositionsMetric();
		const CncLongPosition 				getMaxPositions();
		const CncDoublePosition 			getMaxPositionsMetric();
		const CncLongPosition::Watermarks 	getWaterMarks();
		const CncDoublePosition::Watermarks getWaterMarksMetric();
		
		const double 						getCurAppPosMetricX() { return curAppPos.getX() * THE_CONFIG->getDisplayFactX(); }
		const double 						getCurAppPosMetricY() { return curAppPos.getY() * THE_CONFIG->getDisplayFactY(); }
		const double 						getCurAppPosMetricZ() { return curAppPos.getZ() * THE_CONFIG->getDisplayFactZ(); }
		const double 						getCurCtlPosMetricX() { return curCtlPos.getX() * THE_CONFIG->getDisplayFactX(); }
		const double 						getCurCtlPosMetricY() { return curCtlPos.getY() * THE_CONFIG->getDisplayFactY(); }
		const double 						getCurCtlPosMetricZ() { return curCtlPos.getZ() * THE_CONFIG->getDisplayFactZ(); }
		
		bool								convertPositionToHardwareCoordinate(const CncLongPosition&   pos, CncLongPosition&   ret);
		bool								convertPositionToHardwareCoordinate(const CncDoublePosition& pos, CncDoublePosition& ret);
		
		// query the current controller position
		const CncLongPosition requestControllerPos();
		// query the current controller limit state
		const CncLongPosition requestControllerLimitState();
		const CncLimitStates& getLimitState() { return limitStates; }
		// validates pc and controller positions
		bool validateAppAgainstCtlPosition();
		// execute
		bool execute(const unsigned char* buffer, unsigned int nbByte);
		//
		bool popSerial();
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
		
		bool startInteractiveMove(CncStepSensitivity s, CncInteractiveMoveDriver imd);
		bool updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z);
		bool stopInteractiveMove();
		bool isInteractiveMoveActive() { return currentInteractiveMoveInfo.active(); }
		
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
		void clearControllerConfigControl();
		void appendPidKeyValueToControllerConfig(int pid, const char* key, const char* value, const char* unit);
		
		// controller pin report
		void clearControllerPinControl();
		void appendNumKeyValueToControllerPinInfo(const char* desc, int pin, int type, int mode, int value);
		
		void setStepDelay(unsigned int d) { stepDelay = d; }
		unsigned int getStepDelay() { return stepDelay; }
		
		void startSerialMeasurement() 									{ getSerial()->startMeasurement(); }
		void stopSerialMeasurement()  									{ getSerial()->stopMeasurement(); }
		const CncNanoTimespan getMeasurementNanoTimeSpanTotal()			{ return getSerial()->getMeasurementNanoTimeSpanTotal(); }
		double getTotalDistanceMetricX()								{ return getSerial()->getTotalDistanceMetricX(); }
		double getTotalDistanceMetricY()								{ return getSerial()->getTotalDistanceMetricY(); }
		double getTotalDistanceMetricZ()								{ return getSerial()->getTotalDistanceMetricZ(); }
		double getTotalDistanceMetric() 								{ return getSerial()->getTotalDistanceMetric();  }
		size_t getTotalDistanceStepsX()									{ return getSerial()->getTotalDistanceStepsX(); }
		size_t getTotalDistanceStepsY()									{ return getSerial()->getTotalDistanceStepsY(); }
		size_t getTotalDistanceStepsZ()									{ return getSerial()->getTotalDistanceStepsZ(); }
		size_t getTotalDistanceSteps() 									{ return getSerial()->getTotalDistanceSteps();  }
		double getMeasuredFeedSpeed_MM_MIN() 							{ return getSerial()->getMeasuredFeedSpeed_MM_MIN(); }
		double getMeasuredFeedSpeed_MM_SEC() 							{ return getSerial()->getMeasuredFeedSpeed_MM_SEC(); }
		size_t getStepCounter()											{ return getSerial()->getStepCounter(); }
		size_t getStepCounterX()										{ return getSerial()->getStepCounterX(); }
		size_t getStepCounterY()										{ return getSerial()->getStepCounterY(); }
		size_t getStepCounterZ()										{ return getSerial()->getStepCounterZ(); }
		size_t getPositionCounter()										{ return getSerial()->getPositionCounter(); }
		
		void enableSpyOutput(bool show=true) 							{ getSerial()->enableSpyOutput(show); }
		const char* getPortName() 										{ return getSerial()->getPortName(); }
		const char* getClassName() 										{ return getSerial()->getClassName(); }
		const CncPortType getPortType() 								{ return getSerial()->getPortType(); }
		bool isCommandActive() 											{ return getSerial()->isCommandActive(); }
		bool isOutputAsTemplateAvailable()								{ return getSerial()->isOutputAsTemplateAvailable(); }
		bool isEmulator() 												{ return getSerial()->isEmulator(); }
		bool canProcessIdle() 											{ return getSerial()->canProcessIdle(); }
		bool isIdleActive()    											{ return getSerial()->isIdleActive(); }
		bool serialDataAvailable()										{ return getSerial()->dataAvailable(); }
		
		void setSpyMode(Serial::SypMode sm)								{ getSerial()->setSpyMode(sm); }
		void processTrigger(const Serial::Trigger::BeginRun& tr)		{ getSerial()->processTrigger(tr); }
		void processTrigger(const Serial::Trigger::EndRun& tr)			{ getSerial()->processTrigger(tr); }
		void processTrigger(const Serial::Trigger::NextPath& tr)		{ getSerial()->processTrigger(tr); }
		void processTrigger(const Serial::Trigger::SpeedChange& tr)		{ getSerial()->processTrigger(tr); }

		// 3D control
		void updatePreview3D();
		
		// idle handling
		void sendIdleMessage();
		
		// for testing only
		Serial* getSerialExtern() { wxASSERT(serialPort); return serialPort; }
		
};

#endif
