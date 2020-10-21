#include "ArdoEnv.h"
#include "ArdoObj.h"
#include "CncRndr.h"

#ifndef SKETCH_COMPILE 
  #define CNC_RENDERER_LOG_FUNCTION()  \
    { \
        const wxString logStr(wxString::Format("PositionRenderer::%s", __FUNCTION__)); \
        ARDO_DEBUG_MESSAGE('D', logStr); \
    }
    
  #define SET_DIRECTION_X(dx)             setDirection(IDX_X, dx)
  #define SET_DIRECTION_Y(dy)             setDirection(IDX_Y, dy)
  #define SET_DIRECTION_Z(dz)             setDirection(IDX_Z, dz)

  #define INITIALIZE_SETP_X               initiateStep(IDX_X)
  #define INITIALIZE_SETP_Y               initiateStep(IDX_Y)
  #define INITIALIZE_SETP_Z               initiateStep(IDX_Z)

  #define FINALIZE_SETP_X                 finalizeStep(IDX_X)
  #define FINALIZE_SETP_Y                 finalizeStep(IDX_Y)
  #define FINALIZE_SETP_Z                 finalizeStep(IDX_Z)
  
#else

  #define CNC_RENDERER_LOG_FUNCTION()

  #define SET_DIRECTION_X(dx)             pX->setDirection(dx)
  #define SET_DIRECTION_Y(dy)             pY->setDirection(dy)
  #define SET_DIRECTION_Z(dz)             pZ->setDirection(dz)

  #define INITIALIZE_SETP_X               pX->initiateStep()
  #define INITIALIZE_SETP_Y               pY->initiateStep()
  #define INITIALIZE_SETP_Z               pZ->initiateStep()

  #define FINALIZE_SETP_X                 pX->finalizeStep()
  #define FINALIZE_SETP_Y                 pY->finalizeStep()
  #define FINALIZE_SETP_Z                 pZ->finalizeStep()
  
#endif

// ----------------------------------------------------------------------------------
// static initialization . . .
typedef ArduinoPositionRenderer::RS RS;

int32_t             RS::A[RS::POINT_LENGTH]    = {0,0,0};
int32_t             RS::B[RS::POINT_LENGTH]    = {0,0,0};

uint8_t             RS::stepSignature          = 0;
AxisSignatureIndex  RS::stepSignatureIndex     = ASGI_MASTER;
uint32_t            RS::impulseCount           = 0;
uint32_t            RS::xStepCount             = 0;
uint32_t            RS::yStepCount             = 0;
uint32_t            RS::zStepCount             = 0;

