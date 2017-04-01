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
#include "CncPositionMarker.h"
#include "PenHandler.h"
#include "GuiControlSetup.h"
#include "CncConfig.h"
#include "SerialPort.h"
#include "CncLimitStates.h"
#include "CoordinateSystem.h"
#include "CncPosition.h"

const double endSwitchStepBackMertic = 2.5;

///////////////////////////////////////////////////////////////////
struct PositionInfo3D {
	CncLongPosition lp;
	CncLongPosition cp;
	bool zAxisDown = false;
};

typedef std::vector<PositionInfo3D> DrawPoints3D;

///////////////////////////////////////////////////////////////////
struct PointPair {
	wxPoint lp;
	wxPoint cp;
	bool zAxisDown;
	
	//////////////////////////////////////////////////////////////////
	static PointPair& processOffset(PointPair& ret, int x, int y) {
		ret.lp.x += x;
		ret.lp.y += y;
		ret.cp.x += x;
		ret.cp.y += y;
		return ret;
	}
	
	//////////////////////////////////////////////////////////////////
	static PointPair& processFactor(PointPair& ret, double x, double y) {
		ret.lp.x *= x;
		ret.lp.y *= y;
		ret.cp.x *= x;
		ret.cp.y *= y;
		return ret;
	}

	//////////////////////////////////////////////////////////////////
	friend std::ostream &operator<< (std::ostream &ostr, const PointPair &a) {
		ostr << "lp:"<< a.lp.x << "," << a.lp.y << "\t\tcp:" << a.cp.x << "," << a.cp.y << "\tz down:" << a.zAxisDown << std::endl;
		return ostr;
	}
	//////////////////////////////////////////////////////////////////i
	friend bool operator== (const PointPair &a, const PointPair &b) {
		return  (   (a.lp == b.lp)
				 && (a.cp == b.cp)
				 && (a.zAxisDown == b.zAxisDown)
				);
	}
	//////////////////////////////////////////////////////////////////i
	friend bool operator!= (const PointPair &a, const PointPair &b) {
		return (!operator== (a, b));
	}
};

typedef std::vector<PointPair> DrawPoints;

///////////////////////////////////////////////////////////////////
struct SetterTuple {
	unsigned char 	id;
	int32_t			value;
	
	SetterTuple(unsigned char i, int32_t v)
	: id(i)
	, value(v)
	{}
};

///////////////////////////////////////////////////////////////////
class CncControl {
	public:
		enum MontionMoinorMode {MMM_2D, MMM_3D};
		
	private:
		wxString stringTemplate;
		
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
		CncLongPosition curPos;
		// Stores the lateset requested control positions
		CncLongPosition curCtlPos;
		// Stores the lateset requested control positions
		CncLongPosition controllerPos;
		// Stores the previous draw point
		wxPoint lastDrawPoint;
		CncLongPosition lastDrawPoint3D;
		// Render mode
		CncRenderMode renderMode;
		// Duration counter
		unsigned int durationCounter;
		// Interrupt stae
		bool interruptState;
		// power state
		bool powerOn;
		// Z Axis state
		bool zAxisDown;
		// flag for updating tools
		bool toolUpdateState;
		// flag for showing drawPane grid
		bool showGrid;
		// Artificially Step Delay
		unsigned int stepDelay;
		// output controls
		GuiControlSetup* guiCtlSetup;
		// Tool state handling
		CncToolStateControl toolState;
		//measurements variables
		struct timeb endTime, startTime;
		// command counter
		int32_t commandCounter;
		// Flag to indicatate if a positions check aplies
		bool positionCheck;
		// Draw control
		wxWindow* drawControl;
		DrawPoints drawPoints;
		// 3D draw control
		DrawPoints3D drawPoints3D;
		// Pen handler
		PenHandler penHandler;
		CncPositionMarker posMarker;
		// margin of the draw pane
		CoordinateSystem drawPaneCoordSystem;
		unsigned int drawPaneMargin;
		// determien the preview mode [2D, 3D]
		MontionMoinorMode motionMonitorMode;
		// helper
		double convertToDisplayUnit(int32_t val, double fact);
		//sets a value into the given text control
		void setValue(wxTextCtrl *ctl, int32_t val);
		void setValue(wxTextCtrl *ctl, double val);
		void setValue(wxTextCtrl *ctl, const char* t);

