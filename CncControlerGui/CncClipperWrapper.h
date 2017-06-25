#ifndef CNC_CLIPPER_WRAPPER
#define CNC_CLIPPER_WRAPPER

#include "CncCommon.h"
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
		
		bool getOrientationAsBool() { return Orientation(*this); }
		CncDirection getOrientation(); 
		CncDirection setOrientation(CncDirection dir);
		void reverseOrientation(); 
		
		void append(const wxRealPoint& p);
		void append(double x, double y);
		void append(const IntPoint& p);
		void append(long x, long y);
		void append(int x, int y);
		void append(const ClipperLib::Path& p);
		
		bool remove(iterator position);
		
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
typedef std::vector<CncPolygonPoints> PolygonList;

/////////////////////////////////////////////////////////////////////////////
class CncPolygons : public ClipperLib::Paths {
	
	public:
		CncPolygons();
		CncPolygons(const CncPolygons& ps);
		CncPolygons(const PolygonList& pl);
		
		virtual ~CncPolygons();
		
		unsigned int getTotalCount();
		unsigned int getOuterCount();
		unsigned int getHoleCount();
		
		
		/*
		//////////////////////////////////////////////////////////////////
		CncPolygonPoints& operator[](std::size_t idx) { 
			return (*this)[idx]; 
		}*/
		
		//////////////////////////////////////////////////////////////////
		bool getPolygonPoints(unsigned int idx, CncPolygonPoints& ret);
		
		//////////////////////////////////////////////////////////////////
		int removeHoles() {
			if ( size() <= 1 )
				return 0;
				
			bool outerDir = Orientation(*begin());
			for ( auto it=end()-1; it!=begin(); --it) {
				if ( Orientation(*it) != outerDir )
					erase(it);
			}
			
			return size();
		}
		
		//////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPolygons &a) {
			for ( auto it=a.begin(); it!=a.end(); ++it ) {
				CncPolygonPoints pp = *it;
				ostr << pp;
			}
			
			return ostr;
		}
		
		//////////////////////////////////////////////////////////////////
		void trace(std::ostream &ostr) {
			ostr << "CncPolygons::size: " << size() << "\n";
			
			unsigned int cnt = 0;
			for ( auto it=begin(); it!=end(); ++it ) {
				CncPolygonPoints pp = *it;
				ostr << " ptr  [" << cnt << "]:" << &(*it) << "\n";
				ostr << " size [" << cnt << "]:" << pp.size() << "\n";
				ostr << " dir  [" << cnt << "]:" << Orientation(pp) << "\n";
				
				cnt++;
			}
		}
};

/////////////////////////////////////////////////////////////////////////////
class CncClipperWrapper {
	
	protected:
		double correctEndPoint(IntPoint& p1, const IntPoint& p2, double offset);
		 
	public:
		CncClipperWrapper();
		virtual ~CncClipperWrapper();
		
		////////////////////////////////////////////////////////////////////////
		bool correctEndPoints(CncPolygonPoints& in, double offset);
		
		////////////////////////////////////////////////////////////////////////
		bool offsetPath(const CncPolygonPoints& in, CncPolygons& out, double offset, 
		                CncClipperCornerType joinType=CncCCT_Round, 
						CncClipperEndType endType=CncCET_ClosedPolygon);
						
		////////////////////////////////////////////////////////////////////////
		bool offsetPath(const PolygonList& inList, CncPolygons& out, double offset, 
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