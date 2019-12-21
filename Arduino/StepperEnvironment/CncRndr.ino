#include "ArdoObj.h"
#include "CncRndr.h"

#ifndef SKETCH_COMPILE 
  #define CNC_RENDERER_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("PositionRenderer::%s", __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
#else
  #define CNC_RENDERER_LOG_FUNCTION()
#endif

// ----------------------------------------------------------------------------------
// static initialization . . .
typedef ArduinoPositionRenderer::RS RS;

int32_t       RS::A[RS::POINT_LENGTH]    = {0,0,0};
int32_t       RS::B[RS::POINT_LENGTH]    = {0,0,0};
uint16_t      RS::xStepCount             = 0;
uint16_t      RS::yStepCount             = 0;
uint16_t      RS::zStepCount             = 0;

/////////////////////////////////////////////////////////////////////////////////////
ArduinoPositionRenderer::ArduinoPositionRenderer() {
///////////////////////////////////////////////////////////////////////////////////// 
  CNC_RENDERER_LOG_FUNCTION()
}
/////////////////////////////////////////////////////////////////////////////////////
ArduinoPositionRenderer::~ArduinoPositionRenderer() {
/////////////////////////////////////////////////////////////////////////////////////  
  CNC_RENDERER_LOG_FUNCTION()
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoPositionRenderer::stepping() {  
/////////////////////////////////////////////////////////////////////////////////////
  // if stepAxisXYZ() is called from renderAndStepAxisXYZ - normal processing -
  // RS conatins dx, dy and dz with in range [-1, 0, +1] only
   
  // avoid empty processing
  if ( RS::empty() )
    return RET_OK;

  byte retValue = checkRuntimeEnv();
  if ( retValue != RET_OK )
    return retValue;
  
  if ( RS::dx() != 0) {
    if ( (retValue = performNextStepX()) != RET_OK ) 
      return retValue;

    RS::xStepCount++;
  }
  
  if ( RS::dy() != 0 ) {
     if ( (retValue = performNextStepY()) != RET_OK )
      return retValue;
      
    RS::yStepCount++;
  }
    
  if ( RS::dz() != 0 ) {
     if ( (retValue = performNextStepZ()) != RET_OK )
      return retValue;
      
    RS::zStepCount++;
  }

  notifyPositionChange();
  RS::swap();
      
  return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoPositionRenderer::renderMove(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  // avoid empty processing
  if ( dx == 0 && dy == 0 && dz == 0 ) 
    return RET_OK;

  // initialize
  int32_t i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
  unsigned char ret = RET_ERROR;

  x_inc = (dx < 0) ? -1 : 1;
  l     = ArdoObj::absolute<int32_t>(dx);
  
  y_inc = (dy < 0) ? -1 : 1;
  m     = ArdoObj::absolute<int32_t>(dy);
  
  z_inc = (dz < 0) ? -1 : 1;
  n     = ArdoObj::absolute<int32_t>(dz);
  
  dx2 = l << 1;
  dy2 = m << 1;
  dz2 = n << 1;

  //------------------------------------------------------
  // first setup the directions - this can be done once
  // because the directions didn't switch during a call of
  // renderAndStepAxisXYZ()
  if ( setDirectionX(dx) == false )
    return RET_ERROR;

  if ( setDirectionY(dy) == false )
    return RET_ERROR;

  if ( setDirectionZ(dz) == false )
    return RET_ERROR;

  RS::reset();
  
  //------------------------------------------------------
  if ((l >= m) && (l >= n)) {
    err_1 = dy2 - l;
    err_2 = dz2 - l;
    
    for (i = 0; i < l; i++) {

      ret = stepping();
      if ( ret != RET_OK ) { return ret; }
      if (err_1 > 0)       { RS::A[RS::IDX_Y] += y_inc; err_1 -= dx2; }
      if (err_2 > 0)       { RS::A[RS::IDX_Z] += z_inc; err_2 -= dx2; }
      
      err_1            += dy2;
      err_2            += dz2;
      RS::A[RS::IDX_X] += x_inc;
    }

  //------------------------------------------------------  
  } else if ((m >= l) && (m >= n)) {
    err_1 = dx2 - m;
    err_2 = dz2 - m;
    
    for (i = 0; i < m; i++) {

      ret = stepping();
      if ( ret != RET_OK ) { return ret; }
      if (err_1 > 0)       { RS::A[RS::IDX_X] += x_inc; err_1 -= dy2; }
      if (err_2 > 0)       { RS::A[RS::IDX_Z] += z_inc; err_2 -= dy2; }
      
      err_1            += dx2;
      err_2            += dz2;
      RS::A[RS::IDX_Y] += y_inc;
    }

  //------------------------------------------------------  
  } else {
    err_1 = dy2 - n;
    err_2 = dx2 - n;
    
    for (i = 0; i < n; i++) {

      ret = stepping();
      if ( ret != RET_OK ) { return ret; }
      if (err_1 > 0)       { RS::A[RS::IDX_Y] += y_inc; err_1 -= dz2; }
      if (err_2 > 0)       { RS::A[RS::IDX_X] += x_inc; err_2 -= dz2; }
      
      err_1            += dy2;
      err_2            += dx2;
      RS::A[RS::IDX_Z] += z_inc;
    }
  }
  
  if ( (ret = stepping()) != RET_OK )
    return ret;

  return RET_OK;
}