/////////////////////////////////////////////////////////////////////////////////////
ArduinoPositionRenderer::ArduinoPositionRenderer()
: mode  (RenderMove)
, RD    ()
, pX    (NULL)
, pY    (NULL)
, pZ    (NULL)
///////////////////////////////////////////////////////////////////////////////////// 
{
  //CNC_RENDERER_LOG_FUNCTION()
}
/////////////////////////////////////////////////////////////////////////////////////
ArduinoPositionRenderer::~ArduinoPositionRenderer() {
/////////////////////////////////////////////////////////////////////////////////////  
  //CNC_RENDERER_LOG_FUNCTION()
}
/////////////////////////////////////////////////////////////////////////////////////
bool ArduinoPositionRenderer::isReadyToRender() {
/////////////////////////////////////////////////////////////////////////////////////
  if (     ( ArduinoMainLoop::isSketchEnv() )
        && ( pX == NULL || pY == NULL || pZ == NULL ) 
     ) 
  {
    ArduinoMainLoop::pushMessage(MT_ERROR, E_RMDR_INVALID_STP_POINTER); 
    return false;
  }

  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
void ArduinoPositionRenderer::setupSteppers(CncAxisX* x, CncAxisY* y, CncAxisZ* z) {
/////////////////////////////////////////////////////////////////////////////////////
  pX  = x;
  pY  = y;
  pZ  = z;
  
  if ( pX == NULL || pY == NULL || pZ == NULL )
    ArduinoMainLoop::pushMessage(MT_ERROR, E_RMDR_INVALID_STP_POINTER); 
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoPositionRenderer::stepping() {  
/////////////////////////////////////////////////////////////////////////////////////
  // if stepping() is called from renderMove - normal processing -
  //  - RS conatins dx, dy and dz with a range of [-1, 0, +1] only
  //  - In this connection 0, 1, 2 or 3 axes can be included

  // Attention: checkRuntimeEnv() has to be processed always 
  // to have a correct signal handling, also if the RS::empty() == true
    byte retValue = checkRuntimeEnv();
    if ( retValue != RET_OK )
      return retValue;
    
  // .......................................................  
    // avoid empty processing
    if ( RS::empty() )
      return RET_OK;

    ARDO_TRACE_MOVE(POS_STORE_SID_STEPPING, (int32_t)RS::dx(), (int32_t)RS::dy(), (int32_t)RS::dz());    
    notifyMovePartInit();  
      
    RS::impulseCount++;
    RS::stepSignature = 0;
      
    if ( RS::dx() != 0 ) {
      if ( (retValue = INITIALIZE_SETP_X) != RET_OK ) 
        return retValue;
  
      RS::stepSignature |= ASG_X;
      RS::xStepCount++;
    }
    
    if ( RS::dy() != 0 ) {
       if ( (retValue = INITIALIZE_SETP_Y) != RET_OK )
        return retValue;
        
      RS::stepSignature |= ASG_Y;
      RS::yStepCount++;
    }
      
    if ( RS::dz() != 0 ) {
       if ( (retValue = INITIALIZE_SETP_Z) != RET_OK )
        return retValue;
        
      RS::stepSignature |= ASG_Z;
      RS::zStepCount++;
    }
    
    notifyMovePartBefore();
  
    if ( FINALIZE_SETP_X == false ) { PRINT_DEBUG_VALUE1("FINALIZE_SETP_X","false") }
    if ( FINALIZE_SETP_Y == false ) { PRINT_DEBUG_VALUE1("FINALIZE_SETP_Y","false") }
    if ( FINALIZE_SETP_Z == false ) { PRINT_DEBUG_VALUE1("FINALIZE_SETP_Z","false") }
  
    notifyMovePartAfter();
    
    RS::swap();
    return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoPositionRenderer::directMove(int8_t dx, int8_t dy, int8_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  ARDO_TRACE_MOVE(POS_STORE_SID_DIRECT_MOVE, (int32_t)dx, (int32_t)dy, (int32_t)dz);
  mode = DirectMove;

  if ( SET_DIRECTION_X(dx) == false )
    return RET_ERROR;

  if ( SET_DIRECTION_Y(dy) == false )
    return RET_ERROR;

  if ( SET_DIRECTION_Z(dz) == false )
    return RET_ERROR;
  
  // dx() = A[IDX_X] - B[IDX_X] 
  // dy() = A[IDX_Y] - B[IDX_Y]
  // dz() = A[IDX_Z] - B[IDX_Z]
  
  RS::B[IDX_X] = 0;
  RS::B[IDX_Y] = 0;
  RS::B[IDX_Z] = 0;
  
  RS::A[IDX_X] = dx == 0 ? 0 : dx > 1 ? 0 : -1;
  RS::A[IDX_Y] = dy == 0 ? 0 : dy > 1 ? 0 : -1;
  RS::A[IDX_Z] = dz == 0 ? 0 : dz > 1 ? 0 : -1;
 
  return stepping();
}
/////////////////////////////////////////////////////////////////////////////////////
byte ArduinoPositionRenderer::renderMove(int32_t dx, int32_t dy, int32_t dz) {
/////////////////////////////////////////////////////////////////////////////////////
  ARDO_TRACE_MOVE(POS_STORE_SID_RENDER_AND_MOVE, dx, dy, dz);

  // avoid empty processing
  if ( dx == 0 && dy == 0 && dz == 0 ) 
    return RET_OK;

  mode = RenderMove;

  RD.x_inc = (dx < 0) ? -1 : 1;
  RD.l     = ArdoObj::absolute<int32_t>(dx);
  
  RD.y_inc = (dy < 0) ? -1 : 1;
  RD.m     = ArdoObj::absolute<int32_t>(dy);
  
  RD.z_inc = (dz < 0) ? -1 : 1;
  RD.n     = ArdoObj::absolute<int32_t>(dz);

  RD.dx2 = RD.l << 1;
  RD.dy2 = RD.m << 1;
  RD.dz2 = RD.n << 1;

  //------------------------------------------------------
  // first setup the directions - this can be done once
  // because the directions didn't switch during a call of
  // renderAndStepAxisXYZ()
  if ( SET_DIRECTION_X(dx) == false )
    return RET_ERROR;

  if ( SET_DIRECTION_Y(dy) == false )
    return RET_ERROR;

  if ( SET_DIRECTION_Z(dz) == false )
    return RET_ERROR;
  
  //------------------------------------------------------
  unsigned char ret = RET_ERROR;
  RS::reset();
  
  //------------------------------------------------------
  // render driven by the X axis
  if ((RD.l >= RD.m) && (RD.l >= RD.n)) {
    RD.err_1 = RD.dy2 - RD.l;
    RD.err_2 = RD.dz2 - RD.l;
    
    for (RD.idx = 0; RD.idx < RD.l; RD.idx++) {
      
      ret = stepping();
      if ( ret != RET_OK ) { return ret; }
      if ( RD.err_1 > 0 )  { RS::A[IDX_Y] += RD.y_inc; RD.err_1 -= RD.dx2; }
      if ( RD.err_2 > 0 )  { RS::A[IDX_Z] += RD.z_inc; RD.err_2 -= RD.dx2; }
      
      RD.err_1         += RD.dy2;
      RD.err_2         += RD.dz2;
      RS::A[IDX_X]     += RD.x_inc;
    }

  //------------------------------------------------------  
  // render driven by the Y axis
  } else if ((RD.m >= RD.l) && (RD.m >= RD.n)) {
    RD.err_1 = RD.dx2 - RD.m;
    RD.err_2 = RD.dz2 - RD.m;
    
    for (RD.idx = 0; RD.idx < RD.m; RD.idx++) {

      ret = stepping();
      if ( ret != RET_OK ) { return ret; }
      if ( RD.err_1 > 0 )  { RS::A[IDX_X] += RD.x_inc; RD.err_1 -= RD.dy2; }
      if ( RD.err_2 > 0 )  { RS::A[IDX_Z] += RD.z_inc; RD.err_2 -= RD.dy2; }
      
      RD.err_1         += RD.dx2;
      RD.err_2         += RD.dz2;
      RS::A[IDX_Y]     += RD.y_inc;
    }

  //------------------------------------------------------  
  // render driven by the Z axis
  } else {
    RD.err_1 = RD.dy2 - RD.n;
    RD.err_2 = RD.dx2 - RD.n;
    
    for (RD.idx = 0; RD.idx < RD.n; RD.idx++) {

      ret = stepping();
      if ( ret != RET_OK ) { return ret; }
      if ( RD.err_1 > 0 )  { RS::A[IDX_Y] += RD.y_inc; RD.err_1 -= RD.dz2; }
      if ( RD.err_2 > 0 )  { RS::A[IDX_X] += RD.x_inc; RD.err_2 -= RD.dz2; }
      
      RD.err_1         += RD.dy2;
      RD.err_2         += RD.dx2;
      RS::A[IDX_Z]     += RD.z_inc;
    }
  }

  // final step(s)
  if ( (ret = stepping()) != RET_OK )
    return ret;
  
  return RET_OK;
}
