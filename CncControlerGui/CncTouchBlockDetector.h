#ifndef CNC_TOUCH_BLOCK_DETECTOR_H
#define CNC_TOUCH_BLOCK_DETECTOR_H

#include <wx/string.h>
#include "CncCommon.h"
#include "CncPosition.h"

class CncControl;
class CncTouchBlockDetector {
	
	public:
		
		enum ContactState	{ CS_UNKNOWN, CS_OPENED, CS_CLOSED };
		
		// ----------------------------------------------------------
		class CallbackInterface {

			public:
				CallbackInterface() {}
				virtual ~CallbackInterface() {}

				virtual void notifyProgess(const wxString& msg) {}
				virtual void notifyError(const wxString& msg) {}
		};

		struct Parameters {
			
			// ^ +Y 
			// | TC_LEFT_TOP                  TC_RIGHT_TOP
			// |
			// | TC_LEFT_BOTTOM            TC_RIGHT_BOTTOM
			// ----------------------------------------> +X
			
			enum TouchMode		{ TM_TOUCH_TEST, TM_TOUCH_Z, TM_TOUCH_XYZ };
			enum TouchCorner	{ TC_LEFT_BOTTOM, TC_RIGHT_BOTTOM, TC_LEFT_TOP, TC_RIGHT_TOP };
			
			CallbackInterface* caller				= NULL;
			
			double		touchDiameter				= 0.0;
			TouchMode	touchMode					= TM_TOUCH_Z;
			TouchCorner	touchCorner					= TC_LEFT_BOTTOM;
			
			double		touchBlockZThickness		=  0.0;
			double		touchBlockXCheekThickness	=  0.0;
			double		touchBlockYCheekThickness	=  0.0;
		};
		
		struct Result {
			
			bool			processResult			= false;
			wxString		errorInfo				= "";
			
			ContactState	contactState			= CS_UNKNOWN;
			CncLongPosition	newCurrentPos			= {0, 0, 0};
			
			bool			hasErrorInfo()			const	{ return errorInfo.IsEmpty() == false; }
			
			CncLongOffset	getMesurementOffset() 	const {
				CncLongOffset ret;
				ret.setX(newCurrentPos.getX());
				ret.setY(newCurrentPos.getY());
				ret.setZ(newCurrentPos.getZ());
				
				return ret;
			}
		};
		
	private:
		
		typedef Parameters::TouchCorner Corner;
		
		const double DefaultDist					=  0.0;
		const double moveAxisFreeDist_MM			=  2.0;
		const double moveUntilContactWidth_MM		= 20.0;
		
		CncControl*			cnc;
		Parameters			parameters;
		CncLongPosition		startPos;
		CncLongPosition		touchPos;
		CncLongPosition		newCurrentPos;

		wxString			errorCodes;
		CncSpeedMode		prevSpeedMode;
		float				prevSpeedValue;

		ContactState		contactState;
		
		bool hasError()											const	{ return errorCodes.IsEmpty() == false; }
		
		const CncLongPosition requestControllerPos();
		
		bool checkEnvrionment();
		bool checkParameters();
		bool evaluateContact();
		bool evaluateCurrentValues();
		bool reduceSpeed();
		bool checkResult();
		bool finalize();
		
		bool findStartPosX();
		bool findStartPosY();
		
		bool determineNewCurrentPosX();
		bool determineNewCurrentPosY();
		bool determineNewCurrentPosZ();
		
		bool moveXFree(const wxString& errMsg);
		bool moveYFree(const wxString& errMsg);
		bool moveZFree(const wxString& errMsg);
		
		bool moveUntilContact(const CncAxis axis, const double dist, const wxString& errMsg);
		bool moveRel(int32_t dx, int32_t dy, int32_t dz, const wxString& errMsg);
		bool moveAbs(int32_t px, int32_t py, int32_t pz, const wxString& errMsg);
		
	public:
		
		CncTouchBlockDetector(CncControl * cc);
		~CncTouchBlockDetector();
		
		const wxString&				getErrorCodes()				const	{ return errorCodes; }
		const CncLongPosition&		getStartPos()				const	{ return startPos; }
		const CncLongPosition&		getNewCurrentPos()			const	{ return newCurrentPos; }
		const ContactState			getContactState()			const	{ return contactState; }

		bool						touch(const Parameters& para);
};

#endif