#ifndef CNC_CIRCILAR_BUFFER_H
#define CNC_CIRCILAR_BUFFER_H

#include <cstddef>

template <size_t N, typename T>
class CncCircularBuffer {
	
	public:
		class IteratorBase {
			
			protected:
				
				CncCircularBuffer*	reference;
				T*					buffer;
				size_t				start;
				size_t				position;
				bool				moved;
				
				IteratorBase(CncCircularBuffer* cb, size_t p) 
				: reference	(cb)
				, buffer	(cb ? cb->values : NULL)
				, start		(cb ? p          : 0)
				, position	(start)
				, moved		(false) 
				{}
				
			public:
				
				size_t	getPosition() 							const { return position; }
				
				bool	operator== (const IteratorBase& rhs)	const { return getPosition() == rhs.getPosition(); }
				bool	operator!= (const IteratorBase& rhs)	const { return getPosition() != rhs.getPosition(); }
				T*		operator->	() 							const { return buffer + (position % N); }
				
				bool hasMore(const IteratorBase& end) const {
					if ( reference == NULL )
						return false;
						
					if ( reference->isFilled() == false )
						return position != end.getPosition();
						
					if ( moved == false && start == position ) {
						return position != end.getPosition();
					}
					
					return IteratorBase::position != IteratorBase::start;
				}
		};
		
		class ForwardIterator : public IteratorBase {
			
			public:
				
				ForwardIterator(CncCircularBuffer* cb, size_t p) 
				: IteratorBase(cb, p) 
				{}
				
				ForwardIterator operator+ (size_t s) const { 
					ForwardIterator i(IteratorBase::reference, IteratorBase::position); 
					i.position += s;
					i.position  = i.position % N; 
					return i; 
				}
				
				ForwardIterator operator++ () { 
					IteratorBase::position++;
					IteratorBase::position = IteratorBase::position % N;
					IteratorBase::moved    = true;
					return *this; 
				}
		};
		
		class ReverseIterator : public IteratorBase {
			
			public:
				
				ReverseIterator(CncCircularBuffer* cb, size_t p) 
				: IteratorBase(cb, p) 
				{}
				
				ReverseIterator operator+ (size_t s) const { 
					ReverseIterator i(IteratorBase::reference, IteratorBase::position); 
					
					if ( i.position >= s )	i.position -= s;
					else					i.position  = N - 1;
					
					i.position = i.position % N; 
					return i; 
				}
				
				ReverseIterator operator++ () { 
					if ( IteratorBase::position > 0 )	IteratorBase::position--;
					else								IteratorBase::position = N - 1;
						
					IteratorBase::position = IteratorBase::position % N;
					IteratorBase::moved    = true;
					return *this; 
				}
		};
		
	protected:
		T values[N];
		size_t count;
		size_t index;
		
	public:
		CncCircularBuffer()
		: count(0)
		, index(0)
		{}
		
		~CncCircularBuffer()
		{}
		
		bool	isFilled()	const { return count == N; }
		size_t	getSize()	const { return N; }
		size_t	getCount()	const { return count; }
		size_t	getIndex()	const { return index; }
		
		void clear() {
			count = 0;
			//index = 0;
		}
		
		void add(const T& d) {
			if ( count < N )
				count++;
			
			index++;
			index %= N;
			values[index] = d;
		}
		
		const T& getValue(size_t i) {
			return values[i < N ? i : N - 1];
		}
		
		ForwardIterator begin() {
			if ( count > 0 ) {
				const size_t i = ( isFilled() ? index : index - count);
				return ForwardIterator(this, (i + 1) % N);
			}
			
			return ForwardIterator(this, index % N);
		}
		
		ForwardIterator end() {
			if ( count > 0 ) {
				const size_t i = ( isFilled() ? index : index + 1);
				return ForwardIterator(this, i % N);
			}
			
			return begin();
		}
		
		ReverseIterator rbegin() {
			return ReverseIterator(this, index % N);
		}
		
		ReverseIterator rend() {
			if ( count > 0 ) {
				const size_t i = ( isFilled() ? index + 1 : index - count);
				return ReverseIterator(this, i % N);
			}
			
			return rbegin();
		}
};

bool test_CncCircularBuffer();

#endif