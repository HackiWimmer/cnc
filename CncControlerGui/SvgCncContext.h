#ifndef CncWorkingParameters_H
#define CncWorkingParameters_H

#include "CncCommon.h"
#include "SvgColourScheme.h"
#include "DataControlModel.h"

//////////////////////////////////////////////////////////////////
struct SvgCncBreak {
	public:
		int currentLineNumber		= UNDEFINED_LINE_NUMBER;
};

//////////////////////////////////////////////////////////////////
struct SvgCncPause {
	public:
		int currentLineNumber		= UNDEFINED_LINE_NUMBER;
		int64_t microseconds		= 0LL;
};

//////////////////////////////////////////////////////////////////
class SvgCncContext {
	
	private:
		
		bool					reversePath;
		bool					guidePath;
		
		double					currentZDepth;
		char					currentZDepthMode;
		int						currentLineNumber;
		
		CncToolCorretionType	toolPathCorrection;
		SvgColourDecoder		fillColourDecoder;
		SvgColourDecoder		strokeColourDecoder;
		
	public:
		
		SvgCncContext();
		~SvgCncContext();
		
		CncToolCorretionType	getToolCorrectionType()					const { return toolPathCorrection; }
		const char* 			getToolCorrectionTypeAsStr()			const;
		
		bool					hasSomethingToCorrect()					const;
		bool					hasToolCorrection()						const;
		bool					hasReverseCorrection() 					const	{ return reversePath; }
		
		bool					isGuidePath()							const	{ return guidePath; }
		bool					isCurrentZDepthAbs()					const	{ return currentZDepthMode == 'Z'; }
		bool					isCurrentZDepthRel()					const	{ return !isCurrentZDepthAbs(); }
		
		int						getCurrentLineNumber()					const	{ return currentLineNumber; }
		double					getCurrentZDepth() 						const	{ return currentZDepth; }
		char					getCurrentZDepthMode()					const	{ return currentZDepthMode; }
		const char* 			getZDeptAsStr()							const;
		
		void 					getParameterList(DcmItemList& rows)		const;
		
		void 					setCurrentLineNumber(int cln);
		void					setCurrentZDepth(const wxString& flag);
		void					setReverseFlag(const wxString& flag);
		void					setToolCorrectionType(const wxString& type);
		
		void setFillColour(const wxColour & col);
		void setStrokeColour(const wxColour & col);
		void determineColourEffects();
};

#endif