#ifndef SVG_PATH_HANDLER_CNC_H
#define SVG_PATH_HANDLER_CNC_H

#include "SVGPathHandlerBase.h"
#include "SerialPort.h"
#include "CncUnitCalculator.h"
#include "CncPathListRunner.h"
#include "CncCommon.h"

class CncControl;

//////////////////////////////////////////////////////////////////
class SVGPathHandlerCnc : public SVGPathHandlerBase
						, public CncPathListRunner
{
	private:
		
		typedef CncUnitCalculatorBase::Unit Unit;
		typedef SvgCncContextBase::CLIENT_ID_OFFSET CO;
		
		struct MoveParameter {
			CncSpeedMode 				mode		= CncSpeedWork;
			CO							idOffset	= CO::MAIN;
			bool						zToTop		= true;
			const CncDoublePosition*	pos			= NULL;
		};
		
		CncControl* 			cncControl;
		CncPathListManager* 	guidePath;
		CncDoublePosition		curRunPosition;
		bool					initialized;
		bool					debugState;

		// 
		bool					moveZAxisToLogicalTop();
		bool					moveZAxisToSurface();
		bool					moveZAxisNextStepDown(double zTarget);
		bool					moveXYToStartPos(CncSpeedMode m);
		bool					moveXYToPos(const MoveParameter& mp);
		
		bool					hasMoreDurations(double zTarget) const;
		
		// path handling
		bool					repeatCurrentPath(double zTarget);
		bool					processHelix(double zTarget);
		
		bool					isXYEqual(const CncDoublePosition& p1, const CncDoublePosition& p2) const;
		void					updateXY (const CncDoublePosition& p1, CncDoublePosition& p2) const;
		void					updateZ  (const CncDoublePosition& p1, CncDoublePosition& p2) const;
		
		void					resetGuidePath();
		void					registerGuidePath(CncPathListManager* gp);
		
	protected:
	
		// debug functions
		virtual void			appendDebugValueDetail(const char* key, wxVariant value);
		virtual void			appendDebugValueDetail(const CncPathListEntry& cpe);
		virtual void			appendDebugValueDetail(const CncCurveLib::ParameterSet& ps);
		
		virtual bool			isInitialized() 			const 	{ return initialized; }
		virtual bool			performModifications();
		
		virtual bool			convertToRightHandCoordSystem() const;
		virtual bool			invertPathArgSweepFlag() const;
		virtual void			processFeedSpeed(CncSpeedMode mode);
		
	public:
		
		SVGPathHandlerCnc(CncControl* cnc);
		virtual ~SVGPathHandlerCnc();
		
		virtual void			deligateTrigger(const Trigger::BeginRun& tr)			{ processTrigger(tr); }
		virtual void			deligateTrigger(const Trigger::EndRun& tr)				{ processTrigger(tr); }
		virtual void			changePathListRunnerInterface(const wxString& portName)	{ changePathListRunnerInterfaceImpl(portName); }
		
		virtual const char*		getName()					const	{ return "SVGPathHandlerCnc"; }
			
		double					getW() 								{ return svgRootNode.getWidth();      }
		double					getH() 								{ return svgRootNode.getHeight();     }
		const char*				getViewBox() 						{ return svgRootNode.getViewbox().getViewBoxStr().c_str(); }
		const SVGRootNode&		getSvgRootNode()					{ return svgRootNode; }
		
		void					setDebugState(bool state)			{ debugState = state; }
		
		virtual void			initNextClientId(long clientId);
		virtual bool			activateNextPath(long clientId);
		
		virtual void			setSvgRootNode(const SVGRootNode& srn);
		virtual void			logMeasurementStart();
		virtual void			logMeasurementEnd();

		// path handling
		virtual bool			prepareWork();
		virtual bool			finishCurrentPath();
		virtual bool			runCurrentPath();
		virtual bool			finishWork();
};

#endif
