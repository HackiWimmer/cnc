#define SKETCH_COMPILE = TRUE


//#define SKETCH_TEST
#ifndef SKETCH_TEST

  #include "MainLoop.h"
  ArduinoMainLoop mainEntry;
  
  void setup()  { mainEntry.setup(); }
  void loop()   { mainEntry.loop();  }

#else

  int pin = 21;
  void setup() 
  { 
    
    Serial.begin(9600); 
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);  
  }

  void loop()  
  { 
    if ( Serial.available() <= 0 )
      return;
      
    const byte c  = Serial.read();

    Serial.write(c);
    Serial.print(", ");
    Serial.print(c%2);
    Serial.print("\n");

    digitalWrite(pin,  c%2 ==0 );
  }


#endif
