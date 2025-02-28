#ifndef __GLOBAL_CONFIGURATION_HPP_INCLUDED__
#define __GLOBAL_CONFIGURATION_HPP_INCLUDED__

#define __SELF_TEST__
#undef __SELF_TEST__

// #define __T14X5POINT5DA_D24__
// #define __S20X6POINT4BMD_DSD__
// #define __A17X9BMD_D24__
// #define __S19X6POINT5BMD_D4M__
// #define __HSS18X5POINT5BMD_D18__
// #define __A10X4POINT8BEMD_G28__
#define __HA18x7POINT6BEMD_M1B__

#define __PAN_AND_TILT_SUPPORT__ (1)

// #define __HARDWARE_REV_1_0_SUPPORT__
#ifdef __HARDWARE_REV_1_0_SUPPORT__
  #undef __PAN_AND_TILT_SUPPORT__
  #warning "__PAN_AND_TILT_SUPPORT__ disabled"
#endif

#endif // __GLOBAL_CONFIGURATION_HPP_INCLUDED__
