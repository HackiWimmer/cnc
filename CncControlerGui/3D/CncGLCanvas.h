#ifndef CNC_GL_CANVAS_H
#define CNC_GL_CANVAS_H

#include "3D/GLContextCncPath.h"
#include "CncPosition.h"

#include "3D/GLInclude.h"

///////////////////////////////////////////////////
class CncMetricRulerSetup {
	
	public:
		CncMetricRulerSetup();
		~CncMetricRulerSetup();
		
		void setupSize(double xDim, double yDim, double zDim);
		void setupOrigin(const CncDoublePosition& o);
		
		void createRulerX(GLI::GLAxisRuler& ruler);
		void createRulerY(GLI::GLAxisRuler& ruler);
		void createRulerZ(GLI::GLAxisRuler& ruler);
		
		void createHelpLinesXY(GLI::GLLineCluster& xLines, GLI::GLLineCluster& yLines);
		void createHelpLinesXZ(GLI::GLLineCluster& xLines, GLI::GLLineCluster& zLines);
		void createHelpLinesYZ(GLI::GLLineCluster& yLines, GLI::GLLineCluster& zLines);
		
		bool check(GLContextCncPathBase* monitor, std::ostream& out);
		void trace(GLContextCncPathBase* monitor, std::ostream& out);
	
	private:
		
		const double oMajor = 0.4;
		const double oMinor = 0.3;
		const double oScann = 0.2;
		
		double majorScanning;
		double minorScanning;
		
		CncDoublePosition origin;
		
		float factorX;
		float factorY;
		float factorZ;
		
		double minX;
		double maxX;
		double minY;
		double maxY;
		double minZ;
		double maxZ;
		
		inline void addLine(GLI::GLLineCluster& lines, double x1, double x2, double y1, double y2, double z1, double z2);
		inline void addLabel(GLI::GLLabelCluster& labels, double x, double y, double z, void* font, const wxString label);
		
		inline float cnvX(double val) { return val * factorX; }
		inline float cnvY(double val) { return val * factorY; }
		inline float cnvZ(double val) { return val * factorZ; }
};

///////////////////////////////////////////////////
class CncGlCanvas : public wxGLCanvas {
		
	public:
		CncGlCanvas(wxWindow *parent, int *attribList = NULL);
		virtual ~CncGlCanvas();
		
		void viewTop() 		{ view(GLContextBase::ViewMode::V2D_TOP); }
		void viewBottom() 	{ view(GLContextBase::ViewMode::V2D_BOTTOM); }
		void viewLeft() 	{ view(GLContextBase::ViewMode::V2D_LEFT); }
		void viewRight()	{ view(GLContextBase::ViewMode::V2D_RIGHT); }
		void viewFront() 	{ view(GLContextBase::ViewMode::V2D_FRONT); }
		void viewRear() 	{ view(GLContextBase::ViewMode::V2D_REAR); }
		
		void viewIso1() 	{ view(GLContextBase::ViewMode::V3D_ISO1); }
		void viewIso2() 	{ view(GLContextBase::ViewMode::V3D_ISO2); }
		void viewIso3() 	{ view(GLContextBase::ViewMode::V3D_ISO3); }
		void viewIso4() 	{ view(GLContextBase::ViewMode::V3D_ISO4); }
		
		void incScale();
		void decScale();
		
		void initReshape();
		void reshapeRelative(int dx, int dy);
		
		bool isContextValid()	{ return lastSetCurrent; }
		
	protected:
		GLContextCncPathBase* 	context;
		wxPoint 				lastReshape;
		bool					lastSetCurrent;
		bool 					mouseMoveMode;
		
		void onMouse(wxMouseEvent& event);
		void view(GLContextBase::ViewMode fm);
};
	
#endif