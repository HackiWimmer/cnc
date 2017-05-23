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
class CncPathListManager {
//////////////////////////////////////////////////////////////////
	
	protected:
	
		// Path postion (CncPathListEntry) entries
		CncPathList list;
		
		bool isFirstPath;			// stores if this path is the first path info
		bool isCorrected;			// stores if this path is corrected
		
		wxRealPoint startPos; 		// absolute start position of the path
		wxRealPoint firstMove; 		// relative x, y values for the first move
		
		double minPosX;
		double minPosY;
		double maxPosX;
		double maxPosY;
		
		double xyLength;
		
		//////////////////////////////////////////////////////////////
		void appendEntry(CncPathListEntry& cpe) {
			// additionally calculate length and distance
			if ( list.size() > 0 ) {
				xyLength += sqrt(pow(cpe.move.x, 2) + pow(cpe.move.y, 2));
				cpe.xyDistance = xyLength;
			}
			
			// addionally determine fences
			minPosX = std::min(minPosX, cpe.abs.x);
			minPosY = std::min(minPosY, cpe.abs.y);
			maxPosX = std::max(maxPosX, cpe.abs.x);
			maxPosY = std::max(maxPosY, cpe.abs.y);
			
			// store
			list.push_back(cpe);
		}
		
	public:
		
		//////////////////////////////////////////////////////////////
		CncPathListManager() {
			//preallocate memory
			list.reserve(1000 * 1000);
			
			reset();
		}
		
		//////////////////////////////////////////////////////////////
		~CncPathListManager() {
			reset();
		}
		
		//////////////////////////////////////////////////////////////
		CncPathList& getPathListtoModify() { return list; }
		const CncPathList& getPathList() const { return list; }
		void setPathList(const CncPathList& pl) { list = pl; }
		
		//////////////////////////////////////////////////////////////
		unsigned int getPathListSize() const { return list.size(); }
		
		//////////////////////////////////////////////////////////////
		bool getFirstPathFlag() const { return isFirstPath; }
		void setFirstPathFlag(bool state=true) { isFirstPath = state; }
		
		//////////////////////////////////////////////////////////////
		bool isPathCorrected() const { return isCorrected; }
		void setCorretedFlag(bool state=true) { isCorrected = state; }
		
		//////////////////////////////////////////////////////////////
		double getXYLength() const { return xyLength; }
		
		//////////////////////////////////////////////////////////////
		double getMinPosX() const { return minPosX; }
		double getMinPosY() const { return minPosY; }
		double getMaxPosX() const { return maxPosX; }
		double getMaxPosY() const { return maxPosY; }
		
		//////////////////////////////////////////////////////////////
		const wxRealPoint& getStartPos() const { return startPos; }
		void setStartPos(const wxRealPoint& sp) { startPos = sp; }
		void incStartPos(const wxRealPoint& incSp) { startPos -= incSp; }
		
		//////////////////////////////////////////////////////////////
		const wxRealPoint& getFirstMove() const { return firstMove; }
		void setFirstMove(const wxRealPoint& fm) { firstMove = fm; }
		
		//////////////////////////////////////////////////////////////
		const CncPathList::iterator begin() { return list.begin(); }
		const CncPathList::iterator end()   { return list.end(); }
		
		//////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPathListManager &a) {
			ostr << "CncPathListInfo entries : " << a.list.size() << std::endl;
			ostr << " is correded            : " << a.isPathCorrected() << std::endl;
			ostr << " is first path          : " << a.getFirstPathFlag() << std::endl;
			ostr << " xy length              : " << cnc::dblFormat1(a.getXYLength()) << std::endl;
			ostr << " minPos (x,y)           : " << cnc::dblFormat2(a.getMinPosX(), a.getMinPosY()) << std::endl;
			ostr << " maxPos (X,Y)           : " << cnc::dblFormat2(a.getMaxPosX(), a.getMaxPosY()) << std::endl;
			ostr << " startPos               : " << a.getStartPos() << std::endl;
			ostr << " first move             : " << a.getFirstMove() << std::endl;
			
			ostr << " path list:" << std::endl;
			for ( auto it=a.getPathList().begin(); it!=a.getPathList().end(); ++it )
				ostr << it->getPointAsString() <<  " | zDown: " << it->zAxisDown <<  " | rendered: " << it->alreadyRendered << endl;

