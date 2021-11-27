#ifndef ARDUINO_CNC_PODIUM_MANAGEMENT_H
#define ARDUINO_CNC_PODIUM_MANAGEMENT_H

/////////////////////////////////////////////////////////////////////////////////////
//Corner Schema:
//
// +Y
//        B ----- C
//        |       |
//        |       |
//        |       |
//        A ----- D
// 0                      +X
//
/////////////////////////////////////////////////////////////////////////////////////

class ArduinoPodiumManager {

  public:

    static bool isEnabledCornerA()   { return AE::digitalRead(PIN_ENABLE_PODIUM_A); }
    static bool isEnabledCornerB()   { return AE::digitalRead(PIN_ENABLE_PODIUM_B); }
    static bool isEnabledCornerC()   { return AE::digitalRead(PIN_ENABLE_PODIUM_C); }
    static bool isEnabledCornerD()   { return AE::digitalRead(PIN_ENABLE_PODIUM_D); }

    static bool isEnabled()          { return ( isEnabledCornerA() && isEnabledCornerB() && isEnabledCornerC() && isEnabledCornerD() ); }
    
    enum Mode { M_Off, M_CornerA, M_CornerB, M_CornerC, M_CornerD, M_CornerABCD };
    
    ArduinoPodiumManager();
    virtual ~ArduinoPodiumManager();

    bool enable(Mode m = M_CornerABCD);
    bool disable() { return enable(M_Off); }
    
    Mode getCurrentMode() const { return curMode; }

  protected:
    Mode curMode;
    
  
};

class ArduinoPodiumEnabler : public ArduinoPodiumManager
{
  public:
    ArduinoPodiumEnabler()                                      { enable(M_CornerABCD);  }
    explicit ArduinoPodiumEnabler(ArduinoPodiumManager::Mode m) { enable(m);  }
    virtual ~ArduinoPodiumEnabler()                             { disable(); }
};
     

#endif
