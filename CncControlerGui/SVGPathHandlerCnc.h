#ifndef SVG_PATH_HANDLER_CNC_H
#define SVG_PATH_HANDLER_CNC_H

#include "PathHandlerBase.h"
#include "SerialPort.h"
#include "CncToolCorrection.h"
#include "CncWorkingParameters.h"
#include "CncCommon.h"


class CncControl;
class wxDataViewListCtrl;

enum SvgPhProcessMode {SvgPhController, SvgPhDebug};

//////////////////////////////////////////////////////////////////
class SVGPathHandlerCnc : public PathHandlerBase {
//////////////////////////////////////////////////////////////////

	private:
	
		SVGUnit 			unit;
		CncControl* 		cncControl;
		SvgPhProcessMode 	processMode;
		SvgOriginalPathInfo origPathInfo;
		double 				toolRadius;
		bool 				zAxisDown;
		bool 				initialized;
		bool 				debugState;
		double 				width, height;
		wxString 			viewBox;
		CncWorkingParameters currentCncParameters;
		
		// spool path to cnc control
		bool moveLinearXY(double x, double y, bool alreadyRendered);
		
		// path handling
		bool beginCurrentPath();
		bool repeatCurrentPath();
		inline bool spoolCurrentPathWrapper(bool firstRun);
		bool spoolCurrentPath(bool firstRun);
		bool closeCurrentPath();
		
	protected:
	
		virtual bool shouldConvertRefToMM() const { return true; }
		
		// debug functions
		virtual void appendDebugValueDetail(const char* key, wxVariant value);
		virtual void appendDebugValueDetail(const CncPathListEntry& cpe);
		virtual void debugCurrentPosition();
		
		virtual bool isInitialized();
		
		// z axis management
		virtual void simulateZAxisUp();
		virtual void simulateZAxisDown();
		virtual bool isZAxisUp();
		virtual bool isZAxisDown();
		bool moveUpZ();
		bool moveDownZ();

	public:
		SVGPathHandlerCnc(CncControl* cnc);
		virtual ~SVGPathHandlerCnc();
		
		virtual void initNextClientId(long id);
		
		// Getter and setter
		void setMaxDimensions(SVGUnit u, double width, double height);
		void setViewBox(const wxString& vb);
		
		SVGUnit getSVGUnit() { return unit; }
		double getW() { return width; }
		double getH() { return height; }
		const char* getViewBox() { return viewBox.c_str(); }

		CncWorkingParameters& getCncWorkingParameters();
		void setCncWorkingParameters(CncWorkingParameters& cwp);
		
		void setProcessMode(SvgPhProcessMode pm);
		const SvgPhProcessMode getProcessMode() { return processMode; }
		
		void setDebugState(bool state) { debugState = state; }
		
		// path handling
		virtual void prepareWork();
		virtual bool initNextPath();
		virtual bool initNextPath(const SvgOriginalPathInfo& sopi);
		virtual bool finishCurrentPath();
		virtual bool runCurrentPath();
		virtual void finishWork();
		
};

#endif