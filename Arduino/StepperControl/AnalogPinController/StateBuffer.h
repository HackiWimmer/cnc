#ifndef BYTE_STATE_H
#define BYTE_STATE_H

typedef unsigned char SHORT;

static const char* EM_BUFFER_TO_SMALL    = "Given buffer to small";

//-------------------------------------------------------------
inline void headline() {
  for (short i=0; i<30; i++, Serial.print('-'));
}

//-----------------------------------------------------------------
class StateBuffer {

  protected:

    static const unsigned int INIT_VALUE                  = 0;

    unsigned int pinValue;
    char stringValue[9];

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
      if ( value == true ) {
        switch (idx) {
          case 8:     pinValue |= (1 << 7); break;
          case 7:     pinValue |= (1 << 6); break;
          case 6:     pinValue |= (1 << 5); break;
          case 5:     pinValue |= (1 << 4); break;
          case 4:     pinValue |= (1 << 3); break;
          case 3:     pinValue |= (1 << 2); break;
          case 2:     pinValue |= (1 << 1); break;
          case 1:     pinValue |= (1 << 0); break;
        }
      } else {
         switch (idx) {
          case 8:     pinValue &= ~(1 << 7); break;
          case 7:     pinValue &= ~(1 << 6); break;
          case 6:     pinValue &= ~(1 << 5); break;
          case 5:     pinValue &= ~(1 << 4); break;
          case 4:     pinValue &= ~(1 << 3); break;
          case 3:     pinValue &= ~(1 << 2); break;
          case 2:     pinValue &= ~(1 << 1); break;
          case 1:     pinValue &= ~(1 << 0); break;
        }       
      }
    }

    //-------------------------------------------------------------
    void setValue(unsigned int value)  { pinValue = value; }

  public:
    StateBuffer() 
    : pinValue(INIT_VALUE)
    {}

    explicit StateBuffer(const unsigned int value) 
    : pinValue(value)
    {}

    //-------------------------------------------------------------
    virtual void init()     = 0;
    virtual void report()   = 0;
    virtual void trace()    = 0;
    virtual bool evaluate() = 0;

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

    //-------------------------------------------------------------
    void setBit8(bool value = true) { if ( value ) {pinValue |= (1 << 7);} else {pinValue &= (1 << 7);} }
    void setBit7(bool value = true) { if ( value ) {pinValue |= (1 << 6);} else {pinValue &= (1 << 6);} }
    void setBit6(bool value = true) { if ( value ) {pinValue |= (1 << 5);} else {pinValue &= (1 << 5);} }
    void setBit5(bool value = true) { if ( value ) {pinValue |= (1 << 4);} else {pinValue &= (1 << 4);} }
    void setBit4(bool value = true) { if ( value ) {pinValue |= (1 << 3);} else {pinValue &= (1 << 3);} }
    void setBit3(bool value = true) { if ( value ) {pinValue |= (1 << 2);} else {pinValue &= (1 << 2);} }
    void setBit2(bool value = true) { if ( value ) {pinValue |= (1 << 1);} else {pinValue &= (1 << 1);} }
    void setBit1(bool value = true) { if ( value ) {pinValue |= (1 << 0);} else {pinValue &= (1 << 0);} }

    //-------------------------------------------------------------
    virtual const char* getValueAsString() {
      memset(&stringValue, '0', sizeof(stringValue));
      stringValue[8]= '\0';
      
      if ( pinValue & 128 ) stringValue[0] = '1';
      if ( pinValue &  64 ) stringValue[1] = '1';
      if ( pinValue &  32 ) stringValue[2] = '1';
      if ( pinValue &  16 ) stringValue[3] = '1';
      if ( pinValue &   8 ) stringValue[4] = '1';
      if ( pinValue &   4 ) stringValue[5] = '1';
      if ( pinValue &   2 ) stringValue[6] = '1';
      if ( pinValue &   1 ) stringValue[7] = '1';
      
      return stringValue;
    }

    enum PinType {PT_ANALOG = 0, PT_DIGITAL = 1};


    //-------------------------------------------------------------
    StateBuffer::PinType getPinType(int pin) {
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
        analogWrite(pin, value == HIGH ? 255 : 0);              
      } else {
        pinMode(pin, INPUT);
        digitalWrite(pin, value);              
      }
    }
    //-------------------------------------------------------------
    bool getAnalogStateAsBool(int pin) {
      // The used cricut don't switch the pin to a zero value
      // analog pin switches between 10,11,12 and 1023
      const int VALUE_TO_COMPARE = 64;
      return analogRead(pin) >= VALUE_TO_COMPARE;
    }
    //-------------------------------------------------------------
    bool readPin(int pin) {
      bool ret = false;
      if ( getPinType(pin) == PT_ANALOG ) ret = getAnalogStateAsBool(pin);              
      else                                ret = digitalRead(pin);              
    
      return ret;
    }

};

#endif
