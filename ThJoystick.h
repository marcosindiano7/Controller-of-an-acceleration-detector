#ifndef __THJOYSTICK_H
#define __THJOYSTICK_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define PULSACION     0x001
#define SOLTAR        0x002
#define MSGQUEUE_OBJECTS      1     // number of Message Queue Objects

#define P_CORTA 0x001
#define P_LARGA 0x002

typedef enum{
    Init,
    Default,
    mandar
}Estado_j;

typedef struct                      // object data type
{
  uint8_t gesto;    // 0x01 -> UP   ;   0x02 -> RIGHT   ;   0x03 -> DOWN   ;   0x04 -> LEFT   ;   0x05 -> CENTER
	uint8_t duracion;
} MSGQUEUE_OBJ_t;

int Init_ThJoystick (void);      // Creacion e inicializacion del thread asociado al joystick
int Init_Timer_Rebotes(void);    // Creacion e inicializacion del timer de tipo One-Shot para gestionar los rebotes
    
#endif
