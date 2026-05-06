#ifndef __leds_N_H
#define __leds_N_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#define Led1_ON  0x001
#define Led1_OFF 0x002

#define Led2_ON  0x004
#define Led2_OFF 0x008

#define Led3_ON  0x010
#define Led3_OFF 0x020

int Init_ThLEDS (void);                     // Funcion de creacion e inicializacion del thread asociado a los LEDS

#endif
