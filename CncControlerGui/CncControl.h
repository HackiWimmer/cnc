#ifndef CNC_CONTROL_H
#define CNC_CONTROL_H

#include <vector>
#include <time.h>
#include <assert.h>
#include <wx/string.h>
#include <wx/geometry.h>
#include <wx/menuitem.h>
#include <wx/checkbox.h>
#include <wx/checkbox.h>
#include <wx/dragimag.h>
#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/pen.h>
#include "CncPathListManager.h"
#include "CncCommon.h"
#include "PenHandler.h"
#include "CncConfig.h"
#include "SerialPort.h"
#include "CncLimitStates.h"
#include "CncPosition.h"

class CncTouchBlockDetector;

///////////////////////////////////////////////////////////////////
class CncControl 
{
	public:
		
		enum DimensionMode			{ DM_2D, DM_3D };
		enum CtrlPowerState			{ CPS_ON = POWER_STATE_ON, CPS_OFF = POWER_STATE_OFF, CPS_NOT_INITIALIZED = -1, CPS_UNKNOWN = -2  };
		
		enum RunMode				{ M_DryRun, M_RealRun };
		
		static const char* getCtrlPowerStateAsStr(const CtrlPowerState cps) 
		{
			switch ( cps ) {
				case CPS_ON:				return "ON";
				case CPS_OFF:				return "OFF";
				case CPS_UNKNOWN:			return "Unknown";
				case CPS_NOT_INITIALIZED:	return "Not Initialized";
			}
			
			return "???";
		}
		
	private:
		
		struct CncSpeedMemory : public std::map<CncSpeedMode, float> 
		{
			public:
				CncSpeedMemory()
				{
					(*this)[CncSpeedRapid]			= THE_CONFIG->getDefaultRapidSpeed_MM_MIN();
					(*this)[CncSpeedWork]			= THE_CONFIG->getDefaultWorkSpeed_MM_MIN();
					(*this)[CncSpeedMax]			= THE_CONFIG->getDefaultRapidSpeed_MM_MIN();
					(*this)[CncSpeedUserDefined]	= 0.0;
				}
		};
		
		struct InteractiveMoveInfo 
		{
			CncInteractiveMoveDriver	driver;
			CncStepSensitivity			stepSensitivity;
			
			InteractiveMoveInfo()
			: driver			(IMD_NONE)
			, stepSensitivity	(FINEST)
			{}
			
			bool active()	const	{ return driver != IMD_NONE; }
			void reset()			{ *this = InteractiveMoveInfo(); }
		};
		
		///////////////////////////////////////////////////////////////////
		struct SetterTuple 
		{
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
		
		long	 				currentClientId;
		InteractiveMoveInfo		currentInteractiveMoveInfo;
		SetterMap 				setterMap;
		
		void appendToSetterMap(unsigned char pid, const cnc::SetterValueList& values);
		bool dispatchEventQueue();
		
	protected:
	
		RunMode					runMode;
		Serial* 				serialPort;
		Serial*					realRunSerial;
		Serial*					dryRunSerial;
		
		CncLongPosition 		zeroAppPos;
		CncLongPosition 		startAppPos;
		CncLongPosition 		curAppPos;
		CncLongPosition 		curCtlPos;
		
		float 					realtimeFeedSpeed_MM_MIN;
		float 					defaultFeedSpeedRapid_MM_MIN;
		float 					defaultFeedSpeedWork_MM_MIN;
		CncSpeedMemory			speedMemory_MM_MIN;
		CncSpeedMode			configuredSpeedMode;
		float					configuredFeedSpeed_MM_MIN;
		bool					configuredSpeedModePreviewFlag;
		
		double 					configuredSpindleSpeed;
		
		unsigned int			durationCounter;
		bool					interruptState;
		bool					positionOutOfRangeFlag;
		bool					positionCheck;
		
