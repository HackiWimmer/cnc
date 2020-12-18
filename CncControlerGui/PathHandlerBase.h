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

class FileParser;

//////////////////////////////////////////////////////////////////
class PathHandlerBase : public CncCurveLib::Caller {
//////////////////////////////////////////////////////////////////
	public:
		typedef CncUnitCalculatorBase::Unit Unit;

	protected:
		CncPathListManager 				pathListMgr;

		FileParser*						fileParser;
		bool 							nextPath;

		CncDoublePosition				startPos;
		CncDoublePosition				currentPos;

		double							totalLength;

		CncUnitCalculator<float>		unitCalculator;

		CncLineCurve					lineCurve;
		CncEllipticalCurve				ellipticalCurve;
		CncQuadraticBezierCurve			quadraticBezierCurve;
		CncCubicBezierCurve				cubicBezierCurve;
		
		CncCurveLib::LastControlPoint 	lastQuadraticControlPoint;
		CncCurveLib::LastControlPoint 	lastCubicControlPoint;
		
		virtual bool onPhysicallyClientIdChange(long idx, const CncPathListEntry& cpe) 			{  cnc::cex1 << "PathHandlerBase::onPhysicallyClientIdChange() " 	<< std::endl; return false; }
		virtual bool onPhysicallySpeedChange(unsigned long idx, const CncPathListEntry& cpe) 	{  cnc::cex1 << "PathHandlerBase::onPhysicallySpeedChange() " 		<< std::endl; return false; }
		virtual bool onPhysicallyMove(unsigned long idx, const CncPathListEntry& cpe) 			{  cnc::cex1 << "PathHandlerBase::onPhysicallyMove() " 				<< std::endl; return false; }
		
		// trace functions
		void traceFunctionCall(const char* fn);
		void traceCurrentPosition();
		void tracePositions(const char* userPerspectivePrefix);
		void traceFirstMove(double moveX, double moveY);
		
		void logNextPathListEntry(const CncPathListEntry& cpe);
		
		// Curve lib callback
		virtual bool callback(const CncCurveLib::Point& p);
		
		virtual bool processLinearMove(bool alreadyRendered) = 0;
		void processClientId(long id);
		void processSpeed(CncSpeedMode mode, double feedSpeed_MM_MIN);
		
		// debug functions
		virtual void appendDebugValueDetail(const char* key, wxVariant value);
		virtual void appendDebugValueDetail(const CncPathListEntry& cpe);
		virtual void appendDebugValueDetail(const CncCurveLib::ParameterSet& ps);
		
		//render functions
		bool processMove_2DXY(char c, unsigned int count,  const double values[]);
		bool processLine_2DXY(char c, unsigned int count,  const double values[]);
		bool processHLine_2DXY(char c, unsigned int count, const double values[]);
		bool processVLine_2DXY(char c, unsigned int count, const double values[]);
		bool processClose_2DXY(char c, unsigned int count, const double values[]);

		bool processARC_2DXY(CncCurveLib::ParameterElliptical& ps);
		bool processARC_2DXY(char c, unsigned int count, const double values[]);

		bool processQuadraticBezier_2DXY(CncCurveLib::ParameterQuadraticBezier& ps);
		bool processQuadraticBezier_2DXY(char c, unsigned int count, const double values[]);

		bool processCubicBezier_2DXY(CncCurveLib::ParameterCubicBezier& ps);
		bool processCubicBezier_2DXY(char c, unsigned int count, const double values[]);

		bool processQuadraticBezierSmooth_2DXY(char c, unsigned int count, const double values[]);
		bool processCubicBezierSmooth_2DXY(char c, unsigned int count, const double values[]);
		
		virtual bool isInitialized();
		
		// transformation
		virtual void transform(double& xAbs, double& yAbs) 	{}
		
		const CncCurveLib::Point transformCurveLibPoint(double xAbs, double yAbs) {
			transform(xAbs, yAbs);
			return CncCurveLib::Point(xAbs, yAbs);
		}
		
	public:
	
		PathHandlerBase();
		virtual ~PathHandlerBase();
		
		virtual const char* getName() const { return "PathHandlerBase"; }
		virtual void initNextClientId(long id) {}
		
		void setFileParser(FileParser* fp) { fileParser = fp; }
		void initCurrentPos(const CncDoublePosition& pos);
		
		unsigned int getDataPointCount() const { return pathListMgr.getPathListSize(); }
		const CncPathListManager& getPathList() { return pathListMgr; }
		
		// processing
		bool processCommand_2DXY(char c, unsigned int count, const double values[]);
		
		bool isNextPath()  { return nextPath;  }

		virtual bool prepareWork();
		virtual bool initNextPath();
		virtual bool finishCurrentPath();
		virtual bool runCurrentPath();
		virtual bool finishWork();
		
		virtual void logMeasurementStart()	{}
		virtual void logMeasurementEnd() 	{}
		
		virtual void processWait(int64_t microseconds);
		
		virtual bool shouldAToolChangeProcessed() { return true; }
		
		// get path representations
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
