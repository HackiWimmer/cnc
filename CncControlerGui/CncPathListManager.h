#ifndef CNC_PATH_LIST_MANAGER_H
#define CNC_PATH_LIST_MANAGER_H

#include "Clipper/clipper.hpp"
#include "CncPathListEntry.h"

/**
 * CncPathListManager is a wrapper around an underlying list of CncPatListEntries. 
 * The underlying list can be a cnc path or a guide path. 
 * The wrapper is used to maintain the underlying list.
 */
 
class CncPathListManager {
		
	public:
		enum PathType	{ PT_CNC_PATH, PT_GUIDE_PATH };
		enum GuideType	{ NO_GUIDE_PATH, REFPOS_PATH, HELP_PATH, ORIG_PATH };
		enum JoinType	{ jtSquare, jtRound, jtMiter };
		enum EndType	{ etClosedPolygon, etClosedLine, etOpenButt, etOpenSquare, etOpenRound };
		enum ExeRecomm	{ ER_STAIRS, ER_HELIX };
	
	private:
		
		CncPathList				pathList;		// cnc path (CncPathListEntry) entries
		CncPathList				guideList;		// guide path (CncPathListEntry) entries
		ClipperLib::Path		clipperPath;	// clipper representation
		
		PathType				pathType;
		GuideType				guideType;
		CncPathListEntry		defaultEntry;
		
		ExeRecomm				executionRecommend;
		
		CncDoublePosition		notNormalizeStartDistance;
		
		double 					minPosX;
		double 					minPosY;
		double 					minPosZ;
		double 					maxPosX;
		double					maxPosY;
		double 					maxPosZ;
		
		double 					totalDistance;
		
		bool					adjustZOffset(double offset, bool relative=false);
			
		void					initNextPath(bool linked);
		void					initNextPath(const CncPathListEntry& initialEntry);
		void					appendEntry(CncPathListEntry& cpe);
		CncPathList&			getPathListIntern();
		
	protected:
		
		void					clear();
		
		void					setReferencePos(const CncDoublePosition& p);
		
		const CncPathList&		getProtectedPathList()		const	{ return pathList; }
		const CncPathList&		getProtectedGuideList()		const	{ return guideList; }
		const ClipperLib::Path&	getProtectedClipperPath()	const 	{ return clipperPath; }
		const CncPathListEntry&	getDefaultEntry()			const	{ return defaultEntry; }
		
		CncDirection			getOrientation(ClipperLib::Path& path) 					const;
		bool					ensureDirection(CncDirection d, ClipperLib::Path& path) const;
		void					resetStatistics();
		
		auto 					firstPosEntryIterator();
		auto 					lastPosEntryIterator();
		
	public:
		
		CncPathListManager();
		explicit CncPathListManager(const CncPathListEntry& initialEntry);
		
		explicit CncPathListManager(CncPathListManager& plm);
		explicit CncPathListManager(const CncPathListManager& plm);
		explicit CncPathListManager(CncPathListManager&& plm) = delete;
		
		~CncPathListManager();
		
		CncPathListManager& operator= (CncPathListManager&);
		CncPathListManager& operator= (const CncPathListManager&);
		CncPathListManager& operator= (CncPathListManager&&) = delete;
		
		static CncPathListManager* clone(const CncPathListManager& plm);
		
		static ClipperLib::JoinType	transform(JoinType jt);
		static ClipperLib::EndType	transform(EndType et);
		static JoinType				transform(ClipperLib::JoinType jt);
		static EndType				transform(ClipperLib::EndType et);
		
		bool						getTargetPos(CncDoublePosition& ret)	const;
		bool						isRightTriangle(CncDoublePosition& ret)	const;
		
		const CncDoublePosition&	getStartPos()				const;
		const CncDoublePosition&	getNorNormalizedStartDist()	const	{ return notNormalizeStartDistance; }
		
		ExeRecomm					getExecRecommendation()		const	{ return executionRecommend; }
		
		const char*					getPathTypeAsStr()			const;
		const PathType				getPathType()				const	{ return pathType; }
		const GuideType				getGuideType()				const	{ return guideType; }
		
		const CncPathList&			getPathList()				const;
		unsigned int				getPathListSize()			const;
		
		bool						isPathClosed()				const;
		
		double						getTotalDistance()			const	{ return totalDistance; }
		
