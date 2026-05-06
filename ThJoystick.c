#include "ThJoystick.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'ThJoystick': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThJoystick;        // Thread Joystickid
osMessageQueueId_t mid_MsgQueue;    // message queue id
osTimerId_t tid_pulsacionLarga;     //Timout para saber pulsaciones largas
osEventFlagsId_t eventoPulsacion;   //Flag de pulsacion

MSGQUEUE_OBJ_t msg_env;             // mensaje a enviar

static uint8_t pulsacion_larga=0;


static void one_shot_callback(uint32_t eventFlags);      // Funcion callback asociada al timer que gestiona los rebotes
void ThJoystick (void *argument);                   // Thread function
static void callback_pulsL(void *args);
static void TipoDePulsacion(void *args);
static void pulsacion_larga_callback(void *args);

const osThreadAttr_t thread1_attr_joy = {
  .stack_size = 256                            // Create the thread stack with a size of 512 bytes
};



int Init_ThJoystick (void)
{
  tid_ThJoystick = osThreadNew(ThJoystick, NULL, &thread1_attr_joy);
	
  if (tid_ThJoystick == NULL)
  {
    return(-1);
  }
  return(0);
}

static void one_shot_callback(uint32_t eventFlags)
{
	
	if(eventFlags & PULSACION){   //DURANTE LA PULSACION
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET) // UP
			{
			msg_env.gesto = 0x01;     // GESTO UP
			msg_env.duracion = 0x001;
			}
  
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_SET) // RIGHT
		{
			msg_env.gesto = 0x02;     // GESTO RIGHT
			msg_env.duracion = 0x001;
		}
  
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_SET) // DOWN
		{
			msg_env.gesto = 0x03;     // GESTO DOWN
			msg_env.duracion = 0x001;
		}
  
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_SET) // LEFT
		{
			msg_env.gesto = 0x04;     // GESTO LEFT
			msg_env.duracion = 0x001;
		}
  
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == GPIO_PIN_SET) // CENTER
		{    
			msg_env.gesto = 0x05;     // GESTO CENTER
		}
		
	}
	
	if(eventFlags & SOLTAR){     //SOLTAR
	
		if(pulsacion_larga == 1){ 	//si la pulsacion es larga
			msg_env.duracion= 0x002; //pulsacion de duracion larga 
			pulsacion_larga=0;
			
		}else{                       //si la pulsacion es corta
			msg_env.duracion = 0x001;
		} 
		
	}
 
	osMessageQueuePut(mid_MsgQueue, &msg_env, 0U, 0U);
	msg_env.gesto = 0;
	msg_env.duracion = 1;
	osMessageQueuePut(mid_MsgQueue, &msg_env, 0U, 0U);
	
}

int Init_Timer_Rebotes (void)
{
  static uint32_t exec = 1U;
  
  tid_pulsacionLarga = osTimerNew(pulsacion_larga_callback, osTimerOnce, &exec, NULL);
	
  return(0);
}


void ThJoystick (void *argument)
{

	uint8_t estadoActual;
	uint8_t estadoPasado=0;
  mid_MsgQueue = osMessageQueueNew(2, sizeof(MSGQUEUE_OBJ_t), NULL);     // Se crea e inicia la cola de mensajes
  Init_Timer_Rebotes();                                                                 // Se crea e inicializa el timer One-Shot de 50 ms para gestionar los rebotes
  eventoPulsacion = osEventFlagsNew(NULL);
	
  while(1)
  {
    osThreadFlagsWait(PULSACION, osFlagsWaitAll, osWaitForever);      // Se espera al flag "PULSACION" para volver a lanzar el timer de 50 ms
		osThreadFlagsClear(PULSACION);
		osDelay(50U);
		estadoActual=HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)|HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)|HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)|HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)|HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
		if(estadoActual == 1){
			pulsacion_larga = 0;
			osTimerStart(tid_pulsacionLarga, 1000U);
			osEventFlagsSet(eventoPulsacion, PULSACION);
		}else{
			osTimerStop(tid_pulsacionLarga);
			osEventFlagsSet(eventoPulsacion, SOLTAR);
		}
		
		one_shot_callback(osEventFlagsGet(eventoPulsacion));
		osEventFlagsClear(eventoPulsacion, osEventFlagsGet(eventoPulsacion));
		
		osDelay(10);
	}
}



static void pulsacion_larga_callback(void *args){

	pulsacion_larga=1;
	
}
