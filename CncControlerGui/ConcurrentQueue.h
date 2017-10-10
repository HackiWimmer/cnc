#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
 
template <typename T>
class ConcurrentQueue
{
	public:
		///////////////////////////////////////////////////////////////////
		size_t size() {
			return queue.size();
		}
		
		///////////////////////////////////////////////////////////////////
		void clear() {
			std::unique_lock<std::mutex> mlock(mutex);
			while (queue.size() > 0 )
				queue.pop();
		}
		
		///////////////////////////////////////////////////////////////////
		T pop() {
			std::unique_lock<std::mutex> mlock(mutex);
			while (queue.empty()) {
			  cond.wait(mlock);
			}
			
			auto item = queue.front();
			queue.pop();
			return item;
		}
		
		///////////////////////////////////////////////////////////////////
		void pop(T& item) {
			std::unique_lock<std::mutex> mlock(mutex);
			while (queue.empty()) {
			  cond.wait(mlock);
			}
			
			item = queue.front();
			queue.pop();
		}
		
		///////////////////////////////////////////////////////////////////
		void push(const T& item) {
			std::unique_lock<std::mutex> mlock(mutex);
			queue.push(item);
			mlock.unlock();
			cond.notify_one();
		}
		
		///////////////////////////////////////////////////////////////////
		void push(T&& item) {
			std::unique_lock<std::mutex> mlock(mutex);
			queue.push(std::move(item));
			mlock.unlock();
			cond.notify_one();
		}
	 
	private:
		std::queue<T> queue;
		std::mutex mutex;
		std::condition_variable cond;
};

#endif