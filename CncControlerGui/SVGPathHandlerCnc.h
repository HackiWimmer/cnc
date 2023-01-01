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
		
		struct MoveParameter
		{
			CncSpeedMode 				mode		= CncSpeedWork;
			CO							idOffset	= CO::MAIN;
			CncDoublePosition			pos			= {0.0, 0.0, 0.0};
		};
		
		CncControl* 			cncControl;
		CncPathListManager* 	guidePath;
		CncDoublePosition		curRunPosition;
		bool					initialized;
		bool					debugState;
		
		bool					moveZAxisToLogicalPos(const MoveParameter& mp);
		bool					moveZAxisToLogicalTop(CncSpeedMode m = CncSpeedWork);
		bool					moveZAxisToSurface();
		bool					moveZAxisNextStepDown(double zTarget);
		bool					moveXYToStartPos(CncSpeedMode m);
		bool					moveXYToPos(const MoveParameter& mp);
		bool					moveXYZToPos(const MoveParameter& mp);
		
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
		virtual void			appendDebugValueDetail(const char* key, wxVariant value)		override;
		virtual void			appendDebugValueDetail(const CncPathListEntry& cpe)				override;
		virtual void			appendDebugValueDetail(const CncCurveLib::ParameterSet& ps)		override;
		
		virtual bool			performModifications();
		
		virtual bool			convertToRightHandCoordSystem()			const					override;
		virtual bool			invertPathArgSweepFlag()				const					override;
		virtual void			processFeedSpeed(CncSpeedMode mode)								override;
		
		virtual bool			isInitialized() 						const					{ return initialized; }
		
	public:
		
		SVGPathHandlerCnc(CncControl* cnc);
		virtual ~SVGPathHandlerCnc();
		
		virtual void			deligateTrigger(const Trigger::BeginRun& tr)			override { processTrigger(tr); }
		virtual void			deligateTrigger(const Trigger::EndRun& tr)				override { processTrigger(tr); }
		virtual void			changePathListRunnerInterface(const wxString& portName)	override { changePathListRunnerInterfaceImpl(portName); }
		
		virtual const char*		getName()					const						override { return "SVGPathHandlerCnc"; }
			
		double					getW() 								{ return svgRootNode.getWidth();  }
		double					getH() 								{ return svgRootNode.getHeight(); }
		const char*				getViewBox() 						{ return svgRootNode.getViewbox().getViewBoxStr().c_str(); }
		const SVGRootNode&		getSvgRootNode()					{ return svgRootNode; }
		
		void					setDebugState(bool state)			{ debugState = state; }
		
		virtual void			initNextClientId(long clientId)				override;
		virtual bool			activateNextPath(long clientId)				override;
		
		virtual void			setSvgRootNode(const SVGRootNode& srn)		override;
		virtual void			logMeasurementStart()						override;
		virtual void			logMeasurementEnd()							override;

		// path handling
		virtual bool			prepareWork()								override;
		virtual bool			initToolChange(int id)						override;
		virtual bool			finishCurrentPath()							override;
		virtual bool			runCurrentPath()							override;
		virtual bool			finishWork()								override;
		
		virtual void			traceWorkflow(std::ostream& o)				override { CncPathListRunner::traceWorkflow(o); }
		virtual void			resetWorkflow()								override { CncPathListRunner::resetWorkflow(); }
		virtual bool			spoolWorkflow()								override { return CncPathListRunner::spoolWorkflow(); }
};

#endif
