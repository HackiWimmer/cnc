 #define SKETCH_COMPILE = TRUE


//#define SKETCH_TEST
#ifndef SKETCH_TEST

  #include "MainLoop.h"
  ArduinoMainLoop mainEntry;
  
  void setup()  { mainEntry.setup(); }
  void loop()   { mainEntry.loop();  }

#else

  void setup() { 
    
    Serial.begin(9600); 
    //DDRB = B00111111;
    pinMode(7,OUTPUT);  
    pinMode(8,OUTPUT);  
    pinMode(9,OUTPUT);  

  }

  int c = 0;
  
  void loop()  { 
    
    
    delay(2000); 

    byte port = digitalPinToPort(6);
    byte bit  = digitalPinToBitMask(6);
    volatile uint8_t *out;
    out = portOutputRegister(port);
        
    //if ( ++c%2 == 0 ) *out |=  (1 << 3);
    //else              PORTH &= ~(1 << PH3);


    if ( ++c%2 == 0 ) PORTH |=  (1 << PH4);
    else              PORTH &= ~(1 << PH4);

    if ( ++c%2 == 0 ) PORTH |=  (1 << PH5);
    else              PORTH &= ~(1 << PH5);
    
    if ( ++c%2 == 0 ) PORTH |=  (1 << PH6);
    else              PORTH &= ~(1 << PH6);

    for (int i =7; i<10;i++) {
      Serial.print(i);Serial.print('=');
      Serial.println(digitalRead(i));
      }

  }


#endif
