#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define SIN_ERRORES	0
#define ERROR_ARGUMENTOS 1
#define ERROR_RUNTIME 2


#define CM_PI 3.14159265358979323846
const char opcion_ayuda[] = "-h";
const char opcion_redondear[] = "-v";
const char instrucciones_fmt[] =
"Programa crea muestras de una onda seno en el archivo dado\n\
El 1er argumento indica la cantidad de muestras que tomar,\n\
El 2do argumento el nombre del archivo en el cual escribir los resultados\n\
El 3ro y 4to argumento indican el valor mínimo y máximo de la onda (desplazamiento y escala)\n\
Si el 5to argumento es: %s se redondearán los valores hacia el entero más cercano";

enum str_conv_res {VALIDO, VALIDO_PARCIAL, FUERA_DE_RANGO, INVALIDO};

typedef struct _long_opt {
	enum str_conv_res valido;
	long valor;
} long_opt;

long_opt strtolong(char* cadena) {
	char* fin;
	long_opt retorno = {VALIDO, 0};
	errno = 0;
	retorno.valor = strtol(cadena, &fin, 10);

	if (errno == ERANGE) {	 // Valor fuera de rango
		retorno.valido = FUERA_DE_RANGO;
	}
	else if (fin == cadena) {// No halló número al inicio de cadena (ignorando espacios)
		retorno.valido = INVALIDO;
	}
	else if (*fin != '\0') { // Había caractéres no numéricos después del número
		retorno.valido = VALIDO_PARCIAL;
	}
	return retorno;
}

typedef struct _double_opt {
	enum str_conv_res valido;
	double valor;
} double_opt;

double_opt strtodouble(char* cadena) {
	char* fin;
	double_opt retorno = {VALIDO, 0};
	errno = 0;
	retorno.valor = strtod(cadena, &fin);

	if (errno == ERANGE) {	 // Valor fuera de rango
		retorno.valido = FUERA_DE_RANGO;
	}
	else if (fin == cadena) {// No halló número al inicio de cadena (ignorando espacios)
		retorno.valido = INVALIDO;
	}
	else if (*fin != '\0') { // Había caractéres no numéricos después del número
		retorno.valido = VALIDO_PARCIAL;
	}
	return retorno;
}

int main(int argc, char *argv[] ) {
	// --Escribe ayuda si escribe -h --
	if (argc == 2 && strcmp(argv[1], opcion_ayuda) == 0) {
		printf(instrucciones_fmt, opcion_redondear);
		exit(SIN_ERRORES);
	}

	//-- Checa que haya 4 ó 5 argumentos -
	if (argc != 5 && argc != 6) {
		fprintf(stderr ,"Se esperaban 4 o 5 argumentos, se dieron %d\nUse %s para obtener ayuda",argc-1, opcion_ayuda);
		exit(1);
	}
	
	//-- Checa que argumentos numéricos sean válidos --
	long cant_muestras;
	double limites[2];
	
	
	// valida cantidad de muestras de la onda
	{
		long_opt temp = strtolong(argv[1]);
		if (temp.valido == VALIDO && temp.valor >= 0) {
			cant_muestras = temp.valor;
		} else {
			fputs("No se insertó un valor válido en la cantidad de muestras", stderr);
			exit(ERROR_ARGUMENTOS);
		}
	}
	// valida limites inferior y superior de la onda
	for(int i = 0; i < 2; i++) {
		double_opt temp = strtodouble(argv[3 + i]);
		if (temp.valido == VALIDO) {
			limites[i] = temp.valor;
		} else {
			fputs("Uno de los valores mínimo o máximo de la onda no es válido", stderr);
			exit(ERROR_ARGUMENTOS);
		}
	}

	//-- Aplica otras opciones dadas en la linea de comandos --
	bool redondear = false;
	double factor_escala = limites[1]/2 - limites[0]/2;
	double valor_medio = limites[0]/2 + limites[1]/2;

	if (argc == 6 && strcmp(argv[5], opcion_redondear) == 0) {
		redondear = true;
	}

	//-- Crea onda en el archivo dado por el 2o argumento --
	FILE* archivo = fopen(argv[2], "w");
	if (archivo == NULL) {
		perror("Error al abrir el archivo");
		exit(ERROR_RUNTIME);
	}

	if (redondear == false) {
		for(long i = 0; i< cant_muestras; i++) {
			double valor = sin((double) i / (cant_muestras - 1) * 2 *CM_PI) * factor_escala + valor_medio;
			fprintf(archivo, "%lf, ", valor);
			if(i % 32 == 0) {
				fputs("\n", archivo);
			}
		}
	} else {
		for(long i = 0; i< cant_muestras; i++) {
			double valor = sin((double) i / (cant_muestras - 1) * 2 * CM_PI) * factor_escala + valor_medio;
			fprintf(archivo, "%.0lf, ", round(valor));
			if(i % 32 == 0) {
				fputs("\n", archivo);
			}
		}
	}
	

	fclose(archivo);
	exit(SIN_ERRORES);
}