		// Draw pane helper
		void drawText(wxPaintDC& dc, wxString text, wxPoint pt);

		// Tool management
		void setToolState(bool defaultStyle = false);
		
		// Limit management
		CncLimitStates limitStates;
		void displayLimitState(wxStaticText* ctl, bool value);
		void evaluateLimitState(long x, long y, long z);
		
		//simple move
		bool prepareSimpleMove(bool enaleEventHandling = true);
		void reconfigureSimpleMove(bool correctPositions);
		
		// draw point conversion
		void updateDrawPointFactors();
		void initLastDrawPoint();
		inline long convertToDrawPointX(long val);
		inline long convertToDrawPointY(long val);
		
		void drawGrid(wxPaintDC& dc, double fact); // Only for WM_PAINT event
		void drawOrigin(wxPaintDC& dc); // Only for WM_PAINT event
		
		void traceDrawPaneIntern(wxTextCtrl* ctl, std::ostream& os);
		
		// secial convesion to transfer a double as long
		long convertDoubleToCtrlLong(unsigned char 	id, double d);
		
	public:
		CncControl(CncPortType pt);
		virtual ~CncControl();
		//Get the current port type
		CncPortType getPortType(void) { return portType; }
		// Connection to portName
		bool connect(const char * portName);
		// Check the connection
		bool isConnected();
		//Make Serial available
		Serial* getSerial() { return serialPort; }
		//Make cnc config available
		CncConfig* getCncConfig() { return cncConfig; }
		void updateCncConfig(CncConfig& cc);
		// wrapper
		bool processCommand(const char* cmd, std::ostream& txtCtl);
		// wrapper
		bool processCommand(const unsigned char c, std::ostream& txtCtl);
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
		bool moveStepsZ(int32_t z);
		bool moveMetricZ(double z);
		bool moveLinearStepsXY(int32_t x1, int32_t y1, bool alreadyRendered);
		bool moveLinearStepsXYZ(int32_t x1, int32_t y1, int32_t z1, bool alreadyRendered);
		bool moveLinearMetricXY(double x1, double y1, bool alreadyRendered);
		bool moveLinearMetricXYZ(double x1, double y1, double z1, bool alreadyRendered);
		// Callback from Serial
		bool SerialCallback(int32_t cmdCount);
		// Callback from Serial with controller content
		bool SerialControllerCallback(const ContollerInfo& ci);
		// Callback fromS Serial with a controller message
		bool SerialMessageCallback(const ControllerMsgInfo& cmi);
		// wait an dispath events
		void waitActive(unsigned int milliseconds, bool once = true);
		//interrupt the processing
		void interrupt();
		void resetInterrupt();
		bool isInterrupted();
		bool reset();
		bool resetWatermarks();
		// Setup the cnc control
		void setup(bool reset = true);
		// Measurements funtions
		void logProcessingStart();
		void logProcessingCurrent();
		void logProcessingEnd(bool valuesOnly = false);
		// Sets the output controls for cooridinate infos
		void setGuiControls(GuiControlSetup* guiCtlSetup);
		void setDrawControl(wxWindow* dc) { drawControl = dc; }
		void setCoordinateSystemType(CoordinateSytemType t);
		//handle draw control
		void clearDrawControl();
		void updateDrawControl();
		void reconstructDrawControl(int oldCorrectionY, long oldNpX, long oldNpY);
		void reconstructDrawControl(double fact, double factOld);
		void redrawDrawPane(double fact); // Only for WM_PAINT event
		wxPoint& convertToCoordiateSystem(wxPoint& in);
		unsigned int traceDrawPane(wxTextCtrl* ctl);
		// Duration management
		unsigned int getDurationCount();
		unsigned int getDurationCounter();
		void resetDurationCounter();
		void initNextDuration();
		bool hasNextDuration();
		bool isFirstDuration();
		bool isLastDuration();
		// Z axis management
		bool moveUpZ();
		bool moveDownZ();
		bool moveZToTop();
		bool moveZToBottom();
		bool isZAxisUp()         { return !zAxisDown; }
		bool isZAxisDown()       { return zAxisDown; }
		void simulateZAxisUp()   { zAxisDown = false; }
		void simulateZAxisDown() { zAxisDown = true; }
		// Tool management
		void switchToolOn();
		void switchToolOff(bool force = false);
		bool getToolState() { return powerOn; }	
		// Updates the config trace control
		void updateCncConfigTrace();
		// validating PC pos due to the given reference coordinates
		bool validateCurrentPostion();
		// returns the correponding pc postions
		const CncLongPosition getStartPos() { return startPos; }
		const CncLongPosition getCurPos()   { return curPos; }
		const CncDoublePosition getStartPosMetric();
		const CncDoublePosition getCurPosMetric();
		const CncLongPosition getMinPositions();
		const CncDoublePosition getMinPositionsMetric();
		const CncLongPosition getMaxPositions();
		const CncDoublePosition getMaxPositionsMetric();
		const CncLongPosition::Watermarks getWaterMarks();
		const CncDoublePosition::Watermarks getWaterMarksMetric();
		// query the current error count
		const int32_t getControllerErrorCount();
		// query the current controller position
		const CncLongPosition getControllerPos();
		// query the current controller limit state
		const CncLongPosition getControllerLimitState();
		const CncLimitStates& getLimitState() { return limitStates; }
		// validates pc and controller positions
		bool validatePositions();
		void forceDisplayPositions();
		// processing the given setter values
		bool processSetter(unsigned char id, int32_t value);
		bool processSetterList(std::vector<SetterTuple>& setup);
		// Change the current work speed parameter
		void changeWorkSpeedXY(CncSpeed s);
		void changeWorkSpeedZ(CncSpeed s);
		// Sets a flag that the postions x/y min/max should be checked within the Serial callback
		void activatePositionCheck(bool a) { positionCheck = a; }
		// Sets the enable pin HIGH (s == false) or LOW ( s == true)
		void enableStepperMotors(bool s);
		// Controls the z slider
		void resetZSlider();
		void updateZSlider();
		// zoom handling
		void setDrawPaneZoomFactor(double f);
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
		