		CtrlPowerState			ctrlPowerState;
		CncSpindlePowerState	spindlePowerState;
		unsigned int			stepDelay;
		int32_t					lastCncHeartbeatValue;
		
		CncLimitStates			limitStates;
		
		
		Serial* getSerial() const { wxASSERT(serialPort); return serialPort; }
		
		// Limit management
		void displayLimitState(wxWindow* ctl, bool value);
		void displayLimitStates(const int32_t x, const int32_t y, const int32_t z);
		void displayLimitStates(const CncInterface::ILS::States& ls);
		void displayUnknownSupportStates();
		void displaySupportStates(const CncInterface::ISP::States& sp);
		void displayHealtyStates();
		
		bool prepareSimpleMove(bool enaleEventHandling = true);
		void reconfigureSimpleMove(bool correctPositions);
		
		long convertDoubleToCtrlLong(unsigned char 	id, float d);
		
		void monitorPosition(const CncLongPosition& pos);
		
		inline void postAppPosition(unsigned char pid, bool force = false);
		inline void postCtlPosition(unsigned char pid);
		
	public:
		
		struct DimensionXYPlane 
		{
			double dimensionX;
			double dimensionY;
			
			struct
			{
				CncLongPosition p1;
				CncLongPosition p2;
				CncLongPosition p3;
				CncLongPosition p4;
				CncLongPosition p5;
			} Details;
		};
		
		struct DimensionZAxis 
		{
			double dimensionZ;
			
			struct 
			{
				CncLongPosition pMin;
				CncLongPosition pMax;
			} Details;
		};
		
		CncControl(CncPortType pt);
		virtual ~CncControl();
		
		bool isReadyToRun();
		
		void setClientId(long id);
		void resetClientId()			{ setClientId(CLIENT_ID.INVALID); }
		const long getClientId() const 	{ return currentClientId; }
		
		bool dryRunAvailable() const { return dryRunSerial != NULL; }
		void switchRunMode(RunMode m);
		
		bool connect(const char * portName);
		bool disconnect();
		bool isConnected();
		
		void resetPositionOutOfRangeFlag() { positionOutOfRangeFlag = false; }
		bool getPositionOutOfRangeFlag() { return positionOutOfRangeFlag; }
		
		bool isPositionOutOfRange(const CncLongPosition& pos, bool trace=true);
		
		void onPeriodicallyAppEvent();
		
		void previewConfiguredFeedSpeed_MM_MIN(CncSpeedMode m, float v);

		// Get the current speed parameter
		CncSpeedMode getConfiguredSpeedMode() 		{ return configuredSpeedMode; }
		float getRealtimeFeedSpeed_MM_SEC()			{ return realtimeFeedSpeed_MM_MIN / 60.0; }
		float getRealtimeFeedSpeed_MM_MIN()			{ return realtimeFeedSpeed_MM_MIN; }
		float getConfiguredFeedSpeed_MM_SEC() 		{ return configuredFeedSpeed_MM_MIN / 60.0; }
		float getConfiguredFeedSpeed_MM_MIN() 		{ return configuredFeedSpeed_MM_MIN; }
		
		float getPrevStoredRapidSpeed_MM_MIN()		{ return speedMemory_MM_MIN[CncSpeedRapid]; }
		float getPrevStoredWorkSpeed_MM_MIN()		{ return speedMemory_MM_MIN[CncSpeedWork]; }
		float getPrevStoredMaxSpeed_MM_MIN()		{ return speedMemory_MM_MIN[CncSpeedMax]; }
		float getPrevStoredUserDefSpeed_MM_MIN()	{ return speedMemory_MM_MIN[CncSpeedUserDefined]; }
		
		// Change the current speed parameter
		bool changeSpeedToDefaultSpeed_MM_MIN(CncSpeedMode s);
		bool changeSpeedToPrevStoredSpeed_MM_MIN(CncSpeedMode s);
		void setDefaultRapidSpeed_MM_MIN(float s);
		void setDefaultWorkSpeed_MM_MIN(float s);
		
