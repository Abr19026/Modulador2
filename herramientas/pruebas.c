#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include <windows.h>
enum str_conv_res {VALIDO, VALIDO_PARCIAL, FUERA_DE_RANGO, INVALIDO};

#define maxvalue(type) _Generic(type, \
								char: CHAR_MAX, \
								unsigned char: UCHAR_MAX, \
								signed char: SCHAR_MAX, \
								short: SHRT_MAX, \
								unsigned short: USHRT_MAX, \
								int: INT_MAX, \
                                unsigned int: UINT_MAX,\
								long: LONG_MAX, \
								unsigned long: ULONG_MAX, \
								long long: LONG_LONG_MAX, \
								unsigned long long: ULONG_LONG_MAX\
								)

#define minvalue(type) _Generic(type, \
								char: CHAR_MIN, \
								unsigned char: 0, \
								signed char: SCHAR_MIN, \
								short: SHRT_MIN, \
								unsigned short: 0, \
								int: INT_MIN, \
                                unsigned int: 0,\
								long: LONG_MIN, \
								unsigned long: 0UL, \
								long long: LONG_LONG_MIN, \
								unsigned long long: 0ULL\
								)
								

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

// soluaciones a impresión
// -- usar ConsolePrint en vez de printf() 
// -- usar wmain en vez de main()
// -- usar las wide functions, convertirlas a utf-8 y viceversa
int main(int argc, char** argv)
{
	if (argc != 3) {
		printf("Error, se esperan 2 argumentos numéricos");
		exit(1);
	}

	uint8_t numeros[2];

	for (int i=0; i< 2; i++) {
		long_opt res = strtolong(argv[1 + i]);
		if 
		( res.valido == VALIDO
		&& res.valor <= maxvalue(numeros[0])
		&& res.valor >= minvalue(numeros[0]))
		{
			numeros[i] = res.valor;
		} else { 
			FILE* arch = fopen("salida.txt", "w");
			fprintf(arch,"El argumento %s no es un número válido", argv[1 + i]);
			fclose(arch);
			for (char* michar = argv[1 + i];  *michar != '\0'; michar++ ) {
				printf("\n%u", (unsigned char)*michar);
			}
			exit(1);
		}
	}

	typeof(numeros[0]) resultado = numeros[0] - numeros[1];
	printf("%hhu - %hhu = %hhu\n", numeros[0], numeros[1], resultado);

	resultado = numeros[1] - numeros[0];
	printf("%hhu - %hhu = %hhu\n", numeros[1], numeros[0], resultado);

	return 0;
}