		// contorler configuration output
		bool hasControllerConfigControl();
		void clearControllerConfigControl();
		void appendPidKeyValueToControllerConfig(int pid, const char* key, const char* value);
		
		// controller error output
		bool hasControllerErrorControl();
		void clearControllerErrorControl();
		void appendNumKeyValueToControllerErrorInfo(int num, int code, const char* key, const char* value);
		
		// controller pin report
		bool hasControllerPinControl();
		void clearControllerPinControl();
		void appendNumKeyValueToControllerErrorInfo(const char* desc, int pin, int type, int mode, int value);
		
		void setUpdateToolControlsState(bool state) { toolUpdateState = state; }
		void setShowGridSate(bool show) { showGrid = show; updateDrawControl(); }
		
		void setStepDelay(unsigned int d) { stepDelay = d; }
		unsigned int getStepDelay() { return stepDelay; }
		
		//meassurments
		bool meassureDimension(const char axis, wxCheckBox* min, wxCheckBox* max, double& result);
		bool meassureXDimension(wxCheckBox* min, wxCheckBox* max, double& result) { return meassureDimension('X', min, max, result); }
		bool meassureYDimension(wxCheckBox* min, wxCheckBox* max, double& result) { return meassureDimension('Y', min, max, result); }
		bool meassureZDimension(wxCheckBox* min, wxCheckBox* max, double& result) { return meassureDimension('Z', min, max, result); }
		
		// 3D control
		void set3DData(bool append);
		
		// idle handling
		void sendIdleMessage();
		
		// Motion monitor handling
		void setMotionMonitorMode(const MontionMoinorMode& mmm);
		const MontionMoinorMode& getMotionMonitorMode() const   { return motionMonitorMode; }
		
		// Marker repaint
		void drawXMarkerBottom(wxDC& dc);
		void drawXMarkerTop(wxDC& dc);
		void drawYMarker(wxDC& dc);
};

#endif