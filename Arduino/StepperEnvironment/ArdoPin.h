#ifndef ARDO_PIN_H
#define ARDO_PIN_H

// -------------------------------------------------------------------------------
// Default Pin Setup - has to be overriden by dedicated setups on demand below
// -------------------------------------------------------------------------------
  #define TOOL_STATE_OFF                                    PL_HIGH
  #define TOOL_STATE_ON                                     PL_LOW

  #define ENABLE_STATE_OFF                                  PL_HIGH
  #define ENABLE_STATE_ON                                   PL_LOW 

// -------------------------------------------------------------------------------
// Choose one dedicated pin setup below exclusivly
// -------------------------------------------------------------------------------
  #define PIN_SETUP_CNC_SHIELD                              0
  #define PIN_SETUP_CL57Y                                   1
  
// -------------------------------------------------------------------------------
// PIN_SETUP_CL57Y
// -------------------------------------------------------------------------------
  #if ( PIN_SETUP_CL57Y )

    #define SETUP_ID                                        200
    #define MAX_PINS                                        64
    #define BOARD_TYPE                                      MEGA_2560
    
    const unsigned char PIN_IR_1                            =   2;
    const unsigned char PIN_IR_2                            =   3;

    const unsigned char PIN_X_STP                           =   4;
    const unsigned char PIN_Y_STP                           =   5;
    const unsigned char PIN_Z_STP                           =   6;
    const unsigned char PIN_X_DIR                           =   7;
    const unsigned char PIN_Y_DIR                           =   8;
    const unsigned char PIN_Z_DIR                           =   9;
    
    const unsigned char PIN_ENABLE_STEPPER                  =  10;
    const unsigned char PIN_ENABLE_TOOL                     =  11;

    // PINA
    const unsigned char PIN_IS_TOOL_POWERED                 =  22;
    //const unsigned char PIN_                     =  23;
    //const unsigned char PIN_                     =  24;
    //const unsigned char PIN_                     =  25;
    //const unsigned char PIN_                     =  26;
    //const unsigned char PIN_                     =  27;
    //const unsigned char PIN_                     =  28;
    //const unsigned char PIN_                     =  29;
    
    // PINC
    const unsigned char PIN_X_MIN_LIMIT                     =  37;
    const unsigned char PIN_X_MAX_LIMIT                     =  36;
    const unsigned char PIN_Y_MIN_LIMIT                     =  35;
    const unsigned char PIN_Y_MAX_LIMIT                     =  34;
    const unsigned char PIN_Z_MIN_LIMIT                     =  33;
    const unsigned char PIN_Z_MAX_LIMIT                     =  33;
    const unsigned char PIN_EXTERNAL_INTERRUPT              =  31;

    #define PIN_INTERRUPT                                AE::PN_A0   
    const unsigned char PIN_INTERRUPT_ID                    =   0;
  
    #define PIN_INTERRUPT_LED                            AE::PN_A3   
    const unsigned char PIN_INTERRUPT_LED_ID                 =  3;

    // Direct port (pin) manipulation
    #ifdef SKETCH_COMPILE
      // Attention: the following read and write macros have to be aligned 
      // with the numeric pind declaration above

      #define READ_LMT_PINS                                 ((PINC & B00111111 ) == B00111111 ? LimitSwitch::LIMIT_SWITCH_OFF : LimitSwitch::LIMIT_SWITCH_ON)

      #define READ_LMT_PIN_X_MIN                            ( PINC & B00000001 ) // 37
      #define READ_LMT_PIN_X_MAX                            ( PINC & B00000010 ) // 36
      #define READ_LMT_PIN_X                                ((PINC & B00000011 ) ==  B00000011 ? LimitSwitch::LIMIT_SWITCH_OFF : LimitSwitch::LIMIT_SWITCH_ON)
      
      #define READ_LMT_PIN_Y_MIN                            ( PINC & B00000100) // 35      
      #define READ_LMT_PIN_Y_MAX                            ( PINC & B00001000) // 34
      #define READ_LMT_PIN_Y                                ((PINC & B00001100 ) == B00001100 ? LimitSwitch::LIMIT_SWITCH_OFF : LimitSwitch::LIMIT_SWITCH_ON)

      #define READ_LMT_PIN_Z_MIN                            ( PINC & B00010000 ) // 33
      #define READ_LMT_PIN_Z_MAX                            ( PINC & B00100000 ) // 32
      #define READ_LMT_PIN_Z                                ((PINC & B00110000 ) == B00110000 ? LimitSwitch::LIMIT_SWITCH_OFF : LimitSwitch::LIMIT_SWITCH_ON)

      #define READ_EXT_INNTERRUPT_PIN                       ( PINC & B01000000 ) // 31
      
      #define READ_ENABLE_STEPPER_PIN                       ( PINB & B00010000 ) // 10
      #define READ_ENABLE_TOOL_PIN                          ( PINB & B00100000 ) // 11
      #define READ_IS_TOOL_POWERED_PIN                      ( PINA & B00000001 ) // 22
      
      #define WRITE_DIR_PIN_X(value)                        if ( value ) { PORTH |=  (1 << PH4); } else { PORTH &= ~(1 << PH4); }
      #define WRITE_DIR_PIN_Y(value)                        if ( value ) { PORTH |=  (1 << PH5); } else { PORTH &= ~(1 << PH5); }
      #define WRITE_DIR_PIN_Z(value)                        if ( value ) { PORTH |=  (1 << PH6); } else { PORTH &= ~(1 << PH6); }
      
      #define WRITE_STP_PIN_X(value)                        if ( value ) { PORTG |=  (1 << PG5); } else { PORTG &= ~(1 << PG5); }
      #define WRITE_STP_PIN_Y(value)                        if ( value ) { PORTE |=  (1 << PE3); } else { PORTE &= ~(1 << PE3); }
      #define WRITE_STP_PIN_Z(value)                        if ( value ) { PORTH |=  (1 << PH3); } else { PORTH &= ~(1 << PH3); }
      
    #else
    
      #define READ_LMT_PIN_X_MIN                            ( AE::digitalRead(PIN_X_MIN_LIMIT) )
      #define READ_LMT_PIN_X_MAX                            ( AE::digitalRead(PIN_X_MAX_LIMIT) )
      #define READ_LMT_PIN_X                                ( AE::digitalRead(PIN_X_MIN_LIMIT) && AE::digitalRead(PIN_X_MAX_LIMIT) )

      #define READ_LMT_PIN_Y_MIN                            ( AE::digitalRead(PIN_Y_MIN_LIMIT) )
      #define READ_LMT_PIN_Y_MAX                            ( AE::digitalRead(PIN_Y_MAX_LIMIT) )
      #define READ_LMT_PIN_Y                                ( AE::digitalRead(PIN_Y_MIN_LIMIT) && AE::digitalRead(PIN_Y_MAX_LIMIT) )

      #define READ_LMT_PIN_Z_MIN                            ( AE::digitalRead(PIN_Z_MIN_LIMIT) )
      #define READ_LMT_PIN_Z_MAX                            ( AE::digitalRead(PIN_Z_MAX_LIMIT) )
      #define READ_LMT_PIN_Z                                ( AE::digitalRead(PIN_Z_MIN_LIMIT) && AE::digitalRead(PIN_Z_MAX_LIMIT) )

      #define READ_LMT_PINS                                 ( READ_LMT_PIN_X && READ_LMT_PIN_Y && READ_LMT_PIN_Z )
      
      #define READ_EXT_INNTERRUPT_PIN                       AE::digitalRead(PIN_EXTERNAL_INTERRUPT)

      #define READ_ENABLE_STEPPER_PIN                       AE::digitalRead(PIN_ENABLE_STEPPER)
      #define READ_ENABLE_TOOL_PIN                          AE::digitalRead(PIN_ENABLE_TOOL)
      #define READ_IS_TOOL_POWERED_PIN                      AE::digitalRead(PIN_IS_TOOL_POWERED)

      #define READ_STEPPER_ENABLE_PIN                       AE::digitalRead(PIN_ENABLE_STEPPER)
      #define READ_TOOL_ENABLE_PIN                          AE::digitalRead(PIN_ENABLE_TOOL)
      #define READ_IS_TOOL_POWERED_PIN                      AE::digitalRead(PIN_IS_TOOL_POWERED)
      
      #define WRITE_DIR_PIN_X(value)                        AE::digitalWrite(PIN_X_DIR, value); 
      #define WRITE_DIR_PIN_Y(value)                        AE::digitalWrite(PIN_Y_DIR, value); 
      #define WRITE_DIR_PIN_Z(value)                        AE::digitalWrite(PIN_Z_DIR, value); 
      
      #define WRITE_STP_PIN_X(value)                        AE::digitalWrite(PIN_X_STP, value); 
      #define WRITE_STP_PIN_Y(value)                        AE::digitalWrite(PIN_Y_STP, value); 
      #define WRITE_STP_PIN_Z(value)                        AE::digitalWrite(PIN_Z_STP, value); 
    
    #endif // SKETCH_COMPILE
        
  #endif // PIN_SETUP_CL57Y
  
