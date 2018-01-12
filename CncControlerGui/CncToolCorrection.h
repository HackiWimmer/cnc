#ifndef CNC_TOOL_CORRECTION_H
#define CNC_TOOL_CORRECTION_H

#include <cfloat>
#include "LinearFunc.h"
#include "CncPosition.h"
#include "CncPathListEntry.h"
#include "CncCommon.h"

enum SubPathDirection {	FC_Unknown=0, 
						FC_South=6, 
						FC_North=12, 
						FC_West=9, 
						FC_East=3, 
						FC_NortWest=10, 
						FC_NorthEast=2, 
						FC_SouthWest=7, 
						FC_SouthEast=5
						};

class CncToolCorrection {
	
	protected:

		CncPathList iList;
		CncToolCorretionType type;
		double radius;
		int radiusDir;
		
		LinearFunc f1;
		LinearFunc f2;
		
		// path paramter store
		struct PathParameter {
			private:
				bool closed = false;
				double xMin	= 0.0;
				double xMax = 0.0;
				double yMin	= 0.0;
				double yMax = 0.0;
				
				CncDoublePosition workStart;
				CncDoublePosition workEnd;
				
			public:
				void reset() {
					closed 	= false;
					xMin	= DBL_MAX;
					xMax	= 0.0;
					yMin	= DBL_MAX;
					yMax	= 0.0;
					
					workStart.setXYZ(0, 0, 0);
					workEnd.setXYZ(0, 0, 0);
				}
				//getter/setter
				void setClosed() { closed = true; }
				void setXMin(const double v) { if ( v < xMin ) xMin = v; }
				void setXMax(const double v) { if ( v > xMax ) xMax = v; }
				void setYMin(const double v) { if ( v < yMin ) yMin = v; }
				void setYMax(const double v) { if ( v > yMax ) yMax = v; }
				
				void setWorkStarPoint(const double x, const double y) { workStart.setXYZ(x, y, 0); }
				void setWorkEndPoint(const double x, const double y) { workEnd.setXYZ(x, y, 0); }
				
				bool isClosed() { return closed; }
				const double getXMin() { return xMin; }
				const double getXMax() { return xMax; }
				const double getYMin() { return yMin; }
				const double getYMax() { return yMax; }
				
				const CncDoublePosition getWorkStartPoint() const { return workStart; }
				const CncDoublePosition getWorkEndPoint() const { return workEnd; }
				
				// ostream operator
				friend std::ostream &operator<< (std::ostream &ostr, const PathParameter &a) {
					ostr << "Evaluate path results:" << std::endl;
					ostr << " closed:              " << a.closed << std::endl;
					ostr << " xMin:                " << a.xMin << std::endl; 
					ostr << " xMax:                " << a.xMax << std::endl;
					ostr << " yMin:                " << a.yMin << std::endl; 
					ostr << " yMax:                " << a.yMax << std::endl;
					ostr << " Work start :         " << a.workStart.getX() << ", "<< a.workStart.getY() << std::endl;
					ostr << " Work end:            " << a.workEnd.getX() << ", " << a.workEnd.getY() << std::endl;
					return ostr;
				}		
		};
		// stores some informations filled be evaluate path
		PathParameter pathParameter;
		// debugs the complete list
		void debugList(const CncPathList& list);
		// print out the point dripple
		void debugPoints(const char* userPerspectivePrefix, CncPathListEntry P1, CncPathListEntry P2, CncPathListEntry P3);
		// handle one point path
		bool processBorehole(CncPathList& olist);
		// handle two point path
		bool processSimpleSlot(CncPathList& olist);
		// processes pathes with 3 to n element
		bool processPath(CncPathList& olist); 
		// compact the inputpath
		void compactPath();
		// evaluate the input path
		void evaluatePath();
		// determine the direction (Himmelsrichtung) from P1 to P2
		SubPathDirection determinePathDirection(wxRealPoint P1, wxRealPoint P2);
		// depending on the given direction deliver this function +/- tool diameter
		double getToolDiameterCorrectionValue(SubPathDirection pd);
		
		CncDirection getCncDirection(SubPathDirection d1, SubPathDirection d2);
		
		//the follwing functions are used to step through the path
		bool pathBegin(CncPathListEntry P1, CncPathListEntry P2, CncPathListEntry P3, CncPathList& olist);
		bool pathNext(CncPathListEntry P1, CncPathListEntry P2, CncPathListEntry P3, CncPathList& olist);
		bool pathClose(CncPathListEntry P1, CncPathListEntry P2, CncPathListEntry P3, CncPathList& olist);
		
	public:
		// construcor
		CncToolCorrection(double r, CncToolCorretionType t);
		// destructor
		virtual ~CncToolCorrection();
		// return the resultiong path correction
		const CncToolCorretionType getType() { return type; }
		// process the given path
		bool process(CncPathList& oList);
};

#endif