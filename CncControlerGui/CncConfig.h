#ifndef CNC_CONFIG_H
#define CNC_CONFIG_H

#include <iostream>
#include <wx/vector.h>
#include "DataControlModel.h"
#include "CncCommon.h"

class CncConfig {
	private:
		bool changed;
		CncUnit unit;
		double maxDimensionsX;
		double maxDimensionsY;
		double maxDimensionsZ;
		unsigned int stepsX, stepsY, stepsZ;
		unsigned int multiplierX, multiplierY, multiplierZ;
		unsigned int maxSpeedXY, maxSpeedZ;
		unsigned int flySpeedXY, workSpeedXY, flySpeedZ, workSpeedZ;
		unsigned int speedX, speedY, speedZ;
		unsigned int pulsWidthOffsetX, pulsWidthOffsetY, pulsWidthOffsetZ;
		double pitchX, pitchY, pitchZ;
		double dispFactX, dispFactY, dispFactZ;
		double calcFactX, calcFactY, calcFactZ;

		static const unsigned int maxDurations = 32;
		unsigned int durationCount;
		double durationSteps[maxDurations];
		double workpieceOffset;
		double maxDurationThickness;
		double workpieceThickness;
		double maxZDistance;
		bool referenceIncludesWpt;

		double routerBitDiameter;
		double drawPaneZoomFactor;
		
		// tracing/monitoring options
		bool onlineUpdateCoordinates;
		bool onlineUpdateDrawPane;
		bool allowEventHandling;
		int updateInterval;
		
		unsigned int replyThreshold;
		
		int stepSignX;
		int stepSignY;
		
		double curveLibResolution;

		////////////////////////////////////////////////////////////////////////
		void sc() { changed = true; }
		void rc() { changed = false; }
		
		////////////////////////////////////////////////////////////////////////
		CncConfig& setSpeedX(unsigned int val) { sc(); speedX= val; return *this; }
		CncConfig& setSpeedY(unsigned int val) { sc(); speedY= val; return *this; }
		CncConfig& setSpeedZ(unsigned int val) { sc(); speedZ= val; return *this; }
		
	public:
		////////////////////////////////////////////////////////////////////////
		CncConfig() : changed(true)
					, unit(CncSteps)
					, maxDimensionsX(0.0)
					, maxDimensionsY(0.0)
					, maxDimensionsZ(0.0)
					, stepsX(200), stepsY(200), stepsZ(200)
					, multiplierX(1), multiplierY(1), multiplierZ(1) 
					, maxSpeedXY(100), maxSpeedZ(100)
					, flySpeedXY(100), workSpeedXY(100), flySpeedZ(100), workSpeedZ(100)
					, speedX(1), speedY(1), speedZ(1)
					, pulsWidthOffsetX(100), pulsWidthOffsetY(100), pulsWidthOffsetZ(100)
					, pitchX(2.0), pitchY(2.0), pitchZ(2.0)
					, workpieceOffset(5.0)
					, maxDurationThickness(2.0)
					, workpieceThickness(0.0)
					, maxZDistance(50.0)
					, referenceIncludesWpt(false)
					, routerBitDiameter(0.0)
					, drawPaneZoomFactor(1)
					, onlineUpdateCoordinates(true)
					, onlineUpdateDrawPane(true)
					, allowEventHandling(true)
					, updateInterval(100)
					, replyThreshold(100)
					, stepSignX(1)
					, stepSignY(1)
					, curveLibResolution(0.09)
		{
			calculateFactors();
			initZAxisValues();
		}
		
