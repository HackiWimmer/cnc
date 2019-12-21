#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <algorithm>
#include <cstring>
#include "wx/thread.h"

template <typename T>
class CircularBuffer {

	private:
		
		T* 				buffer;
		int 			totalCapacity;
		int 			front;
		int 			rear;
		int 			count;
		unsigned int 	timeoutMillis;
		wxMutex			mutex;
		
		CircularBuffer(const CircularBuffer&);
		CircularBuffer& operator=(const CircularBuffer&);
		
	public:
		
		typedef unsigned char byte;

		explicit CircularBuffer(int capacity = 64);
		virtual ~CircularBuffer();
		
		void	setTimeout(unsigned int timeout);

		void 	clear();
		int 	capacity();
		int 	size();

		bool 	write(T data);
		bool 	write(T* buffer, int size);

		bool 	peak(T& ret);
		bool 	read(T& ret);
		int 	read(T* buffer, int size);
		
		static void test();
};

///////////////////////////////////////////////////////////////////
template<typename T>
CircularBuffer<T>::CircularBuffer(int capacity) 
: buffer			(NULL)
, totalCapacity		(capacity)
, front				(0)
, rear				(0)
, count				(0)
, timeoutMillis		(100)
, mutex				()
///////////////////////////////////////////////////////////////////
{
	buffer = new T[capacity];
}
///////////////////////////////////////////////////////////////////
template<typename T>
CircularBuffer<T>::~CircularBuffer(){
///////////////////////////////////////////////////////////////////
	delete[] buffer;
}
///////////////////////////////////////////////////////////////////
template<typename T>
int CircularBuffer<T>::capacity() {
///////////////////////////////////////////////////////////////////
	return totalCapacity;
}
///////////////////////////////////////////////////////////////////
template<typename T>
int CircularBuffer<T>::size() {
///////////////////////////////////////////////////////////////////
	return count;
}
///////////////////////////////////////////////////////////////////
template<typename T>
void CircularBuffer<T>::setTimeout(unsigned int timeout) {
///////////////////////////////////////////////////////////////////
	wxMutexLocker mlk(mutex);
	timeoutMillis = timeout;
}
///////////////////////////////////////////////////////////////////
template<typename T>
void CircularBuffer<T>::clear() {
///////////////////////////////////////////////////////////////////
	wxMutexLocker mlk(mutex);
	front	= 0;
	rear	= 0;
	count	= 0;
}
///////////////////////////////////////////////////////////////////
template<typename T>
bool CircularBuffer<T>::peak(T& frontByte) {
///////////////////////////////////////////////////////////////////
	// if nothing available, dont wait here immediately return instead
	if ( count <= 0 )
		return false;
	
	bool ret = true;
	wxMutexLocker mlk(mutex);
	ret = count > 0;
	if ( ret ) 	
		frontByte = buffer[front];
		
	return ret;
}
///////////////////////////////////////////////////////////////////
template<typename T>
bool CircularBuffer<T>::read(T& frontByte){
///////////////////////////////////////////////////////////////////
	// if nothing available, dont wait here immediately return instead
	if ( count <= 0 )
		return false;

	bool ret = true;
	wxMutexLocker mlk(mutex);
	ret = count > 0;
	if ( ret ) {
		frontByte = buffer[front];
		front = (front + 1) % totalCapacity;
		--count;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
template<typename T>
int CircularBuffer<T>::read(T* b, int size) {
///////////////////////////////////////////////////////////////////
	if ( b == NULL )
		return 0;
	
	if ( size <= 0 )
		return 0;
		
	if ( count <= 0 ) 
		return false;
	
	wxMutexLocker mlk(mutex);
	// read whatever exits - max up to size
	int bytesAvailable = std::min(size, count);
	
	for ( auto i=0; i<bytesAvailable; i++ ) {
		b[i] = buffer[front];
		
		front = (front + 1) % totalCapacity;
		--count;
	}
	
	return bytesAvailable;
}
///////////////////////////////////////////////////////////////////
template<typename T>
bool CircularBuffer<T>::write(T data) {
///////////////////////////////////////////////////////////////////
	if ( count >= totalCapacity ) 
		return false;
	
	wxMutexLocker mlk(mutex);
	bool ret = count < totalCapacity;
	if ( ret ) {
		buffer[rear] = data;
		rear = (rear + 1) % totalCapacity;
		++count;
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
template<typename T>
bool CircularBuffer<T>::write(T* b, int size) {
///////////////////////////////////////////////////////////////////
	if ( b == NULL )
		return false;
	
	if ( size <= 0 )
		return false;
		
	if ( count + size >= totalCapacity ) 
		return false;
	
	wxMutexLocker mlk(mutex);
	bool ret =  count + size < totalCapacity;
	if ( ret ) {
		for ( auto i=0; i<size; i++ ) {
			buffer[rear] = b[i];
			rear = (rear + 1) % totalCapacity;
			++count;
		}
	}
	
	return ret;
}

#endif
