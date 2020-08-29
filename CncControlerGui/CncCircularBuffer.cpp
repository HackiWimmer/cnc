#include <iostream>
#include "CncCircularBuffer.h"

////////////////////////////////////////////////////////////////
bool test_CncCircularBuffer() {
////////////////////////////////////////////////////////////////
	struct Data { 
		int i; 
		Data() 		: i(0){} 
		Data(int x) : i(x){} 
	};
	
	// ---------------------------------------------------------
	auto iterOneRound = [](auto cb) {
		auto end = cb.end();
		for ( auto it = cb.begin(); it.hasMore(end); ++it) {
			std::clog << ">" << it.getPosition() << ":" << it->i << std::endl;
		}
		std::clog << std::endl;
		
		auto rend = cb.rend();
		for ( auto it = cb.rbegin(); it.hasMore(rend); ++it) {
			std::clog << "<" << it.getPosition() << ":" << it->i << std::endl;
		}
		std::clog << std::endl;
	};
	
	// ---------------------------------------------------------
	CncCircularBuffer<10, Data> buffer;

	for ( unsigned int i=0; i<17; i++ ) 
		buffer.add(Data(i + 1 * 10));
		
	std::cout << "* start native for loop" << std::endl;
		std::cout << "count: " << buffer.getCount() << std::endl;
		std::cout << "index: " << buffer.getIndex() << std::endl;
		
		for ( unsigned int j=0; j<buffer.getSize(); j++ ) 
			std::cout << j << ": " << buffer.getValue(j).i << std::endl;
	std::cout << "* end native for loop" << std::endl;

	std::cout << "* start iter test" << std::endl;
		auto it1 = buffer.begin();
		auto it2 = it1 + 1;
		auto it3 = buffer.end();
		
		std::clog << it1.getPosition() << " ... " << it2.getPosition() << " --> " << it3.getPosition() << std::endl;
		iterOneRound(buffer);
	std::cout << "* end iter test" << std::endl;
	
	// clear
	std::cout << "* start clear" << std::endl;
		buffer.clear();
		std::cout << "count: " << buffer.getCount() << std::endl;
		std::cout << "index: " << buffer.getIndex() << std::endl;
		iterOneRound(buffer);
	std::cout << "* end clear" << std::endl;
	
	// refill
	std::cout << "* start refill" << std::endl;
		for ( unsigned int i=0; i<1; i++ ) 
			buffer.add(Data(i + 1 * 100));
			
		std::cout << "count: " << buffer.getCount() << std::endl;
		std::cout << "index: " << buffer.getIndex() << std::endl;
		iterOneRound(buffer);
	std::cout << "* end refill" << std::endl;
	
	// add
	std::cout << "* start add" << std::endl;
		for ( unsigned int i=1; i<2; i++ ) 
			buffer.add(Data(i + 1 * 100));
			
		std::cout << "count: " << buffer.getCount() << std::endl;
		std::cout << "index: " << buffer.getIndex() << std::endl;
		iterOneRound(buffer);
	std::cout << "* end add" << std::endl;

	
	return true;
}