// -------------------------------------------------------------------------------
// PIN_SETUP_CNC_SHIEDL
// -------------------------------------------------------------------------------
  #if ( PIN_SETUP_CNC_SHIELD )
  
    #define SETUP_ID                                        100
    #define MAX_PINS                                        32
    #define BOARD_TYPE                                      UNO
    
    #define TOOL_STATE_OFF                                  PL_LOW
    #define TOOL_STATE_ON                                   PL_HIGH 
  
    #define ENABLE_STATE_OFF                                PL_HIGH
    #define ENABLE_STATE_ON                                 PL_LOW 

    const unsigned char PIN_IR_1                            =   0;
    const unsigned char PIN_IR_2                            =   0;
    
    const unsigned char PIN_X_STP                           =   2;
    const unsigned char PIN_Y_STP                           =   3;
    const unsigned char PIN_Z_STP                           =   4;
    const unsigned char PIN_X_DIR                           =   5;
    const unsigned char PIN_Y_DIR                           =   6;
    const unsigned char PIN_Z_DIR                           =   7;
  
    const unsigned char PIN_ENABLE_STEPPER                  =   8;

    const unsigned char PIN_X_MIN_LIMIT                     =   9;
    const unsigned char PIN_X_MAX_LIMIT                     =   9;
    const unsigned char PIN_Y_MIN_LIMIT                     =  10;
    const unsigned char PIN_Y_MAX_LIMIT                     =  10;
    const unsigned char PIN_Z_MIN_LIMIT                     =  11;
    const unsigned char PIN_Z_MAX_LIMIT                     =  11;
  
    const unsigned char PIN_ENABLE_TOOL                     =  12;
    const unsigned char PIN_EXTERNAL_INTERRUPT              =  13;
  
    // A0 CNC Shield: Reset/Abort
    // A1 CNC Shield: Feed Hold
    // A2 CNC Shield: Cycle Start / Resume
    // A3 CNC Shield: Collant Enabled
    // A4 CNC Shield: free
    // A5 CNC Shield: free
    
    #define PIN_INTERRUPT                                AE::PN_A0   
    const unsigned char PIN_INTERRUPT_ID                    =   0;
  
    #define PIN_INTERRUPT_LED                            AE::PN_A3   
    const unsigned char PIN_INTERRUPT_LED_ID                 =  3;

    #define READ_LMT_PIN_X_MIN                            AE::digitalRead(PIN_X_MIN_LIMIT)
    #define READ_LMT_PIN_X_MAX                            AE::digitalRead(PIN_X_MAX_LIMIT)
    #define READ_LMT_PIN_Y_MIN                            AE::digitalRead(PIN_Y_MIN_LIMIT)
    #define READ_LMT_PIN_Y_MAX                            AE::digitalRead(PIN_Y_MAX_LIMIT)
    #define READ_LMT_PIN_Z_MIN                            AE::digitalRead(PIN_Z_MIN_LIMIT)
    #define READ_LMT_PIN_Z_MAX                            AE::digitalRead(PIN_Z_MAX_LIMIT)
    #define READ_EXT_INNTERRUPT_PIN                       AE::digitalRead(PIN_EXTERNAL_INTERRUPT)

    #define READ_STEPPER_ENABLE_PIN                       AE::digitalRead(PIN_STEPPER_ENABLE)
    #define READ_TOOL_ENABLE_PIN                          AE::digitalRead(PIN_TOOL_ENABLE)
    
    #define WRITE_DIR_PIN_X                               AE::digitalWrite(PIN_X_DIR, value); 
    #define WRITE_DIR_PIN_Y                               AE::digitalWrite(PIN_Y_DIR, value); 
    #define WRITE_DIR_PIN_Z                               AE::digitalWrite(PIN_Z_DIR, value); 
    
    #define WRITE_STP_PIN_X                               AE::digitalWrite(PIN_X_STP, value); 
    #define WRITE_STP_PIN_Y                               AE::digitalWrite(PIN_Y_STP, value); 
    #define WRITE_STP_PIN_Z                               AE::digitalWrite(PIN_Z_STP, value); 
    
  #endif  // PIN_SETUP_CNC_SHIELD

#endif // ARDO_PIN_H
