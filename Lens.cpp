#include "Lens.hpp"

// #define __T14X5POINT5DA_D24__
// #define __S20X6POINT4BMD_DSD__
#define __A17X9BMD_D24__

#if defined(__S20X6POINT4BMD_DSD__)

const char *szLensName = "FUJINON S20x6.4BMD-DSD";

const ServoSettings zoomSettings = {
  .setPoints = {
    { .setting =   64, .adcValue = 3030 },
    { .setting =  150, .adcValue = 2560 },
    { .setting =  300, .adcValue = 2280 },
    { .setting =  600, .adcValue = 2010 },
    { .setting = 1280, .adcValue = 1725 },
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 4,
    .timeoutScale = 100,
    .minSpeed = 1,
    .rfu = 0
  }
};


const ServoSettings irisSettings = {
  .setPoints = {
    { .setting =  14, .adcValue = 2985},
    { .setting =  20, .adcValue = 2750},
    { .setting =  28, .adcValue = 2490},
    { .setting =  40, .adcValue = 2265},
    { .setting =  56, .adcValue = 2070},
    { .setting =  80, .adcValue = 1872},
    { .setting = 110, .adcValue = 1665},
    { .setting = 160, .adcValue = 1438},
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 10,
    .timeoutScale = 32,
    .minSpeed = 4,
    .rfu = 0
  }
};

const ServoSettings focusSettings = {
  .setPoints = {
    { .setting =    9, .adcValue = 1293},
    { .setting =   12, .adcValue = 1666},
    { .setting =   15, .adcValue = 1905},
    { .setting =   20, .adcValue = 2140},
    { .setting =   30, .adcValue = 2375},
    { .setting =   50, .adcValue = 2566},
    { .setting =  100, .adcValue = 2708},
    { .setting = 9990, .adcValue = 2852}, // infinity
    { .setting = 9999, .adcValue = 2880}, // and beyond
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 6,
    .timeoutScale = 64,
    .minSpeed = 4,
    .rfu = 0
  }
};

#elif defined(__T14X5POINT5DA_D24__)

// Fujinon AW-LZ14MD55 (T14x5.5DA-D24, 1/3" C-Mount)
// Beware: the test device had been fully disassembled, so some ranges might be shifted
const char *szLensName = "FUJINON AW-LZ14MD55 / T14x5.5DA-D24";

const ServoSettings zoomSettings = {
  .setPoints = {
    { .setting =   55, .adcValue = 3029 },
    { .setting =  100, .adcValue = 2498 },
    { .setting =  200, .adcValue = 1989 },
    { .setting =  400, .adcValue = 1533 },
    { .setting =  770, .adcValue = 1152 },
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 4,
    .timeoutScale = 100,
    .minSpeed = 1,
    .rfu = 0
  }
};

const ServoSettings irisSettings = {
  .setPoints = {
    { .setting =  14, .adcValue = 3010},
    { .setting =  20, .adcValue = 2730},
    { .setting =  28, .adcValue = 2497},
    { .setting =  40, .adcValue = 2275},
    { .setting =  56, .adcValue = 2069},
    { .setting =  80, .adcValue = 1843},
    { .setting = 110, .adcValue = 1633},
    { .setting = 160, .adcValue = 1369},
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 4,
    .timeoutScale = 32,
    .minSpeed = 2,
    .rfu = 0
  }
};

const ServoSettings focusSettings = {
  .setPoints = {
    { .setting =   10, .adcValue = 1296}, // 1.0m
    { .setting =   12, .adcValue = 1600}, // 1.2m
    { .setting =   15, .adcValue = 1913}, // 1.5m
    { .setting =   20, .adcValue = 2235}, // 2.0
    { .setting =   30, .adcValue = 2548}, // 3.0m
    { .setting =   50, .adcValue = 2814}, // 5.0m
    { .setting =  100, .adcValue = 2980}, // 10.0m
    { .setting = 9990, .adcValue = 3118}, // infinity
    { .setting = 9999, .adcValue = 3127}, // and beyond
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 4,
    .timeoutScale = 64,
    .minSpeed = 3,
    .rfu = 0
  }
};

#elif defined(__A17X9BMD_D24__)
const char *szLensName = "FUJINON A17x9BMD-D24";

const ServoSettings zoomSettings = {
  .setPoints = {
    { .setting =   90, .adcValue = 2640 },
    { .setting =  200, .adcValue = 2146 },
    { .setting =  400, .adcValue = 1853 },
    { .setting =  800, .adcValue = 1584 },
    { .setting = 1530, .adcValue = 1320 },
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 4,
    .timeoutScale = 100,
    .minSpeed = 1,
    .rfu = 0
  }
};

const ServoSettings irisSettings = {
  .setPoints = {
    { .setting =  19, .adcValue = 3040},
    { .setting =  28, .adcValue = 2767},
    { .setting =  40, .adcValue = 2516},
    { .setting =  56, .adcValue = 2225},
    { .setting =  80, .adcValue = 1993},
    { .setting = 110, .adcValue = 1786},
    { .setting = 160, .adcValue = 1571},
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 4,
    .timeoutScale = 32,
    .minSpeed = 2,
    .rfu = 0
  }
};

const ServoSettings focusSettings = {
  .setPoints = {
    { .setting =    9, .adcValue = 1323},
    { .setting =   12, .adcValue = 1721},
    { .setting =   15, .adcValue = 1961},
    { .setting =   20, .adcValue = 2190},
    { .setting =   30, .adcValue = 2414},
    { .setting =   50, .adcValue = 2614},
    { .setting =  100, .adcValue = 2770},
    { .setting = 9990, .adcValue = 2912}, // infinity
    { .setting = 9999, .adcValue = 2963}, // and beyond
    { 0, 0}
  },
  .parameters = {
    .pwmScale = 6,
    .timeoutScale = 64,
    .minSpeed = 4,
    .rfu = 0
  }
};

#else
#error "No Lens Selected !"
#endif
