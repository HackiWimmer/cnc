#ifndef SVG_PATH_HANDLER_H
#define SVG_PATH_HANDLER_H

#include <vector>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include "CncSvgCurveLib.h"
#include "SerialPort.h"
#include "CncPosition.h"
#include "SVGTransformMatrix.h"
#include "CncToolCorrection.h"
#include "CncWorkingParameters.h"
#include "CncCommon.h"


class CncControl;
class wxDataViewListCtrl;

enum SvgPhProcessMode {SvgPhController, SvgPhDebug};

//////////////////////////////////////////////////////////////////
struct CncPathListInfo {
//////////////////////////////////////////////////////////////////

	// Path postion (CncPathListEntry) entries	
	CncPathList list;
	
	// aditional information
	bool firstPath   		= false;
	bool isCorrected 		= false;
	
	wxRealPoint startPos	= {0, 0}; // absolute start position of the path
	wxRealPoint firstMove	= {0, 0}; // relative x, y values for the first move
	wxRealPoint prevPosAbs 	= {0, 0}; // contains absolote coordinates of the previous(list.size() - 1) position
	
	// ostream operator
	friend std::ostream &operator<< (std::ostream &ostr, const CncPathListInfo &a) {
		ostr << "CncPathListInfo entries: " << a.list.size() << std::endl;
		ostr << " Start Pos:   " 			<< a.startPos.x 	<< ", " << a.startPos.y 	<< std::endl;
		return ostr;
	}
};

//////////////////////////////////////////////////////////////////
class SVGPathHandlerCnc {
//////////////////////////////////////////////////////////////////
	
	private:
		class LastControlPoint {
			private:	
				SVGCurveLib::PointGeneric<> lastQuadraticBezierControlPoint;
				SVGCurveLib::PointGeneric<> lastCubicBezierControlPoint;
				bool lastQuadraticBezierControlPointValid;
				bool lastCubicBezierControlPointValid;
			
			public:
				LastControlPoint() 
				: lastQuadraticBezierControlPoint({0,0})
				, lastCubicBezierControlPoint({0,0})
				, lastQuadraticBezierControlPointValid(false)
				, lastCubicBezierControlPointValid(false)
				{
				}
				virtual ~LastControlPoint() {}
				
				bool hasLastQuadraticBezierControlPoint() 	{ return lastQuadraticBezierControlPointValid; }
				bool hasLastCubicBezierControlPoint() 		{ return lastCubicBezierControlPointValid; }
				
				void setLastQuadraticBezierControlPoint(SVGCurveLib::PointGeneric<> currentPoint, SVGCurveLib::PointGeneric<> p) { 
					lastQuadraticBezierControlPointValid = true;
					//The first control point is assumed to be the reflection of the last control point 
					//on the previous command relative to the current point.  
					lastQuadraticBezierControlPoint = {(currentPoint.x + (currentPoint.x - p.x)),
											           (currentPoint.y + (currentPoint.y - p.y))}; 
				}
				
				void setLastCubicBezierControlPoint(SVGCurveLib::PointGeneric<> currentPoint, SVGCurveLib::PointGeneric<> p) { 
					lastCubicBezierControlPointValid = true;
					//The first control point is assumed to be the reflection of the last control point 
					//on the previous command relative to the current point.  
					lastCubicBezierControlPoint = {(currentPoint.x + (currentPoint.x - p.x)), 
												   (currentPoint.y + (currentPoint.y - p.y))}; 
				}
				
				void reset() { 
					lastQuadraticBezierControlPointValid = false;
					lastCubicBezierControlPointValid = false;
					lastQuadraticBezierControlPoint = {0,0}; 
					lastCubicBezierControlPoint = {0,0};
				}
				
				SVGCurveLib::PointGeneric<> getLastQuadraticBezierControlPoint(SVGCurveLib::PointGeneric<> currentPoint) {
					if ( hasLastQuadraticBezierControlPoint() )
						return lastQuadraticBezierControlPoint;
					//If there is no previous command or if the previous command was not an beziert+ curve, 
					//assume the first control point is coincident with the current point
					return currentPoint;
				}
				
