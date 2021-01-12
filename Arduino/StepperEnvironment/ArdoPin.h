#ifndef ARDO_PIN_H
#define ARDO_PIN_H

// -------------------------------------------------------------------------------
// Default Pin Setup - has to be overriden by dedicated setups on demand below
// -------------------------------------------------------------------------------
  #define TOOL_STATE_OFF                                    PL_HIGH
  #define TOOL_STATE_ON                                     PL_LOW

  #define ENABLE_STATE_OFF                                  PL_HIGH
  #define ENABLE_STATE_ON                                   PL_LOW 

  #ifndef SKETCH_COMPILE

    //#define ARDUINO_AVR_UNO
    #define ARDUINO_AVR_MEGA2560
    
    #define digitalPinToPort(P) AE::getDigitalPinToPort(P)   
    
    namespace AE {
      // C++ Application Environment
      static const short PN_NOT_A_PIN = 0xFF;
      
      enum PinName   { PN_NULL=PN_NOT_A_PIN, 
                       
                       #if defined(ARDUINO_AVR_MEGA2560)
                         PN_D0 = 0, PN_D1 = 1, PN_D2 = 3, PN_D3 = 3, PN_D4 = 4, PN_D5 = 5, PN_D6 = 6, PN_D7 = 7, PN_D8 = 8, PN_D9 = 9, 
                         PN_D10=10, PN_D11=11, PN_D12=12, PN_D13=13, PN_D14=14, PN_D15=15, PN_D16=16, PN_D17=17, PN_D18=18, PN_D19=19, 
                         PN_D20=20, PN_D21=21, PN_D22=22, PN_D23=23, PN_D24=24, PN_D25=25, PN_D26=26, PN_D27=27, PN_D28=28, PN_D29=29, 
                         PN_D30=30, PN_D31=31, PN_D32=32, PN_D33=33, PN_D34=34, PN_D35=35, PN_D36=36, PN_D37=37, PN_D38=38, PN_D39=39, 
                         PN_D40=40, PN_D41=41, PN_D42=42, PN_D43=43, PN_D44=44, PN_D45=45, PN_D46=46, PN_D47=47, PN_D48=48, PN_D49=49, 
                         PN_D50=50, PN_D51=51, PN_D52=52, PN_D53=53,
                        
                         PN_A0 = 54, PN_A1 = 55, PN_A2 = 56, PN_A3 = 57, PN_A4 = 58, PN_A5 = 59, PN_A6=60, PN_A7 = 61, PN_A8 = 628, PN_A9 = 63, 
                         PN_A10= 64, PN_A11= 65, PN_A12= 66, PN_A13= 67, PN_A14= 68, PN_A15= 69
                       #endif 
                       
                       #if defined(ARDUINO_AVR_UNO)
                         PN_D0 = 0, PN_D1 = 1, PN_D2 = 3, PN_D3 = 3, PN_D4 = 4, PN_D5 = 5, PN_D6 = 6, PN_D7 = 7, PN_D8 = 8, PN_D9 = 9, 
                         PN_D10=10, PN_D11=11, PN_D12=12, PN_D13=13,
                        
                         PN_A0 = 54, PN_A1 = 55, PN_A2 = 56, PN_A3 = 57, PN_A4 = 58, PN_A5 = 59
                       #endif 
                     };
  
                       
      enum PinLevel  { PL_UNDEFINED = -1, PL_LOW = 0, PL_HIGH = 1 };
      enum PinMode   { PM_INPUT, PM_OUTPUT, PM_INPUT_PULLUP };
      enum PinType   { PT_UNKNOWN = 0, PT_ANALOG = 1, PT_DIGITAL = 2 };
    };
    
  #else
    // Arduino Sketch Environment
    #define AE
    
    static const short PN_NOT_A_PIN = NOT_A_PIN;
    enum PinName   { PN_NULL=PN_NOT_A_PIN, 

                     #if defined(ARDUINO_AVR_MEGA2560)
                       PN_D0 = 0, PN_D1 = 1, PN_D2 = 3, PN_D3 = 3, PN_D4 = 4, PN_D5 = 5, PN_D6 = 6, PN_D7 = 7, PN_D8 = 8, PN_D9 = 9, 
                       PN_D10=10, PN_D11=11, PN_D12=12, PN_D13=13, PN_D14=14, PN_D15=15, PN_D16=16, PN_D17=17, PN_D18=18, PN_D19=19, 
                       PN_D20=20, PN_D21=21, PN_D22=22, PN_D23=23, PN_D24=24, PN_D25=25, PN_D26=26, PN_D27=27, PN_D28=28, PN_D29=29, 
                       PN_D30=30, PN_D31=31, PN_D32=32, PN_D33=33, PN_D34=34, PN_D35=35, PN_D36=36, PN_D37=37, PN_D38=38, PN_D39=39, 
                       PN_D40=40, PN_D41=41, PN_D42=42, PN_D43=43, PN_D44=44, PN_D45=45, PN_D46=46, PN_D47=47, PN_D48=48, PN_D49=49, 
                       PN_D50=50, PN_D51=51, PN_D52=52, PN_D53=53,
                       
                       PN_A0 = A0, PN_A1 = A1, PN_A2 = A2, PN_A3 = A3, PN_A4 = A4, PN_A5=A5, PN_A6=A6, PN_A7=A7, PN_A8=A8, PN_A9=A9, 
                       PN_A10=A10, PN_A11=A11, PN_A12=A12, PN_A13=A13, PN_A14=A14, PN_A15=A15
                     #endif                     
                     
                     #if defined(ARDUINO_AVR_UNO)
                       PN_D0 = 0, PN_D1 = 1, PN_D2 = 3, PN_D3 = 3, PN_D4 = 4, PN_D5 = 5, PN_D6 = 6, PN_D7 = 7, PN_D8 = 8, PN_D9 = 9, 
                       PN_D10=10, PN_D11=11, PN_D12=12, PN_D13=13,
                       
                       PN_A0 = A0, PN_A1 = A1, PN_A2 = A2, PN_A3 = A3, PN_A4 = A4, PN_A5=A5
                     #endif                     
                   };

    enum PinLevel  { PL_UNDEFINED = -1, PL_LOW = LOW, PL_HIGH = HIGH };
    enum PinMode   { PM_INPUT = INPUT, PM_OUTPUT = OUTPUT, PM_INPUT_PULLUP = INPUT_PULLUP };
    enum PinType   { PT_UNKNOWN = 0, PT_ANALOG = 1, PT_DIGITAL = 2 };

