#ifndef SVG_PATH_HANDLER_BASE_H
#define SVG_PATH_HANDLER_BASE_H

#include <vector>
#include <wx/variant.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include "SVGTransformMatrix.h"
#include "CncSvgCurveLib.h"
#include "CncPosition.h"
#include "CncPathListEntry.h"

//////////////////////////////////////////////////////////////////
struct CncPathListInfo {
//////////////////////////////////////////////////////////////////
	// Path postion (CncPathListEntry) entries
	CncPathList list;
	
	// aditional information
	bool firstPath;
	bool isCorrected;
	
	wxRealPoint startPos; 		// absolute start position of the path
	wxRealPoint firstMove; 		// relative x, y values for the first move
	double xyLength;
	
	// helper for external usage
	wxRealPoint prevPosAbs; 	// contains absolote coordinates of the previous(list.size() - 1) position
	
	//////////////////////////////////////////////////////////////
	CncPathListInfo() {
		reset();
	}
	
	//////////////////////////////////////////////////////////////
	~CncPathListInfo() {
		reset();
	}
	
	//////////////////////////////////////////////////////////////
	friend std::ostream &operator<< (std::ostream &ostr, const CncPathListInfo &a) {
		ostr << "CncPathListInfo entries: " << a.list.size() << std::endl;
		ostr << " Start Pos:   " 			<< a.startPos 	<< std::endl;
		return ostr;
	}
	
	//////////////////////////////////////////////////////////////
	void reset() {
		firstPath   	= false;
		isCorrected 	= false;
		
		startPos		= {0, 0}; 
		firstMove		= {0, 0};
		prevPosAbs 		= {0, 0};
		
		xyLength		= 0.0;
		
		list.clear();
	}
	
	//////////////////////////////////////////////////////////////
	void appendEntry(CncPathListEntry& cpe) {
		// additionally calculate length and distance
		if ( list.size() > 0 ) {
			xyLength += sqrt(pow(cpe.move.x, 2) + pow(cpe.move.y, 2));
			cpe.xyDistance = xyLength;
		}
		
		// store
		list.push_back(cpe);
	}
	
	//////////////////////////////////////////////////////////////
	void calculateAndEntry(const wxRealPoint& newAbsPoint) {
		CncPathListEntry cpe;
		cpe.abs.x = newAbsPoint.x;
		cpe.abs.y = newAbsPoint.y;

		// calculate
		if ( list.size() == 0 ) {
			cpe.move.x = newAbsPoint.x;
			cpe.move.y = newAbsPoint.y;
			
			startPos  = newAbsPoint;
			firstMove = cpe.move;
			
		} else {
			cpe.move.x = newAbsPoint.x - list.back().abs.x;
			cpe.move.y = newAbsPoint.y - list.back().abs.y;
		}
		
		// append
		appendEntry(cpe);
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

		// members
		bool 				firstPath;
		bool 				newPath;
		CncDoublePosition	startPos;
		CncDoublePosition	currentPos;
		float 				curveLibResolution;
		double				totalLength;
		LastControlPoint 	lastControlPoint;
		CncPathListInfo 	pathList;
		SVGTransformMatrix currentSvgTransformMatrix;
		
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
		bool processLinearMove(bool alreadyRendered);
		
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
		virtual void simulateZAxisUp() 		{}
		virtual void simulateZAxisDown() 	{}
		virtual bool isZAxisUp() 			{ return true; }
		virtual bool isZAxisDown() 			{ return false; }
		
	public:
		// boost wkt types
		enum WktTypeInfo { WKT_EMPTY, WKT_POINT, WKT_POLYGON, WKT_LINESTRING, WKT_UNKNOWN};
	
		SVGPathHandlerBase();
		virtual ~SVGPathHandlerBase();
		
		// setter
		void setCurveLibResolution(float res) { curveLibResolution = res; }
		void setPathList(const CncPathListInfo& newPathList);
		
		// getter
		SVGTransformMatrix& getSvgTransformMatrix() { return currentSvgTransformMatrix; }
		unsigned int getDataPointCount() const { return pathList.list.size(); }
		const CncPathListInfo& getPathList() { return pathList; }
		
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
		const char* getAsWktRepresentation();
		const char* getAsSvgPathRepresentation(const wxString& style="");
		
		// path analytics
		double getCurrentPathLength();
		double getTotalLength();
		
		bool isPathClosed();
		bool getCentroid(wxRealPoint& centroid);
		
		const wxString& getWktTypeAsString();
		SVGPathHandlerBase::WktTypeInfo getWktType();
		
		// path modifiers
		bool reversePath();
		bool centerPath();
		
};

#endif