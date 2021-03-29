#ifndef MANUALLY_PATH_HANDLER_BASE_H
#define MANUALLY_PATH_HANDLER_BASE_H

#include "PathHandlerBase.h"
#include "CncControl.h"
#include "CncPosition.h"

class ManuallyPathHandlerCnc : public PathHandlerBase {
	
	public:
		
		struct MoveDefinition {
			
			enum MoveMode { MM_1D, MM_2D, MM_3D };
			
			CncSpeedMode speedMode	= CncSpeedMode::CncSpeedRapid;
			MoveMode moveMode		= MM_2D;
			bool absoluteMove 		= false;
			bool toolState 			= false;
			bool correctLimit   	= true;
			
			double f = 0.0;
			double x = 0.0;
			double y = 0.0;
			double z = 0.0;
			
			static MoveMode convert(const wxString& s, MoveMode defValue) {
				if      ( s.IsSameAs("1D") )	return MM_1D;
				else if ( s.IsSameAs("2D") )	return MM_2D;
				else if ( s.IsSameAs("3D") )	return MM_3D;
				else							return defValue;
			}
		};
		
		ManuallyPathHandlerCnc(CncControl* cnc);
		virtual ~ManuallyPathHandlerCnc();
		
		virtual const char*		getName() const { return "ManuallyPathHandlerCnc"; }
		
		virtual void 			logMeasurementStart();
		virtual void			logMeasurementEnd();
		
		virtual void			switchSpindleState(bool state);
		
		virtual bool			prepareWork(); 
		virtual bool			finishWork();
		
		virtual bool			processLinearMove(bool alreadyRendered);
		bool					processLinearMove(const MoveDefinition& md);
		
		void					swichtSpindleOn()  { switchSpindleState(true); }
		void					swichtSpindleOff() { switchSpindleState(false); }
		
	private:
		
		CncControl*				cncControl;
		CncSpeedMode			lastSpeedMode; 
		double					lastSpeedValue;
};

#endif