#ifndef ARDUINO_ACCEL_MANAGER_H
#define ARDUINO_ACCEL_MANAGER_H

class ArduinoAccelManager {

  public:
    
    // F(impulse) = A * (impulse)^2 + B * impulse + C
    struct Function {
      float A =  0.0;  // factor
      float B = 10.0;  // factor
      float C =    0;  // F [mm/sec]

      // -----------------------------------------------------------
      float fctValue(int32_t ipl ) { return ( A * (ipl*ipl) + B * ipl  + C ); }
      float defValue()             { return fctValue(0); }

      // -----------------------------------------------------------
      float rampWidth(float F_MMSec) {
        if ( A == 0.0 ) {
          // linear function: F(impulse) = B * impulse + C
          return B ? ( F_MMSec - C ) / B : C;
      
        } else {
          // quadratic functions
      
          if ( B == 0.0 ) {
            // simple quadratic: f(impulse) = A * (impulse)^2 + C
            return sqrt( (F_MMSec - C) / A );
      
          } else {
            // quadratic - todo
            const float p = B / A * 0.5;
            const float q = C - F_MMSec / A;
            float ret = -p + sqrt(p * p -q );
            
            if ( ret >=0 )
              return ret;

            return -p - sqrt(p * p -q );
          }
        }
            
        // error case
        return -1;
      }
    };

  public:
    enum State { P_UNDEF, P_CONST, P_ACCEL, P_TARGET, P_DEACCEL };
    
  private:
    
    bool          initialized;
    bool          active;

    State         curState;
    Function      fA;               // function to accelerate
    Function      fD;               // function to deaccelerate

    float         aRampWidth_IMPL;
    float         dRampWidth_IMPL;

    uint32_t      cD_IMPL;
    uint32_t      iD_IMPL;
    float         cF_MMSec;
  
    ArduinoAccelManager(const ArduinoAccelManager&);

    void changeState(State s);

  protected:

    virtual void notifyACMStateChange(State s) = 0;
    virtual void notifyACMInitMove()           = 0;

    void         debugACM();
    
  public:
    ArduinoAccelManager();
    virtual ~ArduinoAccelManager();

    void          printAccelConfig()            const;
    bool          isAccelInitialized()          const { return initialized; }
    float         getARampWidth()               const { return aRampWidth_IMPL; }
    float         getDRampWidth()               const { return dRampWidth_IMPL; }
    float         getStartSpeed_MMSec()         const { return fA.C; }
    float         getStopSpeed_MMSec()          const { return fD.C; }
    int32_t       getCurrentImpulseCount()      const { return iD_IMPL; }

    void          activateAccel(bool s)               { active = s; }
    
    void          setupAccelManager(const Function& fA, const Function& fD);
    bool          initMove(uint32_t mD_IMPL, float mF_MMSec);
    float         getNextTargetSpeed_MMSec();


    #ifndef SKETCH_COMPILE 
    void          traceACM(std::ostream& o, int indent);
    #endif      

};

#endif
