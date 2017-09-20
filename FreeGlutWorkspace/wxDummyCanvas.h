#ifndef WX_DUMMY_CANVAS_H
#define WX_DUMMY_CANVAS_H

class wxGLCanvas {
	
	public:
		wxGLCanvas() {
		}
		
		virtual ~wxGLCanvas() {
		}
};

class wxGLContext {
	
	public:
		wxGLContext(wxGLCanvas* canvas) {
		}
		
		virtual ~wxGLContext() {
		}
		
		void SetCurrent(wxGLCanvas& canvas) {
			
		}
};

#endif

