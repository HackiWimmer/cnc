#ifndef CNC_AVERAGE_H
#define CNC_AVERAGE_H

#include <list>
#include <numeric>

template <typename T> 
class CncAverage {
	
	protected:
		std::list<T> list;
		
	public:
		CncAverage()
		: list()
		{}
		
		explicit CncAverage(const CncAverage& a)
		: list()
		{
			list.push_back(a.getAvg());
		}
		
		~CncAverage()
		{}
		
		CncAverage& operator= (const CncAverage& a) { 
			list.push_back(a.getAvg());
			return *this; 
		}  
		
		size_t count()	const { return list.size(); }
		T getMin() 		const { return ( list.size() ? std::min(list.begin(), list.end(), 0.0)                      : (T)0 ); }
		T getAvg()		const { return ( list.size() ? std::accumulate(list.begin(), list.end(), 0.0) / list.size() : (T)0 ); }
		T getMax()		const { return ( list.size() ? std::max(list.begin(), list.end(), 0.0)                      : (T)0 ); }
		
		void reset()	{ list.clear(); }
		void add(T v)	{ list.push_back(v); }
};

#endif