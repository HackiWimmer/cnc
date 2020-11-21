#ifndef CNC_PATH_LIST_MANAGER_H
#define CNC_PATH_LIST_MANAGER_H

#include "Clipper/clipper.hpp"
#include "CncPathListEntry.h"

/**
 * CncPathListManager is a wrapper around an underlying list of CncPatListEntries. 
 * The underlying list can be a cnc path or a guide path. 
 * The warper is used to maintaine the underlying list.
 */
 
class CncPathListManager {
		
	public:
		enum PathType	{ PT_CNC_PATH, PT_GUIDE_PATH };
		enum GuideType	{ NO_GUIDE_PATH, HELP_PATH, ORIG_PATH };
		enum JoinType	{ jtSquare, jtRound, jtMiter };
		enum EndType	{ etClosedPolygon, etClosedLine, etOpenButt, etOpenSquare, etOpenRound };
	
	private:
		
		CncPathList				pathList;		// cnc path (CncPathListEntry) entries
		CncPathList				guideList;		// guide path (CncPathListEntry) entries
		ClipperLib::Path		clipperPath;	// clipper representation
		
		PathType				pathType;
		GuideType				guideType;
		CncPathListEntry		defaultEntry;
		CncPathListEntry*		prevCncStartEntry;
		
		bool					isFirstPath;	// stores if this path is the first path info
		bool					isCorrected;	// stores if this path is corrected
		
		CncDoublePosition		referencePos;	// reference position - from where we are coming
		
		double 					minPosX;
		double 					minPosY;
		double 					minPosZ;
		double 					maxPosX;
		double					maxPosY;
		double 					maxPosZ;
		
		double 					totalDistance;
		
		void 					initNextPath();
		void 					appendEntry(CncPathListEntry& cpe);
		CncPathList&			getPathListIntern();
		
	protected:
		
		const CncPathList&		getProtectedPathList()		const	{ return pathList; }
		const CncPathList&		getProtectedGuideList()		const	{ return guideList; }
		const ClipperLib::Path&	getProtectedClipperPath()	const 	{ return clipperPath; }
		const CncPathListEntry&	getDefaultEntry()			const	{ return defaultEntry; }
		CncPathListEntry*		getPrevCncStartEntry()		const	{ return prevCncStartEntry; }
		
		void					resetStatistics();
		
	public:
		
		CncPathListManager();
		~CncPathListManager();
		
		explicit CncPathListManager(CncPathListManager& plm);
		explicit CncPathListManager(const CncPathListManager& plm);
		explicit CncPathListManager(CncPathListManager&& plm) = delete;
		
		CncPathListManager& operator= (CncPathListManager&);
		CncPathListManager& operator= (const CncPathListManager&);
		CncPathListManager& operator= (CncPathListManager&&) = delete;
		
		static CncPathListManager* clone(const CncPathListManager& plm);
		
		static ClipperLib::JoinType	transform(JoinType jt);
		static ClipperLib::EndType	transform(EndType et);
		static JoinType				transform(ClipperLib::JoinType jt);
		static EndType				transform(ClipperLib::EndType et);
		
		const CncDoublePosition&	getStartPos()			const;
		const CncDoublePosition&	getReferencePos()		const	{ return referencePos; }

		const char*					getPathTypeAsStr()		const;
		const PathType				getPathType()			const	{ return pathType; }
		const GuideType				getGuideType()			const	{ return guideType; }
		
		const CncPathList&			getPathList()			const;
		unsigned int				getPathListSize()		const;
		
		bool						isPathFlaggedAsFirst()	const	{ return isFirstPath; }
		bool						isPathCorrected()		const	{ return isCorrected; }
		bool						isPathClosed()			const;
		
		double						getTotalDistance()		const	{ return totalDistance; }
		
		double						getMinPosX()			const	{ return minPosX; }
		double						getMinPosY()			const	{ return minPosY; }
		double						getMinPosZ()			const	{ return minPosZ; }
		double						getMaxPosX()			const	{ return maxPosX; }
		double						getMaxPosY()			const	{ return maxPosY; }
		double						getMaxPosZ()			const	{ return maxPosZ; }
		
		auto begin()												{ return getPathListIntern().begin(); }
		auto end()													{ return getPathListIntern().end(); }
		auto first()												{ return getPathListIntern().begin(); }
		auto last()													{ return getPathListIntern().size() ? getPathListIntern().end() - 1 : getPathListIntern().end(); }
		
		auto rbegin()												{ return getPathListIntern().rbegin(); }
		auto rend()													{ return getPathListIntern().rend(); }
		auto rfirst()												{ return getPathListIntern().rbegin(); }
		auto rlast()												{ return getPathListIntern().size() ? getPathListIntern().rend() - 1 : getPathListIntern().rend(); }

		auto cbegin()										const	{ return getPathList().cbegin(); }
		auto cend()											const	{ return getPathList().cend(); }
		auto cfirst()										const	{ return getPathList().cbegin(); }
		auto clast()										const	{ return getPathList().size() ? getPathList().cend() - 1 : getPathList().cend(); }
		
		auto crbegin()										const	{ return getPathList().crbegin(); }
		auto crend()										const	{ return getPathList().crend(); }
		auto crfirst()										const	{ return getPathList().cbegin(); }
		auto crlast()										const	{ return getPathList().size() ? getPathList().crend() - 1 : getPathList().crend(); }
		
		auto cFirstPosEntryIterator()						const;
		auto crLastPosEntryIterator()						const;

		void setReferencePos(const CncDoublePosition& p)			{ referencePos = p; }
		void setPathFirstFlag(bool state) 							{ isFirstPath  = state; }

		void clear();
		
		void changeToGuideType(GuideType gt = ORIG_PATH);
		bool processOffset		(double offset, JoinType = jtRound,  EndType = etClosedPolygon);
		bool processInnerOffset	(double offset);
		bool processOuterOffset	(double offset);
		bool roundCorners		(double toolDiameter);
		bool processPocket		(double toolDiameter);
		
		void initNextCncPath();
		void initNextGuidePath(GuideType gt = HELP_PATH);
		
		const CncPathListEntry& addEntryAdm(long clientId);
		const CncPathListEntry& addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN);
		const CncPathListEntry& addEntryAbs(const CncDoublePosition& newAbsPos, bool alreadyRendered=false);
		const CncPathListEntry& addEntryRel(const CncDoublePosition& distance,  bool alreadyRendered=false);
		const CncPathListEntry& addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered=false);
		const CncPathListEntry& addEntryRel(double deltaX,     double deltaY,     double deltaZ,     bool alreadyRendered=false);
		
		std::ostream& outputOperator(std::ostream &ostr) const;
		friend std::ostream& operator<<(std::ostream &ostr, const CncPathListManager &a) { return a.outputOperator(ostr); }
		
		
		
		
		bool eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase);
		
		bool reversePath();
		
		void testSomething();
};

#endif


