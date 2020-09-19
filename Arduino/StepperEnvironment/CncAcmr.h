#ifndef ARDUINO_ACCEL_MANAGER_H
#define ARDUINO_ACCEL_MANAGER_H

class ArduinoAccelManager {
  
  public:
    enum State { P_UNDEF = 1, P_CONST = 2, P_ACCEL = 4, P_TARGET = 8, P_DEACCEL = 16 };
    enum Type  { T_MODEL_DRIVEN = 1, T_INTERACTIVE = 2};
    
    // ------------------------------------------------------------------------------       
    struct Setup {

      public:
        
        static uint32_t feedRate_FT[AXIS_SIGNATURE_COUNT];
        static uint32_t feedRate_UM[AXIS_SIGNATURE_COUNT];
        static uint32_t feedRate_Master1000;
      
        Setup(float frX, float frY, float frZ)
        {
          if ( frX > 0.0 && frY > 0.0 && frZ > 0.0) {
            feedRate_Master1000 = ArdoObj::maximum(ArdoObj::maximum(frX, frY), frZ) * 1000;
  
            // fr has thr unit mm  
            const uint32_t cdx_UM = pow(frX * 1000, 2);
            const uint32_t cdy_UM = pow(frY * 1000, 2);
            const uint32_t cdz_UM = pow(frZ * 1000, 2);
      
            feedRate_UM[ASGI_MASTER] = feedRate_Master1000;
            feedRate_UM[ASGI_XYZ]    = sqrt(cdx_UM + cdy_UM + cdz_UM);
            feedRate_UM[ASGI_XY]     = sqrt(cdx_UM + cdy_UM         );
            feedRate_UM[ASGI_YZ]     = sqrt(         cdy_UM + cdz_UM);
            feedRate_UM[ASGI_XZ]     = sqrt(cdx_UM +          cdz_UM);
            feedRate_UM[ASGI_X]      = sqrt(cdx_UM                  );
            feedRate_UM[ASGI_Y]      = sqrt(          cdy_UM        );
            feedRate_UM[ASGI_Z]      = sqrt(                  cdz_UM);
            
            feedRate_FT[ASGI_MASTER] = C_2_08;
            feedRate_FT[ASGI_XYZ]    = C_2_08 * feedRate_UM[ASGI_XYZ] / (float)feedRate_UM[ASGI_MASTER];
            feedRate_FT[ASGI_XY]     = C_2_08 * feedRate_UM[ASGI_XY]  / (float)feedRate_UM[ASGI_MASTER];
            feedRate_FT[ASGI_YZ]     = C_2_08 * feedRate_UM[ASGI_YZ]  / (float)feedRate_UM[ASGI_MASTER];
            feedRate_FT[ASGI_XZ]     = C_2_08 * feedRate_UM[ASGI_XZ]  / (float)feedRate_UM[ASGI_MASTER];
            feedRate_FT[ASGI_X]      = C_2_08 * feedRate_UM[ASGI_X]   / (float)feedRate_UM[ASGI_MASTER];
            feedRate_FT[ASGI_Y]      = C_2_08 * feedRate_UM[ASGI_Y]   / (float)feedRate_UM[ASGI_MASTER];
            feedRate_FT[ASGI_Z]      = C_2_08 * feedRate_UM[ASGI_Z]   / (float)feedRate_UM[ASGI_MASTER];
          }
        }

