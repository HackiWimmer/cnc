#ifndef GCODE_PATH_HANDLER_GL_H
#define GCODE_PATH_HANDLER_GL_H

#include "GCodePathHandlerBase.h"
#include "3D/CncGCodePreview.h"
#include "CncPosition.h"

class GCodePathHandlerGL : public GCodePathHandlerBase {
	
	public:
		GCodePathHandlerGL(CncGCodePreview* gl);
		virtual ~GCodePathHandlerGL();
		
		virtual const char*		getName() 							const	{ return "GCodePathHandlerGL"; }
		
		virtual bool			isPathListUsed() 					const	{ return false; }
		virtual bool			shouldAToolChangeProcessed()				{ return false; }
		
		virtual void			switchSpindleState(bool state)				{}
		virtual void			logMeasurementStart()						{}
		virtual void			logMeasurementEnd()							{}

	protected:
		
		virtual bool			processLinearMove(bool alreadyRendered);
		virtual bool			changeCurrentFeedSpeedXYZ(CncSpeedMode sm, double value = 0.0);
		virtual bool			changeCurrentSpindleSpeed(double value = 0.0);
		virtual bool			initNextPath();
		virtual bool			prepareWorkImpl();
		virtual bool			finishWorkImpl();
		
		virtual void			resetWorkflow()	{ }
		virtual bool			spoolWorkflow()	{ return true; }
		
	private:
		
		CncGCodePreview*		glControl;
		CncSpeedMode			currentSpeedMode;
};

#endif