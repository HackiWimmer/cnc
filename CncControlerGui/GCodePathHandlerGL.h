#ifndef GCODE_PATH_HANDLER_GL_H
#define GCODE_PATH_HANDLER_GL_H

#include "GCodePathHandlerBase.h"
#include "3D/CncGCodePreview.h"
#include "CncPosition.h"

class GCodePathHandlerGL : public GCodePathHandlerBase {
	
	public:
		GCodePathHandlerGL(CncGCodePreview* gl);
		virtual ~GCodePathHandlerGL();
		
		virtual const char*		getName() 							const					override { return "GCodePathHandlerGL"; }
		
		virtual bool			isPathListUsed() 					const					override { return false; }
		virtual bool			shouldAToolChangeProcessed()								override { return false; }
		
		virtual void			switchSpindleState(CncSpindlePowerState state)				override {}
		virtual void			logMeasurementStart()										override {}
		virtual void			logMeasurementEnd()											override {}

	protected:
		
		virtual bool			processLinearMove(bool alreadyRendered);
		virtual bool			changeCurrentFeedSpeedXYZ(CncSpeedMode sm);
		virtual bool			changeCurrentFeedSpeedXYZ(CncSpeedMode sm, double value)	override;
		virtual bool			changeCurrentSpindleSpeed(double value = 0.0)				override;
		virtual bool			initNextPath()												override;
		virtual bool			prepareWorkImpl()											override;
		virtual bool			finishWorkImpl()											override;
		
		virtual bool			moveToParkPosition()										override { }
		virtual void			traceWorkflow(std::ostream& o)								override { }
		virtual void			resetWorkflow()												override { }
		virtual bool			spoolWorkflow()												override { return true; }
		
	private:
		
		CncGCodePreview*		glControl;
		CncSpeedMode			currentSpeedMode;
};

#endif