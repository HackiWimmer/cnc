#ifndef CNC_POSITION_RENDERER_H
#define CNC_POSITION_RENDERER_H

#include "ArdoVal.h"

class ArduinoPositionRenderer {

  public: 
    
    //-------------------------------------------------------------------------
    struct RS {
      
      static const uint8_t  POINT_LENGTH     = 3;
      
      static int32_t        A[POINT_LENGTH];
      static int32_t        B[POINT_LENGTH];
    
      static uint32_t       impulseCount;
      static uint32_t       xStepCount;
      static uint32_t       yStepCount;
      static uint32_t       zStepCount;
    
      //------------------------------------------------------------------------
      static void swap() { memcpy(B, A, sizeof(B)); }
    
      //------------------------------------------------------------------------
      static void reset() {
        memset(A, 0, sizeof(A));
        memset(B, 0, sizeof(B));

        impulseCount  = 0;
        xStepCount    = 0;
        yStepCount    = 0;
        zStepCount    = 0;
      }
    
      //------------------------------------------------------------------------
      static bool empty() {
        return (    A[IDX_X] - B[IDX_X] == 0 
                 && A[IDX_Y] - B[IDX_Y] == 0
                 && A[IDX_Z] - B[IDX_Z] == 0
               );
      }
    
      //------------------------------------------------------------------------
      static int8_t dx() { return A[IDX_X] - B[IDX_X]; } 
      static int8_t dy() { return A[IDX_Y] - B[IDX_Y]; }
      static int8_t dz() { return A[IDX_Z] - B[IDX_Z]; }
    
    };
  
  private:
    ArduinoPositionRenderer(const ArduinoPositionRenderer&);

    inline byte             stepping();
    
  protected:
    ArduinoPositionRenderer();
    virtual ~ArduinoPositionRenderer();

    // controller interface
    byte                    directMove(int8_t dx, int8_t dy, int8_t dz);
    byte                    renderMove(int32_t dx, int32_t dy, int32_t dz);
    
    virtual byte            checkRuntimeEnv()                                 = 0;
    virtual byte            setDirection(AxisId aid, int32_t steps)           = 0;
    virtual byte            performStep (AxisId aid)                          = 0;
    virtual byte            initiateStep(AxisId aid)                          = 0;
    virtual byte            finalizeStep(AxisId aid)                          = 0;
    virtual void            notifyMovePart(int8_t dx, int8_t dy, int8_t dz)   = 0;
};

class ArduinoImpulseCalculator {

  private:
    ArduinoImpulseCalculator(const ArduinoImpulseCalculator&);

  public:
    ArduinoImpulseCalculator() 
    {}
    
    virtual ~ArduinoImpulseCalculator()
    {}

    int32_t calculate(int32_t dx, int32_t dy, int32_t dz) {
      const int32_t DX = ArdoObj::absolute<int32_t>(dx);
      const int32_t DY = ArdoObj::absolute<int32_t>(dy);
      const int32_t DZ = ArdoObj::absolute<int32_t>(dz);
      
      return ArdoObj::maximum(ArdoObj::maximum(DX, DY), DZ);
    }
};

#endif
