#ifndef GL_GUIDE_PATH_H
#define GL_GUIDE_PATH_H

#include <vector>
#include <wx/pen.h>
#include "CncPosition.h"
#include "CncPathListManager.h"

class GLGuidePath : public std::vector<CncDoublePosition> {
	
	public:
		
		explicit GLGuidePath(const CncPathListManager& plm);
		~GLGuidePath();
		
		const bool			isValid()		const	{ return guideStyle != wxPENSTYLE_INVALID; } 
		const wxColour& 	getColour()		const	{ return guideColour; }
		const wxPenStyle	getStyle()		const	{ return guideStyle; }
		
	private:
		wxColour		guideColour;
		wxPenStyle		guideStyle;
};

#endif