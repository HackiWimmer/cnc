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
		
		CncToolCorretionType getCorrectionType() 	const { return toolPathCorrection; }
		bool isGuidePath()							const { return guidePath; }
		bool getReverseFlag() 						const { return reversePath; }
		bool isCurrentZDepthAbs()					const { return currentZDepthMode == 'Z'; }
		bool isCurrentZDepthRel()					const { return !isCurrentZDepthAbs(); }
		
		int getCurrentLineNumber()					const { return currentLineNumber; }
		
		double getCurrentZDepth() 					const { return currentZDepth; }
		char getCurrentZDepthMode()					const { return currentZDepthMode; }
		
		void getParameterList(DcmItemList& rows);
		const char* getCorrectionTypeAsString();
		const char* getZDeptAsString();
		
		void setCurrentLineNumber(int cln);
		void setCurrentZDepth(wxString& flag);
		void setReverseFlag(wxString& flag);
		void setCorrectionType(wxString& type);
		
		void setFillColour(const wxColour & col);
		void setStrokeColour(const wxColour & col);
		void determineColourEffects();
};

#endif