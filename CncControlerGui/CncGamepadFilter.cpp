#include <iostream>
#include "CncGamepadFilter.h"

////////////////////////////////////////////////////////////////////
CncGamepadFilterInstance::CncGamepadFilterInstance()
: filter	(FILTER_NOTHING)
////////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////////
CncGamepadFilterInstance::~CncGamepadFilterInstance() {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
void CncGamepadFilterInstance::rmvFilter(int f) { 
////////////////////////////////////////////////////////////////////
	auto rmvod = [&](int q) {
		if ( f & q )
			filter &= q; 
	};
	
	rmvod(FILTER_CNC_ACTIVATION);
	rmvod(FILTER_QUICK_MENU_ACTIVATION); 
}

#warning
/*
struct XXXXXXXXXXXXXX {
	
	
	XXXXXXXXXXXXXX() {
		
		CncGamepadFilterInstance i;
		std::cout << i.getFilter() << ":" << i.canActicateQuickMenu() << ", " << i.canActivateCncMode() << std::endl;
		
		{
			CncGamepadFilter f1(&i, CncGamepadFilterInstance::FILTER_CNC_AVTIVATION);
			std::cout << i.getFilter() << ":" << i.canActicateQuickMenu() << ", " << i.canActivateCncMode() << std::endl;
			
			CncGamepadFilter f2(&i, CncGamepadFilterInstance::FILTER_QUICK_MENU_AVTIVATION);
			std::cout << i.getFilter() << ":" << i.canActicateQuickMenu() << ", " << i.canActivateCncMode() << std::endl;
		}
		
		std::cout << i.getFilter() << ":" << i.canActicateQuickMenu() << ", " << i.canActivateCncMode() << std::endl;
		
		{
			CncGamepadFilter f2(&i, CncGamepadFilterInstance::FILTER_QUICK_MENU_AVTIVATION | CncGamepadFilterInstance::FILTER_CNC_AVTIVATION);
			std::cout << i.getFilter() << ":" << i.canActicateQuickMenu() << ", " << i.canActivateCncMode() << std::endl;
		}
		
		std::cout << i.getFilter() << ":" << i.canActicateQuickMenu() << ", " << i.canActivateCncMode() << std::endl;

		abort();
	}
	
} xxxx;
*/