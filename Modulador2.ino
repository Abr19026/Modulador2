/*
Modulador de frecuencia y volumen para MegaAVR 328 con reloj de 16Mhz
*/

#include <Arduino.h>
#include <modulador.h>

const int pin_modula_frec = A0;

// inicializa modulador
modulador_onda onda_seno;

void setup() {
	Serial.begin(9600);
	onda_seno.inicializar_modulacion();
	onda_seno.volumen_obj = VOLUMEN_MAX/2;
}

void loop() {
	unsigned int medicion = 0;
	char cantidad_medidas = 10;
	while(cantidad_medidas != 0) {
		medicion += analogRead(pin_modula_frec);
		cantidad_medidas--;
		delay(5);
	}
	medicion /= 10;
	medicion = map(medicion, 0, 1023, FREC_MIN_ONDA, FREC_MAX_ONDA);
	onda_seno.cambiar_frecuencia(medicion);
	Serial.println(onda_seno.get_frecuencia());
	Serial.println(onda_seno.volumen_obj);
}


ISR(TIMER1_COMPA_vect) {
	onda_seno.cambio_onda();
}