		bool changeCurrentFeedSpeedXYZ_MM_SEC(float value = 0.0, CncSpeedMode s = CncSpeedUserDefined);
		bool changeCurrentFeedSpeedXYZ_MM_MIN(float value = 0.0, CncSpeedMode s = CncSpeedUserDefined);
		
		bool changeCurrentSpindleSpeed_U_MIN(double value );
		double getConfiguredSpindleSpeed() const { return configuredSpindleSpeed; }
		
		// signal wrapper
		bool sendInterrupt()		{ wxASSERT(serialPort); return serialPort->sendInterrupt(); }
		bool sendHalt()				{ wxASSERT(serialPort); return serialPort->sendHalt(); }
		bool sendPause()			{ wxASSERT(serialPort); return serialPort->sendPause(); }
		bool sendResume()			{ wxASSERT(serialPort); return serialPort->sendResume(); }
		bool sendQuitMove()			{ wxASSERT(serialPort); return serialPort->sendQuitMove(); }
		bool sendSoftwareReset() 	{ wxASSERT(serialPort); return serialPort->sendSignal(SIG_SOFTWARE_RESET); }
		
		// comand wrapper
		bool pushCommand(const unsigned char cmd);
		bool processCommand(const unsigned char cmd, std::ostream& txtCtl);
		
		// getter list wrapper
		bool displayGetterList(const PidList& pidlist);
		
		bool processGetter(unsigned char pid, GetterValues& ret);
		bool processMovePodium(int32_t steps, bool exact);
		
		bool resetPodiumDistance();
		double getPodiumDistanceMetric();
		
		// Zero positioning
		void setZeroPosX(int32_t x, int32_t y, int32_t z);
		void setZeroPosX(int32_t v);
		void setZeroPosY(int32_t v);
		void setZeroPosZ(int32_t v);
		
		void setZeroPos(bool x, bool y, bool z);
		void setZeroPos();

		// Start positioning
		void setStartPos();
		// Move from current position
		bool moveRelStepsX(int32_t z);
		bool moveRelStepsY(int32_t z);
		bool moveRelStepsZ(int32_t z);
		bool moveRelLinearStepsXY(int32_t x1, int32_t y1, bool alreadyRendered);
		bool moveRelLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);
		
