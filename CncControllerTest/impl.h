#ifndef TEST_IMPL_H
#define TEST_IMPL_H

class Implementation {
	
	public:
		void run();
	
};


#define OPTIMISTIC true
#define PESIMISTIC false


template<bool OPTIMIZED>
class Foo {
	public:
		bool f();
};






#endif