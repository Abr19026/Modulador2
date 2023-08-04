#ifndef _modulador_h_
#define _modulador_h_
#include <Arduino.h>
#include <stdint.h>

#define VOLUMEN_MAX 256
#define PIN_EMITE_ONDA 3 // No modificar, debe ser pin 3 (OC2B)

/* Modula una onda seno en el pin 3*/
extern const uint16_t frec_max;
extern const uint16_t frec_min;
extern unsigned int volumen_obj;
void cambiar_frecuencia(uint16_t nueva_frecuencia);
void inicializar_modulacion(); // Modifica timer 1 y 2
void cambio_onda(); // Llamar dentro de ISR(TIMER1_COMPA_vect)
uint16_t get_frecuencia();
#endif