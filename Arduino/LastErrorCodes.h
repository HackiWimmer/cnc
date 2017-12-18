#ifndef LAST_ERROR_CODES_H
#define LAST_ERROR_CODES_H

#include "CommonValues.h"

///////////////////////////////////////////////////////////////////////////////////////////
class LastErrorCodes {
  private:
    static const unsigned int size = 10;
    unsigned int curIndex;
    unsigned int totalCount;
    
    int     lastErrorCodes[size];
    String  lastErrorTexts[size];

    //////////////////////////////////////////////////////////////////
    void flipArray() {
      for ( unsigned int i=1; i<size-1; i++ ) {
         lastErrorCodes[i-1] = lastErrorCodes[i];
         lastErrorTexts[i-1] = lastErrorTexts[i];
      }
      lastErrorCodes[size-1] = 0;
      lastErrorTexts[size-1] = "";
    }

  public:
    //////////////////////////////////////////////////////////////////
    LastErrorCodes() {
      resetErrorInfo();      
    }

    //////////////////////////////////////////////////////////////////
    virtual ~LastErrorCodes() 
    {}

    //////////////////////////////////////////////////////////////////
    void resetErrorInfo() {
      for ( unsigned int i=0; i< size; i++) {
        lastErrorCodes[i] = 0;
        lastErrorTexts[i] = "";
      }
      
      totalCount = 0;
      curIndex = 0;
    }

    //////////////////////////////////////////////////////////////////
    void setNextErrorInfo(int id, const char* text = NULL );
    void setNextErrorInfo(int id, const String& text);
    
    //////////////////////////////////////////////////////////////////
    int getErrorCount() { return curIndex; }

    //////////////////////////////////////////////////////////////////
    void writeErrorInfo() {
      Serial.write(RET_SOT);

      Serial.print(0);
      Serial.print(TEXT_SEPARATOR);
      Serial.print(E_TOTAL_COUNT);
      Serial.print(TEXT_SEPARATOR);
      Serial.print(String(totalCount));
      Serial.print(TEXT_CLOSE);
     
      for ( unsigned int i=0; i<curIndex; i++) {
        Serial.print(i+1);
        Serial.print(TEXT_SEPARATOR);
        Serial.print(lastErrorCodes[i]);
        Serial.print(TEXT_SEPARATOR);
        Serial.print(lastErrorTexts[i].c_str());
        Serial.print(TEXT_CLOSE);
      }
    }

};

#endif
