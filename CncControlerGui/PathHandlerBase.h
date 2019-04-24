#ifndef PATH_HANDLER_BASE_H
#define PATH_HANDLER_BASE_H

#include <vector>
#include <wx/variant.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include "CncPosition.h"
#include "CncUnitCalculator.h"
#include "CncCurveLib.h"
#include "CncPathListManager.h"

#define TRACE_FUNCTION_CALL(fn)
#define TRACE_FIRST_MOVE(x,y)
#define TRACE_POSITIONS(p)
#define TRACE_CURRENT_POSITION

#define ENABLE_TRACE_FUNCTIONS
#ifdef ENABLE_TRACE_FUNCTIONS
	
	#undef  TRACE_FUNCTION_CALL
	#define TRACE_FUNCTION_CALL(fn) traceFunctionCall(fn);
	
	#undef  TRACE_FIRST_MOVE
	#define TRACE_FIRST_MOVE(x,y) traceFirstMove(x,y);
	
	#undef  TRACE_POSITIONS
	#define TRACE_POSITIONS(p) tracePositions(p);
	
	#undef  TRACE_CURRENT_POSITION
	#define TRACE_CURRENT_POSITION traceCurrentPosition();

#endif

class FileParser;

//////////////////////////////////////////////////////////////////
class PathHandlerBase : public CncCurveLib::Caller {
//////////////////////////////////////////////////////////////////
	public:
		typedef CncUnitCalculatorBase::Unit Unit;

	protected:
	
		class LastControlPoint {
			private:	
				CncCurveLib::Point lastQuadraticBezierControlPoint;
				CncCurveLib::Point lastCubicBezierControlPoint;
				bool lastQuadraticBezierControlPointValid;
				bool lastCubicBezierControlPointValid;
			
			public:
				LastControlPoint() 
				: lastQuadraticBezierControlPoint(0.0, 0.0)
				, lastCubicBezierControlPoint(0.0, 0.0)
				, lastQuadraticBezierControlPointValid(false)
				, lastCubicBezierControlPointValid(false)
				{}
				~LastControlPoint() {}
				
				bool hasLastQuadraticBezierControlPoint() 	{ return lastQuadraticBezierControlPointValid; }
				bool hasLastCubicBezierControlPoint() 		{ return lastCubicBezierControlPointValid; }
				
				void setLastQuadraticBezierControlPoint(const CncCurveLib::Point& currentPoint, const CncCurveLib::Point& p) { 
					lastQuadraticBezierControlPointValid = true;
					//The first control point is assumed to be the reflection of the last control point 
					//on the previous command relative to the current point.  
					lastQuadraticBezierControlPoint = {(currentPoint.x + (currentPoint.x - p.x)),
											           (currentPoint.y + (currentPoint.y - p.y))}; 
				}
				
				void setLastCubicBezierControlPoint(const CncCurveLib::Point& currentPoint, const CncCurveLib::Point& p) { 
					lastCubicBezierControlPointValid = true;
					//The first control point is assumed to be the reflection of the last control point 
					//on the previous command relative to the current point.  
					lastCubicBezierControlPoint = {(currentPoint.x + (currentPoint.x - p.x)), 
												   (currentPoint.y + (currentPoint.y - p.y))}; 
				}
				
				void reset() { 
					lastQuadraticBezierControlPointValid 	= false;
					lastCubicBezierControlPointValid 		= false;
					lastQuadraticBezierControlPoint 		= {0.0, 0.0}; 
					lastCubicBezierControlPoint 			= {0.0, 0.0};
				}
				
				const CncCurveLib::Point& getLastQuadraticBezierControlPoint(const CncCurveLib::Point& currentPoint) {
					if ( hasLastQuadraticBezierControlPoint() )
						return lastQuadraticBezierControlPoint;
					//If there is no previous command or if the previous command was not an beziert+ curve, 
					//assume the first control point is coincident with the current point
					return currentPoint;
				}
				
				const CncCurveLib::Point& getLastCubicBezierControlPoint(const CncCurveLib::Point& currentPoint) {
					if ( hasLastCubicBezierControlPoint() )
						return lastCubicBezierControlPoint;
					//If there is no previous command or if the previous command was not an beziert+ curve, 
					//assume the first control point is coincident with the current point
					return currentPoint;
				}
		};

		// members
		FileParser*					fileParser;
		bool 						firstPath;
		bool 						newPath;
		CncDoublePosition			startPos;
		CncDoublePosition			currentPos;
		double						totalLength;
		LastControlPoint 			lastControlPoint;
		CncPathListManager 			pathListMgr;
		CncUnitCalculator<float>	unitCalculator;
		
		CncLineCurve				lineCurve;
		CncEllipticalCurve			ellipticalCurve;
		CncQuadraticBezierCurve		quadraticBezierCurve;
		CncCubicBezierCurve			cubicBezierCurve;
		
		// trace functions
		void traceFunctionCall(const char* fn);
		void traceCurrentPosition();
		void tracePositions(const char* userPerspectivePrefix);
		void traceFirstMove(double moveX, double moveY);
		
		// 
		virtual bool callback(const CncCurveLib::Point& p);
		virtual bool processLinearMove(bool alreadyRendered) = 0;
		
		// debug functions
		virtual void appendDebugValueDetail(const char* key, wxVariant value);
		virtual void appendDebugValueDetail(const CncPathListEntry& cpe);
		virtual void appendDebugValueDetail(const CncCurveLib::ParameterSet& ps);
		
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
		
		virtual bool isZAxisUp()   { return false; }
		virtual bool isZAxisDown() { return false; }
		virtual bool moveUpZ()	   { return true;  }
		virtual bool moveDownZ()   { return true;  }

		// transformation
		virtual void transform(double& xAbs, double& yAbs) 	{}
		
		const CncCurveLib::Point transformCurveLibPoint(double xAbs, double yAbs) {
			transform(xAbs, yAbs);
			return CncCurveLib::Point(xAbs, yAbs);
		}
		
	public:
	
		PathHandlerBase();
		virtual ~PathHandlerBase();
		
		virtual const char* getName() { return "PathHandlerBase"; }
		virtual void initNextClientId(long id) {}
		
		void setFileParser(FileParser* fp) { fileParser = fp; }
		
		// setter
		void setCurveLibResolution(float res);
		void setPathList(const CncPathListManager& newPathList);
		
		void initCurrentPos(const CncDoublePosition& pos);
		
		unsigned int getDataPointCount() const { return pathListMgr.getPathListSize(); }
		const CncPathListManager& getPathList() { return pathListMgr; }
		
		// processing
		void debugProcess(char c, unsigned int count, double values[]);
		bool process(char c, unsigned int count, double values[]);
		
		virtual void prepareWork();
		virtual bool initNextPath();
		virtual bool finishCurrentPath();
		virtual bool runCurrentPath();
		virtual void finishWork();
		
		virtual void logMeasurementStart()	{}
		virtual void logMeasurementEnd() 	{}
		
		virtual void processWait(int64_t microseconds);
		
		virtual bool shouldAToolChangeProcessed() { return true; }
		
		// get path repesentations
		void tracePathList(std::ostream &ostr);
		
		// path analytics
		double getCurrentPathLength() 			{ return pathListMgr.getTotalDistance(); }
		double getTotalLength() 				{ return totalLength; }
		bool isPathClosed() 					{ return pathListMgr.isPathClosed(); }
		
		// path modifiers
		bool reversePath() 						{ return pathListMgr.reversePath(); }
		
		// unit calculation functions
		void changeInputUnit(const Unit u, bool trace=true);
};

#endif
