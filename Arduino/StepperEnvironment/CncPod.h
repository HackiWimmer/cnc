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
    ArduinoPodiumEnabler()           { enable(M_CornerABCD);  }
    virtual ~ArduinoPodiumEnabler()  { disable(); }

};
     

#endif
