#include "LastErrorCodes.h"

//////////////////////////////////////////////////////////////////
void LastErrorCodes::setNextErrorInfo(int id, const String& text) {
//////////////////////////////////////////////////////////////////
  setNextErrorInfo(id, text.c_str());
}
//////////////////////////////////////////////////////////////////
void LastErrorCodes::setNextErrorInfo(int id, const char* text) {
//////////////////////////////////////////////////////////////////
  unsigned int insertIndex = curIndex;

  if ( curIndex == size ) {
      flipArray();
      curIndex = size-1;
      insertIndex = curIndex;
  }

  lastErrorCodes[insertIndex] = id;
  
  if ( text != NULL) {
    String t(": ");
    t += text;
    lastErrorTexts[insertIndex] = t.c_str();
  }
  else {
    lastErrorTexts[insertIndex] = MBYTE_CLOSE;
  }
  
  curIndex++;
  totalCount++;  

  String msg("{[");
  msg += String(id);
  msg += "]} ";

  if ( text != NULL )
    msg += ": ";
    msg += text;
  
  pushErrorMessage(msg.c_str());
}