				SVGCurveLib::PointGeneric<> getLastCubicBezierControlPoint(SVGCurveLib::PointGeneric<> currentPoint) {
					if ( hasLastCubicBezierControlPoint() )
						return lastCubicBezierControlPoint;
					//If there is no previous command or if the previous command was not an beziert+ curve, 
					//assume the first control point is coincident with the current point
					return currentPoint;
				}
		};

	private:
	
		SVGUnit 			unit;
		CncControl* 		cncControl;
		SvgPhProcessMode 	processMode;
		SvgOriginalPathInfo origPathInfo;
		double 				toolRadius;
		float 				curveLibResolution;
		bool 				firstPath;
		bool 				newPath;
		bool 				initialized;
		bool 				debugState;
		double 				width, height;
		wxString 			viewBox;
		CncDoublePosition	startPos;
		CncDoublePosition	currentPos;
		wxDataViewListCtrl* debuggerControlDetail;
		
		LastControlPoint lastControlPoint;
		CncPathListInfo pathList;
		SVGTransformMatrix currentSvgTransformMatrix;
		CncWorkingParameters currentCncParameters;
		
		
		// store CncPathList
		bool processLinearMove(bool alreadyRendered);
		// curvel ib helper
		inline bool processCurveLibPoint(SVGCurveLib::PointGeneric<> p);
		
		// spool path to cnc control
		bool moveLinearXY(double x, double y, bool alreadyRendered);
		
		//render functions
		bool processMove(char c, unsigned int count, double values[]);
		bool processLine(char c, unsigned int count, double values[]);
		bool processHLine(char c, unsigned int count, double values[]);
		bool processVLine(char c, unsigned int count, double values[]);
		bool processClose(char c, unsigned int count, double values[]);
		bool processARC(char c, unsigned int count, double values[]);
		bool processQuadraticBezier(char c, unsigned int count, double values[]);
		bool processQuadraticBezierSmooth(char c, unsigned int count, double values[]);
		bool processCubicBezier(char c, unsigned int count, double values[]);
		bool processCubicBezierSmooth(char c, unsigned int count, double values[]);

		// path handling
		bool beginCurrentPath();
		bool repeatCurrentPath();
		inline bool spoolCurrentPathWrapper(bool firstRun);
		bool spoolCurrentPath(bool firstRun);
		bool closeCurrentPath();
		
		// trace functions
		inline void traceCurveLibPoint(const char* prefix, SVGCurveLib::PointGeneric<>& p);
		inline void traceCurrentPosition();
		inline void tracePositions(const char* prefix);
		inline void traceFirstMove(double moveX, double moveY);
		inline void traceFunctionCall(const char* fn);
		
		// debug functions
		inline void appendDebugValueDetail(const char* key, wxVariant value);
		inline void appendDebugValueDetail(CncPathListEntry& cpe);
		inline void debugCurrentPosition();

	public:
		SVGPathHandlerCnc(CncControl* cnc);
		virtual ~SVGPathHandlerCnc();
		
		// Getter and setter
		void setCurveLibResolution(float res) { curveLibResolution = res; }
		void setMaxDimensions(SVGUnit u, double width, double height);
		void setViewBox(const wxString& vb);
		
		SVGUnit getSVGUnit() { return unit; }
		double getW() { return width; }
		double getH() { return height; }
		const char* getViewBox() { return viewBox.c_str(); }

		SVGTransformMatrix& getSvgTransformMatrix() { return currentSvgTransformMatrix; }

		CncWorkingParameters& getCncWorkingParameters();
		void setCncWorkingParameters(CncWorkingParameters& cwp);
		
		void setProcessMode(SvgPhProcessMode pm);
		const SvgPhProcessMode getProcessMode() { return processMode; }
		
		void setDebugState(bool state) { debugState = state; }
		void setDebuggerControl(wxDataViewListCtrl* dcd) { debuggerControlDetail = dcd; }
		
		// path handling
		void prepareWork();
		bool initNextPath(const SvgOriginalPathInfo& sopi);
		bool finishCurrentPath();
		bool runCurrentPath();
		void finishWork();
		
		// processing
		void debugProcess(char c, unsigned int count, double values[]);
		bool process(char c, unsigned int count, double values[]);
		
};

#endif