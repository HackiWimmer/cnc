#ifndef PEN_HANDLER_H_INCLUDED
#define PEN_HANDLER_H_INCLUDED
class PenHandler {

	private:
		static const unsigned int workPenStep = 3;
		static const unsigned int maxWorkPens = workPenStep * workPenStep * workPenStep;
		unsigned int durationCounter;
		wxPen workPens[maxWorkPens];
		wxPen flyingPen;
		wxPen currentPen;
		wxPen gridPenDefault;
		wxPen gridPen010;
		wxPen gridPen050;
		wxPen gridPen100;
		
		///////////////////////////////////////////////////////////////////////
		void generateWorkPens() {
			// default
			for ( unsigned int j=0; j<maxWorkPens; j++ )
				workPens[j] = wxPen(wxColor(0, 0, 0), 1, wxSOLID);
				
			// individual colors
			int rc = 255, gc = 0, bc = 0;
			unsigned int i = 0;
			for ( unsigned int r=0; r<workPenStep; r++ ) {
				for ( unsigned int g=0; g<workPenStep; g++ ) {
					for ( unsigned int b=0; b<workPenStep; b++ ) {
						
						if ( i < maxWorkPens ) {
							//clog << i << ": " << rc << ", " << gc << ", " << bc << endl;
							workPens[i] = wxPen(wxColor(rc, gc, bc), 1, wxSOLID);
						}
						
						i++;
						bc += 127;
					}
					bc  = 0;
					gc += 127;
				}
				gc  = 0;
				rc -= 127;
			}
		}
	
	public: 
		///////////////////////////////////////////////////////////////////////
		PenHandler()
		: durationCounter(0)
		, flyingPen(wxPen(wxColor(128,128,128), 1, wxDOT))
		, gridPenDefault(wxPen(*wxWHITE, 1, wxSOLID))
		, gridPen010(wxPen(wxColor(50,50,50), 1,wxDOT ))
		, gridPen050(wxPen(wxColor(0,64,128), 1, wxDOT_DASH))
		, gridPen100(wxPen(wxColor(109,109,109), 1, wxDOT_DASH))
		{
			generateWorkPens();
			currentPen 	= workPens[0];
		}
		virtual ~PenHandler () {
		}
		///////////////////////////////////////////////////////////////////////
		const unsigned int getDurationCounter() { return durationCounter; }
		void initNextDuration() {
			currentPen = workPens[durationCounter%maxWorkPens];
			durationCounter++;
		}
		///////////////////////////////////////////////////////////////////////
		void reset() {
			durationCounter = 0;
		}
		///////////////////////////////////////////////////////////////////////
		wxPen getCurrentPen(bool zAxisDownState) { 
			if ( zAxisDownState == false )
				return flyingPen;
				
			return currentPen; 
		}
		///////////////////////////////////////////////////////////////////////
		wxPen getGridPenDefault() 	{ return gridPenDefault; }
		wxPen getGridPen010() 		{ return gridPen010; }
		wxPen getGridPen050() 		{ return gridPen050; }
		wxPen getGridPen100() 		{ return gridPen100; }
};

#endif