#ifndef __Principal_H
#define __Principal_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "com.h"
#include "lcd.h"
#include "thClk.h"
#include "thJoystick.h"
#include "Mpu6050.h"
#include "leds_N.h"

#define Led1_ON  0x001
#define Led1_OFF 0x002

#define Led2_ON  0x004
#define Led2_OFF 0x008

#define Led3_ON  0x010
#define Led3_OFF 0x020

#define BC_SIZE 10

typedef enum{
	Reposo,
	Activo,
	Depuracion
	
}Estado_LCD;

typedef enum{
	hora,
	minutos,
	segundos,
	aceleracion_x,
	aceleracion_y,
	aceleracion_z
	
}Estado_depuracion;

typedef struct    
	{
	uint8_t horas_bc;
	uint8_t minutos_bc;
	uint8_t segundos_bc;
	float temperatura_bc;
	float ax_bc;
	float ay_bc;
	float az_bc;

	} buffer_circular_t;



int Init_ThPrincipal (void);      // Funcion de creacion e inicializacion del thread asociado al Principal

#endif
