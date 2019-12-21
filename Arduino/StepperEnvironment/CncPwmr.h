#ifndef CNC_PULS_WIDTH_MANAGER_H
#define CNC_PULS_WIDTH_MANAGER_H

class ArduinoPulsWidthManager {

  private:

    bool          initialized;
  
    ArduinoPulsWidthManager(const ArduinoPulsWidthManager&);
    
  public:
    ArduinoPulsWidthManager();
    virtual ~ArduinoPulsWidthManager();

    bool          isPwmInitialized()            const { return initialized; }
    void          printPwmConfig()              const;

    void          setupPwmManager();
    
};

#endif
