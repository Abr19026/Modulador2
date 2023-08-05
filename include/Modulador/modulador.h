#ifndef _modulador_h_
#define _modulador_h_
#include <Arduino.h>
#include <stdint.h>
#include <timers328.h>
#include <TablaSeno.h>

#define CONTROL_VOLUMEN	// Permite modificar volumen por software

#ifdef CONTROL_VOLUMEN
	#define VOLUMEN_MAX 256
#endif
#define PIN_EMITE_ONDA 3 // No modificar, debe ser pin 3 (OC2B)
#define FREC_MAX_ONDA min(2048, 0xFFFF/ESCALA_FASE)
#define FREC_MIN_ONDA max(64, CAMBIO_FASE_POR_HZ)

/* NO MODIFICAR LOS SIGUIENTES MACROS */
// La frecuencia es linealmente proporcional al cambio de fase por muestra
#define CAMBIO_FASE_POR_HZ (clock_frec / (CICLOS_MUESTRA * (uint32_t)TAMANO_TABLA))
// La fase se escala por este valor para no perder tanta precisión
#define ESCALA_FASE 16
// Cantidad de ciclos que dura cada muestra de la onda (duración de 1 ciclo PWM)
#define CICLOS_MUESTRA 255

/* Modula una onda seno en el pin 3*/
class modulador_onda {
public:
	#ifdef CONTROL_VOLUMEN
		unsigned int volumen_obj;
	#endif
		
	modulador_onda();
	void cambiar_frecuencia(uint16_t nueva_frecuencia);
	void inicializar_modulacion(); // Modifica timer 1 y 2
	void cambio_onda(); // Llamar dentro de ISR(TIMER1_COMPA_vect)
	uint16_t get_frecuencia();

private:
	uint16_t frecuencia;
	uint16_t cambio_fase;
	volatile uint16_t fase_act;
	#ifdef CONTROL_VOLUMEN
		volatile unsigned int cuenta;
		unsigned int volumen_act;
	#endif	
};


#endif