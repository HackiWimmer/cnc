#include "LastErrorCodes.h"

//////////////////////////////////////////////////////////////////
unsigned char LastErrorCodes::setNextErrorInfo(int id, const String& text) {
//////////////////////////////////////////////////////////////////
  return setNextErrorInfo(id, text.c_str());
}
//////////////////////////////////////////////////////////////////
unsigned char LastErrorCodes::setNextErrorInfo(int id, const char* text) {
//////////////////////////////////////////////////////////////////
  unsigned int insertIndex = curIndex;

  // log error message
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
    lastErrorTexts[insertIndex] = "";
  }
  
  curIndex++;
  totalCount++;  
  performNextResponseId();

  // puplish this message too
  String msg("{[");
  msg += String(id);
  msg += "]} ";

  if ( text != NULL ) {
    msg += ": ";
    msg += text;
  }

  pushErrorMessage(msg.c_str());
  
  return RET_ERROR;
}
