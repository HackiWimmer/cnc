#ifndef COMMAND_TEMPLATES_H
#define COMMAND_TEMPLATES_H

class CommandTemplates {
	
	private:
		void fill(char c, char* val) {
			for (int32_t i=0;i<MAX_CMD_LEN-1; i++)
				val[i] = c;
			val[MAX_CMD_LEN-1] = '\0';	
		}
		
		char* pointer (int32_t dist) {
			assert(dist > MAX_CMD_LEN - 1);
			p += MAX_CMD_LEN - 1 - dist;
			return p;
		}

		char* p;
		
		static const int32_t MAX_CMD_LEN = (1024 * 256) + 1;
		char x[MAX_CMD_LEN]; // container for xxxxxxxxxxxxxxxx . . .
		char X[MAX_CMD_LEN]; // container for XXXXXXXXXXXXXXXX . . . 
		char y[MAX_CMD_LEN]; // container for yyyyyyyyyyyyyyyy . . . 
		char Y[MAX_CMD_LEN]; // container for YYYYYYYYYYYYYYYY . . . 
		char z[MAX_CMD_LEN]; // container for zzzzzzzzzzzzzzzz . . . 
		char Z[MAX_CMD_LEN]; // container for ZZZZZZZZZZZZZZZZ . . .
		
	public:
		
		CommandTemplates() {
			fill('x', x);
			fill('X', X);
			fill('y', y);
			fill('Y', Y);
			fill('z', z);
			fill('Z', Z);
		}
		virtual ~CommandTemplates() {}
		
		// returns corresponding char pointer with length=dist
		char* px(int32_t dist) { p = x; return pointer(dist); } 
		char* pX(int32_t dist) { p = X; return pointer(dist); }
		char* py(int32_t dist) { p = y; return pointer(dist); }
		char* pY(int32_t dist) { p = Y; return pointer(dist); }
		char* pz(int32_t dist) { p = z; return pointer(dist); }
		char* pZ(int32_t dist) { p = Z; return pointer(dist); }
};

#endif