#endif

// -------------------------------------------------------------------------------
// PIN_SETUP_CL57Y
// -------------------------------------------------------------------------------
  #if defined(ARDUINO_AVR_MEGA2560)

    #define MIN_DPIN                                        AE::PN_D0
    #define MAX_DPIN                                        AE::PN_D53
    
    #define MIN_APIN                                        AE::PN_A0
    #define MAX_APIN                                        AE::PN_A14

    #define MAX_PINS                                        AE::PN_A14

    #define SETUP_ID                                        200
    #define BOARD_TYPE                                      MEGA_2560
    
    const unsigned char PIN_IR_1                            =   2;
    const unsigned char PIN_IR_2                            =   3;

    const unsigned char PIN_X_STP                           =   4;
    const unsigned char PIN_X_DIR                           =   5;
    const unsigned char PIN_Y_STP                           =   6;
    const unsigned char PIN_Y_DIR                           =   7;
    const unsigned char PIN_Z_STP                           =   8;
    const unsigned char PIN_Z_DIR                           =   9;

    const unsigned char PIN_ENABLE_STEPPER                  =  10;
    const unsigned char PIN_ENABLE_TOOL                     =  11;

    // PINA
    //const unsigned char PIN_                              =  29;
    //const unsigned char PIN_                              =  28;
    //const unsigned char PIN_                              =  27;
    //const unsigned char PIN_                              =  26;
    //const unsigned char PIN_                              =  25;
    //const unsigned char PIN_                              =  24;
    //const unsigned char PIN_                              =  23;
    const unsigned char PIN_IS_TOOL_POWERED                 =  22;
    
    // PINC
    const unsigned char PIN_X_MIN_LIMIT                     =  37;
    const unsigned char PIN_X_MAX_LIMIT                     =  36;
    const unsigned char PIN_Y_MIN_LIMIT                     =  35;
    const unsigned char PIN_Y_MAX_LIMIT                     =  34;
    const unsigned char PIN_Z_MIN_LIMIT                     =  33;
    const unsigned char PIN_Z_MAX_LIMIT                     =  32;
    const unsigned char PIN_EXTERNAL_INTERRUPT              =  31;
    //const unsigned char PIN_                              =  30;

    // PIN
    //const unsigned char PIN_                              =  45;
    //const unsigned char PIN_                              =  44;
    //const unsigned char PIN_                              =  43;
    //const unsigned char PIN_                              =  42;
    //const unsigned char PIN_                              =  41;
    //const unsigned char PIN_                              =  40;
    const unsigned char PIN_H_MIN_LIMIT                     =  39;
    const unsigned char PIN_H_MAX_LIMIT                     =  38;

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
/*      
      #define WRITE_DIR_PIN_X(value)                        if ( value ) { PORTH |=  (1 << PH4); } else { PORTH &= ~(1 << PH4); }
      #define WRITE_DIR_PIN_Y(value)                        if ( value ) { PORTH |=  (1 << PH5); } else { PORTH &= ~(1 << PH5); }
      #define WRITE_DIR_PIN_Z(value)                        if ( value ) { PORTH |=  (1 << PH6); } else { PORTH &= ~(1 << PH6); }
      
      #define WRITE_STP_PIN_X(value)                        if ( value ) { PORTG |=  (1 << PG5); } else { PORTG &= ~(1 << PG5); }
      #define WRITE_STP_PIN_Y(value)                        if ( value ) { PORTE |=  (1 << PE3); } else { PORTE &= ~(1 << PE3); }
      #define WRITE_STP_PIN_Z(value)                        if ( value ) { PORTH |=  (1 << PH3); } else { PORTH &= ~(1 << PH3); }
*/
      #define WRITE_STP_PIN_X(value)                        if ( value ) { PORTG |=  (1 << PG5); } else { PORTG &= ~(1 << PG5); }
      #define WRITE_DIR_PIN_X(value)                        if ( value ) { PORTE |=  (1 << PE3); } else { PORTE &= ~(1 << PE3); }
      
      #define WRITE_STP_PIN_Y(value)                        if ( value ) { PORTH |=  (1 << PH3); } else { PORTH &= ~(1 << PH3); }
      #define WRITE_DIR_PIN_Y(value)                        if ( value ) { PORTH |=  (1 << PH4); } else { PORTH &= ~(1 << PH4); }

      #define WRITE_STP_PIN_Z(value)                        if ( value ) { PORTH |=  (1 << PH5); } else { PORTH &= ~(1 << PH5); }
      #define WRITE_DIR_PIN_Z(value)                        if ( value ) { PORTH |=  (1 << PH6); } else { PORTH &= ~(1 << PH6); }
      
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
#if defined(ARDUINO_AVR_UNO)
  
    #define MIN_DPIN                                        AE::PN_D0
    #define MAX_DPIN                                        AE::PN_D13
    
    #define MIN_APIN                                        AE::PN_A0
    #define MAX_APIN                                        AE::PN_A5
    
    #define MAX_PINS                                        AE::PN_A5
    
    #define SETUP_ID                                        100
    #define BOARD_TYPE                                      UNO
    
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
    const unsigned char PIN_IS_TOOL_POWERED                 =   0;
    
  
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

    #define READ_LMT_PIN_X_MIN                            ( AE::digitalRead(PIN_X_MIN_LIMIT) )
    #define READ_LMT_PIN_X_MAX                            ( AE::digitalRead(PIN_X_MAX_LIMIT) )
    #define READ_LMT_PIN_X                                ( AE::digitalRead(PIN_X_MIN_LIMIT) && AE::digitalRead(PIN_X_MAX_LIMIT) )

    #define READ_LMT_PIN_Y_MIN                            ( AE::digitalRead(PIN_Y_MIN_LIMIT) )
    #define READ_LMT_PIN_Y_MAX                            ( AE::digitalRead(PIN_Y_MAX_LIMIT) )
    #define READ_LMT_PIN_Y                                ( AE::digitalRead(PIN_Y_MIN_LIMIT) && AE::digitalRead(PIN_Y_MAX_LIMIT) )
    
    #define READ_LMT_PIN_Z_MIN                            ( AE::digitalRead(PIN_Z_MIN_LIMIT) )
    #define READ_LMT_PIN_Z_MAX                            ( AE::digitalRead(PIN_Z_MAX_LIMIT) )
    #define READ_LMT_PIN_Z                                ( AE::digitalRead(PIN_Z_MIN_LIMIT) && AE::digitalRead(PIN_Z_MAX_LIMIT) )
    
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
    
  #endif  // PIN_SETUP_CNC_SHIELD

#endif // ARDO_PIN_H
