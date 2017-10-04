#ifndef INTERRUPT_THREAD_H
#define INTERRUPT_THREAD_H

#include <wx/thread.h>
#include "CncPosition.h"

typedef std::pair<unsigned char, int32_t> SetterPair;
typedef std::vector<SetterPair> SetterList;

class MainFrame;
class UpdateManagerThread : public wxThread {
	
	public:
		UpdateManagerThread(MainFrame *handler);
		~UpdateManagerThread();
		
		// value interface
		void postAppPos(const CncLongPosition& pos);
		void postCtlPos(const CncLongPosition& pos);
		void postCmdValues(long counter, long duration);
		void postConfigUpdate(CncConfig* cncConfig);
		void postSetterValue(unsigned char id, int32_t value);
		void postResetZView();
		void postUpdateZView();
		
		// thread interface
		void stop();
		void enableDisplay(bool state=true);
		
	protected:
		const unsigned int maxSetterEntries = 1000;
		
		MainFrame* pHandler;
		CncConfig* cncConfig;
		SetterList setterList;
		unsigned int setterCounter;
		bool enabled;
		bool exit;
		
		CncLongPosition appPos;
		CncLongPosition ctlPos;
		
		long cmdCounter;
		long cmdDuration;
		
		virtual ExitCode Entry();
		
		void display();
		inline void updatePositionControls();
		inline void updateConfigurationControls();
		inline void updateSetterControls();
};

#endif