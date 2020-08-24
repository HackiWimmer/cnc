#ifndef LIMIT_STATES_H_INCLUDED
#define LIMIT_STATES_H_INCLUDED

#include <iostream>
#include "CncCommon.h"

class CncLimitStates {
	
	private:
		bool xMinLimit;
		bool xMaxLimit;
		bool yMinLimit;
		bool yMaxLimit;
		bool zMinLimit;
		bool zMaxLimit;
		
		bool previousHasLimit;
	
		///////////////////////////////////////////////////////////////////
		void setLimit(int32_t v, bool &min, bool &max) {
			switch ( v ) {
				case LimitSwitch::LIMIT_MIN:
					min = true;
					max = false;
					break;
				case LimitSwitch::LIMIT_MAX:
					min = false;
					max = true;
					break;
				case LimitSwitch::LIMIT_UNSET:
					min= false;
					max = false;
					break;
				case LimitSwitch::LIMIT_UNKNOWN:
				default: 
					min = true;
					max = true;
					break;
			}
		}
		
		///////////////////////////////////////////////////////////////////
		int32_t getLimit(bool min, bool max) const {
			
			if ( min == true && max == true )
				return LimitSwitch::LIMIT_UNKNOWN;
			
			if ( min == false && max == false )
				return LimitSwitch::LIMIT_UNSET;
				
			if ( min == true )
				return LimitSwitch::LIMIT_MIN;
				
			if ( max == true )
				return LimitSwitch::LIMIT_MAX;
				
			return LimitSwitch::LIMIT_UNKNOWN;
		}
		
		///////////////////////////////////////////////////////////////////
		wxString& getLimitString(wxString& ret, int32_t limit) const {
			ret.clear();
			switch ( limit ) {
				case LimitSwitch::LIMIT_UNSET:
							ret = "No limit active.";
							break;
				case LimitSwitch::LIMIT_MIN:
							ret = "Min switch is active.";
							break;
				case LimitSwitch::LIMIT_MAX:
							ret = "Max switch is active.";
							break;
				case LimitSwitch::LIMIT_UNKNOWN:
							ret = "Limit active, but the position is unclear.";
							break;
				default:	ret = "Unknown limit state!";
			}
			
			return ret;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		CncLimitStates() 
		: xMinLimit(false)
		, xMaxLimit(false)
		, yMinLimit(false)
		, yMaxLimit(false)
		, zMinLimit(false)
		, zMaxLimit(false)
		, previousHasLimit(false)
		{}
		
		~CncLimitStates() 
		{}
		
		///////////////////////////////////////////////////////////////////
		const bool getXMinLimit() const { return xMinLimit; }
		const bool getXMaxLimit() const { return xMaxLimit; }
		const bool getYMinLimit() const { return yMinLimit; }
		const bool getYMaxLimit() const { return yMaxLimit; }
		const bool getZMinLimit() const { return zMinLimit; }
		const bool getZMaxLimit() const { return zMaxLimit; }
		
		///////////////////////////////////////////////////////////////////
		const int32_t getXLimit() const { return getLimit(xMinLimit, xMaxLimit); }
		const int32_t getYLimit() const { return getLimit(yMinLimit, yMaxLimit); }
		const int32_t getZLimit() const { return getLimit(zMinLimit, zMaxLimit); }
		
		///////////////////////////////////////////////////////////////////
		void reset() {
			previousHasLimit = hasLimit();
			
			xMinLimit = false;
			xMaxLimit = false;
			yMinLimit = false;
			yMaxLimit = false;
			zMinLimit = false;
			zMaxLimit = false;
		}
		
		///////////////////////////////////////////////////////////////////
		const bool hasPreviousLimit() const { return previousHasLimit; }
		
		///////////////////////////////////////////////////////////////////
		const bool hasLimit() const {
			return (xMinLimit || xMaxLimit || yMinLimit || yMaxLimit || zMinLimit || zMaxLimit);
		}
		///////////////////////////////////////////////////////////////////
		const bool isXLimitStateValid() const { return !( xMinLimit == true && xMaxLimit == true ); }
		const bool isYLimitStateValid() const { return !( yMinLimit == true && yMaxLimit == true ); }
		const bool isZLimitStateValid() const { return !( zMinLimit == true && zMaxLimit == true ); }
		
		///////////////////////////////////////////////////////////////////
		void setXMinLimit(bool l) { xMinLimit = l; }
		void setXMaxLimit(bool l) { xMaxLimit = l; }
		void setYMinLimit(bool l) { yMinLimit = l; }
		void setYMaxLimit(bool l) { yMaxLimit = l; }
		void setZMinLimit(bool l) { zMinLimit = l; }
		void setZMaxLimit(bool l) { zMaxLimit = l; }
		
		///////////////////////////////////////////////////////////////////
		void setXLimit(int32_t v) { setLimit(v, xMinLimit, xMaxLimit); }
		void setYLimit(int32_t v) { setLimit(v, yMinLimit, yMaxLimit); }
		void setZLimit(int32_t v) { setLimit(v, zMinLimit, zMaxLimit); }
		
		void setLimitX(int32_t v) { setXLimit(v); }
		void setLimitY(int32_t v) { setYLimit(v); }
		void setLimitZ(int32_t v) { setZLimit(v); }
		
		///////////////////////////////////////////////////////////////////
		void setXYZLimit(const CncLongPosition& limitInfo) {
			setLimit(limitInfo.getX(), xMinLimit, xMaxLimit);
			setLimit(limitInfo.getY(), yMinLimit, yMaxLimit);
			setLimit(limitInfo.getZ(), zMinLimit, zMaxLimit);
		}
		
		///////////////////////////////////////////////////////////////////
		wxString& getLimitInfoString(wxString& ret) const {
			wxString ls;
			
			ret.clear();
			ret  = "X Axis: ";
			ret += getLimitString(ls, getXLimit());
			ret += "\n";
			
			ret += "Y Axis: ";
			ret += getLimitString(ls, getYLimit());
			ret += "\n";
			
			ret += "Z Axis: ";
			ret += getLimitString(ls, getZLimit());
			ret += "\n";
			
			return ret;
		}
		
		///////////////////////////////////////////////////////////////////
		void traceLimitInfo() {
			
			if ( hasLimit() == false ) {
				wxString m(cnc::trc.getCurrentMessage());
				if ( m.StartsWith("Limit state for") )
					cnc::trc.clear();
					
				return;
			}
			
			// -------------------------------------------------------------
			auto append = [](wxString& ret, const wxString& append) {
				if ( ret.IsEmpty() ) 	ret.append(append);
				else					ret.append(wxString::Format(", %s", append));
			};
			
			wxString invalid("");
			wxString unknown("");
			
			if ( isXLimitStateValid() == false )				append(invalid, "X");
			if ( isYLimitStateValid() == false )				append(invalid, "Y");
			if ( isZLimitStateValid() == false )				append(invalid, "Z");
				
			if ( getXLimit() == LimitSwitch::LIMIT_UNKNOWN )	append(unknown, "X");
			if ( getYLimit() == LimitSwitch::LIMIT_UNKNOWN )	append(unknown, "Y");
			if ( getZLimit() == LimitSwitch::LIMIT_UNKNOWN )	append(unknown, "Z");
			
			if ( invalid.IsEmpty() == false || unknown.IsEmpty() == false )
				cnc::trc.logErrorMessage(wxString::Format("Limit state summary: Unknown=[%s] axis; Invalid=[%s] axis", unknown, invalid));
		}
};

#endif