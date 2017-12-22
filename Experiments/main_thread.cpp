

#include <iostream>
#include <chrono>
#include <sys/time.h>
#include <wx/init.h>
#include <wx/string.h>
#include "wx/thread.h"

bool __exit = false;

class MySignallingThread : public wxThread
{
public:
    MySignallingThread(wxMutex *mutex, wxCondition *condition)
    {
        m_mutex = mutex;
        m_condition = condition;
    }
    virtual ExitCode Entry()
    {
		unsigned int cnt = 0;
		while ( !TestDestroy() ) {
		
			wxThread::Sleep(500);
			std::clog << "Entry: " << cnt << std::endl;
			
			if ( __exit == true )
				break;

			if ( cnt > 12 )
				break;
				
			cnt++;
		}
		
		// tell the other(s) thread(s) that we're about to terminate: we must
        // lock the mutex first or we might signal the condition before the
        // waiting threads start waiting on it!
        
		std::clog << "before broadcast " << std::endl;
		wxMutexLocker lock(*m_mutex);
        m_condition->Broadcast(); // same as Signal() here -- one waiter only
        return 0;
    }
private:
    wxCondition *m_condition;
    wxMutex *m_mutex;
};


int main()
{
	// initialize wxWidgets
	wxInitializer init;

	
    wxMutex mutex;
    wxCondition condition(mutex);
	
	
	
    // the mutex should be initially locked
    mutex.Lock();
	
    // create and run the thread but notice that it won't be able to
    // exit (and signal its exit) before we unlock the mutex below
    MySignallingThread *thread = new MySignallingThread(&mutex, &condition);
    thread->Run();
    // wait for the thread termination: Wait() atomically unlocks the mutex
    // which allows the thread to continue and starts waiting


	
	//MessageBoxA(0,"","",0);
	//__exit = true;
	
	
	std::clog << "before wait " << std::endl;
	
    condition.Wait();
	std::clog << "after wait" << std::endl;
    // now we can exit
    return 0;
}
