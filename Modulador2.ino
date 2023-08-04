/*
Modulador de frecuencia y volumen para MegaAVR 328 con reloj de 16Mhz
*/

#include <Arduino.h>
#include <modulador.h>

const int pin_modula_frec = A0;

void setup() {
	Serial.begin(9600);
	inicializar_modulacion();
	volumen_obj = VOLUMEN_MAX/2;
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
	medicion = map(medicion, 0, 1023, frec_min, frec_max);
	cambiar_frecuencia(medicion);
	Serial.println(get_frecuencia());
	Serial.println(volumen_obj);
}


ISR(TIMER1_COMPA_vect) {
	cambio_onda();
}