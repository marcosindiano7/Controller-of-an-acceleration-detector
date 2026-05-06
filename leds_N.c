#include "leds_N.h"
 
/*----------------------------------------------------------------------------
 
Thread 1 'LEDS': Sample thread
---------------------------------------------------------------------------*/

osThreadId_t tid_Control_Led;        // Thread Leds
void ThControlLed (void* argument);                  // Thread function led 1


const osThreadAttr_t thread1_attr_leds = {
  .stack_size = 128                            // Create the thread stack with a size of 512 bytes
};


int Init_ThLEDS (void)
{
	
	tid_Control_Led = osThreadNew(ThControlLed, NULL, &thread1_attr_leds);
	
  if (tid_Control_Led == NULL)
  {
    return(-1);
  }
	
  return(0);
}


void ThControlLed(void* args){

	uint32_t status;
	
  while(1){
		status = osThreadFlagsWait(Led1_ON | Led1_OFF | Led2_ON | Led2_OFF | Led3_ON | Led3_OFF , osFlagsWaitAny, osWaitForever);
		
		if (status & Led1_ON) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }
    if (status & Led1_OFF) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    }
    if (status & Led2_ON) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    }
    if (status & Led2_OFF) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    }
    if (status & Led3_ON) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    }
    if (status & Led3_OFF) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }
		
		
  }
}