		double						getMinPosX()				const	{ return minPosX; }
		double						getMinPosY()				const	{ return minPosY; }
		double						getMinPosZ()				const	{ return minPosZ; }
		double						getMaxPosX()				const	{ return maxPosX; }
		double						getMaxPosY()				const	{ return maxPosY; }
		double						getMaxPosZ()				const	{ return maxPosZ; }
		
		auto	begin()													{ return getPathListIntern().begin(); }
		auto	end()													{ return getPathListIntern().end(); }
		auto	first()													{ return getPathListIntern().begin(); }
		auto	last()													{ return getPathListIntern().size() ? getPathListIntern().end() - 1 : getPathListIntern().end(); }
		
		auto	rbegin()												{ return getPathListIntern().rbegin(); }
		auto	rend()													{ return getPathListIntern().rend(); }
		auto	rfirst()												{ return getPathListIntern().rbegin(); }
		auto	rlast()													{ return getPathListIntern().size() ? getPathListIntern().rend() - 1 : getPathListIntern().rend(); }

		auto	cbegin()											const	{ return getPathList().cbegin(); }
		auto	cend()												const	{ return getPathList().cend(); }
		auto	cfirst()											const	{ return getPathList().cbegin(); }
		auto	clast()												const	{ return getPathList().size() ? getPathList().cend() - 1 : getPathList().cend(); }
		
		auto	crbegin()											const	{ return getPathList().crbegin(); }
		auto	crend()												const	{ return getPathList().crend(); }
		auto	crfirst()											const	{ return getPathList().cbegin(); }
		auto	crlast()											const	{ return getPathList().size() ? getPathList().crend() - 1 : getPathList().crend(); }
		
		auto	cFirstPosEntryIterator()							const;
		auto	crLastPosEntryIterator()							const;
		bool	hasMovement()										const;
		
		auto	crLastValidEntryWithF()								const;
		auto	crLastValidEntryWithS()								const;
		
		long	firstClientID()										const;
		
		void	init(const CncDoublePosition& p);
		
		void	changeToGuideType(GuideType gt = ORIG_PATH);
		
		bool	processXYOffset			(double offset, JoinType = jtRound,  EndType = etClosedPolygon);
		bool	processXYInnerOffset	(double offset);
		bool	processXYOuterOffset	(double offset);
		bool	processXYPocket			(double toolDiameter);
		bool	processXYHelix			(double zCurDepth, double zTotDist, double zFeed360);
		
		bool	adjustZOffsetAbs		(double offset);
		bool	adjustZOffsetRel		(double offset);
		
		bool	roundXYCorners			(double toolDiameter);
		
		bool	normalizeStartPosDistance(const CncDoublePosition& pos = {0.0, 0.0, 0.0});
		bool	normalizeStartPos(const CncDoublePosition& pos = {0.0, 0.0, 0.0});
		bool	normalizeEndPos(const CncDoublePosition& pos = {0.0, 0.0, 0.0});
		size_t	normalizeLinkedEntry(const CncPathListEntry& ple);
		
		bool	ensureDirection(CncDirection d);
		bool	reversePath();
		
		void	initNextCncPath();
		void	initNextGuidePath(GuideType gt = HELP_PATH);
		
		const	CncPathListEntry& addEntryAdm(long clientId);
		const	CncPathListEntry& addEntryAdm(CncSpeedMode mode);
		const	CncPathListEntry& addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN);
		const	CncPathListEntry& addEntrySpl(CncSpindlePowerState spindleState);
		const	CncPathListEntry& addEntrySpl(double spindleSpeed_U_MIN);
		const	CncPathListEntry& addEntrySpl(CncSpindlePowerState spindelState, double spindleSpeed_U_MIN);
		const	CncPathListEntry& addEntryAbs(const CncDoublePosition& newAbsPos, bool alreadyRendered=false);
		const	CncPathListEntry& addEntryRel(const CncDoublePosition& distance,  bool alreadyRendered=false);
		const	CncPathListEntry& addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered=false);
		const	CncPathListEntry& addEntryRel(double deltaX,     double deltaY,     double deltaZ,     bool alreadyRendered=false);
		
		std::ostream& outputOperator(std::ostream &ostr) const;
		friend std::ostream& operator<<(std::ostream &ostr, const CncPathListManager &a) { return a.outputOperator(ostr); }
};

#endif


