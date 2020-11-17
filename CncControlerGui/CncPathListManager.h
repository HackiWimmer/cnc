#ifndef CNC_PATH_LIST_MANAGER_H
#define CNC_PATH_LIST_MANAGER_H

#include "CncPathListEntry.h"

/**
 * CncPathListManager is a wrapper around an underlying list of CncPatListEntries. 
 * The underlying list can be a cnc path or a guide path. 
 * The warper is used to maintaine the underlying list.
 */
 
class CncPathListManager {
		
	public:
		enum PathType  { PT_CNC_PATH, PT_GUIDE_PATH };
		enum GuideType { NO_GUIDE_PATH, HELP_PATH, ORIG_PATH };
	
	private:
		
		CncPathList			pathList;		// cnc path (CncPathListEntry) entries
		CncPathList			guideList;		// guide path (CncPathListEntry) entries
		PathType			pathType;
		GuideType			guideType;
		CncPathListEntry	defaultEntry;
		CncPathListEntry*	lastCncEntry;
		
		bool				isFirstPath;	// stores if this path is the first path info
		bool				isCorrected;	// stores if this path is corrected
		
		CncDoublePosition	referencePos;	// reference position - from where we are coming
		
		double 				minPosX;
		double 				minPosY;
		double 				minPosZ;
		double 				maxPosX;
		double				maxPosY;
		double 				maxPosZ;
		
		double 				totalDistance;
		
		void 				initNextPath();
		void 				appendEntry(CncPathListEntry& cpe);
		CncPathList&		getPathListIntern();
		
	public:
		
		CncPathListManager();
		~CncPathListManager();
		
		const char*			getPathTypeAsStr()			const;
		const PathType		getPathType()				const	{ return pathType; }
		const GuideType		getGuideType()				const	{ return guideType; }
		
		const CncPathList&	getPathList()				const;
		unsigned int		getPathListSize()			const;
		bool				getFirstPathFlag()			const	{ return isFirstPath; }
		bool				isPathCorrected()			const	{ return isCorrected; }
		
		double				getTotalDistance()			const	{ return totalDistance; }
		
		double				getMinPosX()				const	{ return minPosX; }
		double				getMinPosY()				const	{ return minPosY; }
		double				getMinPosZ()				const	{ return minPosZ; }
		double				getMaxPosX()				const	{ return maxPosX; }
		double				getMaxPosY()				const	{ return maxPosY; }
		double				getMaxPosZ()				const	{ return maxPosZ; }
		
		const CncDoublePosition& getStartPos()			const;
		const CncDoublePosition& getReferencePos()		const	{ return referencePos; }
		
		const CncPathList::iterator begin()						{ return getPathListIntern().begin(); }
		const CncPathList::iterator end()						{ return getPathListIntern().end(); }
		const CncPathList::iterator first()						{ return getPathListIntern().begin(); }
		const CncPathList::iterator last()						{ return getPathListIntern().size() ? getPathListIntern().end() - 1 : getPathListIntern().end(); }
		
		const CncPathList::const_iterator const_begin()	const	{ return getPathList().cbegin(); }
		const CncPathList::const_iterator const_end()	const	{ return getPathList().cend(); }
		const CncPathList::const_iterator const_first()	const	{ return getPathList().cbegin(); }
		const CncPathList::const_iterator const_last()	const	{ return getPathList().size() ? getPathList().cend() - 1 : getPathList().cend(); }
		
		void setReferencePos(const CncDoublePosition& p)		{ referencePos = p; }
		void setPathFirstFlag(bool state) 						{ isFirstPath  = state; }

		void clear();
		void resetMinMax();
		
		bool isPathClosed() const;
		bool reversePath();
		
		void initNextCncPath();
		void initNextGuidePath(GuideType gt = HELP_PATH);
		
		const CncPathListEntry& addEntryAdm(long clientId);
		const CncPathListEntry& addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN);
		const CncPathListEntry& addEntryAbs(const CncDoublePosition& newAbsPos, bool alreadyRendered=false);
		const CncPathListEntry& addEntryRel(const CncDoublePosition& distance,  bool alreadyRendered=false);
		const CncPathListEntry& addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered=false);
		const CncPathListEntry& addEntryRel(double deltaX,     double deltaY,     double deltaZ,     bool alreadyRendered=false);
		
		bool eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase);
		
		std::ostream& outputOperator(std::ostream &ostr) const;
		friend std::ostream& operator<<(std::ostream &ostr, const CncPathListManager &a) { return a.outputOperator(ostr); }
};

#endif


