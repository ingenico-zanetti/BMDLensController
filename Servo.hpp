#ifndef __SERVO_HPP_INCLUDED__
#define __SERVO_HPP_INCLUDED__

#include "SlidingWindow.hpp"
#include "ServoAndLens.hpp"
#include "GlobalConfiguration.hpp"

#define PWM_RATIO_MAX (16) // MUST be 2^n !!!
#define PWM_RATIO_MASK (PWM_RATIO_MAX - 1)

class Servo {
  private:
    SetPoint setPoints[MAX_SET_POINTS];
    int setPointCount;
    unsigned short int adcValue;
    unsigned short int adcMinValue;
    unsigned short int adcMaxValue;
    const char *szName;
    int remainingTimeMs;
    bool forward;
    int adcPin;
    int phase0Pin;
    int phase1Pin;
    int mode;
    SlidingWindow filter;
    unsigned int targetAdcValue;
    bool driving;
    unsigned int pwmRatio;
    unsigned int pwmRatioMax;
    unsigned int pwmRatioMin;
    unsigned int pwmCounter;
    unsigned int pwmScale;
    unsigned int timeoutScale;
    unsigned int timeout;
    unsigned short selfTestForwardResult;
    unsigned short selfTestBackwardResult;

    void runPWM(void);
    unsigned int updatePWMRatio(void);
    int getSetPointIndex(unsigned short setting);
    int getSetPointPreviousIndex(unsigned short setting);

    unsigned int eepromOffset;
    const ServoSettings *servoSettingsFromFW;
    void loadSettingsFromMemory(const ServoSettings *settings);

  public:
    static const int MODE_TIME = 0;
    static const int MODE_ADC = 1;
    Servo(const ServoSettings *s, const char *name, unsigned int offset);
    void print(const char *szUnit);
    
    static char *setPointSettingToString(char *szString, SetPoint *setPoint);
    static bool stringToSetPointSetting(const char *start, int sLen, SetPoint *setPoint);

    // bool insertSetPoint(unsigned short setting, unsigned short adcValue);
    bool setSetPoint(unsigned short setting, unsigned short adcValue);
    // bool deleteSetPoint(unsigned short setting);
    bool getAdcValueFromSetting(SetPoint *setPoint);

    SetPoint *getSetPoints(int *actualCount);
    unsigned short getAdcValue(void);
    unsigned short getAdcMinValue(void);
    unsigned short getAdcMaxValue(void);
    unsigned short readAdc(void);
    void resetMinMax(void);
    const char *getName(void);

    void setPins(int adc, int phase0, int phase1);
    void setDirection(bool = true);
    bool getDirection(void);
    int setMode(int newMode);
    int getMode(void);

    bool setTimeMs(int t);
    int getTimeMs(void);

    bool setDeltaAdc(int t);
    bool setTargetAdcValue(int i);

    int run(void);

    SetPoint *getFirstSetPoint(void);
    SetPoint *getLastSetPoint(void);
    bool isAdcTargetValid(unsigned int adcValue);
    bool isSettingValid(unsigned int setting);

    void stopMotor(const char *szReason);

    unsigned int setPwmRatioMax(unsigned char max);
    unsigned int setPwmRatioMin(unsigned char min);
    bool setPwmScale(unsigned char scale);
    bool setTimeoutScale(unsigned char scale);
    unsigned char getPwmRatioMin(void){ return pwmRatioMin;};
    unsigned char getPwmScale(void) { return pwmScale;};
    unsigned char getTimeoutScale(void) { return timeoutScale;};

    void storeTestForwardResult(void);
    void storeTestBackwardResult(void);

    void loadSettingsFromFW();

    bool loadSettingsFromEEPROM();
    void storeSettingsToEEPROM();
};

extern Servo zoomServo;
extern Servo irisServo;
extern Servo focusServo;
extern Servo *getServo(int c);

#endif // __SERVO_HPP_INCLUDED__
