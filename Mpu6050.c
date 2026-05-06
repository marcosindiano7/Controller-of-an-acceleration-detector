#include "Mpu6050.h"
 
/*----------------------------------------------------------------------------
 
Thread 1 'Acelerometro': Sample thread
---------------------------------------------------------------------------*/

osThreadId_t tid_Control_Acelerometro;        // Thread Joystickid
osMessageQueueId_t colaMpu;
void ThControlAcelerometro (void* argument);

extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;
void I2C1_Init(void);
void MPU6050_Init(void);
void I2C_Callback(uint32_t event);
void MPU6050_ReadRegisters(uint8_t reg, uint8_t *buffer, uint8_t length);
osEventFlagsId_t busyEventFlag_mpu;

const osThreadAttr_t thread1_attr_mpu = {
  .stack_size = 256                            // Create the thread stack with a size of 512 bytes
};


int Init_ThAcelerometro (void)
{
	
  tid_Control_Acelerometro = osThreadNew(ThControlAcelerometro, NULL, &thread1_attr_mpu);
	colaMpu = osMessageQueueNew(4, sizeof(DatosSensor_t), NULL);
		
  if (tid_Control_Acelerometro == NULL)
  {
    return(-1);
  }
  return(0);
}

void ThControlAcelerometro(void *argument) {
    uint8_t rawData[6];
	
		busyEventFlag_mpu = osEventFlagsNew(NULL);
		I2C1_Init();
		DatosSensor_t datos_env;
    MPU6050_Init(); // Inicializar el sensor
		
		int16_t ac_x;
		int16_t ac_y;
		int16_t ac_z;
		int16_t temp;
	
	
    while (1) {
        // Leer registros de aceleración
        MPU6050_ReadRegisters(MPU6050_ACCEL_XOUT_H, rawData, 6);
        ac_x = (int16_t)((rawData[0] << 8) | rawData[1]);
        ac_y = (int16_t)((rawData[2] << 8) | rawData[3]);
        ac_z = (int16_t)((rawData[4] << 8) | rawData[5]);
					
				datos_env.a_x= ac_x / 16384.0f;
				datos_env.a_y= ac_y / 16384.0f;
				datos_env.a_z= ac_z / 16384.0f;
        
			// Leer registro de temperatura
        MPU6050_ReadRegisters(MPU6050_TEMP_OUT_H, rawData, 2);
        temp = (int16_t)(((rawData[0] << 8) | rawData[1]));
				datos_env.temperatura = temp /340.0 + 36.53;
			
        // Enviar datos a la cola
        osMessageQueuePut(colaMpu, &datos_env, 0, osWaitForever);

        osDelay(1000); // Esperar 1 segundo
    }
}


void I2C1_Init(void) {
    I2Cdrv->Initialize(I2C_Callback);                        // Inicializar el driver I2C
    I2Cdrv->PowerControl(ARM_POWER_FULL);                    // Encender el periférico I2C
    I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
		I2Cdrv->Control(ARM_I2C_BUS_CLEAR, 0);
}

void I2C_Callback(uint32_t event) {
    // Manejo de eventos del driver
	uint32_t mask;
	mask = ARM_I2C_EVENT_TRANSFER_DONE;
	if (event & ARM_I2C_EVENT_BUS_ERROR) {
        osEventFlagsSet(busyEventFlag_mpu, 0x02); // Setea el flag de error
    }	
	if (event & mask) {
        osEventFlagsSet(busyEventFlag_mpu, 0x01); // Establece el flag cuando se completa
  }
	
}

// Inicialización del MPU6050
void MPU6050_Init(void) {
    uint8_t data[2];
    data[0] = MPU6050_PWR_MGMT_1; // Registro para configuración de energía
    data[1] = 0x00;              // Salir del modo de reposo
    I2Cdrv->MasterTransmit(MPU6050_I2C_ADDRESS, data, 2, false); // Transmitir datos al MPU6050
    osEventFlagsWait(busyEventFlag_mpu, 0x01 | 0x02, osFlagsWaitAny, osWaitForever);
	
		data[0] = 0x1C; // Registro para configuración de energía
    data[1] = 0x00;              // Salir del modo de reposo
    I2Cdrv->MasterTransmit(MPU6050_I2C_ADDRESS, data, 2, false); // Transmitir datos al MPU6050
		osEventFlagsWait(busyEventFlag_mpu, 0x01 | 0x02, osFlagsWaitAny, osWaitForever);
	
		data[0] = 0x19; // Registro para configuración de energía
    data[1] = 0x07;              // Salir del modo de reposo
    I2Cdrv->MasterTransmit(MPU6050_I2C_ADDRESS, data, 2, false); // Transmitir datos al MPU6050
		osEventFlagsWait(busyEventFlag_mpu, 0x01 | 0x02, osFlagsWaitAny, osWaitForever);
	
		data[0] = 0x6C; // Registro para configuración de energía
    data[1] = 0x07;              // Salir del modo de reposo
    I2Cdrv->MasterTransmit(MPU6050_I2C_ADDRESS, data, 2, false); // Transmitir datos al MPU6050
		osEventFlagsWait(busyEventFlag_mpu, 0x01 | 0x02, osFlagsWaitAny, osWaitForever);
	
}


// Leer registros del MPU6050
void MPU6050_ReadRegisters(uint8_t reg, uint8_t *buffer, uint8_t length) {
    I2Cdrv->MasterTransmit(MPU6050_I2C_ADDRESS, &reg, 1, true); // Enviar registro que queremos leer
    osEventFlagsWait(busyEventFlag_mpu, 0x01, osFlagsWaitAny, osWaitForever);
    
		I2Cdrv->MasterReceive(MPU6050_I2C_ADDRESS, buffer, length, false); // Leer datos del sensor
    osEventFlagsWait(busyEventFlag_mpu, 0x01, osFlagsWaitAny, osWaitForever);
		
}
