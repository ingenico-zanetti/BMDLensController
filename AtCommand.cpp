#include "AtCommand.hpp"
#include <ctype.h>

AtCommandAnalyzer::AtCommandAnalyzer(void){
  init(MAX_COMMAND_SIZE);
}

char AtCommandAnalyzer::read(void){
  if(currentSize > 0){
    char c = *--rPtr;
    if(data == rPtr){
      rPtr = data + maxSize; // Wrap around
    }
    currentSize--;
    return(c);
  }
  return(0);
}

void AtCommandAnalyzer::init(uint32_t size){
  wPtr = data + size;
  rPtr = wPtr;
  maxSize = size;
  currentSize = 0;
}

void AtCommandAnalyzer::reInit(void){
  wPtr = data + maxSize;
  rPtr = wPtr;
  currentSize = 0;
}

void AtCommandAnalyzer::checkAndStrip(void){
  // Serial.printf("%s()" "\n", __func__);
  char A = read();
  if('A' == A){
    char T = read();
    if('T' == T){
      char callbackBuffer[MAX_COMMAND_SIZE];
      int i = 0;
      while(currentSize > 1){
        callbackBuffer[i] = read();
        i++;
      }
      callbackBuffer[i] = '\0';
//      Serial.printf("callbackBuffer[]=\"%s\"" "\n", callbackBuffer);
      analyze(callbackBuffer);
    }
  }
  reInit();
}

void AtCommandAnalyzer::addChar(char car){
  *(--wPtr) = toupper(car);
  if(data == wPtr){
    wPtr = data + maxSize; // Wrap around
  }
  if(currentSize == maxSize){
    rPtr = wPtr; // discard old data
  }else{
    currentSize++;
  }
  if(('\r' == car) || ('\n' == car)){
    checkAndStrip();
  }
}

void AtCommandAnalyzer::analyze(char *szString){
  char *start = szString;
  bool error = false;
  for(;;){
    bool endReached = false;
    char first = *start;
    if('\0' == first){
      break;
    }
    int length = 0;
    char *next = strchr(start, ';');
    if(NULL == next){
      endReached = true;
      length = strlen(start);
    }else{
      *next = '\0';
      length = (int)(next - start);
    }
    if(length > 0){
      bool found = false;
      std::vector<FirstLevelCommand*>::iterator it;
      for(it = firstLevelCommands.begin() ; it != firstLevelCommands.end() ; it++){
        if((*it)->getFirstChar() == first){
          error = (*it)->handleCommand(start, length);
          found = true;
          break;
        }
      }
      if(!found){
        error = true;
      }
    }
    if(error || endReached){
      break;
    }
    start = next + 1;
  }
  if(error){
    Serial.printf("\r\nERROR\r\n");
  }else{
    Serial.printf("\r\nOK\r\n");
  }
}

void AtCommandAnalyzer::addCallback(char c, bool (*f)(const char *, int)){
  firstLevelCommands.push_back(new FirstLevelCommand(c, f));
}
