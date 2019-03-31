#ifndef CNC_CONTEXT_H
#define CNC_CONTEXT_H


struct CncContext {
	
	private:
		bool probeMode	= true;
		
	public:
		CncContext();
		~CncContext();
		
		void setProbeMode(bool state); 
		bool isProbeMode() { return  probeMode; }
		
	
};

#endif