		CncConfig(CncConfig& cc) 
					: changed(true)
					, unit(cc.getUnit())
					, maxDimensionsX(cc.getMaxDimensionX())
					, maxDimensionsY(cc.getMaxDimensionY())
					, maxDimensionsZ(cc.getMaxDimensionZ())
					, stepsX(cc.getStepsX()), stepsY(cc.getStepsY()), stepsZ(cc.getStepsX())
					, multiplierX(cc.getMultiplierX()), multiplierY(cc.getMultiplierY()), multiplierZ(cc.getMultiplierZ()) 
					, maxSpeedXY(cc.getMaxSpeedXY()), maxSpeedZ(cc.getMaxSpeedZ())
					, flySpeedXY(cc.getFlySpeedXY()), workSpeedXY(cc.getWorkSpeedXY())
					, flySpeedZ(cc.getFlySpeedZ()), workSpeedZ(cc.getWorkSpeedZ())
					, speedX(cc.getSpeedX()), speedY(cc.getSpeedY()), speedZ(cc.getSpeedX())
					, pulsWidthOffsetX(cc.getPulsWidthOffsetX()), pulsWidthOffsetY(cc.getPulsWidthOffsetY()), pulsWidthOffsetZ(cc.getPulsWidthOffsetZ())
					, pitchX(cc.getPitchX()), pitchY(cc.getPitchY()), pitchZ(cc.getPitchZ())
					, workpieceOffset(cc.getWorkpieceOffset())
					, maxDurationThickness(cc.getMaxDurationThickness())
					, workpieceThickness(cc.getWorkpieceThickness())
					, maxZDistance(cc.getMaxZDistance())
					, referenceIncludesWpt(cc.getReferenceIncludesWpt())
					, routerBitDiameter(cc.getRouterBitDiameter())
					, drawPaneZoomFactor(cc.getDrawPaneZoomFactor())
					, onlineUpdateCoordinates(cc.isOnlineUpdateCoordinates())
					, onlineUpdateDrawPane(cc.isOnlineUpdateDrawPane())
					, allowEventHandling(cc.isAllowEventHandling())
					, updateInterval(cc.getUpdateInterval())
					, replyThreshold(cc.getRelyThreshold())
					, stepSignX(cc.getStepSignX())
					, stepSignY(cc.getStepSignY())
					, curveLibResolution(cc.getCurveLibResolution())
		{
			calculateFactors();
			initZAxisValues();
		}
		////////////////////////////////////////////////////////////////////////
		virtual ~CncConfig() {}
		////////////////////////////////////////////////////////////////////////
		void calculateFactors() {
			dispFactX = 0.0 + (pitchX/stepsX);
			dispFactY = 0.0 + (pitchY/stepsY);
			dispFactZ = 0.0 + (pitchZ/stepsZ);
			
			calcFactX = 0.0 + (stepsX/pitchX);
			calcFactY = 0.0 + (stepsY/pitchY);
			calcFactZ = 0.0 + (stepsZ/pitchZ);
		}
		////////////////////////////////////////////////////////////////////////
		const bool isModified() { return changed; }
		
		////////////////////////////////////////////////////////////////////////
		void discardModifications() { changed = false; }
		
		////////////////////////////////////////////////////////////////////////
		unsigned int getRelyThreshold() { return replyThreshold; }
		CncConfig& setRelyThreshold(unsigned int rt) { replyThreshold = rt; return *this; }
		
