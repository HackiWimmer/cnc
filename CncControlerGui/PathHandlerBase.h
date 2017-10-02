#ifndef SVG_PATH_HANDLER_BASE_H
#define SVG_PATH_HANDLER_BASE_H

#include <vector>
#include <wx/variant.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include "SVGTransformMatrix.h"
#include "CncSvgCurveLib.h"
#include "CncPosition.h"
#include "CncPathListManager.h"

#define TRACE_FUNCTION_CALL(fn)
#define TRACE_FIRST_MOVE(x,y)
#define TRACE_POSITIONS(p)
#define TRACE_CURRENT_POSITION

//#define ENABLE_TRACE_FUNCTIONS
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

//////////////////////////////////////////////////////////////////
class PathHandlerBase {
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

		// members
		bool 				firstPath;
		bool 				newPath;
		CncDoublePosition	startPos;
		CncDoublePosition	currentPos;
		double				totalLength;
		LastControlPoint 	lastControlPoint;
		CncPathListManager 	pathListMgr;
		SVGTransformMatrix  currentSvgTransformMatrix;
		
		// trace functions
		void traceCurveLibPoint(const char* prefix, SVGCurveLib::PointGeneric<>& p);
		void traceFunctionCall(const char* fn);
		void traceCurrentPosition();
		void tracePositions(const char* prefix);
		void traceFirstMove(double moveX, double moveY);
		
		// curvel lib helper
		bool processCurveLibPoint(SVGCurveLib::PointGeneric<> p);
		
		// store CncPathList
		virtual bool shouldConvertRefToMM() const { return false; }
		virtual bool processLinearMove(bool alreadyRendered);
		
		// debug functions
		virtual void appendDebugValueDetail(const char* key, wxVariant value);
		virtual void appendDebugValueDetail(const CncPathListEntry& cpe);
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
		virtual void simulateZAxisUp() 		{}
		virtual void simulateZAxisDown() 	{}
		virtual bool isZAxisUp() 			{ return true; }
		virtual bool isZAxisDown() 			{ return false; }
		
	public:
	
		PathHandlerBase();
		virtual ~PathHandlerBase();
		
		// setter
		void setCurveLibResolution(float res);
		void setPathList(const CncPathListManager& newPathList);
		
		// getter
		SVGTransformMatrix& getSvgTransformMatrix() { return currentSvgTransformMatrix; }
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
		
		// get path repesentations
		void tracePathList(std::ostream &ostr);
		const char* getAsWktRepresentation() { return pathListMgr.getAsWktRepresentation(); }
		const char* getAsSvgPathRepresentation(const wxString& style="") { return pathListMgr.getAsSvgPathRepresentation(style); }
		
		// path analytics
		double getCurrentPathLength() { return pathListMgr.getXYLength(); }
		double getTotalLength() { return totalLength; }
		
		bool isPathClosed() { return pathListMgr.isPathClosed(); }
		bool getCentroid(wxRealPoint& centroid) { return pathListMgr.getCentroid(centroid); }
		
		const wxString& getWktTypeAsString() { return pathListMgr.getWktTypeAsString(); }
		CncPathListManager::WktTypeInfo getWktType() { return pathListMgr.getWktType(); }
		
		// path modifiers
		bool reversePath() { return pathListMgr.reversePath(); }
		bool centerPath() { return pathListMgr.centerPath(); }
		
};

#endif