		bool moveAbsLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);

		bool moveRelMetricZ(double z);
		bool moveRelLinearMetricXY(double x1, double y1, bool alreadyRendered);
		bool moveRelLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered);
		
		bool moveAbsMetricZ(double z);
		bool moveAbsLinearMetricXY(double x1, double y1, bool alreadyRendered);
		bool moveAbsLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered);
		
		bool processMoveImage(const CncMoveSequenceImage& moveImage);
		bool processMoveSequence(CncMoveSequence& moveSequence);
		
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
		
		// Callback from Serial with a controller message
		bool SerialMessageCallback(const ControllerMsgInfo& cmi);

		void interrupt(const char* why = NULL);
		void resetInterrupt();
		bool isInterrupted();
		bool reset();

		bool resetWatermarks();
		void resetSetterMap();
		bool setup(bool reset = true);
		
		unsigned int	getDurationCount();
		unsigned int	getDurationCounter();
		void			resetDurationCounter();
		void			initNextDuration();
		bool			hasNextDuration();
		bool			isFirstDuration();
		bool			isLastDuration();
		
		// Spindle management
		bool								switchSpindleState(CncSpindlePowerState state, bool force = false);
		bool								switchSpindleOn();
		bool								switchSpindleOff(bool force = false);
		
		CncSpindlePowerState				evaluateSpindlePowerState();
		CncSpindlePowerState				getSpindlePowerState() const { return spindlePowerState; }
		bool isSpindleOn()					const { return spindlePowerState == SPINDLE_STATE_ON; }
		bool isSpindleOff()					const { return spindlePowerState == SPINDLE_STATE_OFF; }
		
		CncLongPosition& 					getCurAppPosAsReference()			{ return curAppPos;   }
		const CncLongPosition 				getCurAppPos()				const	{ return curAppPos;   }
		const CncLongPosition 				getCurCtlPos() 				const	{ return curCtlPos;   }
		const CncLongPosition 				getAppStartPos() 			const	{ return startAppPos; }
		const CncLongPosition 				getAppZeroPos() 			const	{ return zeroAppPos;  }
		
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
		const CncLongPosition				requestControllerPos();
		
		// query the current controller limit state
		const CncLongPosition				requestControllerLimitState();
		const CncLimitStates&				getLimitState() { return limitStates; }
		
		// validates pc and controller positions
		bool 								validateAppAgainstCtlPosition();
		
		// execute
		bool 								execute(const unsigned char* buffer, unsigned int nbByte);
		bool 								popSerial(bool dispatchUserEvents=true);
		
		// processing the given setter values
		bool								processSetter(unsigned char pid, int32_t value);
		bool								processSetter(unsigned char pid, const cnc::SetterValueList& values);
		bool								processSetterList(const Setters& setup);
		
		// Sets a flag that the postions x/y min/max should be checked within the Serial callback
		void								activatePositionCheck(bool a) { positionCheck = a; }
		
		// Sets the enable pin HIGH (s == false) or LOW ( s == true)
		bool								enableStepperMotors(bool s);
		bool								enableProbeMode(bool s);
		
		//Limit management
		wxString& 							getLimitInfoString(wxString& ret);
		void								evaluateLimitState();
		bool								isALimitSwitchActive() { return limitStates.hasLimit(); }
		
		bool moveToLogicalPos(const CncDoublePosition& p, CncMoveOrder mo);
		bool moveToLogicalPos(const CncLongPosition& p, CncMoveOrder mo);
		
		bool moveToPhysicalPos(const CncDoublePosition& p, CncMoveOrder mo);
		bool moveToPhysicalPos(const CncLongPosition& p, CncMoveOrder mo);
		
		bool moveToZeroZAxisAnchor();
		bool moveToToolChangeAnchor();
		bool interactToolChange(int toolId);
		
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
		bool updateInteractiveMove(const CncLinearDirection x, const CncLinearDirection y, const CncLinearDirection z, int modifySpeed=0);
		bool updateInteractiveMove();
		bool stopInteractiveMove();
		bool isInteractiveMoveActive() const { return currentInteractiveMoveInfo.active(); }
		
		bool manualSimpleMoveSteps(int32_t x, int32_t y, int32_t z, bool alreadyRendered = false);
		bool manualSimpleMoveSteps3D(int32_t x, int32_t y, int32_t z, bool alreadyRendered = false);
		bool manualSimpleMoveMetric(double x, double y, double z, bool alreadyRendered = false);
		bool manualSimpleMoveMetric3D(double x, double y, double z, bool alreadyRendered = false);
		
		// Moves curPos to zeroPos
		bool moveXYToZeroPos(CncDimensions dim = CncDimension1D);
		bool moveXYZToZeroPos(CncDimensions dim = CncDimension1D);
		bool moveZToZeroPos();
		bool moveZToTop();
		
		// Moves curPos to startPos
		bool moveXYToStartPos();
		bool moveToStartPos();
		bool moveToPos(const CncLongPosition& pos);
		
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
		
		void				startSerialMeasurement() 									{ getSerial()->startMeasurement(); }
		void				stopSerialMeasurement()  									{ getSerial()->stopMeasurement(); }
		const				CncNanoTimespan getMeasurementNanoTimeSpanTotal()	const	{ return getSerial()->getMeasurementNanoTimeSpanTotal(); }
		double				getTotalDistanceMetricX()							const	{ return getSerial()->getTotalDistanceMetricX(); }
		double				getTotalDistanceMetricY()							const	{ return getSerial()->getTotalDistanceMetricY(); }
		double				getTotalDistanceMetricZ()							const	{ return getSerial()->getTotalDistanceMetricZ(); }
		double				getTotalDistanceMetric() 							const	{ return getSerial()->getTotalDistanceMetric();  }
		size_t				getTotalDistanceStepsX()							const	{ return getSerial()->getTotalDistanceStepsX(); }
		size_t				getTotalDistanceStepsY()							const	{ return getSerial()->getTotalDistanceStepsY(); }
		size_t				getTotalDistanceStepsZ()							const	{ return getSerial()->getTotalDistanceStepsZ(); }
		size_t				getTotalDistanceSteps() 							const	{ return getSerial()->getTotalDistanceSteps();  }
		double				getMeasuredFeedSpeed_MM_MIN() 								{ return getSerial()->getMeasuredFeedSpeed_MM_MIN(); }
		double				getMeasuredFeedSpeed_MM_SEC() 								{ return getSerial()->getMeasuredFeedSpeed_MM_SEC(); }
		
		void 				enableSpyOutput(bool show=true) 							{ getSerial()->enableSpyOutput(show); }
		const char* 		getPortName() 										const	{ return getSerial()->getPortName(); }
		const char*			getClassName() 										const	{ return getSerial()->getClassName(); }
		const CncPortType	getPortType() 										const	{ return getSerial()->getPortType(); }
		bool				isSpyOutputOn()										const	{ return getSerial()->isSpyOutputOn(); }
		bool				isCommandActive() 									const	{ return getSerial()->isCommandActive(); }
		bool				isOutputAsTemplateAvailable()						const	{ return getSerial()->isOutputAsTemplateAvailable(); }
		bool				isEmulator() 										const	{ return getSerial()->isEmulator(); }
		bool				canProcessIdle() 									const 	{ return getSerial()->canProcessIdle(); }
		bool				isIdleActive()    									const	{ return getSerial()->isIdleActive(); }
		bool				serialDataAvailable()								const	{ return getSerial()->dataAvailable(); }
		void				setSpyMode(Serial::SypMode sm)								{ getSerial()->setSpyMode(sm); }
		bool				addGuidePath(const CncPathListManager& plm);
		void				updatePreview3D();
		bool 				sendIdleMessage();
		
		int					traceReadBuffer(std::ostream& ostr)					const	{ return getSerial()->traceReadBuffer(ostr); }
		int					peekAndTraceReadBuffer(std::ostream& ostr)					{ return getSerial()->peekAndTraceReadBuffer(ostr); }
		
		// for testing only
		Serial* getSerialExtern() const { wxASSERT(serialPort); return serialPort; }
		
		void				processTrigger(const Trigger::BeginRun& tr)					{ getSerial()->processTrigger(tr); }
		void				processTrigger(const Trigger::EndRun& tr)					{ getSerial()->processTrigger(tr); }
		void				processTrigger(const Trigger::NextPath& tr)					{ getSerial()->processTrigger(tr); }
		void				processTrigger(const Trigger::SpeedChange& tr)				{ getSerial()->processTrigger(tr); }
		void				processTrigger(const Trigger::GuidePath& tr)				{ getSerial()->processTrigger(tr); }
		
		friend class CncTouchBlockDetector;
		
		// -------------------------------------------------------------------------------------------------------------
		class InitializeMove
		{
			CncControl* cnc;
			bool correctPositions;
			
			public:
				explicit InitializeMove(CncControl* c, bool cp = true, bool ee = true)
				: cnc				(c)
				, correctPositions	(cp)
				{
					wxASSERT(cnc);
					cnc->prepareSimpleMove(ee);
				}
				
				~InitializeMove()
				{
					cnc->reconfigureSimpleMove(true);
				}
		};
};

#endif

