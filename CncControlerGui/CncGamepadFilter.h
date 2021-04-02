#ifndef CNC_GAMEPAD_FILTER_H
#define CNC_GAMEPAD_FILTER_H

class CncGamepadFilterInstance {
	
	private:
		int filter;
		
	public:
		
		static const int FILTER_NOTHING							=   0;
		static const int FILTER_CNC_AVTIVATION					=   1;
		static const int FILTER_QUICK_MENU_AVTIVATION			=   2;
		
		CncGamepadFilterInstance();
		~CncGamepadFilterInstance();
		
		int getFilter()					const	{ return filter; }
		bool canActivateCncMode()		const	{ return !( filter & FILTER_CNC_AVTIVATION );        }
		bool canActicateQuickMenu()		const	{ return !( filter & FILTER_QUICK_MENU_AVTIVATION ); }
		
		void clear()							{ filter = filter; }
		void addFilter(int f)					{ filter |= f; }
		void rmvFilter(int f);
		
};

class CncGamepadFilter {
	
	private:
		CncGamepadFilterInstance*	instance;
		int							filter;
	
	public:
		CncGamepadFilter(CncGamepadFilterInstance* i, int f)
		: instance	(i)
		, filter	(f)
		{
			instance->addFilter(filter);
		}
		
		~CncGamepadFilter() {
			instance->rmvFilter(filter);
		}
};

#endif