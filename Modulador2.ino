/*
Modulador de frecuencia y volumen para MegaAVR 328 con reloj de 16Mhz
*/

#include <Arduino.h>
#include <modulador.h>
#include <stdint.h>

/*Configuración Melodía*/
#define elems_array(array) sizeof(array)/sizeof(array[0])

//#define FUR_ELISE
#define BUTTERFLY

const float frecs_notas[] = {130.81, 138.59, 146.83, 155.56, 164.81, 174.61,
						185.00, 196.00, 207.65, 220.00, 233.08, 246.94};
#define NOTA_C3  0  
#define NOTA_C3S 1
#define NOTA_D3  2
#define NOTA_D3S 3
#define NOTA_E3  4
#define NOTA_F3  5
#define NOTA_F3S 6
#define NOTA_G3  7
#define NOTA_G3S 8
#define NOTA_A3  9
#define NOTA_A3S 10
#define NOTA_B3  11


typedef struct _nota {
	unsigned char indice;
	unsigned char octava;
} nota;

const int duracion_incremento = 30;
typedef struct repeticion_tiempo {
  char veces;
  char duracion;
  char delay;
  //duración y delay en incrementos de duracion_incremento
} repetic_tiempo;

/*Melodías*/
#ifdef FUR_ELISE
//Repetriciones de tiempo, Duración y pausa entre notas
const repetic_tiempo delay_nota[] = {
  {5, 6, 1},
  {3, 6, 2},
  {1, 18, 2},
  {1, 6, 1},
  {2, 6, 2},
  {1, 18, 2},
  {3, 6, 2},
  {1, 18, 2},
  {1, 6, 2}
};
//Notas de la canción
const nota notas_melodia[] = {
  //Du Da Du Da
  {NOTA_E3, 2},
  {NOTA_D3S,2},
  {NOTA_E3, 2},
  {NOTA_D3S,2},
  //Du Da Da Duun...
  {NOTA_E3, 2},
  {NOTA_B3, 1},
  {NOTA_D3, 2},
  {NOTA_C3, 2},
  {NOTA_A3, 1},
  //Do Da Da Dunn..
  {NOTA_C3, 1},
  {NOTA_E3, 1},
  {NOTA_A3, 1},
  {NOTA_B3, 1},
  //Do Da Da Dunn..
  {NOTA_E3, 1},
  {NOTA_G3S,1},
  {NOTA_B3, 1},
  {NOTA_C3, 2},
  //Ding
  {NOTA_E3, 1}
};
#endif

#ifdef BUTTERFLY
const repetic_tiempo delay_nota[] = {
  {1, 12, 2},
  {2, 6, 1},
  {1, 12, 2},
  {6, 6, 1},
  {1, 12, 11},
  {1, 12, 2},
  {2, 6, 1},
  {1, 12, 2},
  {6, 6, 1},
  {1, 12, 11},
};
const nota notas_melodia[] = {
  //Ay ay ay
  {NOTA_G3S, 1},{NOTA_G3S,1},{NOTA_B3,1},{NOTA_C3S,2},
  //I'm your little butterfñy
  {NOTA_G3S,1},{NOTA_B3,1},
  {NOTA_C3S,2},{NOTA_E3,2},{NOTA_C3S,2},{NOTA_B3,1},{NOTA_G3S,1},
  //Green black and blue
  {NOTA_F3S,1},{NOTA_F3S,1},{NOTA_G3S,1},{NOTA_B3,1},
  //Make the colors in the sky
  {NOTA_G3S,1},{NOTA_E3,1},
  {NOTA_F3S,1},{NOTA_G3S,1},{NOTA_F3S,1},{NOTA_E3,1},{NOTA_C3S,1}
};
#endif

#define get_frecuencia(struct_nota) (frecs_notas[struct_nota.indice] * struct_nota.octava)

// inicializa modulador
modulador_onda onda_seno;

void tocar_nota(uint16_t frecuencia, char incrementos) {
  onda_seno.cambiar_frecuencia(frecuencia);
  #ifdef CONTROL_VOLUMEN
  onda_seno.volumen_obj = VOLUMEN_MAX;
  #endif

  delay(incrementos*duracion_incremento);
}

void silenciar_notas(char incrementos) {
  #ifdef CONTROL_VOLUMEN
  onda_seno.volumen_obj = 0;
  #else
  onda_seno.cambiar_frecuencia(FREC_MIN_ONDA);
  #endif
  delay(incrementos * duracion_incremento);
}


void setup() {
	Serial.begin(9600);
	onda_seno.inicializar_modulacion();
	#ifdef CONTROL_VOLUMEN
	onda_seno.volumen_obj = VOLUMEN_MAX/2;
	#endif
}

void loop() {
	unsigned char pos_rep_act = 0;
  	repetic_tiempo rep_act = delay_nota[pos_rep_act];
  	for(unsigned int i=0; i < elems_array(notas_melodia) ; ++i) {
  	  if (rep_act.veces <= 0) {
  	    pos_rep_act++;
  	    rep_act = delay_nota[pos_rep_act];
  	  }
  	  //Toca nota actual
  	  tocar_nota(get_frecuencia(notas_melodia[i]) * 2, rep_act.duracion);
  	  //Deja nota en silencio
  	  silenciar_notas(rep_act.delay);
  	  rep_act.veces--;
  	}
}

ISR(TIMER1_COMPA_vect) {
	onda_seno.cambio_onda();
}


