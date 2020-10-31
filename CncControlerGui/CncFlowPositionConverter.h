#ifndef CNC_FLOW_POS_CNV_H
#define CNC_FLOW_POS_CNV_H

#include "CncPosition.h"

class CncFlowPositionConverter {
	
	private:
		CncDoublePosition	pos;
		CncLongPosition		ret;
		
	public:
		CncFlowPositionConverter();
		~CncFlowPositionConverter();
		
		void	reset();
		void	set(double dx, double dy, double dz);
		const	CncLongPosition& get();
};

#endif