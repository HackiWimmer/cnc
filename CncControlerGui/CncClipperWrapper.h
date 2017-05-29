#ifndef CNC_CLIPPER_WRAPPER
#define CNC_CLIPPER_WRAPPER

#include "Clipper/clipper.hpp"
#include "SvgUnitCalculator.h"
#include "CncPathListManager.h"

using namespace ClipperLib;

/////////////////////////////////////////////////////////////////////////////
class CncPolygonPoints : public ClipperLib::Path {
	
	private:
		static IntPoint glbTempIntPointRetVal;
		static wxRealPoint gblTmpRealPointRetVal;
		
		static const IntPoint& getPointValue(long x, long y) {
			glbTempIntPointRetVal.X = x;
			glbTempIntPointRetVal.Y = y;
			
			return glbTempIntPointRetVal;
		}
		
	protected:
		IntPoint minPos;
		IntPoint maxPos;
		
		inline void updateMinMaxPos(const IntPoint& p);
		
	public:
		static const unsigned int doubleToIntFactor;
		
		CncPolygonPoints();
		CncPolygonPoints(const CncPolygonPoints& p);
		CncPolygonPoints(const ClipperLib::Path& p);
		virtual ~CncPolygonPoints();
		
		void append(const wxRealPoint& p);
		void append(double x, double y);
		void append(const IntPoint& p);
		void append(long x, long y);
		void append(int x, int y);
		void append(const ClipperLib::Path& p);
		
		bool isPolygonClosed();
		void closePolygon();
		
		void evaluateAdditionalValues();
		
		const IntPoint& getMinIntPos() const { return ( size() > 0 ? minPos : getPointValue(0, 0)); }
		const IntPoint& getMaxIntPos() const { return ( size() > 0 ? maxPos : getPointValue(0, 0)); }
		
		const wxRealPoint& getMinRealPos() const { return convertToRealPoint(getMinIntPos()); }
		const wxRealPoint& getMaxRealPos() const { return convertToRealPoint(getMaxIntPos()); }
		
		const char* getDataPointsAsWktString(wxString& ret);
		const char* getAsSvgPathRepresentation(SVGUnit inputUnit, bool withElement, const wxString& style="style=\"fill:none;stroke:#0000ff;stroke-width:1.0\"") const;
		
		static const IntPoint& convertToIntPoint(const wxRealPoint& p);
		static const wxRealPoint& convertToRealPoint(const IntPoint& p);
		
		static double convertToDouble(const cInt& v);
		static cInt convertToInt(double v);
		
		//////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPolygonPoints &a) {
			ostr << "MinPos: " << a.getMinRealPos() << std::endl;
			ostr << "MaxPos: " << a.getMaxRealPos() << std::endl;
			ostr << "Size:   " << a.size() << std::endl;
			
			for ( auto it=a.begin(); it!=a.end(); ++it ) {
				ostr << CncPolygonPoints::convertToRealPoint(*it);
				ostr << std::endl;
			}
			return ostr;
		}
		
};

/////////////////////////////////////////////////////////////////////////////
class CncPolygons : public ClipperLib::Paths {
	
	public:
		CncPolygons();
		virtual ~CncPolygons();
		
		bool getPolygonPoints(unsigned int idx, CncPolygonPoints& ret);
		
		

		
		
		
};

/////////////////////////////////////////////////////////////////////////////
class CncClipperWrapper {
	
	public:
		CncClipperWrapper();
		virtual ~CncClipperWrapper();
		
		////////////////////////////////////////////////////////////////////////
		bool offsetPath(const CncPolygonPoints& in, CncPolygons& out, double offset, 
		                CncClipperCornerType joinType=CncCCT_Round, 
						CncClipperEndType endType=CncCET_ClosedPolygon);
						
		/////////////////////////////////////////////////////////////////////////
		JoinType convertCornerType(CncClipperCornerType type ) {
			switch ( type ) {
				case CncCCT_Round: 	return jtRound;
				case CncCCT_Square: return jtSquare;
				case CncCCT_Miter: 	return jtMiter;
			}
			
			return jtRound;
		}
		
		/////////////////////////////////////////////////////////////////////////
		EndType convertEndType(CncClipperEndType type ) {
			switch ( type ) {
				case CncCET_ClosedPolygon: 	return etClosedPolygon;
				case CncCETClosedLine: 		return etClosedLine;
				case CncCETOpenSquare: 		return etOpenSquare;
				case CncCETOpenRound: 		return etOpenRound;
				case CncCETOpenButt: 		return etOpenButt;
			}
			
			return etClosedPolygon;
		}
};


#endif