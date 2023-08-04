/*
Modulador de frecuencia y volumen para MegaAVR 328 con reloj de 16Mhz
*/

#include "modulador.h"
#include <stdint.h>
#include <timers328.h>
#include <TablaSeno.h>

#define PRESCALER_1 1       // Prescaler a usar en timer1
#define PRESCALER_2 1       // Prescaler a usar en timer2
#define CICLOS_MUESTRA 255	// Cantidad de ciclos que dura cada muestra de la onda

#define CONTROL_VOLUMEN	// Permite modificar volumen por software

/*Configuración*/
const int pin_onda = PIN_EMITE_ONDA;//[[--NO-- MODIFICAR]]Pin 3 (OC2B)

// La frecuencia es linealmente proporcional al cambio de fase por muestra
const unsigned int cambio_fase_por_hz = clock_frec / (CICLOS_MUESTRA * (uint32_t)TAMANO_TABLA);

// La fase se escala por este valor para no perder tanta precisión
#define ESCALA_FASE 16
// Está escalado por el factor ESCALA_FASE para tener más precisión en los calculos
volatile uint16_t fase_act = 0;
uint16_t frecuencia = 440;
uint16_t cambio_fase = frecuencia*ESCALA_FASE/cambio_fase_por_hz;

const uint16_t frec_max = min(2048, 0xFFFF/ESCALA_FASE);
const uint16_t frec_min = max(64, cambio_fase_por_hz);

// tiene que estar entre frec_max y frec_min
void cambiar_frecuencia(uint16_t nueva_frecuencia) {
	frecuencia = nueva_frecuencia;
	cambio_fase = frecuencia * ESCALA_FASE / cambio_fase_por_hz;
}

uint16_t get_frecuencia() {
	return frecuencia;
}

void inicializar_modulacion() {
	//Configuro pin 3 (OC2B) como output
  	uint8_t bitSalida = digitalPinToBitMask(pin_onda);
  	DDRD |= bitSalida;

  	//Configuro timer 2 como PWM
  	SET_TIMER2_SCALER(PRESCALER_2);  //Configuro Prescaler
  	SET_WGM_TIMER2(T02_FPWM);     //Pongo timer en modo FastPWM
  	SET_COMB_T2(FPWM_NO_INVERT);  //Pongo pin 3 en FastPWM no invertido

  	//Configuro timer 1 para llamar la rutina de interrupción cada CICLOS_MUESTRA ciclos

  	SET_TIMER1_SCALER(PRESCALER_1);  //Configuro Prescaler
  	SET_WGM_TIMER1(T1_CTC_OC);    //Pongo timer 1 en modo CTC con top en OCR1A
  	TIMSK1 |= _BV(OCIE1A);        //Llamo interrupt TIMER1_COMPA_vect en Compare-Match con OCR1A

  	OCR1A = CICLOS_MUESTRA;

  	//Reinicio Timers
  	TCNT1 = 0;
  	TCNT2 = 0;

  	//Activo interrupts globales
  	sei();
}

/* Prueba control volumen*/
#ifdef CONTROL_VOLUMEN
#define CAMBIO_VOLUMEN 1
unsigned int volumen_act = VOLUMEN_MAX / 2; // No modificar directamente
unsigned int volumen_obj = volumen_act;
volatile unsigned int cuenta = 0;	// Para medir cada cuantos ciclos actualizar el volumen
#endif

// cada que se llama se cambia la muestra de la onda (fase) emitida
// por la señal PWN en el pin_onda
void cambio_onda() {
	#ifdef CONTROL_VOLUMEN
	cuenta++;
  	if(cuenta % 64 == 0) {
  		uint16_t diferencia = volumen_act - volumen_obj;
  		if(volumen_act > volumen_obj) {
  			volumen_act-= CAMBIO_VOLUMEN;
  		} else if (volumen_act < volumen_obj) {
  			volumen_act+= CAMBIO_VOLUMEN;
  		}
  	}
  	#endif
  	//actualizo fase de la muestra
  	fase_act = (fase_act + cambio_fase) % ((uint16_t)TAMANO_TABLA * ESCALA_FASE); 
  	
  	//Nuevo duty cycle deL PWM es valoresSeno[fase_act]
  	uint8_t nuevo_valor = pgm_read_word_near(valores_tabla + fase_act / ESCALA_FASE * sizeof(valores_tabla[0]));
  	#ifdef CONTROL_VOLUMEN
  	OCR2B = nuevo_valor * volumen_act / VOLUMEN_MAX;
  	#elif
  	OCR2B = nuevo_valor;
  	#endif
}