			return ostr;
		}
		
		//////////////////////////////////////////////////////////////
		void reset() {
			isFirstPath   	= false;
			isCorrected 	= false;
			
			startPos		= {0, 0}; 
			firstMove		= {0, 0};
			
			resetMinMax();
			
			xyLength		= 0.0;
			
			list.clear();
		}
		
		//////////////////////////////////////////////////////////////
		void resetMinMax() {
			minPosX			= DBL_MAX;
			minPosY			= DBL_MAX;
			maxPosX			= DBL_MIN;
			maxPosY			= DBL_MIN;
		}
		
		//////////////////////////////////////////////////////////////
		bool reversePath() {
			// empty or sinle point, nothing should happen
			if ( getPathListSize() < 2 )
				return true;
			
			CncPathListEntry first = *(begin());
			
			// update first entry and append it, after reverse it will be first again
			first.move.x = list.back().abs.x;
			first.move.y = list.back().abs.y;
			first.alreadyRendered = true;
			first.zAxisDown = false;
			list.push_back(first);
			
			// remove old first entry
			list.erase(list.begin());
			
			// reverse list
			std::reverse(list.begin(), list.end());
			
			// reset length - will be recalculate in next loop
			xyLength = 0.0;
			
			// over all entries 
			unsigned int cnt = 0;
			for (CncPathList::iterator it = list.begin(); it != list.end(); ++it) {
				// recalculate distance
				if ( cnt > 0 ) {
					xyLength += sqrt(pow(it->move.x, 2) + pow(it->move.y, 2));
					it->xyDistance = xyLength;
					
					// reverse relativ move steps
					(*it).move.x *= -1;
					(*it).move.y *= -1;
					
				} else {
					it->xyDistance = 0.0;
				}
				
				cnt++;
			}
			
			// correct the start posistion
			if ( getFirstPathFlag() == true ) {
				CncPathList::iterator it = begin();
				setStartPos( {(*it).move.x, (*it).move.y});
			} else {
				CncPathList::iterator it = begin();
				incStartPos(getFirstMove() - (*it).move);
			}
			
			return true;
		}
		
		//////////////////////////////////////////////////////////////
		const CncPathListEntry& calculateAndAddEntry(double newAbsPosX, 
		                                             double newAbsPosY,
		                                             bool alreadyRendered=false, 
		                                             bool zAxisDown= false) {
			CncPathListEntry cpe;
			cpe.zAxisDown		= zAxisDown;
			cpe.alreadyRendered	= alreadyRendered;
			cpe.abs.x 			= newAbsPosX;
			cpe.abs.y 			= newAbsPosY;

			// calculate
			if ( list.size() == 0 ) {
				cpe.move.x = newAbsPosX;
				cpe.move.y = newAbsPosY;
				
				startPos  = {newAbsPosX, newAbsPosY};
				firstMove = {cpe.move.x, cpe.move.y};
				
			} else {
				cpe.move.x = newAbsPosX - list.back().abs.x;
				cpe.move.y = newAbsPosY - list.back().abs.y;
			}
			
			// append
			appendEntry(cpe);
			return list.back();
		}
		
		//////////////////////////////////////////////////////////////
		const CncPathListEntry& calculateAndAddEntry(const wxRealPoint& newAbsPoint, 
		                                             bool alreadyRendered=false, 
		                                             bool zAxisDown= false) {
			
			return calculateAndAddEntry(newAbsPoint.x, newAbsPoint.y, alreadyRendered, zAxisDown);
		}
		
		//////////////////////////////////////////////////////////////
		bool eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase) {
			if ( itToErase < begin() )
				return false;

			if ( itToErase >= end() )
				return false;
				
			// store entry 
			CncPathListEntry entry = *itToErase;
			
			// check first position
			bool first = false;
			if ( itToErase == begin() )
				first = true;
			
			// remove entry
			if ( list.erase(itToErase) == end() )
				return false;
			
			// redetermine additional values
			if ( first == true ) {
				startPos 					= begin()->abs;
				firstMove					= begin()->move;
				begin()->alreadyRendered 	= true;
				begin()->zAxisDown 			= false;
			}
			
			// recalculate min or max on demand
			if ( cnc::dblCompare(entry.abs.x, minPosX) || cnc::dblCompare(entry.abs.y, minPosY) || 
			     cnc::dblCompare(entry.abs.x, maxPosX) || cnc::dblCompare(entry.abs.y, maxPosY) ) {
					 
				resetMinMax();
				for ( auto it=begin(); it !=end(); ++it ) {
					minPosX = std::min(minPosX, it->abs.x);
					minPosY = std::min(minPosY, it->abs.y);
					maxPosX = std::max(maxPosX, it->abs.x);
					maxPosY = std::max(maxPosY, it->abs.y);
				}
			} 
			
			// recalculate length
			xyLength = 0.0;
			unsigned int cnt = 0;
			for ( auto it=begin(); it !=end(); ++it ) {
				if ( cnt > 0 ) {
					xyLength += sqrt(pow(it->move.x, 2) + pow(it->move.y, 2));
					it->xyDistance = xyLength;
				} else {
					it->xyDistance = 0.0;
				}
				cnt++;
			}
			
			return true;
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
		CncPathListManager 	pathListMgr;
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
		
		// demo function
		bool overAllBoostWktEntriesSample();
		
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