        static void trace() {
          ARDO_DEBUG_VALUE("Setup::FeedRateM1000  [nm]", Setup::feedRate_UM[ASGI_MASTER])
          ARDO_DEBUG_VALUE("Setup::FeedRate XYZ   [nm]", Setup::feedRate_UM[ASGI_XYZ]   )
          ARDO_DEBUG_VALUE("Setup::FeedRate XY    [nm]", Setup::feedRate_UM[ASGI_XY]    )
          ARDO_DEBUG_VALUE("Setup::FeedRate YZ    [nm]", Setup::feedRate_UM[ASGI_YZ]    )
          ARDO_DEBUG_VALUE("Setup::FeedRate XZ    [nm]", Setup::feedRate_UM[ASGI_XZ]    )
          ARDO_DEBUG_VALUE("Setup::FeedRate X     [nm]", Setup::feedRate_UM[ASGI_X]     )
          ARDO_DEBUG_VALUE("Setup::FeedRate Y     [nm]", Setup::feedRate_UM[ASGI_Y]     )
          ARDO_DEBUG_VALUE("Setup::FeedRate Z     [nm]", Setup::feedRate_UM[ASGI_Z]     )
          
          ARDO_DEBUG_VALUE("Setup::FDR Fact XYZ    [#]", ( Setup::feedRate_UM[ASGI_XYZ] / (float)C_2_08 ) )
          ARDO_DEBUG_VALUE("Setup::FDR Fact XY     [#]", ( Setup::feedRate_UM[ASGI_XY]  / (float)C_2_08 ) )
          ARDO_DEBUG_VALUE("Setup::FDR Fact YZ     [#]", ( Setup::feedRate_UM[ASGI_YZ]  / (float)C_2_08 ) )
          ARDO_DEBUG_VALUE("Setup::FDR Fact XZ     [#]", ( Setup::feedRate_UM[ASGI_XZ]  / (float)C_2_08 ) )
          ARDO_DEBUG_VALUE("Setup::FDR Fact X      [#]", ( Setup::feedRate_UM[ASGI_X]   / (float)C_2_08 ) )
          ARDO_DEBUG_VALUE("Setup::FDR Fact Y      [#]", ( Setup::feedRate_UM[ASGI_Y]   / (float)C_2_08 ) )
          ARDO_DEBUG_VALUE("Setup::FDR Fact Z      [#]", ( Setup::feedRate_UM[ASGI_Z]   / (float)C_2_08 ) )
        }
      
        #ifndef SKETCH_COMPILE 
          static std::ostream& trace (std::ostream &o) {
            o << "FeedRate M1000 [um] : "  << Setup::feedRate_UM[ASGI_MASTER] << std::endl 
              << "FeedRate XYZ   [um] : "  << Setup::feedRate_UM[ASGI_XYZ]    << std::endl
              << "FeedRate XY    [um] : "  << Setup::feedRate_UM[ASGI_XY]     << std::endl
              << "FeedRate YZ    [um] : "  << Setup::feedRate_UM[ASGI_YZ]     << std::endl
              << "FeedRate XZ    [um] : "  << Setup::feedRate_UM[ASGI_XZ]     << std::endl
              << "FeedRate X     [um] : "  << Setup::feedRate_UM[ASGI_X]      << std::endl
              << "FeedRate Y     [um] : "  << Setup::feedRate_UM[ASGI_Y]      << std::endl
              << "FeedRate Z     [um] : "  << Setup::feedRate_UM[ASGI_Z]      << std::endl
              
              << "FDR Fact XYZ    [#] : "  << ( Setup::feedRate_FT[ASGI_XYZ] / (float)C_2_08 ) << std::endl
              << "FDR Fact XY     [#] : "  << ( Setup::feedRate_FT[ASGI_XY]  / (float)C_2_08 ) << std::endl
              << "FDR Fact YZ     [#] : "  << ( Setup::feedRate_FT[ASGI_YZ]  / (float)C_2_08 ) << std::endl
              << "FDR Fact XZ     [#] : "  << ( Setup::feedRate_FT[ASGI_XZ]  / (float)C_2_08 ) << std::endl
              << "FDR Fact X      [#] : "  << ( Setup::feedRate_FT[ASGI_X]   / (float)C_2_08 ) << std::endl
              << "FDR Fact Y      [#] : "  << ( Setup::feedRate_FT[ASGI_Y]   / (float)C_2_08 ) << std::endl
              << "FDR Fact Z      [#] : "  << ( Setup::feedRate_FT[ASGI_Z]   / (float)C_2_08 ) << std::endl
              ;

            return o;    
          }
        #endif    
    };
    
    // ------------------------------------------------------------------------------       
    struct Function {
      
      protected:
        uint32_t  A_1000;
        uint32_t  B_1000;
        uint32_t  C_1000;
        uint32_t  minSpeedDelay_US;
        uint32_t  stdRampWidth_IMPL;
        uint32_t  relRampWidth_IMPL;
        
        void      initMinSpeedDelay();
        void      initTrgSpeedDelay(const uint32_t trgF1000_MMSEC);
        uint32_t  rampWidth(uint32_t trgF1000_MMSEC);
        
        friend ArduinoAccelManager;
        
