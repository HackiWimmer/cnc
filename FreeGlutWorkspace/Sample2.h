

/////////////////////////////////////////////////////////////////
class OpenGLContext {
	
	public:
		OpenGLContext();
		virtual ~OpenGLContext();
		
		virtual void display() = 0;
		virtual void reshape(int w, int h) = 0;
		
		virtual void keyboardHandler(unsigned char c) {}
};

/////////////////////////////////////////////////////////////////
class OpenGLDrawPaneContext0 : public OpenGLContext {
	
	public:
		OpenGLDrawPaneContext0();
		virtual ~OpenGLDrawPaneContext0();
		
		virtual void display();
		virtual void reshape(int w, int h);
};

/////////////////////////////////////////////////////////////////
class OpenGLDrawPaneContext1 : public OpenGLContext {
	
	public:
		OpenGLDrawPaneContext1();
		virtual ~OpenGLDrawPaneContext1();
		
		virtual void display();
		virtual void reshape(int w, int h);
};