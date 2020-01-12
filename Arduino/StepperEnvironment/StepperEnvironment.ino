#define SKETCH_COMPILE = TRUE


//#define SKETCH_TEST
#ifndef SKETCH_TEST

  #include "MainLoop.h"
  ArduinoMainLoop mainEntry;
  
  void setup()  { mainEntry.setup(); }
  void loop()   { mainEntry.loop();  }

#else

  void setup() { Serial.begin(9600); }
  void loop()  { Serial.println("Here I'am . . ."); delay(2000); }

#endif