		////////////////////////////////////////////////////////////////////////
		void getStaticValues(wxVector<wxVector<wxVariant>>& ret) {
			DataControlModel::addKeyValueRow(ret, "Steps (x)", 				(int)stepsX);
			DataControlModel::addKeyValueRow(ret, "Steps (y)", 				(int)stepsY);
			DataControlModel::addKeyValueRow(ret, "Steps (z)", 				(int)stepsZ);
			DataControlModel::addKeyValueRow(ret, "Puls width offset (x)", 	(int)pulsWidthOffsetX);
			DataControlModel::addKeyValueRow(ret, "Puls width offset (y)", 	(int)pulsWidthOffsetY);
			DataControlModel::addKeyValueRow(ret, "Puls width offset (z)", 	(int)pulsWidthOffsetZ);
			DataControlModel::addKeyValueRow(ret, "Pitch (x)", 				pitchX);
			DataControlModel::addKeyValueRow(ret, "Pitch (y)", 				pitchY);
			DataControlModel::addKeyValueRow(ret, "Pitch (z)",				pitchZ);
			DataControlModel::addKeyValueRow(ret, "Multiplier (x)", 		(int)multiplierX);
			DataControlModel::addKeyValueRow(ret, "Multiplier (y)", 		(int)multiplierY);
			DataControlModel::addKeyValueRow(ret, "Multiplier (z)", 		(int)multiplierZ);
			DataControlModel::addKeyValueRow(ret, "Max speed XY", 			(int)maxSpeedXY);
			DataControlModel::addKeyValueRow(ret, "Fly speed XY", 			(int)flySpeedXY);
			DataControlModel::addKeyValueRow(ret, "Work speed XY", 			(int)workSpeedXY);
			DataControlModel::addKeyValueRow(ret, "Max speed Z", 			(int)maxSpeedZ);
			DataControlModel::addKeyValueRow(ret, "Fly speed Z", 			(int)flySpeedZ);
			DataControlModel::addKeyValueRow(ret, "Work speed Z", 			(int)workSpeedZ);
		}
		////////////////////////////////////////////////////////////////////////
		void getDynamicValues(wxVector<wxVector<wxVariant>>& ret) {
			DataControlModel::addKeyValueRow(ret, "Tool diameter", 				routerBitDiameter);
			DataControlModel::addKeyValueRow(ret, "Curve lib resolution", 		(double)getCurveLibResolution());
			DataControlModel::addKeyValueRow(ret, "Current Speed (x)", 			(int)speedX);
			DataControlModel::addKeyValueRow(ret, "Current Speed (y)", 			(int)speedY);
			DataControlModel::addKeyValueRow(ret, "Current Speed (z)", 			(int)speedZ);
			DataControlModel::addKeyValueRow(ret, "Max Dimension (x)", 			maxDimensionsX);
			DataControlModel::addKeyValueRow(ret, "Max Dimension (y)", 			maxDimensionsY);
			DataControlModel::addKeyValueRow(ret, "Step Sign (x)", 				stepSignX);
			DataControlModel::addKeyValueRow(ret, "Step Sign (y)", 				stepSignY);
			DataControlModel::addKeyValueRow(ret, "Reply Threshold", 			(int)replyThreshold);
			DataControlModel::addKeyValueRow(ret, "Z axis values:", 			"");
			DataControlModel::addKeyValueRow(ret, "  Max durations", 			(int)maxDurations);
			DataControlModel::addKeyValueRow(ret, "  Duration count", 			(int)durationCount);
			DataControlModel::addKeyValueRow(ret, "  Workpiece offset", 		workpieceOffset);
			DataControlModel::addKeyValueRow(ret, "  Max duration thickness",	 maxDurationThickness);
			DataControlModel::addKeyValueRow(ret, "  Calculated durations", 	(int)durationCount);
			DataControlModel::addKeyValueRow(ret, "  Current Z distance", 		getCurZDistance());
			DataControlModel::addKeyValueRow(ret, "  Wpt is included", 			getReferenceIncludesWpt());
			
			for (unsigned int i=0; i<maxDurations; i++) {
				if ( durationSteps[i] != 0.0 ) {
					wxString key("  Duration step[");
					key << i;
					key << "]";
					DataControlModel::addKeyValueRow(ret, key, durationSteps[i]);
				}
			}
		}
		////////////////////////////////////////////////////////////////////////
		bool getReferenceIncludesWpt() { return referenceIncludesWpt; }
		CncConfig& setReferenceIncludesWpt(bool b) { sc(); referenceIncludesWpt = b; return *this; }
		void initZAxisValues() {
			// example:
			// workpieceOffset		= 1.0;
			// maxDurationThickness	= 2.0;
			// workpieceThickness	= 6.4;
			
			for (unsigned int i=0; i<maxDurations; i++ ) {
				durationSteps[i] = 0.0;
			}

			durationCount = 0;
			if ( maxDurationThickness > 0 ) {
				durationCount = workpieceThickness/maxDurationThickness;					// --> 3
				if ( durationCount > 0 ) {
					double rest = workpieceThickness - (durationCount * maxDurationThickness);	//--> 0.4
					
					if ( durationCount < maxDurations ) {
						for (unsigned int i=0; i<maxDurations; i++ ) {
							if ( i < durationCount )
								durationSteps[i] = maxDurationThickness;
							else
								durationSteps[i] = 0.0;
						}

						if ( rest > 0 ) {
							durationSteps[durationCount] = rest;					// --> add 0.4
							durationCount++;										// --> 3 + 1 = 4 durations (3 * 2mm + 1 * 0.4mm )
						}
					} else {
						std::cerr << "CncConfig: maxDurations reached: " << durationCount << std::endl;
					}
				} else {
					// no workpieceThickness
					// set durationCount = 1 to path around one time for plotting;
					durationCount = 1;
					durationSteps[0] = 0.0;
				}
			} else {
				std::cerr << "CncConfig: Invalid maxDurationThickness: " << maxDurationThickness << std::endl;
			}	
		}
		////////////////////////////////////////////////////////////////////////
		CncConfig& setActiveSpeedXY(CncSpeed s) {
			switch( s ) {
				
				case CncSpeedWork: 	setSpeedX(getWorkSpeedXY());
									setSpeedY(getWorkSpeedXY());
									break;
									
				case CncSpeedFly:	setSpeedX(getFlySpeedXY());
									setSpeedY(getFlySpeedXY());
									break;
									
				default:
									setSpeedX(1);
									setSpeedY(1);
			}
			return *this;
		}
		////////////////////////////////////////////////////////////////////////
		CncConfig& setActiveSpeedZ(CncSpeed s) {
			switch( s ) {
				
				case CncSpeedWork: 	setSpeedZ(getWorkSpeedZ());
									break;
									
				case CncSpeedFly:	setSpeedZ(getFlySpeedZ());
									break;
									
				default:
									setSpeedZ(1);
			}
			return *this;
		}
		
