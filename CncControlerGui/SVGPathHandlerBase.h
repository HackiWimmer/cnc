#ifndef SVG_PATH_HANDLER_BASE_H
#define SVG_PATH_HANDLER_BASE_H

#include <vector>
#include <wx/variant.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include "CncSvgCurveLib.h"
#include "CncPosition.h"
#include "CncPathListEntry.h"

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
class SVGPathHandlerBase {
//////////////////////////////////////////////////////////////////

	protected:
	
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

		bool 				firstPath;
		bool 				newPath;
		CncDoublePosition	startPos;
		CncDoublePosition	currentPos;
		float 				curveLibResolution;
		LastControlPoint 	lastControlPoint;
		CncPathListInfo pathList;
		
		// trace functions
		void traceCurveLibPoint(const char* prefix, SVGCurveLib::PointGeneric<>& p);
		void traceFunctionCall(const char* fn);
		void traceCurrentPosition();
		void tracePositions(const char* prefix);
		void traceFirstMove(double moveX, double moveY);
		
		// curvel lib helper
		virtual bool processCurveLibPoint(SVGCurveLib::PointGeneric<> p);
		
		// processor
		virtual bool processLinearMove(bool alreadyRendered);
		
		// debug functions
		virtual void appendDebugValueDetail(const char* key, wxVariant value);
		virtual void appendDebugValueDetail(CncPathListEntry& cpe);
		virtual void debugCurrentPosition();
		
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
		
		virtual bool isInitialized();
		
		// controller helper
		virtual void simulateZAxisUp();
		virtual void simulateZAxisDown();
		
	public:
		SVGPathHandlerBase();
		virtual ~SVGPathHandlerBase();
		
		void setCurveLibResolution(float res) { curveLibResolution = res; }
		
		// processing
		void debugProcess(char c, unsigned int count, double values[]);
		bool process(char c, unsigned int count, double values[]);
	
};

#endif