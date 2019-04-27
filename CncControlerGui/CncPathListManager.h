#ifndef CNC_PATH_LIST_MANAGER_H
#define CNC_PATH_LIST_MANAGER_H

#include "CncPathListEntry.h"

//////////////////////////////////////////////////////////////////
class CncPathListManager {
	
	public:
		struct Data {
			
			unsigned int posChgCounter		= 0;
			unsigned int speedChgCounter	= 0;
			
			void reset() {
				
			}
		};
		
	protected:
	
		// Path position (CncPathListEntry) entries
		CncPathList list;
		
		bool isFirstPath;			// stores if this path is the first path info
		bool isCorrected;			// stores if this path is corrected
		
		CncDoublePosition referencePos;	// reference position - from where we are coming
		
		double minPosX;
		double minPosY;
		double minPosZ;

		double maxPosX;
		double maxPosY;
		double maxPosZ;
		
		double totalDistance;
		
		Data data;
		
		//////////////////////////////////////////////////////////////
		void appendEntry(CncPathListEntry& cpe);
		
	public:
		
		//////////////////////////////////////////////////////////////
		CncPathListManager();
		~CncPathListManager();
		
		CncPathList& getPathListtoModify() { return list; }
		const CncPathList& getPathList() const { return list; }
		void setPathList(const CncPathList& pl) { list = pl; }
		
		unsigned int getPathListSize() const { return list.size(); }
		
		bool getFirstPathFlag() const { return isFirstPath; }
		void setFirstPathFlag(bool state=true) { isFirstPath = state; }
		
		bool isPathCorrected() const { return isCorrected; }
		void setCorretedFlag(bool state=true) { isCorrected = state; }
		
		double getTotalDistance() const { return totalDistance; }
		
		double getMinPosX() const { return minPosX; }
		double getMinPosY() const { return minPosY; }
		double getMinPosZ() const { return minPosZ; }
		
		double getMaxPosX() const { return maxPosX; }
		double getMaxPosY() const { return maxPosY; }
		double getMaxPosZ() const { return maxPosZ; }
		
		const CncDoublePosition& getStartPos() const;
		const CncDoublePosition& getReferencePos() const { return referencePos; }
		void setReferencePos(const CncDoublePosition& p) { referencePos = p; }
		
		const CncPathList::iterator begin() { return list.begin(); }
		const CncPathList::iterator end()   { return list.end(); }
		const CncPathList::iterator last()  { return list.end() - 1; }
		const CncPathList::iterator next(CncPathList::iterator& it)  { return ++it; }
		
		const CncPathList::const_iterator const_begin() const { return list.cbegin(); }
		const CncPathList::const_iterator const_end()   const { return list.cend(); }
		const CncPathList::const_iterator const_last()  const { return list.cend() - 1; }
		
		bool isPathClosed() const;

		void clear();
		void reset();
		void resetMinMax();
		
		bool reversePath();
		
		const CncPathListEntry& addEntryAdm(long clientId);
		const CncPathListEntry& addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN);
		
		const CncPathListEntry& addEntryAbs(const CncDoublePosition& newAbsPos, bool alreadyRendered=false);
		const CncPathListEntry& addEntryRel(const CncDoublePosition& newAbsPos, bool alreadyRendered=false);
		
		const CncPathListEntry& addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered=false);
		const CncPathListEntry& addEntryRel(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered=false);
		
		bool eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase);
		
		std::ostream& outputOperator(std::ostream &ostr) const;
		friend std::ostream& operator<<(std::ostream &ostr, const CncPathListManager &a) {
			return a.outputOperator(ostr);
		}
};

#endif