		////////////////////////////////////////////////////////////////////////
		double getCurveLibResolution() { return curveLibResolution; }
		CncConfig& setCurveLibResolution(double v) { sc(); curveLibResolution = v; return * this; }
		
		////////////////////////////////////////////////////////////////////////
		double convertX(CncUnit oldUnit, CncUnit newUnit, double value) {
			if ( oldUnit == CncMetric && newUnit == CncSteps ) {
				return value * calcFactX;
			} else if ( oldUnit == CncSteps && newUnit == CncMetric) {
				return value * dispFactX;
			}
			return value;
		}
		double convertY(CncUnit oldUnit, CncUnit newUnit, double value) {
			if ( oldUnit == CncMetric && newUnit == CncSteps ) {
				return value * calcFactY;
			} else if ( oldUnit == CncSteps && newUnit == CncMetric) {
				return value * dispFactY;
			}
			return value;
		}
		double convertZ(CncUnit oldUnit, CncUnit newUnit, double value) {
			if ( oldUnit == CncMetric && newUnit == CncSteps ) {
				return value * calcFactZ;
			} else if ( oldUnit == CncSteps && newUnit == CncMetric) {
				return value * dispFactZ;
			}
			return value;
		}
		////////////////////////////////////////////////////////////////////////
		int getStepSignX() { return stepSignX; }
		int getStepSignY() { return stepSignY; }
		CncConfig& setStepSignX(int value) {
			if ( value >= 0 )	stepSignX = +1;
			else				stepSignX = -1;
			return *this;
		}
		CncConfig& setStepSignY(int value) {
			if ( value >= 0 )	stepSignY = +1;
			else				stepSignY = -1;
			return *this;
		}
		////////////////////////////////////////////////////////////////////////
		CncUnit getUnit(void) { return unit; }
		CncConfig& setUnit(CncUnit u) { sc(); unit = u; return *this;}
		////////////////////////////////////////////////////////////////////////
		const double getMaxDimensionX() { return maxDimensionsX; }
		CncConfig& setMaxDimensionX(double val) { sc(); maxDimensionsX = val; return *this; }
		const double getMaxDimensionY() { return maxDimensionsY; }
		CncConfig& setMaxDimensionY(double val) { sc(); maxDimensionsY = val; return *this; }
		const double getMaxDimensionZ() { return maxDimensionsZ; }
		CncConfig& setMaxDimensionZ(double val) { sc(); maxDimensionsZ = val; return *this; }
		////////////////////////////////////////////////////////////////////////
		CncConfig& setStepsX(unsigned int v) { sc(); stepsX = v; calculateFactors(); return *this; }
		CncConfig& setStepsY(unsigned int v) { sc(); stepsY = v; calculateFactors(); return *this; }
		CncConfig& setStepsZ(unsigned int v) { sc(); stepsZ = v; calculateFactors(); return *this; }
		unsigned int getStepsX(void) { return stepsX; }
		unsigned int getStepsY(void) { return stepsY; }
		unsigned int getStepsZ(void) { return stepsZ; }
		////////////////////////////////////////////////////////////////////////
		CncConfig& setPulsWidthOffsetX(unsigned int v) { sc(); pulsWidthOffsetX = v; return *this; }
		CncConfig& setPulsWidthOffsetY(unsigned int v) { sc(); pulsWidthOffsetY = v; return *this; }
		CncConfig& setPulsWidthOffsetZ(unsigned int v) { sc(); pulsWidthOffsetZ = v; return *this; }
		unsigned int getPulsWidthOffsetX(void) { return pulsWidthOffsetX; }
		unsigned int getPulsWidthOffsetY(void) { return pulsWidthOffsetY; }
		unsigned int getPulsWidthOffsetZ(void) { return pulsWidthOffsetZ; }
		////////////////////////////////////////////////////////////////////////
		CncConfig& setPitchX(double val) { sc(); pitchX = val; calculateFactors(); return *this; }
		CncConfig& setPitchY(double val) { sc(); pitchY = val; calculateFactors(); return *this; }
		CncConfig& setPitchZ(double val) { sc(); pitchZ = val; calculateFactors(); return *this; }
		double getPitchX(void) { return pitchX; }
		double getPitchY(void) { return pitchY; }
		double getPitchZ(void) { return pitchZ; }
		////////////////////////////////////////////////////////////////////////
		int getMaxSpeedXY() { return maxSpeedXY; }
		CncConfig& setMaxSpeedXY(int s) { sc(); maxSpeedXY = s; return *this; }
		int getMaxSpeedZ() { return maxSpeedZ; }
		CncConfig& setMaxSpeedZ(int s) { sc(); maxSpeedZ = s; return *this; }
		////////////////////////////////////////////////////////////////////////
		int getFlySpeedXY() { return flySpeedXY; }
		CncConfig& setFlySpeedXY(int s) { sc(); flySpeedXY = s; return *this; }
		int getWorkSpeedXY() { return workSpeedXY; }
		CncConfig& setWorkSpeedXY(int s) { sc(); workSpeedXY = s; return *this; }
		////////////////////////////////////////////////////////////////////////
		int getFlySpeedZ() { return flySpeedZ; }
		CncConfig& setFlySpeedZ(int s) { sc(); flySpeedZ = s; return *this; }
		int getWorkSpeedZ() { return workSpeedZ; }
		CncConfig& setWorkSpeedZ(int s) { sc(); workSpeedZ = s; return *this; }
		////////////////////////////////////////////////////////////////////////
		unsigned int getSpeedX(void) { return speedX; }
		unsigned int getSpeedY(void) { return speedY; }
		unsigned int getSpeedZ(void) { return speedZ; }
		////////////////////////////////////////////////////////////////////////
		CncConfig& setMultiplierX(unsigned int val) { sc(); multiplierX = val; return *this; }
		CncConfig& setMultiplierY(unsigned int val) { sc(); multiplierX = val; return *this; }
		CncConfig& setMultiplierZ(unsigned int val) { sc(); multiplierX = val; return *this; }
		unsigned int getMultiplierX(void) { return multiplierX; }
		unsigned int getMultiplierY(void) { return multiplierY; }
		unsigned int getMultiplierZ(void) { return multiplierZ; }
		////////////////////////////////////////////////////////////////////////
		double getDisplayFactX(void) { return dispFactX; }
		double getDisplayFactY(void) { return dispFactY; }
		double getDisplayFactZ(void) { return dispFactZ; }
		////////////////////////////////////////////////////////////////////////
		double getCalculationFactX(void) { return calcFactX; }
		double getCalculationFactY(void) { return calcFactY; }
		double getCalculationFactZ(void) { return calcFactZ; }
		////////////////////////////////////////////////////////////////////////
		const unsigned int	getMaxDurations() 			{ return maxDurations; }
		unsigned int		getDurationCount() 			{ return durationCount; }
		double				getWorkpieceOffset()		{ return workpieceOffset; }
		double				getMaxDurationThickness()	{ return maxDurationThickness; }
		double				getWorkpieceThickness()		{ return workpieceThickness; }
		double				getMaxZDistance()			{ return maxZDistance; }
		void				setMaxZDistance(double d)	{ maxZDistance = d; }
		double				getCurZDistance() 			{ return workpieceThickness + workpieceOffset; }
		////////////////////////////////////////////////////////////////////////
		double setWorkpieceThickness(double wpt) {
			sc();
			if ( wpt < 0 ) 
				wpt = 0;

			workpieceThickness = wpt;
			initZAxisValues();
			return wpt;
		}
		////////////////////////////////////////////////////////////////////////
		double setMaxDurationThickness(double mdt) {
			sc();
			if ( mdt < 0 ) 
				mdt = 2.0;

			maxDurationThickness = mdt;
			initZAxisValues();
			return mdt;
		}////////////////////////////////////////////////////////////////////////
		double getDurationThickness(unsigned int duration) {
			if ( duration < maxDurations ) {
				return durationSteps[duration];
			}
			return 0.0;
		}
		////////////////////////////////////////////////////////////////////////
		double getDurationPositionAbs(unsigned int duration) {
			if ( duration < maxDurations ) {
				double ret = workpieceOffset;
				for (unsigned int i=0; i<duration ;i++) {
					ret += durationSteps[i];
				}
				
				return getCurZDistance() - ret;
			}
			return 0.0;
		}
		////////////////////////////////////////////////////////////////////////
		void setRouterBitDiameter(double d) {
			sc(); 
			if ( d < 0 )
				d = 0;
			routerBitDiameter = d;
		}
		////////////////////////////////////////////////////////////////////////
		double getRouterBitDiameter() {
			return routerBitDiameter;
		}
		
		////////////////////////////////////////////////////////////////////////
		bool isOnlineUpdateCoordinates() { return onlineUpdateCoordinates; }
		CncConfig& setOnlineUpdateCoordinates(bool b) { sc(); onlineUpdateCoordinates=b; return *this; }

		bool isOnlineUpdateDrawPane() { return onlineUpdateDrawPane; }
		CncConfig& setOnlineUpdateDrawPane(bool b) { sc(); onlineUpdateDrawPane=b; return *this; }
		
		bool isAllowEventHandling() { return allowEventHandling; }
		CncConfig& setAllowEventHandling(bool b) { sc(); allowEventHandling=b; return *this; }
		
		int getUpdateInterval() { return updateInterval; }
		CncConfig& setUpdateInterval(int i) { sc(); updateInterval=i; return *this; }
		
		////////////////////////////////////////////////////////////////////////
		double getDrawPaneZoomFactor() { return drawPaneZoomFactor; }
		CncConfig& setDrawPaneZoomFactor(double f) { sc(); drawPaneZoomFactor = f; return *this; }
};

#endif
