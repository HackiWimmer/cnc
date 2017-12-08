#ifndef GCODE_PATH_HANDLER_GL_H
#define GCODE_PATH_HANDLER_GL_H

#include "GCodePathHandlerBase.h"
#include "3D/CncGCodePreview.h"
#include "CncPosition.h"

class GCodePathHandlerGL : public GCodePathHandlerBase {
	
	public:
		GCodePathHandlerGL(CncGCodePreview* gl);
		virtual ~GCodePathHandlerGL();
		
		virtual void switchToolState(bool state) {}
		
	protected:
		
		virtual bool processLinearMove(bool alreadyRendered);
		virtual bool changeCurrentFeedSpeedXYZ(CncSpeed s, double value = 0.0);
		virtual void prepareWorkImpl();
		virtual void finishWorkImpl();
		
		virtual void logMeasurementStart() {}
		virtual void logMeasurementEnd() {}
		
	private:
		CncGCodePreview* glControl;
		CncSpeed currentSpeed;
};

#endif