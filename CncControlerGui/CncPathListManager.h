#ifndef CNC_PATH_LIST_MANAGER_H
#define CNC_PATH_LIST_MANAGER_H

#include "CncPathListEntry.h"

//////////////////////////////////////////////////////////////////
class CncPathListManager {

	protected:
	
		// Path postion (CncPathListEntry) entries
		CncPathList list;
		
		bool isFirstPath;			// stores if this path is the first path info
		bool isCorrected;			// stores if this path is corrected
		
		wxRealPoint referencePos;	// reference postion - from where we are coming
		
		double minPosX;
		double minPosY;
		double maxPosX;
		double maxPosY;
		
		double xyLength;
		
		//////////////////////////////////////////////////////////////
		void appendEntry(CncPathListEntry& cpe);
		
	public:
		
		// boost wkt types
		enum WktTypeInfo { WKT_EMPTY, WKT_POINT, WKT_POLYGON, WKT_LINESTRING, WKT_UNKNOWN};
			
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
		const wxRealPoint& getStartPos() const;
		const wxRealPoint& getReferencePos() const { return referencePos; }
		void setReferencePos(const wxRealPoint& p) { referencePos = p; }
		
		//////////////////////////////////////////////////////////////
		const CncPathList::iterator begin() { return list.begin(); }
		const CncPathList::iterator end()   { return list.end(); }
		const CncPathList::iterator last()  { return list.end() - 1; }
		
		//////////////////////////////////////////////////////////////
		const CncPathList::const_iterator const_begin() const { return list.begin(); }
		const CncPathList::const_iterator const_end()   const { return list.end(); }
		const CncPathList::const_iterator const_last()  const { return list.end() - 1; }
		
		//////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncPathListManager &a) {
			ostr << "CncPathListInfo entries : " << a.list.size() << std::endl;
			ostr << " is correded            : " << a.isPathCorrected() << std::endl;
			ostr << " is first path          : " << a.getFirstPathFlag() << std::endl;
			ostr << " xy length              : " << cnc::dblFormat1(a.getXYLength()) << std::endl;
			ostr << " minPos (x,y)           : " << cnc::dblFormat2(a.getMinPosX(), a.getMinPosY()) << std::endl;
			ostr << " maxPos (X,Y)           : " << cnc::dblFormat2(a.getMaxPosX(), a.getMaxPosY()) << std::endl;
			ostr << " referencePos           : " << a.getReferencePos() << std::endl;
			ostr << " startPos               : " << a.getStartPos() << std::endl;
			
			ostr << " path list:" << std::endl;
			for ( auto it=a.getPathList().begin(); it!=a.getPathList().end(); ++it )
				ostr << it->getPointAsString() <<  " | zDown: " << it->zAxisDown <<  " | rendered: " << it->alreadyRendered << endl;

			return ostr;
		}
		
		//////////////////////////////////////////////////////////////
		void reset() {
			isFirstPath   	= false;
			isCorrected 	= false;
			
			referencePos	= {0, 0}; 
			
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
		
		// demo function
		bool overAllBoostWktEntriesSample();
		
		//////////////////////////////////////////////////////////////////
		bool isPathClosed();
		
		//////////////////////////////////////////////////////////////////
		const wxString& getWktTypeAsString();
		CncPathListManager::WktTypeInfo getWktType();
		
		//////////////////////////////////////////////////////////////////
		const char* getAsWktRepresentation();
		
		//////////////////////////////////////////////////////////////////
		const char* getAsSvgPathRepresentation(const wxString& style);
		
		//////////////////////////////////////////////////////////////////
		bool getCentroid(wxRealPoint& centroid);
		
		//////////////////////////////////////////////////////////////
		bool shiftPathStart();
		
		//////////////////////////////////////////////////////////////
		bool centerPath();
		
		//////////////////////////////////////////////////////////////
		bool reversePath();
		
		//////////////////////////////////////////////////////////////
		const CncPathListEntry& calculateAndAddEntry(double newAbsPosX, 
		                                             double newAbsPosY,
		                                             bool alreadyRendered=false, 
		                                             bool zAxisDown= false) ;
		
		//////////////////////////////////////////////////////////////
		const CncPathListEntry& calculateAndAddEntry(const wxRealPoint& newAbsPoint, 
		                                             bool alreadyRendered=false, 
		                                             bool zAxisDown= false);
		
		//////////////////////////////////////////////////////////////
		bool eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase);
};

#endif