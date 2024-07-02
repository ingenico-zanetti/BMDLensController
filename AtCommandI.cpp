#include <Arduino.h>

#include "Servo.hpp"
#include "Lens.hpp"
#include "Version.hpp"
#include "GlobalConfiguration.hpp"

#ifdef __PAN_AND_TILT_SUPPORT__
extern HardwareSerial panAndTiltUnit;

static bool panOrTiltForward(const char *szString){
  bool raiseError = false;
  Serial.printf("%s: Received command for Pan&Tilt unit ; command=[AT%s]" "\n", __FILE_NAME__, szString);
  panAndTiltUnit.printf("AT%s" "\r", szString);
  return raiseError;
}
#endif

bool handleATI(const char *szString, int length) {
  bool raiseError = false;
  int index = 0;
  if(length > 1){
    index = szString[1] - '0';
  }
  switch (index) {
    default:
      raiseError = true;
    break;
    case 0:
      Serial.println(getFWVersion());
#ifdef __PAN_AND_TILT_SUPPORT__
      panOrTiltForward("I0");
#endif
      break;
    case 1:{
        char minZoomString[8];
        char maxZoomString[8];
        char minIrisString[8];
        char maxIrisString[8];
        char minFocusString[8];
        char maxFocusString[8];
        zoomServo.setPointSettingToString(minZoomString, zoomServo.getFirstSetPoint());
        zoomServo.setPointSettingToString(maxZoomString, zoomServo.getLastSetPoint());
        irisServo.setPointSettingToString(minIrisString, irisServo.getFirstSetPoint());
        irisServo.setPointSettingToString(maxIrisString, irisServo.getLastSetPoint());
        focusServo.setPointSettingToString(minFocusString, focusServo.getFirstSetPoint());
        focusServo.setPointSettingToString(maxFocusString, focusServo.getLastSetPoint());
        Serial.printf("%s:" " Zoom in [%s..%s]mm," " Iris in [%s..%s]," " Focus in [%s..%s]m" "\n", szLensName, minZoomString, maxZoomString, minIrisString, maxIrisString, minFocusString, maxFocusString);
#ifdef __PAN_AND_TILT_SUPPORT__
        panOrTiltForward("I1");
#endif
      }
      break;
      case 2:
        Serial.printf("%s:" "\n", zoomServo.getName());
        zoomServo.print("mm");
        Serial.printf("%s:" "\n", focusServo.getName());
        focusServo.print("m");
        Serial.printf("%s:" "\n", irisServo.getName());
        irisServo.print("");
#ifdef __PAN_AND_TILT_SUPPORT__
        panOrTiltForward("I2");
#endif
      break;
  }
  return(raiseError);
}
