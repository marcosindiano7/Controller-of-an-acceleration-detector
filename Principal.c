#include "Principal.h"
 
/*----------------------------------------------------------------------------
 
Thread 1 'Principal': Sample thread
---------------------------------------------------------------------------*/

osThreadId_t tid_Control_Principal;        // Thread Principal

//CLK variables

extern uint8_t Segundos;  
extern uint8_t Minutos;
extern uint8_t Horas;

//JOYSTICK variables

extern osMessageQueueId_t mid_MsgQueue; 					//cola para recibir datos
MSGQUEUE_OBJ_t msg_recibir_joystick;    					//estructura donde se reciben datos

//LEDS variables

extern osThreadId_t tid_Control_Led;    					//thread para mandar los flags de Leds on/off

//LCD variables

extern osMessageQueueId_t lcd_Queue;     					//cola para enviar datos al lcd
MSGQUEUE_lcd_t msg_enviar_lcd;           					// estrucutura a enviar al lcd

//Acelerometro

extern osMessageQueueId_t colaMpu;       					//cola para recibir datos del mpu6050
DatosSensor_t datos_recibir_mpu;         					//estructura para recibir datos de la mpu

//COM-PC

extern osMessageQueueId_t cola_entrada;  					//cola para enviar datos al com-pc
extern osMessageQueueId_t cola_salida;   					//cola para recibir datos del com-pc
char buffer_datos_entrada_compc[BUFFER_SIZE];  //buffer que tendrá los datos de entrada del com_pc
char buffer_datos_salida_compc[BUFFER_SIZE];   //buffer que tendrá los datos de salida del com_pc



void ThControlPrincipal (void* argument);         // Thread function

int Init_ThPrincipal (void)
{
	static uint32_t exec;
	
  tid_Control_Principal = osThreadNew(ThControlPrincipal, NULL, NULL);
	
  if (tid_Control_Principal == NULL)
  {
    return(-1);
  }
  return(0);
}

