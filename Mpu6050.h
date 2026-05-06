#ifndef __Mpu6050_H
#define __Mpu6050_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "Driver_I2C.h"

int Init_ThAcelerometro (void);                                                  // Funcion de creacion e inicializacion del thread asociado al CLK

typedef struct{
	float temperatura;
	float a_x;
	float a_y;
	float a_z;	
}DatosSensor_t;


#define MPU6050_I2C_ADDRESS 0x68  // Dirección I2C del MPU6050
#define MPU6050_ACCEL_XOUT_H 0x3B      // Registro inicial de datos de aceleración
#define MPU6050_TEMP_OUT_H 0x41        // Registro inicial de datos de temperatura
#define MPU6050_PWR_MGMT_1 0x6B        // Registro de gestión de energía


extern I2C_HandleTypeDef hi2c1;

#endif
