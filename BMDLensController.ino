#include "AtCommand.hpp"
#include "AtCommandI.hpp"
#include "AtCommandZ.hpp"
#include "AtCommandPlus.hpp"
#include "AtCommandAmpersAnd.hpp"
#include "Servo.hpp"
#include "Lens.hpp"

#define __SELF_TEST__
#undef __SELF_TEST__

/*
 * PIN configuration
 */
#define ZoomADC (PA0)  // PA0 / ADC0 / BluePill.PIN05
#define IrisADC (PA3)  // PA3 / ADC3 / BluePill.PIN08
#define FocusADC (PB0) // PB0 / ADC8 / BluePill.PIN13

#define FocusP1 (PB13)
#define FocusP0 (PB12)

#define IrisP0 (PA10)
#define IrisP1 (PA9)

#define ZoomP0 (PB7)
#define ZoomP1 (PB5)

int ledStatus;
uint32_t oldSeconds;
uint32_t oldMillis;
uint32_t oldQuarter;

bool powerPresent;

#ifdef __SELF_TEST__
enum {
  SELF_TEST_STATE_DONE = 0,
  SELF_TEST_STATE_FORWARD,
  SELF_TEST_STATE_BACKWARD,
  SELF_TEST_STATE_PENDING
} selfTestState;
#endif

AtCommandAnalyzer analyzer;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  ledStatus = HIGH;
  digitalWrite(LED_BUILTIN, ledStatus);
  
  analogReference(AR_DEFAULT);
  analogReadResolution(12);
  pinMode(PB10, INPUT); // Rev 1.0 HW had PB10 as FocusADC. PB10 is NOT an ADC pin, so it is strapped to PB0, a valid ADC

  oldMillis = millis();
  oldQuarter = oldMillis / 250;
  oldSeconds = oldMillis / 1000;
  analyzer.addCallback('I', handleATI);
  analyzer.addCallback('+', handlePlus);
  analyzer.addCallback('&', handleAmpersAnd);
  analyzer.addCallback('Z', handleATZ);

  focusServo.setPins(FocusADC, FocusP0, FocusP1);
  zoomServo.setPins(ZoomADC, ZoomP1, ZoomP0);
  irisServo.setPins(IrisADC, IrisP0, IrisP1);

  focusServo.setMode(Servo::MODE_TIME);
  zoomServo.setMode(Servo::MODE_TIME);
  irisServo.setMode(Servo::MODE_TIME);

  focusServo.setPwmRatioMin(focusSettings.parameters.minSpeed);
  focusServo.setPwmScale(focusSettings.parameters.pwmScale);
  focusServo.setTimeoutScale(focusSettings.parameters.timeoutScale);

  zoomServo.setPwmRatioMin(zoomSettings.parameters.minSpeed);
  zoomServo.setPwmScale(zoomSettings.parameters.pwmScale);
  zoomServo.setTimeoutScale(zoomSettings.parameters.timeoutScale);
  
  irisServo.setPwmRatioMin(irisSettings.parameters.minSpeed);
  irisServo.setPwmScale(irisSettings.parameters.pwmScale);
  irisServo.setTimeoutScale(irisSettings.parameters.timeoutScale);
  
 
#ifdef __SELF_TEST__
  selfTestState = SELF_TEST_STATE_PENDING;
#endif
  powerPresent = (zoomServo.getAdcValue() > 1100);
}

#ifdef __SELF_TEST__
static void printSelfTestResult(Servo *servo, const char *szDirection){
  Serial.printf("selfTest[%s]:%s: %4d" "\n", szDirection, servo->getName(), servo->getAdcValue());
}

static void storeSelfTestForwardResult(Servo *servo){
  servo->storeTestForwardResult();
  printSelfTestResult(servo, "FORWARD");
}

static void storeSelfTestBackwardResult(Servo *servo){
  servo->storeTestBackwardResult();
  printSelfTestResult(servo, "BACKWARD");
}
#endif

void loop() {
  focusServo.readAdc();
  zoomServo.readAdc();
  irisServo.readAdc();
  uint32_t newMillis = millis();
  if(newMillis != oldMillis){
    oldMillis = newMillis;
    powerPresent = (zoomServo.getAdcValue() > 1100);
    if(powerPresent){
      focusServo.run();
      zoomServo.run();
      irisServo.run();
    }
  }
#ifdef __SELF_TEST__
  if(selfTestState > SELF_TEST_STATE_DONE){
    if(powerPresent){
      switch(selfTestState){
        case SELF_TEST_STATE_PENDING:
          selfTestState = SELF_TEST_STATE_BACKWARD;
          zoomServo.setDirection(false); zoomServo.setTimeMs(2000);
          irisServo.setDirection(false); irisServo.setTimeMs(2000);
          focusServo.setDirection(false); focusServo.setTimeMs(2000);
        break;
        case SELF_TEST_STATE_BACKWARD:
          if(0 == zoomServo.getTimeMs()){
            storeSelfTestBackwardResult(&zoomServo);
            storeSelfTestBackwardResult(&irisServo);
            storeSelfTestBackwardResult(&focusServo);
            selfTestState = SELF_TEST_STATE_FORWARD;
            zoomServo.setDirection(true); zoomServo.setTimeMs(2000);
            irisServo.setDirection(true); irisServo.setTimeMs(2000);
            focusServo.setDirection(true); focusServo.setTimeMs(2000);
          }
        break;
        case SELF_TEST_STATE_FORWARD:
          if(0 == zoomServo.getTimeMs()){
            selfTestState = SELF_TEST_STATE_DONE;
            storeSelfTestForwardResult(&zoomServo);
            storeSelfTestForwardResult(&irisServo);
            storeSelfTestForwardResult(&focusServo);
          }
        break;
        default:
        break;
      }
    }
  }else
#endif
  {
    int available = Serial.available();
    if(available > 0){
#define BUFFER_SIZE (64)
      char buffer[BUFFER_SIZE];
      if((unsigned int)available > sizeof(buffer)){
        available = sizeof(buffer);
      }
      int lus = Serial.readBytes(buffer, available);
      if(lus > 0){
        for(int i = 0 ; i < lus ; i++){
          analyzer.addChar(buffer[i]);
        }
      }
    }
  }
  uint32_t newSeconds = newMillis / 1000;
  if(newSeconds != oldSeconds){
    oldSeconds = newSeconds;
    if(powerPresent){
      ledStatus = (HIGH == ledStatus) ? LOW : HIGH;
      digitalWrite(LED_BUILTIN, ledStatus);
    }
#ifdef __SELF_TEST__
    else{
      if(selfTestState > SELF_TEST_STATE_DONE){
        Serial.printf("!!! SELF_TEST_PENDING !!!" "\n");
      }
    }
#endif
  }
  uint32_t newQuarter = newMillis / 250;
  if(newQuarter != oldQuarter){
    oldQuarter = newQuarter;
    if(!powerPresent){
      ledStatus = (HIGH == ledStatus) ? LOW : HIGH;
      digitalWrite(LED_BUILTIN, ledStatus);
    }
  }
}
