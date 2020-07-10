#ifndef ARDUINO_TEST_MANAGER_H
#define ARDUINO_TEST_MANAGER_H

//#define USE_CNC_TEST 

class CncArduinoController;
class ArduinoTestManager {

  private:
  
    const int   steps       = 800;          // #
    const float resolution  = 12.0;         // mmm/360
    const float feedRate    = resolution / steps;

    CncArduinoController* controller;

    ArduinoTestManager(const ArduinoTestManager&);

    void summerizeSpeed           (float totalMM, uint32_t ts);
    void stepDist                 (uint32_t dist, bool x, bool y, bool z);
    
    byte performTestCase1         ();
    byte performTestCase2         ();

  public:
    ArduinoTestManager(CncArduinoController* ctrl);
    virtual ~ArduinoTestManager();
  
    byte performTest();
};

#endif
