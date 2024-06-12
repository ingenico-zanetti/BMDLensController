#ifndef __AT_COMMAND_HPP_INCLUDED__
#define __AT_COMMAND_HPP_INCLUDED__

#include <cstdint>
#include <Arduino.h>
#include <vector>
#include "FirstLevelCommand.hpp"

#define MAX_COMMAND_SIZE (200)

class AtCommandAnalyzer {
  private:
  char data[MAX_COMMAND_SIZE];
  char *rPtr;
  char *wPtr;
  uint32_t maxSize;
  uint32_t currentSize;
  std::vector <FirstLevelCommand*> firstLevelCommands;

  public:
  AtCommandAnalyzer(void);
  void init(uint32_t size);
  char read(void);
  void reInit(void);
  void checkAndStrip(void);
  void addChar(char car);
  void analyze(char *szString);
  void addCallback(char c, bool (*f)(const char *, int));

};


#endif // __AT_COMMAND_HPP_INCLUDED__
