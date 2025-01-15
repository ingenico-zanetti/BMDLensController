#include "AtCommand.hpp"
#include "AtCommandI.hpp"
#include "AtCommandZ.hpp"
#include "AtCommandPlus.hpp"
#include "AtCommandAmpersAnd.hpp"
#include "Servo.hpp"
#include "Lens.hpp"
#include "GlobalConfiguration.hpp"

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLL_Source                     = HSE
  *            PLL_Mul                        = 6
  *            Flash Latency(WS)              = 2
  *            ADC Prescaler                  = 4
  *            USB Prescaler                  = 1
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC | RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/*
 * PIN configuration
 */
#define ZoomADC (PA0)  // PA0 / ADC0 / BluePill.PIN05
#define IrisADC (PA3)  // PA3 / ADC3 / BluePill.PIN08
#define FocusADC (PB0) // PB0 / ADC8 / BluePill.PIN13 (was PB10 in HW Rev 1.0.0)

#define FocusP1 (PB13)
#define FocusP0 (PB12)

#define IrisP0 (PA10)
#define IrisP1 (PA9)

#define ZoomP0 (PB7)
#define ZoomP1 (PB5)

int ledStatus;
uint32_t oldSeconds;
uint32_t oldMillis;
uint32_t oldMicros;
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

#ifdef __PAN_AND_TILT_SUPPORT__
HardwareSerial panAndTiltUnit(PB11, PB10); // RX3, TX3
#endif

void setup() {
  Serial.begin(115200);
#ifdef __PAN_AND_TILT_SUPPORT__
  panAndTiltUnit.begin(9600);
#endif

#define __DIM_GREEN_LED__
#ifdef __DIM_GREEN_LED__
  pinMode(LED_BUILTIN, INPUT_PULLUP);
#else
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  ledStatus = HIGH;
  digitalWrite(LED_BUILTIN, ledStatus);
  
  analogReference(AR_DEFAULT);
  analogReadResolution(12);
#ifdef __HARDWARE_REV_1_0_SUPPORT__
  pinMode(PB10, INPUT); // Rev 1.0 HW had PB10 as FocusADC. PB10 is NOT an ADC pin, so it is strapped to PB0, a valid ADC
#endif

  oldMillis = millis();
  oldMicros = micros();
  oldQuarter = oldMillis / 250;
  oldSeconds = oldMillis / 1000;
  analyzer.addCallback('I', handleATI);
  analyzer.addCallback('+', handlePlus);
  analyzer.addCallback('&', handleAmpersAnd);
  analyzer.addCallback('Z', handleATZ);

  focusServo.setPins(FocusADC, FocusP0, FocusP1);
  zoomServo.setPins(ZoomADC, ZoomP1, ZoomP0);
  irisServo.setPins(IrisADC, IrisP0, IrisP1);

  focusServo.setMode(Servo::MODE_DURATION);
  zoomServo.setMode(Servo::MODE_DURATION);
  irisServo.setMode(Servo::MODE_DURATION);
 
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

#define __RUN_MILLIS__
void loop() {
  focusServo.readAdc();
  zoomServo.readAdc();
  irisServo.readAdc();
  bool runServos = false;
  uint32_t newMicros = micros();
  uint32_t deltaMicro = newMicros - oldMicros;
  if(deltaMicro > 50){
#ifndef __RUN_MILLIS__
      runServos = true;
#endif
  }
  uint32_t newMillis = millis();
  if(newMillis != oldMillis){
    oldMillis = newMillis;
    powerPresent = (zoomServo.getAdcValue() > 1100);
    if(powerPresent){
#ifdef __RUN_MILLIS__
      runServos = true;
#endif
    }
  }
  if(runServos){
    focusServo.run();
    zoomServo.run();
    irisServo.run();
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
#ifdef __PAN_AND_TILT_SUPPORT__
  {
    int available = panAndTiltUnit.available();
    if(available > 0){
#define BUFFER_SIZE (64)
      char buffer[BUFFER_SIZE];
      if((unsigned int)available > sizeof(buffer)){
        available = sizeof(buffer);
      }
      int lus = panAndTiltUnit.readBytes(buffer, available);
      if(lus > 0){
        Serial.write(buffer, lus);
      }
    }
  }
#endif
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
