#ifndef CNC_GL_METRIC_RULER_SETUP_H
#define CNC_GL_METRIC_RULER_SETUP_H

#include "CncPosition.h"
#include "3D/GLContextCncPath.h"

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

#endif