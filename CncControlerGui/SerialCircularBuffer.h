#ifndef SERIAL_CIRCULAR_BUFFER
#define SERIAL_CIRCULAR_BUFFER

#include <mutex>
#include <condition_variable>
#include <boost/circular_buffer.hpp>

// Thread safe serial circular buffer 
class SerialCircularBuffer {
	
	public:
		typedef std::unique_lock<std::mutex> Lock;
		
		SerialCircularBuffer();
		virtual ~SerialCircularBuffer();
		
		void 	clear();
		int 	capacity();
		int 	size();
		
		bool 	write(unsigned char* buffer, int size);
		int 	read(unsigned char* buffer, int size);
		
	private:
		
		unsigned int							msReadTimeout;
		std::condition_variable					condition;
		std::mutex 								monitor;
		
		boost::circular_buffer<unsigned char> 	circularBuffer;
		
};

class SerialReadBuffer : public SerialCircularBuffer {
	
};

class SerialWriteBuffer : public SerialCircularBuffer {
	
};

#endif
