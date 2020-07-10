#ifndef ARDUINO_ACCEL_MANAGER_H
#define ARDUINO_ACCEL_MANAGER_H

class ArduinoAccelManager {

  public:
    
    // F(impulse) = A * (impulse)^2 + B * impulse + C
    struct Function {
      float A         =  0.0;  // factor
      float B         = 10.0;  // factor
      float C         =  0.0;  // F [mm/sec]

      float refValue  =  0.0;  // F [mm/sec]
      float cSlider   =  0.0;  // +/- F [mm/sec]

      // -----------------------------------------------------------
      float c()                    { return C + cSlider; }
      float defValue()             { return fctValue(0); }
      
      // -----------------------------------------------------------
      float fctValue(int32_t ipl ) { 
        const float cc = c();
        const float cv = ( A * (ipl*ipl) + B * ipl  + cc );

        // the refValue acts as target value and effects the pitch:
        // if refValue >= cc --> pos. pitch;
        // if refValue  < cc --> neg. pitch;
        return refValue >= cc ? cv : cc - ( cv - cc ); 
      }

      // -----------------------------------------------------------
      float rampWidth(float F_MMSec) {
        
        if ( A == 0.0 ) {
          // linear function: F(impulse) = B * impulse + c()
          return B ? ( ArdoObj::absolute( F_MMSec - c() ) ) / B : c();
      
        } else {
          // quadratic functions
      
          if ( B == 0.0 ) {
            // simple quadratic: F(impulse) = A * (impulse)^2 + c()
            return sqrt( (F_MMSec - c()) / A );
      
          } else {
            // quadratic - todo
            const float p = B / A * 0.5;
            const float q = c() - F_MMSec / A;
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
    enum State { P_UNDEF = 1, P_CONST = 2, P_ACCEL = 4, P_TARGET = 8, P_DEACCEL = 16 };
    
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
    float         currentTargetF_MMSec;
  
    #ifndef SKETCH_COMPILE 
      uint32_t    cFT1_IMPL;
      uint32_t    cFT2_IMPL;
      uint32_t    cFT3_IMPL;
  
      float       cFT1_MMSec;
      float       cFT2_MMSec;
      float       cFT3_MMSec;
    #endif

    ArduinoAccelManager(const ArduinoAccelManager&);

    void changeState(State s);

  protected:

    virtual void notifyACMStateChange(State s) = 0;
    virtual void notifyACMInitMove()           = 0;

  public:
    ArduinoAccelManager();
    virtual ~ArduinoAccelManager();

    void          printAccelConfig()            const;
    bool          isAccelInitialized()          const { return initialized; }
    float         getARampWidth()               const { return aRampWidth_IMPL; }
    float         getDRampWidth()               const { return dRampWidth_IMPL; }
    float         getStartSpeed_MMSec()         const { return fA.C; }
    float         getStopSpeed_MMSec()          const { return fD.C; }
    float         getCurrentTargetSpeed_MMSec() const { return currentTargetF_MMSec; }
    int32_t       getCurrentImpulseCount()      const { return iD_IMPL; }

    void          activate(bool s)                    { active = s; }
    void          finalize()                          { changeState(P_UNDEF); }
    
    void          setup(const Function& fA, const Function& fD);
    bool          initMove(uint32_t mD_IMPL, float mF_MMSec);
    float         initNextTargetSpeed_MMSec();
    bool          updateSpeedConfig(float mF_MMSec) ;

    #ifndef SKETCH_COMPILE 
      void        setSpecialSpeedTrigger1(uint32_t i, float f) { cFT1_IMPL = i; cFT1_MMSec = f; }
      void        setSpecialSpeedTrigger2(uint32_t i, float f) { cFT2_IMPL = i; cFT2_MMSec = f; }
      void        setSpecialSpeedTrigger3(uint32_t i, float f) { cFT3_IMPL = i; cFT3_MMSec = f; }
      const char* getStateAsString(State s);
      void        traceACM(std::ostream& o, int indent);
    #endif      

};

#endif
