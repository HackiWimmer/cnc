#ifndef CNC_POSITION_RENDERER_H
#define CNC_POSITION_RENDERER_H

class ArduinoPositionRenderer {

  public: 
    
    //-------------------------------------------------------------------------
    struct RS {
      
      static const int      POINT_LENGTH     = 3;
      static const int      IDX_X            = 0;
      static const int      IDX_Y            = 1;
      static const int      IDX_Z            = 2;
      
      static int32_t        A[POINT_LENGTH];
      static int32_t        B[POINT_LENGTH];
    
      static uint16_t       xStepCount;
      static uint16_t       yStepCount;
      static uint16_t       zStepCount;
    
      //------------------------------------------------------------------------
      static void swap() { memcpy(B, A, sizeof(B)); }
    
      //------------------------------------------------------------------------
      static void reset() {
        memset(A, 0, sizeof(A));
        memset(B, 0, sizeof(B));
    
        xStepCount = 0;
        yStepCount = 0;
        zStepCount = 0;
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
    byte                    renderMove(int32_t dx, int32_t dy, int32_t dz);
    
    virtual void            notifyPositionChange()          = 0;
    virtual byte            checkRuntimeEnv()               = 0;
    
    virtual byte            setDirectionX(int32_t steps)    = 0;
    virtual byte            setDirectionY(int32_t steps)    = 0;
    virtual byte            setDirectionZ(int32_t steps)    = 0;
   
    virtual byte            performNextStepX()              = 0;
    virtual byte            performNextStepY()              = 0;
    virtual byte            performNextStepZ()              = 0;
 
};

#endif
