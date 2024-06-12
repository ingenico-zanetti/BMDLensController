#ifndef __FIRST_LEVEL_COMMAND_HPP_INCLUDED__
#define __FIRST_LEVEL_COMMAND_HPP_INCLUDED__

class FirstLevelCommand {
  private:
    char firstChar;
    bool (*parse)(const char *, int);
  public:
    FirstLevelCommand(char car = '\0', bool (*f)(const char *, int) = NULL):firstChar(car), parse(f){};
    char getFirstChar(){return(firstChar);};
    bool handleCommand(const char *szString, int length){bool returnValue = false ; if(NULL != parse){returnValue = parse(szString, length);};return returnValue;};
};

#endif // __FIRST_LEVEL_COMMAND_HPP_INCLUDED__
