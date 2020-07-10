#ifndef CNCSPEEDPLAYGROUND_H
#define CNCSPEEDPLAYGROUND_H

#include "CncSpeedAccelerationDiagram.h"
#include "CncFloatingPointValidator.h"
#include "wxCrafterSpeedMonitor.h"

#include "../Arduino/StepperEnvironment/ArdoEnv.h"
#include "../Arduino/StepperEnvironment/ArdoObj.h"
#include "../Arduino/StepperEnvironment/ArdoVal.h"
#include "../Arduino/StepperEnvironment/CncRndr.h"
#include "../Arduino/StepperEnvironment/CncAcmr.h"


class CncTextCtrl;
class StdStreamRedirector;
class CncSpeedPlayground 	: public CncSpeedPlaygroundBase 
							, public ArduinoAccelManager
							, public ArduinoPositionRenderer
{
	
	public:
		CncSpeedPlayground(wxWindow* parent);
		virtual ~CncSpeedPlayground();
		
		void appendArdoMessage(char type, const wxString& msg, const wxString& context=wxEmptyString);
		
	protected:
		virtual void enableSpecialSpeedDisplayHeight(wxCommandEvent& event);
		virtual void updateSpecialSpeedDisplayHeightValue(wxCommandEvent& event);
		virtual void updateTriggerF1(wxCommandEvent& event);
		virtual void updateTriggerF2(wxCommandEvent& event);
		virtual void updateTriggerF3(wxCommandEvent& event);
		virtual void updateTriggerI1(wxCommandEvent& event);
		virtual void updateTriggerI2(wxCommandEvent& event);
		virtual void updateTriggerI3(wxCommandEvent& event);
		virtual void enableSpecialAccelDisplayWidth(wxCommandEvent& event);
		virtual void updateSpecialAccelDisplayWidthValue(wxCommandEvent& event);
		virtual void onRun(wxCommandEvent& event);
		virtual void onClearLogger(wxCommandEvent& event);
		virtual void onChangeDisplayType(wxCommandEvent& event);
		virtual void onValKeyDown(wxKeyEvent& event);
		virtual void onTimer(wxTimerEvent& event);
		virtual void onActivate(wxActivateEvent& event);
		virtual void changeSliderF(wxScrollEvent& event);
		virtual void changeSliderX(wxScrollEvent& event);
		virtual void changeSliderY(wxScrollEvent& event);
		virtual void changeSliderZ(wxScrollEvent& event);
		virtual void updateValueF(wxCommandEvent& event);
		virtual void updateValueX(wxCommandEvent& event);
		virtual void updateValueY(wxCommandEvent& event);
		virtual void updateValueZ(wxCommandEvent& event);
		
		// -----------------------------------------------------------------------------------
		// ArduinoAccelManager interface
		virtual void notifyACMStateChange(State s);
		virtual void notifyACMInitMove();
		
		// -----------------------------------------------------------------------------------
		// ArduinoPositionRenderer interface
		virtual byte setDirection(AxisId aid, int32_t steps)	{ return RET_OK; }
		virtual byte checkRuntimeEnv()  						{ return RET_OK; }

		virtual void notifyMovePart (int8_t dx, int8_t dy, int8_t dz);
		virtual byte performNextStep(AxisId aid);
		virtual byte performStep    (AxisId aid);
		virtual byte initiateStep   (AxisId aid);
		virtual byte finalizeStep   (AxisId aid);
		
	private:
	
		struct TestData {
			
			struct Ramp {
				uint32_t xSteps 	= 0;
				uint32_t ySteps 	= 0;
				uint32_t zSteps 	= 0;
				uint32_t impulses 	= 0;
			};
			
			uint32_t 	totalImpulseCounter = 0;
			Ramp		A;
			Ramp		D;
			
			void reset() { *this = TestData(); }
		};
		
		
		enum DisplayTypeH { DTH_IMPULSE };
		enum DisplayTypeV { DTV_MM_MIN, DTV_MM_SEC };
		
		DisplayTypeH					displayTypeH;
		DisplayTypeV					displayTypeV;
		
		CncFloatingPointValidator		valF;
		CncFloatingPointValidator		valX;
		CncFloatingPointValidator		valY;
		CncFloatingPointValidator		valZ;
		
		uint32_t						minDA;
		uint32_t						maxDA;
		uint32_t						minFA;
		uint32_t						maxFA;
		
		uint32_t						minDD;
		uint32_t						maxDD;
		uint32_t						minFD;
		uint32_t						maxFD;

		TestData						testData;
		
		CncTextCtrl* 					logger;
		StdStreamRedirector* 			streamRedirector;
		CncSpeedAccelerationDiagram*	accelerationDiagram;
		CncSpeedAccelerationDiagram*	deaccelerationDiagram;
		
		CncSpeedAccelerationDiagram::Graph* graphA;
		CncSpeedAccelerationDiagram::Graph* graphD;
		
		bool determineDisplayType();
		void updateAxisListControl(wxListCtrl* ctrl, const char* values) { updateListControl(ctrl, 3, values); }
		void updateTestListControl(wxListCtrl* ctrl, const char* values) { updateListControl(ctrl, 3, values); }
		void updateListControl(wxListCtrl* ctrl, int columns, const char* values);

		void runFromGui();
		void run();
};

#endif // CNCSPEEDPLAYGROUND_H