void ThControlPrincipal(void* args){

	Estado_LCD modo_lcd = Reposo;
	static uint8_t hora_aux = 0;
	static uint8_t minutos_aux = 0;
	static uint8_t segundos_aux = 0;
	static float ax_aux=0;
	static float ay_aux=0;
	static float az_aux=0;
	static char parametro_cambiar = 'H';
	float ax=1;
	float ay=1;
	float az=1;
	static int i=0;
	static int a=0;
	buffer_circular_t buffer_circular[BC_SIZE];
	static uint8_t parte_entera;
	static uint8_t parte_decimal;
	static uint8_t status;
	static uint8_t larga= 0;
	
	Estado_depuracion estado_depuracion;
	
  while(1){
		
		status = osMessageQueueGet(mid_MsgQueue, &msg_recibir_joystick, NULL, 0U);
		if(status == osOK){
			
			if(msg_recibir_joystick.duracion == P_LARGA){
				larga = 1;
			}else{
				larga=0;
			}
		}
		
		
		if(osMessageQueueGetSpace(colaMpu) != 4){
			osMessageQueueGet(colaMpu, &datos_recibir_mpu, NULL, osWaitForever);
		}
		
		
		switch(modo_lcd){
			default:
				
			//---MODO REPOSO
			
			case Reposo:
				
				
				if(larga == 1){
					larga=0;
					modo_lcd = Activo;
					
				} 
				sprintf(msg_enviar_lcd.mensaje, "      SBM 2024");
				msg_enviar_lcd.linea = 1;
				osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				
				sprintf(msg_enviar_lcd.mensaje, "       %02d:%02d:%02d", Horas, Minutos, Segundos);
				msg_enviar_lcd.linea = 2;
				osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				
				
				
			break;
				
				
				
				//---MODO ACTIVO---
			
			case Activo:
				
				if(larga==1){
					larga=0;
					modo_lcd = Depuracion;
					hora_aux = Horas;
					minutos_aux = Minutos;
					segundos_aux = Segundos;
					ax_aux=0;
					ay_aux=0;
					az_aux=0;
					parametro_cambiar = 'H';
				}
				
				if(datos_recibir_mpu.a_x > ax){
					osThreadFlagsSet(tid_Control_Led, Led1_ON);
				}else{
					osThreadFlagsSet(tid_Control_Led, Led1_OFF);
				}
				
				
				if(datos_recibir_mpu.a_y > ay){
					osThreadFlagsSet(tid_Control_Led, Led2_ON);
				}else{
					osThreadFlagsSet(tid_Control_Led, Led2_OFF);
				}
				
				if(datos_recibir_mpu.a_z > az){
					osThreadFlagsSet(tid_Control_Led, Led3_ON);
				}else{
					osThreadFlagsSet(tid_Control_Led, Led3_OFF);
				}
				
				
				sprintf(msg_enviar_lcd.mensaje, " ACTIVO-- T:%.1f ", datos_recibir_mpu.temperatura);
				msg_enviar_lcd.linea = 1;
				osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				
				sprintf(msg_enviar_lcd.mensaje, "X:%.1f Y:%.1f Z:%.1f ",datos_recibir_mpu.a_x, datos_recibir_mpu.a_y, datos_recibir_mpu.a_z);
				msg_enviar_lcd.linea = 2;
				osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				
				
				buffer_circular[a].horas_bc = Horas;
				buffer_circular[a].minutos_bc = Minutos;
				buffer_circular[a].segundos_bc = Segundos;
				buffer_circular[a].temperatura_bc = datos_recibir_mpu.temperatura;
				buffer_circular[a].ax_bc = datos_recibir_mpu.a_x;
				buffer_circular[a].ay_bc = datos_recibir_mpu.a_y;
				buffer_circular[a].az_bc = datos_recibir_mpu.a_z;
				
				if (a == 9){
					a = 0;

				}else {
					a++;
				}
			
			break;
			
			
				
				//---MODO DEPURACION---
				
				
			case Depuracion:
				status = osMessageQueueGet(cola_salida, &buffer_datos_entrada_compc, NULL, 0U);
				
				if(status == osOK){
					
					
					sprintf(msg_enviar_lcd.mensaje, "     ---P&D---");
					msg_enviar_lcd.linea = 1;
					osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
					
					
					//CAMBIO DE HORA
					if(buffer_datos_entrada_compc[1] == 0x20){
						hora_aux = (buffer_datos_entrada_compc[3] & 0x0F)*10 + (buffer_datos_entrada_compc[4] & 0x0F);
						minutos_aux = (buffer_datos_entrada_compc[6] & 0x0F)*10 + (buffer_datos_entrada_compc[7] & 0x0F);
						segundos_aux = (buffer_datos_entrada_compc[9] & 0x0F)*10 + (buffer_datos_entrada_compc[10] & 0x0F);
						
						for(i = 0; i<sizeof(buffer_datos_entrada_compc); i++){
							buffer_datos_salida_compc[i] = buffer_datos_entrada_compc[i];
						}
						buffer_datos_salida_compc[1] = 0xDF;
						osMessageQueuePut(cola_entrada, &buffer_datos_salida_compc, NULL, 0U);
						estado_depuracion = hora;
					}
					
					//ESTABLECER DATOS Ax
					if(buffer_datos_entrada_compc[1] == 0x25){
						
						ax_aux = buffer_datos_entrada_compc[3] & 0x0F + (buffer_datos_entrada_compc[5] & 0x0F)/10 + (buffer_datos_entrada_compc[6] & 0x0F)/100 ;
						for(i = 0; i<sizeof(buffer_datos_entrada_compc); i++){
							buffer_datos_salida_compc[i] = buffer_datos_entrada_compc[i];
						}
						buffer_datos_salida_compc[1] = 0xDA;
						osMessageQueuePut(cola_entrada, &buffer_datos_salida_compc, NULL, 0U);
						estado_depuracion = aceleracion_x;
					}
					
					
					//ESTABLECER DATOS Ay
					if(buffer_datos_entrada_compc[1] == 0x26){
						
						ay_aux = buffer_datos_entrada_compc[3] & 0x0F + (buffer_datos_entrada_compc[5] & 0x0F)/10 + (buffer_datos_entrada_compc[6] & 0x0F)/100 ;
						for(i = 0; i<sizeof(buffer_datos_entrada_compc); i++){
							buffer_datos_salida_compc[i] = buffer_datos_entrada_compc[i];
						}
						buffer_datos_salida_compc[1] = 0xD9;
						osMessageQueuePut(cola_entrada, &buffer_datos_salida_compc, NULL, 0U);
						estado_depuracion = aceleracion_y;
					}
					
					//ESTABLECER DATOS Az
					if(buffer_datos_entrada_compc[1] == 0x27){
						
						az_aux = buffer_datos_entrada_compc[3] & 0x0F + (buffer_datos_entrada_compc[5] & 0x0F)/10 + (buffer_datos_entrada_compc[6] & 0x0F)/100 ;
						for(i = 0; i<sizeof(buffer_datos_entrada_compc); i++){
							buffer_datos_salida_compc[i] = buffer_datos_entrada_compc[i];
						}
						buffer_datos_salida_compc[1] = 0xD8;
						osMessageQueuePut(cola_entrada, &buffer_datos_salida_compc, NULL, 0U);
						estado_depuracion = aceleracion_z;
					}
					
					//Enviar todas las medidas
					if(buffer_datos_entrada_compc[1] == 0x55){
						for(i = 0; i<sizeof(buffer_circular); i++){
							sprintf(buffer_datos_salida_compc, "%02d:%02d:%02d--Tm:%.1f-Ax:%.1f-Ay:%.1f,-Az:%.1f", buffer_circular[i].horas_bc, buffer_circular[i].minutos_bc, buffer_circular[i].segundos_bc, buffer_circular[i].temperatura_bc, buffer_circular[i].ax_bc, buffer_circular[i].ay_bc, buffer_circular[i].az_bc);
							osMessageQueuePut(cola_entrada, &buffer_datos_salida_compc, NULL, 0U);
						}
					}
					
					
					//BORRAR DATOS BUFFER CIRCULAR
					if(buffer_datos_entrada_compc[1] == 0x60){
						
						for(i=0;i<sizeof(buffer_circular); i++){
							
							buffer_circular[i].horas_bc = 0;
							buffer_circular[i].minutos_bc = 0;
							buffer_circular[i].segundos_bc = 0;
							buffer_circular[i].temperatura_bc = 0;
							buffer_circular[i].ax_bc = 0;
							buffer_circular[i].ay_bc = 0;
							buffer_circular[i].az_bc = 0;							
						}
						
						for(i = 0; i<sizeof(buffer_datos_entrada_compc); i++){
							buffer_datos_salida_compc[i] = buffer_datos_entrada_compc[i];
						}
						buffer_datos_salida_compc[1] = 0x9F;
						osMessageQueuePut(cola_entrada, &buffer_datos_salida_compc, NULL, 0U);
					}
					
					
					
					
									
				}else{
			
				sprintf(msg_enviar_lcd.mensaje, "     ---P&D---");
				msg_enviar_lcd.linea = 1;
				osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				
				if(parametro_cambiar == 'H'){
					sprintf(msg_enviar_lcd.mensaje, "     %s:%02d:%02d:%02d", &parametro_cambiar, hora_aux, minutos_aux, segundos_aux);
					msg_enviar_lcd.linea = 2;
					osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				}else{
					sprintf(msg_enviar_lcd.mensaje, "   A_%s:%.1f:%.1f:%.1f", &parametro_cambiar, ax_aux, ay_aux, az_aux);
					msg_enviar_lcd.linea = 2;
					osMessageQueuePut(lcd_Queue, &msg_enviar_lcd, 0U, 0U);
				
				}
				
				
				
				
				
				if(larga==1){
					larga=0;
					modo_lcd = Reposo;
					Horas = hora_aux;
					Minutos = minutos_aux;
					Segundos = segundos_aux;
					ax = ax_aux;
					ay = ay_aux;
					az = az_aux;
					
				}else{
					switch(estado_depuracion){
						default:
						case hora:
							parametro_cambiar= 'H';
							if(msg_recibir_joystick.gesto == 0x01){  //arriba
								if(hora_aux == 23){
									hora_aux =0;
								}else{
									hora_aux++;
								}
							}
							if(msg_recibir_joystick.gesto == 0x03){ //abajo
								if(hora_aux == 0){
									hora_aux=23;
								}else{
								hora_aux--;
								}	
							}
							if(msg_recibir_joystick.gesto == 0x02){ //derecha
								estado_depuracion = minutos;
							}
							
							if(msg_recibir_joystick.gesto == 0x05){ //centro
								estado_depuracion = aceleracion_x;
							}
							
							
						break;
						
						case minutos:
						
							if(msg_recibir_joystick.gesto == 0x01){  //arriba
								if(minutos_aux == 59){
									minutos_aux =0;
								}else{
									minutos_aux++;
								}
							}
							if(msg_recibir_joystick.gesto == 0x03){ //abajo
								if(minutos_aux == 0){
									minutos_aux=59;
								}else{
								minutos_aux--;
								}	
							}
							if(msg_recibir_joystick.gesto == 0x02){ //derecha
								estado_depuracion = segundos;
							}
							if(msg_recibir_joystick.gesto == 0x04){ //izquierda
								estado_depuracion = hora;
							}
							if(msg_recibir_joystick.gesto == 0x05){ //centro
								estado_depuracion = aceleracion_x;
							}
						
						break;
						
						case segundos:
							
							if(msg_recibir_joystick.gesto == 0x01){  //arriba
								if(segundos_aux == 59){
									segundos_aux =0;
								}else{
									segundos_aux++;
								}
							}
							if(msg_recibir_joystick.gesto == 0x03){ //abajo
								if(segundos_aux == 0){
									segundos_aux=59;
								}else{
								segundos_aux--;
								}	
							}
							if(msg_recibir_joystick.gesto == 0x04){ //izquierda
								estado_depuracion = minutos;
							}
							if(msg_recibir_joystick.gesto == 0x05){ //centro
								estado_depuracion = aceleracion_x;
							}
						
						break;
						
						case aceleracion_x:
							parametro_cambiar= 'x';
							if(msg_recibir_joystick.gesto == 0x01){  //arriba
								if(ax_aux > 2){
									ax_aux =0;
								}else{
									ax_aux= ax_aux + 0.1;
								}
							}
							if(msg_recibir_joystick.gesto == 0x03){ //abajo
								if(ax_aux == 0){
									ax_aux=2;
								}else{
								ax_aux= ax_aux - 0.1;
								}	
							}
							
							if(msg_recibir_joystick.gesto == 0x05){ //centro
								estado_depuracion = aceleracion_y;
							}
						
						break;
						
						case aceleracion_y:
							parametro_cambiar= 'y';
						
							if(msg_recibir_joystick.gesto == 0x01){  //arriba
								if(ay_aux > 2){
									ay_aux =0;
								}else{
									ay_aux= ay_aux + 0.1;
								}
							}
							if(msg_recibir_joystick.gesto == 0x03){ //abajo
								if(ay_aux == 0){
									ay_aux=2;
								}else{
								ay_aux= ay_aux - 0.1;
								}	
							}
							
							if(msg_recibir_joystick.gesto == 0x05){ //centro
								estado_depuracion = aceleracion_z;
							}
						break;
						
						case aceleracion_z:
							
							parametro_cambiar= 'z';
						
							if(msg_recibir_joystick.gesto == 0x01){  //arriba
								if(az_aux > 2){
									az_aux =0;
								}else{
									az_aux= az_aux + 0.1;
								}
							}
							if(msg_recibir_joystick.gesto == 0x03){ //abajo
								if(az_aux == 0){
									az_aux=2;
								}else{
								az_aux= az_aux - 0.1;
								}	
							}
							
							if(msg_recibir_joystick.gesto == 0x05){ //centro
								estado_depuracion = hora;
							}
						
						break;  
						
					}
				}
			}				
				
			break;  //fin depuracion
		}
		osDelay(50);
  }


}
