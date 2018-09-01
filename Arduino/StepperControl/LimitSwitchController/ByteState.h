#ifndef BYTE_STATE_H
#define BYTE_STATE_H

typedef unsigned char SHORT;

//-------------------------------------------------------------
inline void headline() {
  for (short i=0; i<30; i++, Serial.print('-'));
}

//-----------------------------------------------------------------
class ByteState {

  protected:

    static const unsigned short ANALOG_READ_WRITE_FACTOR  = 1024/256;
    static const unsigned int INIT_VALUE                  = 0;

    unsigned int pinValue;
    char stringValue[8];

    //-------------------------------------------------------------
    bool getBit(unsigned char idx) const { 
      switch (idx) {
        case 8:     return pinValue & 128;
        case 7:     return pinValue &  64;
        case 6:     return pinValue &  32;
        case 5:     return pinValue &  16;
        case 4:     return pinValue &   8;
        case 3:     return pinValue &   4;
        case 2:     return pinValue &   2;
        case 1:     return pinValue &   1;
      }

      return false;
    }
    
    //-------------------------------------------------------------
    void setBit(unsigned char idx, bool value) { 
      switch (idx) {
        case 8:     pinValue |= value << 7;
        case 7:     pinValue |= value << 6;
        case 6:     pinValue |= value << 5;
        case 5:     pinValue |= value << 4;
        case 4:     pinValue |= value << 3;
        case 3:     pinValue |= value << 2;
        case 2:     pinValue |= value << 1;
        case 1:     pinValue |= value << 0;
      }
    }

    //-------------------------------------------------------------
    void setValue(unsigned int value)  { pinValue = value; }

  public:
    ByteState() 
    : pinValue(INIT_VALUE)
    {}

    explicit ByteState(const unsigned int value) 
    : pinValue(value)
    {}

    //-------------------------------------------------------------
    virtual void init()     = 0;
    virtual void report()   = 0;
    virtual void evaluate() = 0;

    //-------------------------------------------------------------
    void reset() { pinValue = INIT_VALUE; }
    unsigned int getValue() const { return pinValue; }

  protected:
  
    //-------------------------------------------------------------
    bool getBit8() const { return pinValue & 128; }
    bool getBit7() const { return pinValue &  64; }
    bool getBit6() const { return pinValue &  32; }
    bool getBit5() const { return pinValue &  16; }
    bool getBit4() const { return pinValue &   8; }
    bool getBit3() const { return pinValue &   4; }
    bool getBit2() const { return pinValue &   2; }
    bool getBit1() const { return pinValue &   1; }

    void setBit8(bool value = true) { pinValue |= value << 7; }
    void setBit7(bool value = true) { pinValue |= value << 6; }
    void setBit6(bool value = true) { pinValue |= value << 5; }
    void setBit5(bool value = true) { pinValue |= value << 4; }
    void setBit4(bool value = true) { pinValue |= value << 3; }
    void setBit3(bool value = true) { pinValue |= value << 2; }
    void setBit2(bool value = true) { pinValue |= value << 1; }
    void setBit1(bool value = true) { pinValue |= value << 0; }

    //-------------------------------------------------------------
    virtual const char* getValueAsString() {
      memset(&stringValue, '0', sizeof(stringValue));
      
      if ( pinValue & 128 ) stringValue[7] = '1';
      if ( pinValue &  64 ) stringValue[6] = '1';
      if ( pinValue &  32 ) stringValue[5] = '1';
      if ( pinValue &  16 ) stringValue[4] = '1';
      if ( pinValue &   8 ) stringValue[3] = '1';
      if ( pinValue &   4 ) stringValue[2] = '1';
      if ( pinValue &   2 ) stringValue[1] = '1';
      if ( pinValue &   1 ) stringValue[0] = '1';
      
      return stringValue;
    }

    enum PinType {PT_ANALOG = 0, PT_DIGITAL = 1};


    //-------------------------------------------------------------
    ByteState::PinType getPinType(int pin) {
      switch ( pin ) {
        case A0: 
        case A1: 
        case A2: 
        case A3: return PT_ANALOG;
      }
      
      return PT_DIGITAL;
    }
    //-------------------------------------------------------------
    void initInputPin(int pin, bool value) {
      if ( getPinType(pin) == PT_ANALOG ) {
        pinMode(pin, INPUT);
        analogWrite(pin, value=HIGH ? 255 : 0);              
      } else {
        pinMode(pin, INPUT);
        digitalWrite(pin, value);              
      }
    }
    //-------------------------------------------------------------
    bool getAnalogStateAsBool(int pin) {
      return analogRead(pin) == 255;
    }
    //-------------------------------------------------------------
    bool readPin(int pin) {
      bool ret = false;
      if ( getPinType(pin) == PT_ANALOG ) ret = getAnalogStateAsBool(pin);              
      else                                ret = digitalRead(pin);              
    
      return ret;
    }

  public:  
    
    //-------------------------------------------------------------
    unsigned int readAnalogValue(int pin, int msdelay) {
      if ( msdelay > 0 )
        delay(msdelay);
        
      // consider analogRead delivers values from 0 to 1023
      int readValue = analogRead(pin);
    
      // recalulate the readValue to a range from 0 ... 255
      //       0:      0,  1,  2,  3
      //       1:      4,  5,  6,  7
      //       2:      8,  9, 10, 11
      //       3:     12, 13, 14, 15
      //       4:     16, 17, 18, 19
      // ...
      // example:   readValue  = 18
      // offset     = 18 % 4 = 2
      // writeValue = (18 - 2) / 4 = 4
       
      short offset              = readValue % ANALOG_READ_WRITE_FACTOR;
      unsigned int writtenValue = (readValue - offset) / ANALOG_READ_WRITE_FACTOR;
    
      setValue(writtenValue);
      return writtenValue;
    }
};

#endif
