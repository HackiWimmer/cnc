#ifndef CNC_FILE_VIEW_CALLBACK_H
#define CNC_FILE_VIEW_CALLBACK_H

namespace CncFileViewCallback {
	
	class Interface {
		
		public:
			Interface() {}
			virtual ~Interface() {}
			
			virtual void fileListSelected(long item) {}
			virtual void fileListActivated(long item) {}
			virtual void fileListLeave(wxMouseEvent& event) {}
		
	};
};

#endif