      public:
        Function(float a, float b, float c) 
        : A_1000         ( a > 0.0 ? a * 1000 : 0 )
        , B_1000         ( b > 0.0 ? b * 1000 : 0 )
        , C_1000         ( c > 0.0 ? c * 1000 : 0 )
        , minSpeedDelay_US    (0)
        , stdRampWidth_IMPL   (0)
        , relRampWidth_IMPL   (0)
        {}
        
        uint32_t  getA_1000()         const { return A_1000; }
        uint32_t  getB_1000()         const { return B_1000; }
        uint32_t  getC_1000()         const { return C_1000; }
        uint32_t  getRelRampWidth()   const { return relRampWidth_IMPL; }

        uint32_t fctValue1000(int32_t ipl) const
          { return ( A_1000 * (ipl * ipl) + B_1000 * ipl + C_1000 ); }
          
        
        #ifndef SKETCH_COMPILE 
          friend std::ostream &operator<< (std::ostream &o, const Function &f) {
            o <<   "A="         << f.A_1000 
              << ", B="         << f.B_1000 
              << ", C="         << f.C_1000 
              << ", minSpeedDelay = "   << f.minSpeedDelay_US  << " us"
              << ", stdRampWidth_IMPL = " << f.stdRampWidth_IMPL << " impl"
              << ", relRampWidth_IMPL = " << f.relRampWidth_IMPL << " impl"
              ;
            return o;
          }
        #endif
    };
    
  private:
    typedef AxisSignatureIndex ASI;
    
    explicit ArduinoAccelManager(const ArduinoAccelManager&);
    
    bool        initialized;
    bool        active;
    
    State       curState;
    Type        curType;
    uint32_t    curSpeedDelay;
    uint32_t    curImplIdx;
    uint32_t    curImplCnt;
    uint32_t    cfgSpeedDelay;
    Function    fA;               // function to accelerate
    Function    fD;               // function to deaccelerate
    
    uint32_t cnvSpeedToDelay(uint32_t v, uint32_t d = 5000) const
      { return v ? 1000L * 1000 * Setup::feedRate_Master1000 / v : d; }
    
    uint32_t cnvDelayToSpeed(uint32_t v, uint32_t d = 500) const 
      { return v ? 1000L * 1000 * Setup::feedRate_Master1000 / v : d; }
      
    void        changeState(State s);
    
  protected:

    virtual void notifyACMStateChange(State s) = 0;
    virtual void notifyACMInitMove()           = 0;
    
  public:
    ArduinoAccelManager();
    virtual ~ArduinoAccelManager();
    
    void        finalize();
    void        reset();            
    
    void        initialize      (const Function& fA, const Function& fD);
    bool        initSpeed       (uint32_t mF1000_MMSec);
    bool        initMove        (uint32_t mD_IMPL);
    bool        initMove        (uint32_t mD_IMPL, uint32_t mF1000_MMSec);
    uint32_t    initNextImpulse (AxisSignatureIndex axisSignatureIdx);

    void        activate(bool s)                          { active = s; }
    bool        isAccelInitialized()                const { return initialized; }
    bool        isInteractiveMoveType()             const { return curType == T_INTERACTIVE; }
    uint32_t    getDefalutDelay()                   const { return cnvSpeedToDelay(fA.fctValue1000(0), 5000); }
    uint32_t    getARampWidth()                     const { return fA.getRelRampWidth(); }
    uint32_t    getDRampWidth()                     const { return fD.getRelRampWidth(); }    
    uint32_t    getCurrentTargetSpeedDelay_US()     const { return curSpeedDelay;}
    uint32_t    getCurrentTargetSpeed1000_MMSec()   const { return cnvDelayToSpeed(curSpeedDelay, 0); }
    uint32_t    getCurrentImpulseCount()            const { return curImplCnt; }
    void        printAccelConfig()                  const;
    
    #ifndef SKETCH_COMPILE 
      friend std::ostream &operator<< (std::ostream &o, const ArduinoAccelManager &a) {

        Setup::trace(o);
        o << std::endl;
        
        o << "State             : " << a.curState       << std::endl;
        o << "cfgSpeedDelay     : " << a.cfgSpeedDelay  << std::endl;
        o << "curSpeedDelay     : " << a.curSpeedDelay  << std::endl;
        o << "curImplCnt        : " << a.curImplCnt     << std::endl;
        
        o << "fA : [ " << a.fA << " ]"  << std::endl;
        o << "fD : [ " << a.fD << " ]"  << std::endl;
        
        return o;
      }
    #endif

};

#endif
