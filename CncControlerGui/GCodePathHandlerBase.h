#ifndef GCODE_PATH_HANDLER_BASE_H
#define GCODE_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"
#include "GCodeBlock.h"

class GCodePathHandlerBase : public PathHandlerBase {
	
	public:
		enum CutterCompensation { CC_OFF, CC_STATIC_LEFT, CC_STATIC_RIGHT, CC_DYNAMIC_LEFT, CC_DYNAMIC_RIGHT };
		
		GCodePathHandlerBase();
		virtual ~GCodePathHandlerBase();
		
		virtual const char* getName() 													const	{ return "GCodePathHandlerBase"; }
		
		GCodePathHandlerBase::CutterCompensation getCutterCompensationMode() 			const	{ return cutterCompMode; }
		void setCutterCompensationMode(GCodePathHandlerBase::CutterCompensation ccm)			{ cutterCompMode = ccm; }
		
		int					getToolLengthOffsetId()										const	{ return toolLengthOffsetId; }
		void				setToolLengthOffsetId(int tloi)										{ toolLengthOffsetId = tloi; }
		
		virtual bool		isPathListUsed()								const	= 0;
		virtual void		switchSpindleState(CncSpindlePowerState state)			= 0;
		virtual bool		initNextPath()											= 0;
		virtual bool		moveToParkPosition()									= 0;
		virtual bool		prepareWork() override; 
		virtual bool		finishWork() override; 
		
		bool				processRapidLinearMove(GCodeBlock& gcb);
		bool				processLinearMove(GCodeBlock& gcb);
		bool				processArcMove(GCodeBlock& gcb, bool sweep);
		bool				processDwell(GCodeBlock& gcb);
		bool				moveToOrigin(GCodeBlock& gcb);
		bool				processParameterEFS(GCodeBlock& gcb);
		
	protected:
		
		virtual bool		processDwellIntern(int64_t microseconds)				{ return true; }
		virtual bool		processLinearMove(bool alreadyRendered)					= 0;
		virtual bool		changeCurrentFeedSpeedXYZ(CncSpeedMode s)				= 0;
		virtual bool		changeCurrentFeedSpeedXYZ(CncSpeedMode s, double value)	= 0;
		virtual bool		changeCurrentSpindleSpeed(double value = 0.0)			= 0;
		virtual bool		prepareWorkImpl()										= 0;
		virtual bool		finishWorkImpl()										= 0;
		
	private:
		
		CutterCompensation	cutterCompMode;
		int 				toolLengthOffsetId;
		
		bool				updateCurrentPosition(GCodeBlock& gcb);
};

#endif